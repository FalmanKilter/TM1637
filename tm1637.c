#include "tm1637.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"

/*========================================================================================================*/

TIM_HandleTypeDef tm_lcl;       /* configured TM */

GPIO_TypeDef*  sig_port;        /* signal port */
  
GPIO_TypeDef*  clk_port;        /* clock port */
  
uint16_t      sig_pin;          /* signal pin*/
  
uint16_t      clk_pin;          /* clock pin */

uint16_t      tic;              /* tic length*/

uint8_t       sep_mask;         /* separator mask */

uint8_t       brightness;       /* brightness level */

/*========================================================================================================*/

/*INITIALISATION */
/* display instance init*/
void TM1637_init(TM1637_display_type display, TIM_HandleTypeDef tm)
{
  /* get structure fields to pass to other functions */
  sig_port = display.sig_port;
  sig_pin = get_gpio_pin(display.sig_pin);
  clk_port = display.clk_port;
  clk_pin = get_gpio_pin(display.clk_pin);
  tic = display.tic;
  brightness = display.brightness;
  
  /* if separator is turned on, set a mask to add to data for grid #2
      on a 4-digit display it is a two dots separator between grid #2 and #3*/
  if (display.separator !=0) sep_mask = 0x80;  
  else sep_mask = 0;
  
  /* GPIO initialisation*/
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO ports clock enable, covers all pin groups on stm32f4xx
      one may comment unnecessary out*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /* GPIO pin output levels set to low by default */
  HAL_GPIO_WritePin(sig_port, sig_pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET);

  /* signal pin config */
  GPIO_InitStruct.Pin = sig_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(sig_port, &GPIO_InitStruct);

  /* clock pin config */
  GPIO_InitStruct.Pin = clk_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(clk_port, &GPIO_InitStruct);
  
  /* start the configured TIM timer */
   tm_lcl = tm;
   HAL_TIM_Base_Start(&tm);
   
   /* set display brightness level 0-7, default is 0, display is then off*/
   set_brightness(brightness);
   
   /* reset all grids segments to 0*/
   refresh_disp();
}

/* DATA OUTPUT TO DISPLAY*/
/* display decimal number*/
void TM1637_disp_d(uint16_t data)
{
  refresh_disp();
  
  /* data is divided into digits and input starts from left to right
      so the starting grid is initiated as 
      grid_1 + how many digits a display can handle - 1*/
  uint8_t current_grid = grid_1 + digit_num - 1;
  
  /* limit the input data to a 4-digit display
      if more grids or digits are available, 
      define a new max_data in tm1637.h */
  if (data > max_data) 
  {
    disp_err();
    current_grid = 0;
  }
  
  while (current_grid !=0)
  {
    seg_on(digit_to_hex((uint8_t)(data%10)),current_grid--);
    data /= 10;
    if (data == 0) break;
  }  
}

/*========================================================================================================*/

/*PRIVATE AUXILIARY FUNCTIONS*/
/* convert human pin number to bit field definition given in stm32f4xx_hal_gpio.h
    to pass to GPIO init functions*/
uint16_t get_gpio_pin(uint8_t num)
{
  switch(num)
    {
    case 1: return 0x0002; break;
    case 2: return 0x0004; break;
    case 3: return 0x0008; break;
    case 4: return 0x0010; break;
    case 5: return 0x0020; break;
    case 6: return 0x0040; break;
    case 7: return 0x0080; break;
    case 8: return 0x0100; break;
    case 9: return 0x0200; break;
    case 10: return 0x0400; break;
    case 11: return 0x0800; break;
    case 12: return 0x1000; break;
    case 13: return 0x2000; break;
    case 14: return 0x4000; break;
    case 15: return 0x8000; break;
    default : return 0x0001; break;
    }
}

/* pin control */
void sig_high()
{
  HAL_GPIO_WritePin(sig_port, sig_pin, GPIO_PIN_SET);      /* set signal pin HIGH*/
}

void sig_low()
{
  HAL_GPIO_WritePin(sig_port, sig_pin, GPIO_PIN_RESET);      /* set signal pin LOW*/
}

