# TM1637
TM1637 library
features functions to work with a TM1637 driver for LED display.
tested on a 4-digit display, stm32f407g-disc1 board.
NB: the driver clocking is done using a preconfigured TIM timer set for 1 microsecond tic by means of setting an appropriate prescaler.

4.04.2022
lib version: 1.0
can set brightness and display decimals, configured for fixed addressing.
