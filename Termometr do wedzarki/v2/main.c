#include "stm32f1xx.h"
#include "i2c-lcd.h"
#include "ds18b20.h"
#include "wire.h"
#include <stdio.h>
#include <string.h>

#define SLAVE_ADDRESS_LCD	0x27
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef tim1;
UART_HandleTypeDef huart3;
UART_HandleTypeDef uart;
GPIO_InitTypeDef gpio;

void send_char(char c)
{
	HAL_UART_Transmit(&uart, (uint8_t*)&c, 1, 1000);
}

int __io_putchar(int ch)
{
	if (ch == '\n')
		send_char('\r');
	send_char(ch);
	return ch;
}

int main(void)
{
	SystemCoreClock = 8000000; // taktowanie 8Mhz
	HAL_Init();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE(); //pod³¹czenie zegara do TIM1
	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_2;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_INPUT;
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;		// SCL, SDA
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_10;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gpio);

	//gpio.Mode = GPIO_MODE_AF_INPUT;
	//gpio.Pin = GPIO_PIN_11;
	//HAL_GPIO_Init(GPIOB, &gpio);

	uart.Instance = USART2;
	uart.Init.BaudRate = 115200;
	uart.Init.WordLength = UART_WORDLENGTH_8B;
	uart.Init.Parity = UART_PARITY_NONE;
	uart.Init.StopBits = UART_STOPBITS_1;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart.Init.OverSampling = UART_OVERSAMPLING_16;
	uart.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&uart);

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

	hi2c1.Instance             = I2C1;
	hi2c1.Init.ClockSpeed      = 100000;
	hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1     = 0;
	hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2     = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hi2c1);

	TIM_MasterConfigTypeDef sMasterConfig = {0};

	tim1.Instance = TIM1;
	tim1.Init.Prescaler = 8;
	tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim1.Init.Period = 65535;
	tim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;


	lcd_init ();
	HAL_Delay(1000);
	lcd_send_string ("Witaj");
	lcd_put_cur(1, 0);
	lcd_send_string ("Termometr ON");
	HAL_Delay(1000);
	lcd_clear ();
	lcd_put_cur(0, 0);
	lcd_send_string ("Loading..");

	uint8_t ds1[DS18B20_ROM_CODE_SIZE];

	printf("2\n");
	 while (1) {
		   if (ds18b20_read_address(ds1) != HAL_OK)
		   {
		     printf("\n w  0x%02X	0x%02X	0x%02X	0x%02X	0x%02X	0x%02X	0x%02X	0x%02X\n",ds1[0],ds1[1],ds1[2],ds1[3],ds1[4],ds1[5],ds1[6],ds1[7]);
		   }
		   ds18b20_read_address(ds1);
		   printf("\n za   0x%02X	0x%02X	0x%02X	0x%02X	0x%02X	0x%02X	0x%02X	0x%02X\n",ds1[0],ds1[1],ds1[2],ds1[3],ds1[4],ds1[5],ds1[6],ds1[7]);
	 }

}

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
