#include <cstdio>
#include <unistd.h>
#include "ADS1X15_TLA2024.h"

// ADS1115 ads;  /* Use this for the 16-bit version */
// ADS1015 ads;     /* Use thi for the 12-bit version */
TLA2024 ads;

int main()
{
    printf("Example for differential readings!\n");

    printf("Getting differential reading from AIN0 (P) and AIN1 (N)\n");
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
        int16_t results;

        /* Be sure to update this value based on the IC and the gain settings! */
        float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
        //float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

        results = ads.readADC_Differential_0_1();
        //results = ads.readADC_Differential_2_3();

        printf("Differential: %d(%fmV)\n", results, results * multiplier);

        sleep(1);
    }
}

