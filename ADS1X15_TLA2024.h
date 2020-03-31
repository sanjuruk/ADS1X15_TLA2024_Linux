/**************************************************************************/
/*!
    @file     ADS1X15_TLA2024.h
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

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*=========================================================================
    Configs
    -----------------------------------------------------------------------*/
#define I2CDeviceDefaultName "/dev/i2c-0"
#define FailTryCount 10
    //#define DEBUG
            /*=========================================================================*/

            /*=========================================================================
                I2C ADDRESS/BITS
                -----------------------------------------------------------------------*/
#define I2CADDRESS_1                 (0x48)    // 1001 000 (ADDR = GND)
#define I2CADDRESS_2                 (0x49)    // 1001 000 (ADDR = VDD)
#define I2CADDRESS_3                 (0x4B)    // 1001 000 (ADDR = SCL)
                /*=========================================================================*/

                /*=========================================================================
            ADS TYPE
            -----------------------------------------------------------------------*/
#define tla2024                         (2)
#define ads1015                         (1)
#define ads1115                         (0)
            /*=========================================================================*/

            /*=========================================================================
                CONVERSION DELAY (in uS)
                -----------------------------------------------------------------------*/
#define TLA2024_CONVERSIONDELAY (1000) ///< Conversion delay
#define ADS1015_CONVERSIONDELAY (1000) ///< Conversion delay
#define ADS1115_CONVERSIONDELAY (9000) ///< Conversion delay
                /*=========================================================================*/

                /*=========================================================================
                    POINTER REGISTER
                    -----------------------------------------------------------------------*/
#define ADS1015_REG_POINTER_MASK (0x03)      ///< Point mask
#define ADS1015_REG_POINTER_CONVERT (0x00)   ///< Conversion
#define ADS1015_REG_POINTER_CONFIG (0x01)    ///< Configuration
#define ADS1015_REG_POINTER_LOWTHRESH (0x02) ///< Low threshold
#define ADS1015_REG_POINTER_HITHRESH (0x03)  ///< High threshold
                    /*=========================================================================*/

                    /*=========================================================================
                        CONFIG REGISTER
                        -----------------------------------------------------------------------*/
#define ADS1015_REG_CONFIG_OS_MASK (0x8000) ///< OS Mask
#define ADS1015_REG_CONFIG_OS_SINGLE                                           \
  (0x8000) ///< Write: Set to start a single-conversion
#define ADS1015_REG_CONFIG_OS_BUSY                                             \
  (0x0000) ///< Read: Bit = 0 when conversion is in progress
#define ADS1015_REG_CONFIG_OS_NOTBUSY                                          \
  (0x8000) ///< Read: Bit = 1 when device is not performing a conversion

#define ADS1015_REG_CONFIG_MUX_MASK (0x7000) ///< Mux Mask
#define ADS1015_REG_CONFIG_MUX_DIFF_0_1                                        \
  (0x0000) ///< Differential P = AIN0, N = AIN1 (default)
#define ADS1015_REG_CONFIG_MUX_DIFF_0_3                                        \
  (0x1000) ///< Differential P = AIN0, N = AIN3
#define ADS1015_REG_CONFIG_MUX_DIFF_1_3                                        \
  (0x2000) ///< Differential P = AIN1, N = AIN3
#define ADS1015_REG_CONFIG_MUX_DIFF_2_3                                        \
  (0x3000) ///< Differential P = AIN2, N = AIN3
#define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000) ///< Single-ended AIN0
#define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000) ///< Single-ended AIN1
#define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000) ///< Single-ended AIN2
#define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000) ///< Single-ended AIN3

#define ADS1015_REG_CONFIG_PGA_MASK (0x0E00)   ///< PGA Mask
#define ADS1015_REG_CONFIG_PGA_6_144V (0x0000) ///< +/-6.144V range = Gain 2/3
#define ADS1015_REG_CONFIG_PGA_4_096V (0x0200) ///< +/-4.096V range = Gain 1
#define ADS1015_REG_CONFIG_PGA_2_048V                                          \
  (0x0400) ///< +/-2.048V range = Gain 2 (default)
#define ADS1015_REG_CONFIG_PGA_1_024V (0x0600) ///< +/-1.024V range = Gain 4
#define ADS1015_REG_CONFIG_PGA_0_512V (0x0800) ///< +/-0.512V range = Gain 8
#define ADS1015_REG_CONFIG_PGA_0_256V (0x0A00) ///< +/-0.256V range = Gain 16

#define ADS1015_REG_CONFIG_MODE_MASK (0x0100)   ///< Mode Mask
#define ADS1015_REG_CONFIG_MODE_CONTIN (0x0000) ///< Continuous conversion mode
#define ADS1015_REG_CONFIG_MODE_SINGLE                                         \
  (0x0100) ///< Power-down single-shot mode (default)

#define ADS1015_REG_CONFIG_DR_MASK (0x00E0)   ///< Data Rate Mask
#define ADS1015_REG_CONFIG_DR_128SPS (0x0000) ///< 128 samples per second
#define ADS1015_REG_CONFIG_DR_250SPS (0x0020) ///< 250 samples per second
#define ADS1015_REG_CONFIG_DR_490SPS (0x0040) ///< 490 samples per second
#define ADS1015_REG_CONFIG_DR_920SPS (0x0060) ///< 920 samples per second
#define ADS1015_REG_CONFIG_DR_1600SPS                                          \
  (0x0080) ///< 1600 samples per second (default)
