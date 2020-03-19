/**************************************************************************/
/*!
    @file     ADS1X15_TLA2024.cpp
    @author   K.Townsend (Adafruit Industries)

    @mainpage Adafruit ADS1X15 ADC Breakout Driver

    @section intro_sec Introduction

    This is a library for the Adafruit ADS1X15 ADC breakout boards.

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section author Author

    Written by Kevin "KTOWN" Townsend for Adafruit Industries.

    @section  HISTORY

    v1.0  - First release
    v1.1  - Added ADS1115 support - W. Earl
    v2.0  - Added TLA2024 support - Sanju Lokuhitige
    v2.1  - Ported to Linux - Sanju Lokuhitige

    @section license License

    BSD license, all text here must be included in any redistribution
*/
/**************************************************************************/

#include "ADS1X15_TLA2024.h"

int i2cHandle;

/**************************************************************************/
/*!
	@brief Init the i2c communication
*/
/**************************************************************************/
static void beginTransmission(const char* i2cDeviceName, uint8_t i2cAddress) {
	// Create the file descriptor for the i2c bus
	i2cHandle = open(i2cDeviceName, O_RDWR);
	if (i2cHandle < 0)
	{
		fprintf(stderr, "Error while opening the i2c-0 device! Error: %s\n", strerror(errno));
		exit(1);
	}
	// Set the slave address
	if (ioctl(i2cHandle, I2C_SLAVE, i2cAddress) < 0)
	{
		fprintf(stderr, "Error while configuring the slave address %d. Error: %s\n", i2cAddress, strerror(errno));
		exit(1);
	}
}

/**************************************************************************/
/*!
	@brief End the i2c communication
*/
/**************************************************************************/
static void endTransmission(void) {
	close(i2cHandle);
}

/**************************************************************************/
/*!
    @brief  Writes 16-bits to the specified destination register
    
    @param i2cDeviceName I2C device name
    @param i2cAddress I2C address of device
    @param reg register address to write to
    @param value value to write to register
*/
/**************************************************************************/
static void writeRegister(const char* i2cDeviceName, uint8_t i2cAddress, uint8_t reg, uint16_t value) {
	beginTransmission(i2cDeviceName, i2cAddress);
	int rc;
	unsigned char buf[3] = { reg, (uint8_t)(value >> 8) , (uint8_t)(value & 0xFF) };
	rc = write(i2cHandle, buf, 3);
	if (rc == -1) { printf("Write Error\n"); };
	endTransmission();
}

/**************************************************************************/
/*!
    @brief  Read 16-bits from the specified destination register
    
    @param i2cDeviceName I2C device name
    @param i2cAddress I2C address of device
    @param reg register address to read from

    @return 16 bit register value read
*/
/**************************************************************************/
static uint16_t readRegister(const char* i2cDeviceName, uint8_t i2cAddress, uint8_t reg) {
	beginTransmission(i2cDeviceName, i2cAddress);
	int rc;
	unsigned char buf[1] = { reg };
	rc = write(i2cHandle, buf, 1);
	if (rc == -1) { printf("Write Error\n"); }

	unsigned char readbuf[2] = {  };
	rc = read(i2cHandle, readbuf, 2);
	if (rc == -1) { printf("Read Error\n"); }

	endTransmission();
	return ((readbuf[0] << 8) | readbuf[1]);
}

/**************************************************************************/
/*!
    @brief  Instantiates a new TLA2024 class w/appropriate properties

    @param i2cDeviceName I2C device name
    @param i2cAddress I2C address of device
*/
/**************************************************************************/
TLA2024::TLA2024(const char* i2cDeviceName, uint8_t i2cAddress)
{
    m_i2cDeviceName = i2cDeviceName;
    m_i2cAddress = i2cAddress;
    m_conversionDelay = TLA2024_CONVERSIONDELAY;
    m_bitShift = 4;
    m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1015 class w/appropriate properties

    @param i2cDeviceName I2C device name
    @param i2cAddress I2C address of device
*/
/**************************************************************************/
ADS1015::ADS1015(const char* i2cDeviceName, uint8_t i2cAddress)
{
    m_i2cDeviceName = i2cDeviceName;
    m_i2cAddress = i2cAddress;
    m_conversionDelay = ADS1015_CONVERSIONDELAY;
    m_bitShift = 4;
    m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

/**************************************************************************/
/*!
    @brief  Instantiates a new ADS1115 class w/appropriate properties

    @param i2cDeviceName I2C device name
    @param i2cAddress I2C address of device
*/
/**************************************************************************/
ADS1115::ADS1115(const char* i2cDeviceName, uint8_t i2cAddress)
{
    m_i2cDeviceName = i2cDeviceName;
    m_i2cAddress = i2cAddress;
    m_conversionDelay = ADS1115_CONVERSIONDELAY;
    m_bitShift = 0;
    m_gain = GAIN_TWOTHIRDS; /* +/- 6.144V range (limited to VDD +0.3V max!) */
}

/**************************************************************************/
/*!
    @brief  Sets the gain and input voltage range

    @param gain gain setting to use
*/
/**************************************************************************/
void TLA2024::setGain(adsGain_t gain) { m_gain = gain; }

/**************************************************************************/
/*!
    @brief  Gets a gain and input voltage range

    @return the gain setting
*/
/**************************************************************************/
adsGain_t TLA2024::getGain() { return m_gain; }

/**************************************************************************/
/*!
    @brief  Gets a single-ended ADC reading from the specified channel

    @param channel ADC channel to read

    @return the ADC reading
*/
/**************************************************************************/
uint16_t TLA2024::readADC_SingleEnded(uint8_t channel) {
  if (channel > 3) {
    return 0;
  }

  // Start with default values
  uint16_t config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  // Set single-ended input channel
  switch (channel) {
  case (0):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
    break;
  case (1):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
    break;
  case (2):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
    break;
  case (3):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
    break;
  }

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

#ifdef DEBUG
  uint16_t checkConfig = readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG);
  if (config == checkConfig)
  {
      printf("Configurations succefully written\n");
  }
  else
  {
      printf("Config register values are different from written\n");
  }
#endif // DEBUG

  // Wait for the conversion to complete
  usleep(m_conversionDelay);

  // Read the conversion results
  // Shift 12-bit results right 4 bits for the ADS1015
  return readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN0) and N (AIN1) input.  Generates
            a signed value since the difference can be either
            positive or negative.

    @return the ADC reading
*/
/**************************************************************************/
int16_t TLA2024::readADC_Differential_0_1() {
  // Start with default values
  uint16_t config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1; // AIN0 = P, AIN1 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

#ifdef DEBUG
  uint16_t checkConfig = readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG);
  if (config == checkConfig)
  {
      printf("Configurations succefully written\n");
  }
  else
  {
      printf("Config register values are different from written\n");
  }
#endif // DEBUG

  // Wait for the conversion to complete
  usleep(m_conversionDelay);

  // Read the conversion results
  uint16_t res =
      readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == 0) {
    return (int16_t)res;
  } else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}

