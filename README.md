# TM1637 for STM32
TM1637 library
features functions to work with a TM1637 driver for LED display.
tested on a 4-digit display, stm32f407g-disc1 board.
NB: the driver clocking is done using a preconfigured TIM timer set for 1 microsecond tic by means of setting an appropriate prescaler.

EXAMPLE:

  TM1637_display_type my_display = {0};
  
  my_display.sig_pin = 1;
  my_display.sig_port = GPIOE;
  
  my_display.clk_pin = 0;
  my_display.clk_port = GPIOE;
  
  my_display.tic = 5;
  
  my_display.brightness = 0;
  
  my_display.separator = 0;
  
  TM1637_init(my_display, htim2); // htim2 is a preconfigured timer as in 'TIM_HandleTypeDef htim2;'
  
while(1)
{
  TM1637_disp_d(1234);
}

4.04.2022
lib version: 1.1
can set brightness and display decimals, configured for fixed addressing.