void clk_high()
{
  HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_SET);      /* set clock pin HIGH*/
}

void clk_low()
{
  HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET);      /* set clock pin LOW*/
}

/* refresh display - reset all grid segments*/
void refresh_disp()
{
  seg_on(0x00, grid_1);
  seg_on(0x00, grid_2);
  seg_on(0x00, grid_3);
  seg_on(0x00, grid_4);
}

/* microsecond delay function based on TIM timer*/
void delay (uint16_t dt)
{
  /*reset the counter to 0*/
  __HAL_TIM_SET_COUNTER(&tm_lcl,0);
  
  /*wait for the counter to tictoc to the desired value*/
  while (__HAL_TIM_GET_COUNTER(&tm_lcl) < dt);  
}

/* convert a digit into segment code g f e d c b a  style*/
uint8_t digit_to_hex(uint8_t digit)
{
  switch(digit)
  {
  case 1: return 0x06; break;
  case 2: return 0x5B; break;
  case 3: return 0x4F; break;
  case 4: return 0x66; break;
  case 5: return 0x6D; break;
  case 6: return 0x7D; break;
  case 7: return 0x07; break;
  case 8: return 0x7F; break;
  case 9: return 0x6F; break;
  default : return 0x3F; break;
  }
}

/* display 'Err' */
void disp_err()
{
  seg_on(0x79,grid_1);  // code for 'E'
  seg_on(0x50,grid_2);  // code for 'r'
  seg_on(0x50,grid_3);
  seg_on(0x5C,grid_4);  // code for 'o'
}

/* display separator between grid #2 and #3*/
void disp_separator()
{
  /* for a 4-digit display it is a digital clock-like two dots separator */
  seg_on(0x80, grid_2); // code for decimal point of grid #2
}

/* INTERFACE COMMAND FUNCTIONS */
/* start command sequence */
void cmd_seq_start()
{
  /* when clk is a high level and sig changes from high to low level, data input starts */
  clk_high();
  sig_high();
  delay(tic);
  clk_high();
  sig_low();
  delay(tic);
}

/* end command sequence */
void cmd_seq_end()
{
  /* when clk is a high level and sig changes from low to high level, data input starts */
  clk_low();
  sig_low();
  delay(tic);
  clk_high();
  sig_low();
  delay(tic);
  clk_high();
  sig_high();
  delay(tic);
}

/* set brightness of the display */
void set_brightness(uint8_t lvl)
{
  cmd_seq_start();
  tx_cmd(0x87 + lvl);
  await_ack();
  cmd_seq_end();
}

/* send command - write one byte of data to sig, bit per tic*/
void tx_cmd(uint8_t data)
{
  uint8_t cnt = 8;
  /* bits are written when clk is LOW and latched when clk is HIGH */
  while(cnt != 0)
  {
    /*perform a tic*/
    clk_low();
    /* chop off LSB of input data*/
    /* and set sig accordingly*/
    data % 2 == 1 ? sig_high() : sig_low();
    
    /*shift data to the right by one bit*/
    data >>= 1;
   
    delay(tic);
   /* perform a toc */
    clk_high();
    delay(tic);
    
    cnt--;
  }
}

/* wait for acknowledgment */
void await_ack()
{
  clk_low();
  sig_low();
  delay(tic);
  delay(tic);
  clk_high();
  delay(tic);
}


/* SEGMENT CONTROL BY ADDRESS */
/* turns on the desired segments by the desired address*/
void seg_on(uint8_t num, uint8_t addr)
{
  if (addr == grid_2)
  {
    num |= sep_mask;
  }
  /* num must follow conventional 7-digit display segment numbering*/
  /* g f e d c b a for each grid*/
  
  cmd_seq_start();
  /* write to fixed address */
  tx_cmd(write_to_fixed_addr);
  await_ack();
  cmd_seq_end();
  
  /* send address*/
  cmd_seq_start();
  tx_cmd(addr);
  await_ack();
  
  /* send data*/
  tx_cmd(num); 
  await_ack();
  cmd_seq_end();
}





