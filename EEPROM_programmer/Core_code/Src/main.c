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
//Niedziała dla adresów ff+
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define eeprom24C16_ADDR			0xa0
#define max_address_mem			    0x7ff
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void send_char(char c) {
	HAL_UART_Transmit(&huart2, (uint8_t*) &c, 1, 1000);
}

int __io_putchar(int ch) {
	if (ch == '\n')
		send_char('\r');
	send_char(ch);
	return ch;
}

void infoMenu() {
	printf(
			"\n\tWybierz:\n\t1 - Odczytaj adres ADMO (BLACK)\n\t2 - Wgraj nowy adres ADMO (BLACK)\n");
	printf(
			"\t3 - Wyswietl cala zawartosc pamieci EEPROM 24LC16BI_SN\n\t4 - Usun cala zawartosc pamieci EEPROM 24LC16BI_SN\n\n");
}

char scan()
{
	char choice_scan = '0';
	while (choice_scan == '0') {
		if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) == SET)
			HAL_UART_Receive(&huart2, (uint8_t*) &choice_scan, 1, 100);
	}
	return choice_scan;
}

char check()
{
	printf("Nacisnij 'y' aby potwierdzic wybor, lub 'q'  aby anulowac\n");
	char choice_check = '0';
	while (choice_check != 'y'  && choice_check != 'q') {
	choice_check = scan();
	}
	if(choice_check == 'q')
		printf("ANULOWANO\n");
	return choice_check;
}

void readADMOBlack() {
	uint16_t reg = 0;
	uint8_t value = 0;
	uint8_t tab_value[12] = {0};
	int i=0;
	int check_addres = 0;

	while (reg <= 11) {
		HAL_I2C_Mem_Read(&hi2c2, eeprom24C16_ADDR, reg, 1, &value,
				sizeof(value), HAL_MAX_DELAY);
		HAL_Delay(4);

		tab_value[i] = value;
		i++;
/*
		if (reg == 0x06)
			printf("Dopelnienie do 9:\n");

		//printf("Komorka: 0x%02X, wartosc: 0x%02X\n", reg, value); //w hex
		printf("Komorka: 0x%02X, wartosc: %d\n", reg, value);  // w dec
*/
			reg++;
	}

	if((tab_value[0]-'0')+(tab_value[6]-'0') == 9)
		if((tab_value[1]-'0')+(tab_value[7]-'0') == 9)
				if((tab_value[2]-'0')+(tab_value[8]-'0') == 9)
						if((tab_value[3]-'0')+(tab_value[9]-'0') == 9)
								if((tab_value[4]-'0')+(tab_value[10]-'0') == 9)
										if((tab_value[5]-'0')+(tab_value[11]-'0') == 9)
												check_addres = 1;

	if(check_addres == 1)
		printf("\tOdczytany adres ADMO: %d%d%d%d%d%d\n",tab_value[0]-'0',tab_value[1]-'0',tab_value[2]-'0',tab_value[3]-'0',tab_value[4]-'0',tab_value[5]-'0');
	else
		printf("\tOdczytany adres ADMO jest nieprawidlowy !\n");
}

void readEEPROM() {
	uint16_t reg = 0x0000;
	uint8_t value = 0;

	while (reg <= max_address_mem) {

		HAL_I2C_Mem_Read(&hi2c2, eeprom24C16_ADDR, reg, 1, &value,
				sizeof(value), HAL_MAX_DELAY);
		HAL_Delay(4);
		printf("Komorka: 0x%02X, wartosc[HEX]: 0x%02X, wartosc[DEC]: %d\n", reg, value, value -'0');

			reg++;
	}
}

void clearEEPROM() {
	uint16_t reg = 0x000;
	uint8_t val = 0xff;

	while (reg <= max_address_mem) {
		HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, reg, 1, &val, sizeof(val),
				HAL_MAX_DELAY);
		HAL_Delay(4);

			reg++;
	}
}

