#include <cstdio>
#include <unistd.h>
#include "ADS1X15_TLA2024.h"

// ADS1115 ads;  /* Use this for the 16-bit version */
TLA2024 tla_sigleEnded(I2CDeviceDefaultName, I2CADDRESS_1); /* Use thi for the 12-bit version */
TLA2024 tla_differential(I2CDeviceDefaultName, I2CADDRESS_2);

int main()
{
	printf("In this explame, 2 tla2024 chips are used in the same i2c bus.\nOne reads 2 differential readings and the other reads 3 single ended readings!\n\n");

	printf("Getting differential reading from AIN0 (P) and AIN1 (N)\n");
	printf("Getting single-ended readings from AIN0..3\n");
	printf("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)\n");

	// The ADC input range (or gain) can be changed via the following
	// functions, but be careful never to exceed VDD +0.3V max, or to
	// exceed the upper and lower limits if you adjust the input range!
	// Setting these values incorrectly may destroy your ADC!
	//                                                                ADS1015  ADS1115
	//                                                                -------  -------
	// ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
	// ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
	// ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
	// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
	// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
	// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

	while (1)
	{
		int16_t results1;
		int16_t results2;

		/* Be sure to update this value based on the IC and the gain settings! */
		float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
		//float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

		results1 = tla_differential.readADC_Differential_0_1();
		results2 = tla_differential.readADC_Differential_2_3();

		printf("Differential_0_1: %d(%fmV) | ", results1, results1 * multiplier);
		printf("Differential_2_3: %d(%fmV)\n", results2, results2 * multiplier);

		int16_t adc0, adc1, adc2, adc3;

		adc0 = tla_sigleEnded.readADC_SingleEnded(0);
		adc1 = tla_sigleEnded.readADC_SingleEnded(1);
		adc2 = tla_sigleEnded.readADC_SingleEnded(2);
		adc3 = tla_sigleEnded.readADC_SingleEnded(3);

		printf("AIN0: %d\n", adc0);
		printf("AIN1: %d\n", adc1);
		printf("AIN2: %d\n", adc2);
		printf("AIN3: %d\n", adc3);
		printf("\n");

		sleep(1);
	}
}
