/**
  ******************************************************************************
  * @file project\main.c
  * @brief This file contains the firmware main function.
  * @author STMicroelectronics - MCD Application Team
  * @version V1.1.1
  * @date 06/05/2009
  ******************************************************************************
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  * @image html logo.bmp
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "atom.h"
#include "atomport-private.h"
#include "atomtimer.h"
#include "uart.h"
#include "stm8s.h"

/* Private defines -----------------------------------------------------------*/
#define TESTS_LOG_STACK_USAGE
#define IDLE_STACK_SIZE_BYTES       128
#define MAIN_STACK_SIZE_BYTES       384
/* Local data */
/* Application threads' TCBs */
static ATOM_TCB main_tcb;
/* Main thread's stack area (large so place outside of the small page0 area on STM8) */
NEAR static uint8_t main_thread_stack[MAIN_STACK_SIZE_BYTES];
/* Idle thread's stack area (large so place outside of the small page0 area on STM8) */
NEAR static uint8_t idle_thread_stack[IDLE_STACK_SIZE_BYTES];
/* Private function prototypes -----------------------------------------------*/
void HSE_CLK_Init(void);
static void main_thread_func (uint32_t param);
/* Private functions ---------------------------------------------------------*/

/* Forward declarations */
static void main_thread_func (uint32_t param)
{
    uint32_t test_status;
    int sleep_ticks;

    /* Compiler warnings */
    param = param;

    /* Initialise UART (9600bps) */
    if (uart_init(9600) != 0)
    {
        /* Error initialising UART */
    }

    /* Put a message out on the UART */
    printf ("Go\n");
    test_status = 0;
    /* Check main thread stack usage (if enabled) */
#ifdef ATOM_STACK_CHECKING
    if (test_status == 0)
    {
        uint32_t used_bytes, free_bytes;

        /* Check idle thread stack usage */
        if (atomThreadStackCheck (&main_tcb, &used_bytes, &free_bytes) == ATOM_OK)
        {
            /* Check the thread did not use up to the end of stack */
            if (free_bytes == 0)
            {
                printf ("Main stack overflow\n");
                test_status++;
            }
#endif
            /* Log the stack usage */
#ifdef TESTS_LOG_STACK_USAGE
            printf ("MainUse:%d\n", (int)used_bytes);
#endif
        }
    }
    /* Log final status */
    if (test_status == 0)
    {
        printf ("Pass\n");
    }
    else
    {
        printf ("Fail(%d)\n", (int)test_status);
    }

    /* Flash LED once per second if passed, very quickly if failed */
    sleep_ticks = SYSTEM_TICKS_PER_SEC;

    /* Configure GPIO for flashing the STM8S Discovery LED on GPIO D0 */
    GPIO_DeInit(GPIOE);
    GPIO_Init(GPIOE, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);

    /* Test finished, flash slowly for pass, fast for fail */
    while (1)
    {
        /* Toggle LED on pin D0 (Discovery-specific) */
        GPIO_WriteReverse(GPIOE, GPIO_PIN_5);
        printf ("GPIO_WriteReverse(GPIOE, GPIO_PIN_5);\n");
        /* Sleep then toggle LED again */
        atomTimerDelay (sleep_ticks);
    }
}


void main(void)
{
  int8_t status;
  HSE_CLK_Init();
  /* Initialise the OS before creating our threads */
  status = atomOSInit(&idle_thread_stack[0], IDLE_STACK_SIZE_BYTES, TRUE);
  /* Infinite loop */
  if (status == ATOM_OK)
  {
    /* Enable the system tick timer */
    archInitSystemTickTimer();

    /* Create an application thread */
    status = atomThreadCreate(&main_tcb, 10, main_thread_func, 0, &main_thread_stack[0], MAIN_STACK_SIZE_BYTES, TRUE);
    if (status == ATOM_OK)
    {
        atomOSStart();
    }
  }
}

void HSE_CLK_Init(void)
{
    CLK_DeInit();
    CLK_HSECmd(ENABLE);
    while(SET != CLK_GetFlagStatus(CLK_FLAG_HSERDY));
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    CLK_ClockSwitchCmd(ENABLE); 
    CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO , CLK_SOURCE_HSE , DISABLE ,CLK_CURRENTCLOCKSTATE_ENABLE);
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
