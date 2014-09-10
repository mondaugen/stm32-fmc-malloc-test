/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h> 
#include "stm32f4xx_conf.h" 
#include "stm32f429-leds.h" 
#include "fmc.h" 

#define ALLOC_SIZE          0x200000
#define BUFFER_SIZE         (ALLOC_SIZE / sizeof(uint32_t))
#define WRITE_READ_ADDR     ((uint32_t)0x0)
/* Read/Write Buffers */
uint32_t aTxBuffer[BUFFER_SIZE];
uint32_t aRxBuffer[BUFFER_SIZE];
/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;
/* Counter index */
uint32_t uwIndex = 0;
/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);

void Delay(__IO uint32_t nCount);

int main(void)
{
  stmeval_leds_setup();

  /* Configure FMC */
  FMC_Config();
  /* Fill buffer to write */
/*   Fill_Buffer(aTxBuffer, BUFFER_SIZE, 0x250F);    */
  /* Allocate memory on heap */
  uint32_t sizeDec = 20;
  uint32_t *heapMemory[16];
  memset(heapMemory,NULL,sizeof(heapMemory));
  /* doing this doesn't make the heap smaller right? */
  uint32_t stackFiller[25000];
  /* no it doesn't */
  stmeval_green_led_on();
  while (1) {
      size_t idx = lrand48() % 16;
      size_t allocSize = lrand48() % ALLOC_SIZE;
      free(heapMemory[idx]);
      heapMemory[idx] = NULL;
      heapMemory[idx] = (uint32_t*)malloc(allocSize);
      if (!heapMemory[idx]) {
          stmeval_green_led_off();
          stmeval_red_led_on();
      } else {
          uint32_t i;
          for (i = 0; i < (allocSize / sizeof(uint32_t)); i++) {
              heapMemory[idx][i] = i;
          }
          for (i = 0; i < (allocSize / sizeof(uint32_t)); i++) {
              if (heapMemory[idx][i] != i) {
                  stmeval_green_led_off();
                  stmeval_red_led_on();
                  while(1);
              }
          }
          stmeval_green_led_on();
          stmeval_red_led_off();
      }
      Delay(1000000);
  }

  while (!heapMemory);

  stmeval_red_led_off();
  while(1);
  
  /* Write data to the SDRAM memory */
  for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++)
  {
      heapMemory[uwIndex] = uwIndex;
/*     *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex) = aTxBuffer[uwIndex]; */
  }    
  /* Read back data from the SDRAM memory */
/*   for (uwIndex = 0; uwIndex < BUFFER_SIZE; uwIndex++) */
/*   { */
/*       aRxBuffer[uwIndex] = heapMemory[uwIndex]; */
/*     aRxBuffer[uwIndex] = *(__IO uint32_t*) (SDRAM_BANK_ADDR + WRITE_READ_ADDR + 4*uwIndex); */
/*   }  */
  /* Check the SDRAM memory content correctness */   
  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
  {
    if (heapMemory[uwIndex] != uwIndex)
    {
      uwWriteReadStatus++;
    }
  }	

  if (uwWriteReadStatus)
  {
    /* KO */
    /* Turn on red LED */
    stmeval_red_led_on();     
  }
  else
  { 
    /* OK */
    /* Turn on green LED */
    stmeval_green_led_on();     
  }


  while (1)
  {
  }
}

/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}