void writeAdmo() {

	uint8_t address[6]={0};

	printf("Wpisz 6 cyfr adresu ADMO:\n ");

	for(int i = 0; i<=5;i++){
		address[i] = scan();
		printf("%d ",address[i]-48);
	}

	// Pierwsza cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x00, 1, &address[0],
			sizeof(address[0]), HAL_MAX_DELAY);
	HAL_Delay(10);
	address[0] = 0x39 - (address[0] - 0x30);
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x06, 1, &address[0],
			sizeof(address[0]), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Druga cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x01, 1, &address[1],
			sizeof(address[1]), HAL_MAX_DELAY);
	HAL_Delay(10);
	address[1] = 0x39 - (address[1] - 0x30);
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x07, 1, &address[1],
			sizeof(address[1]), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Trzecia cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x02, 1, &address[2],
			sizeof(address[2]), HAL_MAX_DELAY);
	HAL_Delay(10);
	address[2] = 0x39 - (address[2] - 0x30);
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x08, 1, &address[2],
			sizeof(address[2]), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Czwarta cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x03, 1, &address[3],
			sizeof(address[3]), HAL_MAX_DELAY);
	HAL_Delay(10);
	address[3] = 0x39 - (address[3] - 0x30);
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x09, 1, &address[3],
			sizeof(address[3]), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Piąta cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x04, 1, &address[4],
			sizeof(address[4]), HAL_MAX_DELAY);
	HAL_Delay(10);
	address[4] = 0x39 - (address[4] - 0x30);
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x0a, 1, &address[4],
			sizeof(address[4]), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Szosta cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x05, 1, &address[5],
			sizeof(address[5]), HAL_MAX_DELAY);
	HAL_Delay(10);
	address[5] = 0x39 - (address[5] - 0x30);
	HAL_I2C_Mem_Write(&hi2c2, eeprom24C16_ADDR, 0x0b, 1, &address[5],
			sizeof(address[5]), HAL_MAX_DELAY);
	HAL_Delay(10);
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */
	char choice = '0';
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
	MX_I2C2_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */

	infoMenu();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		choice = scan();

		switch (choice) {
		case '1':
			printf("Odczytywanie adresu ADMO (BLACK)...\n");
			choice = check();
			if(choice == 'y'){
				readADMOBlack();
				printf("Zakonczono odczytywanie adresu ADMO (BLACK).\n");
			}

			break;
		case '2':
			printf("Wgrywanie nowego adresu ADMO (BLACK)...\n");
			choice = check();
			if(choice == 'y'){
				writeAdmo();
				printf("\nZakonczono wgrywanie nowego adresu ADMO (BLACK).\n");
			}
			break;
		case '3':
			printf("Wyswietlanie calej zawartosci pamieci EEPROM 24LC16BI_SN...\n");
			choice = check();
			if(choice == 'y'){
				readEEPROM();
				printf("Zakonczono wyswietlanie calej zawartosci pamieci EEPROM 24LC16BI_SN.\n");
			}
				break;
		case '4':
			printf("Usuwanie calej zawartosci pamieci EEPROM 24LC16BI_SN...\n");
			choice = check();
			if(choice == 'y'){
				clearEEPROM();
				printf("Zakonczono usuwanie calej zawartosci pamieci EEPROM 24LC16BI_SN.\n");
			}
				break;
		default:
			printf("Wybrano nieprawidlowy klawisz\n");
			break;
		}

		infoMenu();

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C2_Init(void) {

	/* USER CODE BEGIN I2C2_Init 0 */

	/* USER CODE END I2C2_Init 0 */

	/* USER CODE BEGIN I2C2_Init 1 */

	/* USER CODE END I2C2_Init 1 */
	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 100000;
	hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c2.Init.OwnAddress1 = 0;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c2.Init.OwnAddress2 = 0;
	hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C2_Init 2 */

	/* USER CODE END I2C2_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
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
