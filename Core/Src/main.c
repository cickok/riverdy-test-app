/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "platform.h"
#include "App_Common.h"

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

/* USER CODE BEGIN PV */
/* global used for buffer optimization */
Gpu_Hal_Context_t host, *phost;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void SAMAPP_GPU_Ball_Stencil();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
	phost = &host;
	/* Init HW Hal */
	App_Common_Init(&host);

	/* Screen Calibration*/
//	App_Calibrate_Screen(&host);
	/* Show Bridgetek logo */
	App_Show_Logo(&host);

	/* Main application - endless loop */
	SAMAPP_GPU_Ball_Stencil();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_TogglePin(USR_LED_1_GPIO_Port, USR_LED_1_Pin);
	  HAL_Delay(100);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* Close all the opened handles */
  Gpu_Hal_Close(phost);
  Gpu_Hal_DeInit();
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static void
SAMAPP_GPU_Ball_Stencil()
{
  uint8_t xflag, yflag;
  int16_t xball, yball, rball, pixel_precision, gridsize;
  int32_t displ, dispr, dispa, dispb;

  /* grid margins */
  displ = 10;
  dispr = (DispWidth - 10);
  dispa = 50;
  dispb = (DispHeight - 10);

  /* grid size */
  gridsize = 20;

  /* ball dimensions */
  xball = (DispWidth/2);
  yball = (DispHeight/2);
  rball = (DispWidth/8);
  xflag = 1;
  yflag = 1;

  dispr -= ((dispr - displ)%gridsize);
  dispb -= ((dispb - dispa)%gridsize);

  /* endless loop */
  while(1)
    {
      /* ball movement */
      if(((xball + rball + 2) >= dispr) || ((xball - rball - 2) <= displ))
        xflag ^= 1;

      if(((yball + rball + 8) >= dispb) || ((yball - rball - 8) <= dispa))
        yflag ^= 1;

      if(xflag)
        xball += 2;
      else
        xball -= 2;

      if(yflag)
        yball += 8;
      else
        yball -= 8;

      /* set the precision of VERTEX2F coordinates */
#if defined (IPS_70) || (IPS_101)
      /* VERTEX2F range: -2048 to 2047 */
      App_WrDl_Buffer(phost, VERTEX_FORMAT(3));
      pixel_precision = 8;
#else
      /* use default VERTEX_FORMAT(3) with VERTEX2F range: -1024 to 1023 */
      pixel_precision = 16;
#endif

      /* init and set background */
      App_WrDl_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
      App_WrDl_Buffer(phost, CLEAR(1, 1, 1));
      App_WrDl_Buffer(phost, STENCIL_OP(INCR,INCR));
      App_WrDl_Buffer(phost, COLOR_RGB(0, 0, 0));

      /* draw grid */
      App_WrDl_Buffer(phost, LINE_WIDTH(pixel_precision));
      App_WrDl_Buffer(phost, BEGIN(LINES));

      for(uint16_t i=0; i<=((dispr - displ)/gridsize); i++)
        {
          App_WrDl_Buffer(phost, VERTEX2F((displ + i*gridsize)*pixel_precision,dispa*pixel_precision));
          App_WrDl_Buffer(phost, VERTEX2F((displ + i*gridsize)*pixel_precision,dispb*pixel_precision));
        }

      for(uint16_t i=0; i<=((dispb - dispa)/gridsize); i++)
        {
          App_WrDl_Buffer(phost, VERTEX2F(displ*pixel_precision,(dispa + i*gridsize)*pixel_precision));
          App_WrDl_Buffer(phost, VERTEX2F(dispr*pixel_precision,(dispa + i*gridsize)*pixel_precision));
        }
      App_WrDl_Buffer(phost, END());

      /* add simple text using built-in fonts */
      {
        Gpu_Fonts_t   font;
        uint8_t       font_size;
        uint32_t      font_table;
        uint32_t      text_hoffset, text_voffset;

#if defined (NTP_35) || (RTP_35) || (CTP_35) || (IPS_35) || (NTP_43) || (RTP_43) || (CTP_43) || (IPS_43)
        const uint8_t text[] = "Riverdi EVE Demo";
#elif defined (NTP_50) || (RTP_50) || (CTP_50) || (IPS_50) || (NTP_70) || (RTP_70) || (CTP_70) || (IPS_70)
        const uint8_t text[] = "Riverdi EVE Demo - https://www.riverdi.com";
#elif defined (IPS_101)
        const uint8_t text[] = "Riverdi EVE Demo - https://www.riverdi.com - contact@riverdi.com";
#endif

        text_hoffset = displ; /* set the same offset like for grid */
        text_voffset = 5;

        font_size = 30;
        font_table = Gpu_Hal_Rd32(phost, ROMFONT_TABLEADDRESS);

        Gpu_Hal_RdMem(phost, (font_table + (font_size-16) * GPU_FONT_TABLE_SIZE),
                      (uint8_t*)&font, GPU_FONT_TABLE_SIZE);

        App_WrDl_Buffer(phost, COLOR_RGB(0, 96, 169));
        App_WrDl_Buffer(phost, BEGIN(BITMAPS));
        App_WrDl_Buffer(phost, BITMAP_HANDLE((font_size%32)));

        for (uint8_t cnt = 0; cnt < sizeof(text)-1; cnt++)
          {
            App_WrDl_Buffer(phost, CELL(text[cnt]));
            App_WrDl_Buffer(phost, VERTEX2F(text_hoffset*pixel_precision, text_voffset*pixel_precision));
            text_hoffset += font.FontWidth[text[cnt]];
          }
        App_WrDl_Buffer(phost, END());
      }

      /* draw ball and shadow */
      App_WrDl_Buffer(phost, COLOR_MASK(1,1,1,1));
      App_WrDl_Buffer(phost, STENCIL_FUNC(ALWAYS,1,255));
      App_WrDl_Buffer(phost, STENCIL_OP(KEEP,KEEP));
      App_WrDl_Buffer(phost, COLOR_RGB(255, 255, 255));
      App_WrDl_Buffer(phost, POINT_SIZE(rball*16));
      App_WrDl_Buffer(phost, BEGIN(FTPOINTS));
      App_WrDl_Buffer(phost, VERTEX2F((xball - 1)*pixel_precision,(yball - 1)*pixel_precision));
      App_WrDl_Buffer(phost, COLOR_RGB(0, 0, 0));
      App_WrDl_Buffer(phost, COLOR_A(160));
      App_WrDl_Buffer(phost, VERTEX2F((xball+pixel_precision)*pixel_precision,(yball+8)*pixel_precision));
      App_WrDl_Buffer(phost, COLOR_A(255));
      App_WrDl_Buffer(phost, COLOR_RGB(254, 172, 0));
      App_WrDl_Buffer(phost, VERTEX2F(xball*pixel_precision,yball*pixel_precision));
      App_WrDl_Buffer(phost, COLOR_RGB(255, 255, 255));
      App_WrDl_Buffer(phost, STENCIL_FUNC(GEQUAL,1,1));
      App_WrDl_Buffer(phost, STENCIL_OP(KEEP,KEEP));
      App_WrDl_Buffer(phost, VERTEX2F(xball*pixel_precision,yball*pixel_precision));
      App_WrDl_Buffer(phost, END());

      /* display */
      App_WrDl_Buffer(phost, DISPLAY());

      /* download display list into DL RAM */
      App_Flush_DL_Buffer(phost);

      /* do a swap */
      Gpu_Hal_DLSwap(phost, DLSWAP_FRAME);

    } /* while */
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
