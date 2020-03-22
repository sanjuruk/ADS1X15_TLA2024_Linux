# Driver: ADS1115 ADS1015 TLA2024 

Driver for TI's ADS1115(16-bit) ADS1015(12-bit) TLA2024(12-bit) Differential or Single-Ended ADC with PGA and Comparator.

## Forked for Linux

This is a modified version of the [Adafruit's ADS1015 library](https://github.com/adafruit/Adafruit_ADS1X15).
This library can be directly used through the i2c bus of a Single Board Computer using Linux like Raspberry Pi, Orange Pi, pcDuino, Odroid etc.

1) Activate Linux i2c module on your board
    > raspi-config/armbian-config or other ways. Please review board specifications and Linux distro settings
2) Confirm the activated i2c device name
    > ls /dev/i2c*
3) Use this device in the constructor of the device you are using
    > TLA2024 tla_sigleEnded("/dev/i2c-0", 0x48);
   
4 examples are provided to highlight different uses of the library.
Example 'multiDeviceOnSameBus' added to show how to use 2 chips on the same bus. Max devices supported on the same bus are 3. Check the datasheet for more information.

## Build

Build the static library and the examples using the 'Makefile'
Go to the project folder and type <code>make</code>

## Hardware

This family of ADCs provide 4 single-ended or 2 differential channels.
Each has a programmable gain amplifier from 2/3 up to 16x. Available in 12 or 16 bit versions:

* [ADS1015 12-bit ADC](http://www.ti.com/lit/ds/symlink/ads1015.pdf)
* [ADS1115 16-bit ADC](http://www.ti.com/lit/ds/symlink/ads1015.pdf)
* [TLA2024 12-bit ADC](http://www.ti.com/lit/ds/symlink/tla2024.pdf)
