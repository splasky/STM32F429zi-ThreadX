
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_azure_rtos.c
  * @author  MCD Application Team
  * @brief   azure_rtos application implementation file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "app_azure_rtos.h"
#include "stm32f429i_discovery.h"
#include "../Components/ili9341/ili9341.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN TX_Pool_Buffer */
/* USER CODE END TX_Pool_Buffer */
static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE];
static TX_BYTE_POOL tx_app_byte_pool;
static uint16_t red_pixel __attribute__ ((aligned (4))) = 0xF800U;
static uint16_t purple_pixel __attribute__ ((aligned (4))) = 0x8010U;
#define DISPLAY_THREAD_STACK_SIZE 1024        // Stack size for the display thread
#define DISPLAY_THREAD_PRIORITY   10          // Priority for the display thread
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
// ThreadX thread control block and stack
extern LTDC_HandleTypeDef hltdc;
TX_THREAD display_thread;
UCHAR display_thread_stack[DISPLAY_THREAD_STACK_SIZE];

// ThreadX thread entry function prototype
void display_thread_entry(ULONG thread_input);

/* USER CODE END PFP */
/**
  * @brief   Main application thread entry point.
  *          This function contains the core logic for LCD configuration and color cycling.
  * @param  thread_input: Input parameter, not used in this example.
  * @retval None
  */
void display_thread_entry(ULONG thread_input)
{
  UNUSED(thread_input); // To avoid unused parameter warning

  /* Infinite loop for alternating colors on the display */
  while (1)
  {
    /* Display Red color: Update Layer 0's frame buffer address (without immediate reload) */
    HAL_LTDC_SetAddress_NoReload(&hltdc, (uint32_t)&red_pixel, 0);
    /* Ask for LTDC reload to apply the above change. Reload will occur during the next vertical blanking. */
    HAL_LTDC_Reload(&hltdc, LTDC_SRCR_VBR);
    HAL_Delay(5000); // Apply the required 0.5ms (500 microseconds) delay using busy-wait

    /* Display Purple color: Update Layer 0's frame buffer address (without immediate reload) */
    HAL_LTDC_SetAddress_NoReload(&hltdc, (uint32_t)&purple_pixel, 0);

    /* Ask for LTDC reload during the next vertical blanking period */
    HAL_LTDC_Reload(&hltdc, LTDC_SRCR_VBR);
    HAL_Delay(5000); // Apply the required 0.5ms (500 microseconds) delay using busy-wait
  }
}

/**
  * @brief  Define the initial system.
  * @param  first_unused_memory : Pointer to the first unused memory
  * @retval None
  */
VOID tx_application_define(VOID *first_unused_memory)
{
  /* USER CODE BEGIN  tx_application_define */

  /* USER CODE END  tx_application_define */

  VOID *memory_ptr;

  if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    /* USER CODE BEGIN TX_Byte_Pool_Error */

    /* USER CODE END TX_Byte_Pool_Error */
  }
  else
  {
    /* USER CODE BEGIN TX_Byte_Pool_Success */

    /* USER CODE END TX_Byte_Pool_Success */

    memory_ptr = (VOID *)&tx_app_byte_pool;

    if (App_ThreadX_Init(memory_ptr) != TX_SUCCESS)
    {
      /* USER CODE BEGIN  App_ThreadX_Init_Error */

      /* USER CODE END  App_ThreadX_Init_Error */
    }


    /* USER CODE BEGIN  App_ThreadX_Init_Success */
    tx_thread_create(&display_thread,
                      "Display Thread",          // Thread name
                      display_thread_entry,      // Entry function
                      0,                         // Entry input parameter (0 in this case)
                      display_thread_stack,      // Stack pointer
                      DISPLAY_THREAD_STACK_SIZE, // Stack size
                      DISPLAY_THREAD_PRIORITY,   // Priority (lower value = higher priority)
                      DISPLAY_THREAD_PRIORITY,   // Preemption threshold (same as priority for no preemption)
                      TX_NO_TIME_SLICE,          // No time slicing for this thread
                      TX_AUTO_START);            // Start the thread automatically
    /* USER CODE END  App_ThreadX_Init_Success */

  }

}

/* USER CODE BEGIN  0 */

/* USER CODE END  0 */
