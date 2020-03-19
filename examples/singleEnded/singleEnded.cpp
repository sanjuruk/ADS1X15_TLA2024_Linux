#include <cstdio>
#include <unistd.h>
#include "ADS1X15_TLA2024.h"

// ADS1115 ads;  /* Use this for the 16-bit version */
// ADS1015 ads;     /* Use thi for the 12-bit version */
TLA2024 ads;

int main()
{
    printf("Example for reading single ended readings!\n");

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
        int16_t adc0, adc1, adc2, adc3;

        adc0 = ads.readADC_SingleEnded(0);
        adc1 = ads.readADC_SingleEnded(1);
        adc2 = ads.readADC_SingleEnded(2);
        adc3 = ads.readADC_SingleEnded(3);
        printf("AIN0: %d\n", adc0);
        printf("AIN1: %d\n", adc1);
        printf("AIN2: %d\n", adc2);
        printf("AIN3: %d\n", adc3);
        printf("\n");

        sleep(1);
    }
}
