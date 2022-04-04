#include "tm1637.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal_tim.h"

/*========================================================================================================*/

/*local timer copy of the one initiliased as TIM*/
TIM_HandleTypeDef tm_lcl; 

/*========================================================================================================*/

/*INITIALISATION FUNCTIONS */
/* disp initial reset to 0 */
void disp_init()
{
   refresh_disp();
}

/* TIM start*/
void tm_start(TIM_HandleTypeDef tm)
{
  /* start the configured TIM timer */
   tm_lcl = tm;
   HAL_TIM_Base_Start(&tm);
}

/* GPIO pins initialisation */
void gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO ports clock enable, depends on the chosen ports for signal and clock*/
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
}

/*PRIVATE AUXILIARY FUNCTIONS*/
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
/* INTERFACE COMMAND FUNCTIONS */
/* start command sequence */
void cmd_seq_start()
{
  /* when clk is a high level and sig changes from high to low level, data input starts */
  clk_high;
  sig_high;
  delay(tic);
  clk_high;
  sig_low;
  delay(tic);
}

/* end command sequence */
void cmd_seq_end()
{
  /* when clk is a high level and sig changes from low to high level, data input starts */
  clk_low;
  sig_low;
  delay(tic);
  clk_high;
  sig_low;
  delay(tic);
  clk_high;
  sig_high;
  delay(tic);
}

/* set brightness of the display */
void set_brightness(uint8_t lvl)
{
  cmd_seq_start();
  tx_cmd(disp_on+lvl);
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
    clk_low;
    /* chop off LSB of input data*/
    /* and set sig accordingly*/
    data % 2 == 1 ? sig_high : sig_low;
    
    /*shift data to the right by one bit*/
    data >>= 1;
   
    delay(tic);
   /* perform a toc */
    clk_high;
    delay(tic);
    
    cnt--;
  }
}

/* wait for acknowledgment */
void await_ack()
{
  clk_low;
  sig_low;
  delay(tic);
  delay(tic);
  clk_high;
  delay(tic);
}

/* SEGMENT CONTROL BY ADDRESS */
/* turns on the desired segments by the desired address*/
void seg_on(uint8_t num, uint8_t addr)
{
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

/* DATA OUTPUT TO DISPLAY*/
/* display decimal number*/
void disp_d(uint16_t data)
{
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



