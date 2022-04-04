#include <stdint.h>
#include "stm32f4xx.h"

/*========================================================================================================*/

#define sig_pin         GPIO_PIN_1      /* define signal pin*/
#define sig_port        GPIOA           /* and corresponding port*/

#define clk_pin         GPIO_PIN_1      /* define clock pin*/
#define clk_port        GPIOB           /* and corresponding port*/

#define sig_high        HAL_GPIO_WritePin(sig_port, sig_pin, GPIO_PIN_SET)      /* set signal pin HIGH*/
#define sig_low         HAL_GPIO_WritePin(sig_port, sig_pin, GPIO_PIN_RESET)    /* set signal pin LOW*/

#define clk_high        HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_SET)      /* set clock pin HIGH*/
#define clk_low         HAL_GPIO_WritePin(clk_port, clk_pin, GPIO_PIN_RESET)    /* set clock pin LOW*/

#define tic             5       /* clocking half period in microseconds*/

#define disp_off        0x80    /* display off setting*/
#define disp_on         0x88    /* display on setting, initial zero brightness*/

#define write_to_fixed_addr      0x44    /* set write to fixed address command */   

#define grid_1          0xC0    /* grid 1 address*/
#define grid_2          0xC1    /* grid 2 address*/
#define grid_3          0xC2    /* grid 3 address*/
#define grid_4          0xC3    /* grid 4 address*/
#define grid_5          0xC4    /* grid 5 address*/
#define grid_6          0xC5    /* grid 6 address*/

#define digit_num       4       /* 4-digit display connected to tm1637*/

#define max_data        9999    /* maximum number 4-digit display can handle*/
/*========================================================================================================*/

/*INITIALISATION FUNCTIONS */
/* disp initial reset to 0 */
void disp_init();

/* GPIO pins initialisation */
void gpio_init(void);

/* TIM start*/
void tm_start(TIM_HandleTypeDef tm);

/* PRIVATE AUXILIARY FUNCTIONS */
/* refresh display - reset all grid segments*/
void refresh_disp();

/* microsecond delay function based on TIM timer*/
void delay(uint16_t dt);

/* convert a digit into segment code g f e d c b a  style*/
uint8_t digit_to_hex(uint8_t digit);

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

/* DATA OUTPUT TO DISPLAY*/
/* display decimal number*/
void disp_d(uint16_t data);

/* display 'Err' */
void disp_err();

/* display separator between grid #2 and #3*/
void disp_separator();
