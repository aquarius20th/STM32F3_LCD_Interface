/*
   FILE          : lcd.c
   PROJECT       : Stm32f3 Discovery Board/Linux/ HD44780 based LCD display
   PROGRAMMER    : Rohit Bhardwaj
   DESCRIPTION   : Commands present and work to create initialize the display, and show/clears user supplied message
   
	The program make use of HAL(Hardware Abstraction Layer) which is C code that implements basic drivers for all the peripherals 
	in the STM32 Family. It makes the code more portable over STM32 Family.
*/


#include <stdint.h>
#include <stdio.h>
#include "stm32f3xx_hal.h"
#include "common.h"


void functionCmd(uint32_t pin, uint32_t state);

// FUNCTION      : gpioinitLCD()
// DESCRIPTION   : The function initialize the GPIO Pins for LCD display
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void gpioinitLCD(int mode)
{
/* Turn on clocks to I/O */
__GPIOD_CLK_ENABLE();

/* Configure GPIO pins */
GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitStruct.Pin = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 
                       | GPIO_PIN_6 | GPIO_PIN_7  | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


__GPIOA_CLK_ENABLE();

//GPIO_InitTypeDef  GPIO_InitStruct;
GPIO_InitStruct.Pin = (GPIO_PIN_10);
GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
GPIO_InitStruct.Alternate = 0;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,1);
return;
}

ADD_CMD("gpioinitLCD",gpioinitLCD,"              Initialize GPIO Pins for LCD");


void delay1(int tick)
{
   myTickCount= 0;
   while(myTickCount < tick)
   {
    asm volatile ("nop\n");
   }
}

// FUNCTION      : delay()
// DESCRIPTION   : The function provides delay in millisec
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void delay(int tick1)
{
  for(int data = 0; data < tick1; data++)
  {
    asm volatile ("nop\n");
   }
}

// FUNCTION      : lcdinit()
// DESCRIPTION   : The function intialize the LCD display
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void lcdinit(int mode)
{
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,0);                     //RS   0 means command mode

  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,0);                      //RW   0 means write mode
  
  delay(100);

  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);                      //Enable

  
  delay(300);
  //initilaze the lcd in 16*2 mode
  functionCmd(0,0);
  functionCmd(1,0);
  functionCmd(2,1);
  functionCmd(3,1);
  functionCmd(4,1);
  functionCmd(5,1);
  functionCmd(6,0);
  functionCmd(7,0);

  //latch
  delay(100);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,0);                      //Enable
  delay(500);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1); 
  delay1(5);

  //clear the lcd and set the DDRAM address 0 in address counter
  functionCmd(0,1);
  functionCmd(1,0);
  functionCmd(2,0);
  functionCmd(3,0);
  functionCmd(4,0);
  functionCmd(5,0);
  functionCmd(6,0);
  functionCmd(7,0);

   //latch                  
  delay(100);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,0); 
  delay(500);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);
  delay1(5);

  //Display On/Off and display cursor and make it blink
  functionCmd(0,1);
  functionCmd(1,1);
  functionCmd(2,1);
  functionCmd(3,1);
  functionCmd(4,0);
  functionCmd(5,0);
  functionCmd(6,0);
  functionCmd(7,0);

  //latch
  delay(100);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,0); 
  delay(500);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);
  delay1(5);

  //Enter Mode Set
  functionCmd(0,0);
  functionCmd(1,1);
  functionCmd(2,1);
  functionCmd(3,0);
  functionCmd(4,0);
  functionCmd(5,0);
  functionCmd(6,0);
  functionCmd(7,0);

  //latch
  delay(100);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,0); 
  delay(500);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);
  
}

ADD_CMD("lcdinit",lcdinit,"              Initialize the LCD");


// FUNCTION      : clearLCD()
// DESCRIPTION   : The function clears the LCD display
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void clearLCD(int mode)
{
  if(mode != CMD_INTERACTIVE)
  {
    return;
  }
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,0);                     //RS   0 means command mode

  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,0);                      //RW   0 means write mode
  
  delay(100);

  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);                      

  delay(300); 
 
  //clear the lcd and set the DDRAM address 0 in address counter
  functionCmd(0,1);
  functionCmd(1,0);
  functionCmd(2,0);
  functionCmd(3,0);
  functionCmd(4,0);
  functionCmd(5,0);
  functionCmd(6,0);
  functionCmd(7,0);

  //latch                  
  delay(100);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,0); 
  delay(500);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);
  delay1(5);
  
}

ADD_CMD("clearlcd",clearLCD,"            clears the LCD Screen");


// FUNCTION      : lcd()
// DESCRIPTION   : The function display user supplied message on the LCD display 
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void lcd(int mode)
{

  uint32_t data;
  char *message;
  fetch_uint32_arg(&data);
  fetch_string_arg(&message);

  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,data);                     

  uint32_t temp;

  for(int k=0;message[k];k++)
  {
   temp = GPIOD->ODR;
   temp &= ~0xff;
   temp |= message[k];
   GPIOD->ODR = temp; 
   delay(100);
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,0); 
   delay(500);
   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,1);
   }
}

ADD_CMD("lcd",lcd,"              lcd<register><message>");


// FUNCTION      : functionCmd()
// DESCRIPTION   : The function set/clears the gpio pins
// PARAMETERS    : The function accepts an int value
// RETURNS       : returns nothing
void functionCmd(uint32_t pin, uint32_t state)
{
  switch(pin)
  {
    case 0:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_0,state);
    break;

    case 1:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_1,state);
    break;

    case 2:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,state);
    break;

    case 3:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,state);
    break;

    case 4:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_4,state);
    break;

    case 5:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_5,state);
    break;

    case 6:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_6,state);
    break;

    case 7:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_7,state);
    break;

    case 9:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,state);
    break;

    case 10:
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_10,state);
    break;

    default:
    printf("Enter between 0-10\n");
    break;
  }
}