#define ADS1015_REG_CONFIG_DR_2400SPS (0x00A0) ///< 2400 samples per second
#define ADS1015_REG_CONFIG_DR_3300SPS (0x00C0) ///< 3300 samples per second
#define ADS1115_REG_CONFIG_DR_860SPS    (0x00E0)  // 860 samples per second for ADS1115 (3300 for ADS1015)

#define ADS1015_REG_CONFIG_CMODE_MASK (0x0010) ///< CMode Mask
#define ADS1015_REG_CONFIG_CMODE_TRAD                                          \
  (0x0000) ///< Traditional comparator with hysteresis (default)
#define ADS1015_REG_CONFIG_CMODE_WINDOW (0x0010) ///< Window comparator

#define ADS1015_REG_CONFIG_CPOL_MASK (0x0008) ///< CPol Mask
#define ADS1015_REG_CONFIG_CPOL_ACTVLOW                                        \
  (0x0000) ///< ALERT/RDY pin is low when active (default)
#define ADS1015_REG_CONFIG_CPOL_ACTVHI                                         \
  (0x0008) ///< ALERT/RDY pin is high when active

#define ADS1015_REG_CONFIG_CLAT_MASK                                           \
  (0x0004) ///< Determines if ALERT/RDY pin latches once asserted
#define ADS1015_REG_CONFIG_CLAT_NONLAT                                         \
  (0x0000) ///< Non-latching comparator (default)
#define ADS1015_REG_CONFIG_CLAT_LATCH (0x0004) ///< Latching comparator

#define ADS1015_REG_CONFIG_CQUE_MASK (0x0003) ///< CQue Mask
#define ADS1015_REG_CONFIG_CQUE_1CONV                                          \
  (0x0000) ///< Assert ALERT/RDY after one conversions
#define ADS1015_REG_CONFIG_CQUE_2CONV                                          \
  (0x0001) ///< Assert ALERT/RDY after two conversions
#define ADS1015_REG_CONFIG_CQUE_4CONV                                          \
  (0x0002) ///< Assert ALERT/RDY after four conversions
#define ADS1015_REG_CONFIG_CQUE_NONE                                           \
  (0x0003) ///< Disable the comparator and put ALERT/RDY in high state (default)

#define TLA2024_REG_RESERVED (0x0003)  ///< Reserved section must be written 03h
                        /*=========================================================================*/

                        /** Gain settings */
typedef enum {
    GAIN_TWOTHIRDS = ADS1015_REG_CONFIG_PGA_6_144V,
    GAIN_ONE = ADS1015_REG_CONFIG_PGA_4_096V,
    GAIN_TWO = ADS1015_REG_CONFIG_PGA_2_048V,
    GAIN_FOUR = ADS1015_REG_CONFIG_PGA_1_024V,
    GAIN_EIGHT = ADS1015_REG_CONFIG_PGA_0_512V,
    GAIN_SIXTEEN = ADS1015_REG_CONFIG_PGA_0_256V
} adsGain_t;

/** Sampling settings */
typedef enum
{
    SPS_128 = ADS1015_REG_CONFIG_DR_128SPS,
    SPS_250 = ADS1015_REG_CONFIG_DR_250SPS,
    SPS_490 = ADS1015_REG_CONFIG_DR_490SPS,
    SPS_920 = ADS1015_REG_CONFIG_DR_920SPS,
    SPS_1600 = ADS1015_REG_CONFIG_DR_1600SPS,
    SPS_2400 = ADS1015_REG_CONFIG_DR_2400SPS,
    SPS_3300 = ADS1015_REG_CONFIG_DR_3300SPS,
    SPS_860 = ADS1115_REG_CONFIG_DR_860SPS
} adsSps_t;

/**************************************************************************/
/*!
    @brief  Sensor driver for the TLA2024 ADC breakout.
*/
/**************************************************************************/
class TLA2024 {
protected:
    // Instance-specific properties
    const char* m_i2cDeviceName;
    uint8_t m_i2cAddress;      ///< the I2C address
    uint8_t m_conversionDelay; ///< conversion deay
    uint8_t m_bitShift;        ///< bit shift amount
    adsGain_t m_gain;          ///< ADC gain
    adsSps_t  m_sps;
    uint8_t   m_adsType;

public:
    TLA2024(const char* i2cDeviceName = I2CDeviceDefaultName, uint8_t i2cAddress = I2CADDRESS_1);
    uint16_t readADC_SingleEnded(uint8_t channel);
    int16_t readADC_Differential_0_1(void);
    int16_t readADC_Differential_2_3(void);
    int16_t getLastConversionResults();
    void updateI2cDevice(const char* i2cDeviceName);
    void setGain(adsGain_t gain);
    adsGain_t getGain(void);
    void      setSps(adsSps_t sps);
    adsSps_t  getSps(void);
    void      setConversionDelay(void);

private:
};

/**************************************************************************/
/*!
    @brief  Sensor driver for the ADS1015 ADC breakout.
*/
/**************************************************************************/
class ADS1015 : public TLA2024 {
public:
    ADS1015(const char* i2cDeviceName = I2CDeviceDefaultName, uint8_t i2cAddress = I2CADDRESS_1);
    void startComparator_SingleEnded(uint8_t channel, int16_t threshold);

private:
};

/**************************************************************************/
/*!
    @brief  Sensor driver for the ADS1115 ADC breakout.
*/
/**************************************************************************/
class ADS1115 : public ADS1015 {
public:
    ADS1115(const char* i2cDeviceName = I2CDeviceDefaultName, uint8_t i2cAddress = I2CADDRESS_1);

private:
};