/**************************************************************************/
/*!
    @brief  Reads the conversion results, measuring the voltage
            difference between the P (AIN2) and N (AIN3) input.  Generates
            a signed value since the difference can be either
            positive or negative.

    @return the ADC reading
*/
/**************************************************************************/
int16_t TLA2024::readADC_Differential_2_3() {
  // Start with default values
  uint16_t config =
      ADS1015_REG_CONFIG_CQUE_NONE |    // Disable the comparator (default val)
      ADS1015_REG_CONFIG_CLAT_NONLAT |  // Non-latching (default val)
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

  // Set PGA/voltage range
  config |= m_gain;

  // Set channels
  config |= ADS1015_REG_CONFIG_MUX_DIFF_2_3; // AIN2 = P, AIN3 = N

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  // Write config register to the ADC
  writeRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);

#ifdef DEBUG
  uint16_t checkConfig = readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG);
  if (config == checkConfig)
  {
      printf("Configurations succefully written\n");
  }
  else
  {
      printf("Config register values are different from written\n");
  }
#endif // DEBUG

  // Wait for the conversion to complete
  usleep(m_conversionDelay);

  // Read the conversion results
  uint16_t res =
      readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == 0) {
    return (int16_t)res;
  } else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}

/**************************************************************************/
/*!
    @brief  Sets up the comparator to operate in basic mode, causing the
            ALERT/RDY pin to assert (go from high to low) when the ADC
            value exceeds the specified threshold.

            This will also set the ADC in continuous conversion mode.

    @param channel ADC channel to use
    @param threshold comparator threshold
*/
/**************************************************************************/
void ADS1015::startComparator_SingleEnded(uint8_t channel,
                                                   int16_t threshold) {
  // Start with default values
  uint16_t config =
      ADS1015_REG_CONFIG_CQUE_1CONV |   // Comparator enabled and asserts on 1
                                        // match
      ADS1015_REG_CONFIG_CLAT_LATCH |   // Latching mode
      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
      ADS1015_REG_CONFIG_CMODE_TRAD |   // Traditional comparator (default val)
      ADS1015_REG_CONFIG_DR_1600SPS |   // 1600 samples per second (default)
      ADS1015_REG_CONFIG_MODE_CONTIN |  // Continuous conversion mode
      ADS1015_REG_CONFIG_MODE_CONTIN;   // Continuous conversion mode

  // Set PGA/voltage range
  config |= m_gain;

  // Set single-ended input channel
  switch (channel) {
  case (0):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
    break;
  case (1):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
    break;
  case (2):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
    break;
  case (3):
    config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
    break;
  }

  // Set the high threshold register
  // Shift 12-bit results left 4 bits for the ADS1015
  writeRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_HITHRESH,
                threshold << m_bitShift);

  // Write config register to the ADC
  writeRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONFIG, config);
}

/**************************************************************************/
/*!
    @brief  In order to clear the comparator, we need to read the
            conversion results.  This function reads the last conversion
            results without changing the config value.

    @return the last ADC reading
*/
/**************************************************************************/
int16_t TLA2024::getLastConversionResults() {
  // Wait for the conversion to complete
  usleep(m_conversionDelay);

  // Read the conversion results
  uint16_t res =
      readRegister(m_i2cDeviceName, m_i2cAddress, ADS1015_REG_POINTER_CONVERT) >> m_bitShift;
  if (m_bitShift == 0) {
    return (int16_t)res;
  } else {
    // Shift 12-bit results right 4 bits for the ADS1015,
    // making sure we keep the sign bit intact
    if (res > 0x07FF) {
      // negative number - extend the sign to 16th bit
      res |= 0xF000;
    }
    return (int16_t)res;
  }
}
