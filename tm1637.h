#include <stdint.h>
#include "stm32f4xx.h"

/*========================================================================================================*/

#define write_to_fixed_addr      0x44    /* set write to fixed address command */   

#define grid_1          0xC0    /* grid 1 address*/
#define grid_2          0xC1    /* grid 2 address*/
#define grid_3          0xC2    /* grid 3 address*/
#define grid_4          0xC3    /* grid 4 address*/
#define grid_5          0xC4    /* grid 5 address*/
#define grid_6          0xC5    /* grid 6 address*/

#define digit_num       4       /* 4-digit display connected to tm1637*/

#define max_data        9999    /* maximum number 4-digit display can handle*/

typedef struct 
{
  
  GPIO_TypeDef*  sig_port;      /* signal port */
  
  GPIO_TypeDef*  clk_port;      /* clock port */
  
  uint16_t      sig_pin;        /* signal pin */
  
  uint16_t      clk_pin;        /* clock pin */
    
  uint16_t      tic;            /* clocking half-period*/
  
  uint8_t       brightness;     /* brightness setting */
  
  uint8_t       separator;      /* separator on/off */
  
} TM1637_display_type;
/*========================================================================================================*/

/*INITIALISATION */
/* display instance init*/
void TM1637_init(TM1637_display_type display, TIM_HandleTypeDef tm);

/* DATA OUTPUT TO DISPLAY*/
/* display decimal number*/
void TM1637_disp_d(uint16_t data);

/*========================================================================================================*/

/* PRIVATE AUXILIARY FUNCTIONS */
/* convert human pin number to bit field definition given in stm32f4xx_hal_gpio.h
    to pass to GPIO init functions*/
uint16_t get_gpio_pin(uint8_t num);

/* pin control */
void sig_high();
void sig_low();
void clk_high();
void clk_low();

/* refresh display - reset all grid segments*/
void refresh_disp();

/* microsecond delay function based on TIM timer*/
void delay(uint16_t dt);

/* convert a digit into segment code g f e d c b a  style*/
uint8_t digit_to_hex(uint8_t digit);

/* display 'Err' */
void disp_err();

/* display separator between grid #2 and #3*/
void disp_separator();

/* INTERFACE COMMAND FUNCTIONS */
/* start command sequence */
void cmd_seq_start();

/* end command sequence */
void cmd_seq_end();

/* set brightness of the display */
void set_brightness(uint8_t lvl);

/* send command - write one byte of data to sig, bit per tic*/
void tx_cmd(uint8_t data);

/* wait for acknowledgment */
void await_ack();

/* SEGMENT CONTROL BY ADDRESS */
/* turns on the desired segments by the desired address*/
void seg_on(uint8_t num, uint8_t addr);




