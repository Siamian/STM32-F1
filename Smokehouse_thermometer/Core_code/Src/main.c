/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "i2c-lcd.h"
#include "wire.h"
#include "ds18b20.h"
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
volatile char tab[3];
volatile char znak = ' ';
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
int __io_putchar(int ch) //funkcja do poprawnego wyświetlania printf
{
	if (ch == '\n') {
		uint8_t ch2 = '\r';
		HAL_UART_Transmit(&huart1, &ch2, 1, HAL_MAX_DELAY);
	}

	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, HAL_MAX_DELAY);
	return 1;
}
*/

void convert_int_to_char(int value) //konwersja temp z int do char
{
	int temp = value;

	if(value<0){
		temp = temp*-1;
		znak = '-';
	}else
		znak = ' ';

	int temp_100 = temp / 100;
	int temp_10 = (temp - (temp_100 * 100)) / 10;
	int temp_1 = temp - (temp_100 * 100) - (temp_10 * 10);

	tab[0] = (char)temp_100 + '0';
	tab[1] = (char)temp_10 + '0';
	tab[2] = (char)temp_1 + '0';
}


void display_no_zero() //wyswietla temp na LCD bez zer z przodu
{
		if(tab[0]== '0')
			if(tab[1]== '0'){
				tab[0] = ' ';
				tab[1] = ' ';
			}else
				tab[0] = ' ';
}


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
	int temp1 = 0;
	int temp2 = 0;
	int temp3 = 0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */


  const uint8_t ds1[] = { 0x28, 0x67, 0x84, 0xe9, 0xc, 0x0, 0x0, 0xed};
  const uint8_t ds2[] = { 0x28, 0xaa, 0xbf, 0xe9, 0xc, 0x0, 0x0, 0xcc};
  const uint8_t ds3[] = { 0x28, 0xd, 0xa9, 0x71, 0xa, 0x0, 0x0, 0xd9};

  //w pobranym nowym adresie należy odjąć 1 od pierwszego elementu w tablicy, reszta jest ok
  //const uint8_t nowy_adres[] = { 0x0, 0x0, 0xa0, 0x0, 0x0, 0x0, 0x0, 0x0};
  //ds18b20_read_address(nowy_adres);

	lcd_init();
	HAL_Delay(1000);
	lcd_send_string("Witaj");
	lcd_put_cur(1, 0);
	lcd_send_string("Termometr ON");
	HAL_Delay(1000);
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Loading..");
	HAL_Delay(500);
	lcd_clear();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		  ds18b20_start_measure(ds1);
		  ds18b20_start_measure(ds2);
		  ds18b20_start_measure(ds3);

		  HAL_Delay(750);

		  float temp = ds18b20_get_temp(ds1);
		  if (temp <= -80.0f)
		    printf("Sensor error (1)...\n");
		  else if(temp != 85.0f && temp < 200.0f)
			  temp1 = (int)temp;
		  else if(temp > 200.0f)
			  temp1 = 0;

		  temp = ds18b20_get_temp(ds2);
		  if (temp <= -80.0f)
		    printf("Sensor error (2)...\n");
		  else if(temp != 85.0f && temp < 200.0f)
			  temp2 = (int)temp;
		  else if(temp > 200.0f)
			  temp2 = 0;

		  temp = ds18b20_get_temp(ds3);
		  if (temp <= -80.0f)
		    printf("Sensor error (3)...\n");
		  else if(temp != 85.0f && temp < 200.0f)
			  temp3 = (int)temp;
		  else if(temp > 200.0f)
			  temp3 = 0;

		//Wyświetl temperature czujnika 1
		lcd_put_cur(0, 0);
		lcd_send_string("T1: ");
		convert_int_to_char(temp1);
		lcd_put_cur(0, 3);
		lcd_send_data (znak);
		lcd_put_cur(0, 4);
		display_no_zero();
		lcd_send_string((char*)&tab[0]);

		//Wyświetl temperature czujnika 2
		lcd_put_cur(0, 9);
		lcd_send_string("T2: ");
		convert_int_to_char(temp2);
		lcd_put_cur(0, 12);
		lcd_send_data (znak);
		lcd_put_cur(0, 13);
		display_no_zero();
		lcd_send_string((char*)&tab[0]);

		//Wyświetl temperature czujnika 3
		lcd_put_cur(1, 0);
		lcd_send_string("T3: ");
		convert_int_to_char(temp3);
		lcd_put_cur(1, 3);
		lcd_send_data (znak);
		lcd_put_cur(1, 4);
		display_no_zero();
		lcd_send_string((char*)&tab[0]);

		//Wyświetl jednostkę temperatury
		lcd_put_cur(1, 12);
		lcd_send_string("['C]");

		//Wyświetl w terminalu BR 9600
		//printf("T1 = %d'C\t", temp1);
		//printf("T2 = %d'C\t", temp2);
		//printf("T3 = %d'C\n", temp3);



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_HalfDuplex_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

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
	while (1) {
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
