#include "stm32f1xx.h"

#define eeprom24C16_ADDR			0xa0

I2C_HandleTypeDef i2c;
UART_HandleTypeDef uart;

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

void infoMenu()
{
	 printf("\nWybierz:\n1 - Odczytaj adres ADMO (BLACK)\n2 - Wyczysc pamiec ADMO (BLACK)\n");
	 printf("3 - Wgraj adres ADMO (BLACK)\n4 - Sprawdz cala pamiec eeprom (BLACK)\n");
	 printf("5 - Sprawdz cala pamiec eeprom (YELLOW)\n6 - Wyczysc pamiec ADMO (YELLOW)\n");
}

void clearADMO()
{
	 uint8_t reg = 0x00;
	 uint8_t val = 0x00;
	 int cont = 1;
	 while(cont)
	 {
		 HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, reg, 1, &val, sizeof(val), HAL_MAX_DELAY);
		 HAL_Delay(4);

			if(reg < 0xff)
			{
				reg++;
			}else
			{
				cont = 0;
			}
	 }
	 printf("Czyszczenie zakonczone!\n");
}

void testBlack()
{
	uint16_t reg = 0x000;
	uint8_t value = 0;
	int control = 1;

	while(control)
	{
		HAL_I2C_Mem_Read(&i2c, eeprom24C16_ADDR, reg, 2, &value, sizeof(value), HAL_MAX_DELAY);
		HAL_Delay(4);
		printf("Komorka: 0x%02X, wartosc: 0x%02X\n",reg,value);

		if(reg < 0xfff)
		{
			reg++;
		}else
		{
			control = 0;
		}
	}
 }

void testADMOBlack()
{
	uint16_t reg = 0x00;
	uint8_t value = 0;
	int control = 1;

	while(control)
	{
		HAL_I2C_Mem_Read(&i2c, eeprom24C16_ADDR, reg, 1, &value, sizeof(value), HAL_MAX_DELAY);
		HAL_Delay(4);

		if(reg == 0x06)
			printf("Dopelnienie do 9:\n");

		printf("Komorka: 0x%02X, wartosc: 0x%02X\n",reg,value);


		if(reg < 0x0b)
		{
			reg++;
		}else
		{
			control = 0;
		}
	}
 }

void writeAdmo(uint8_t  tab[6])
{
	uint8_t no1 = tab[0];
	uint8_t no2 = tab[1];
	uint8_t no3 = tab[2];
	uint8_t no4 = tab[3];
	uint8_t no5 = tab[4];
	uint8_t no6 = tab[5];

	// Pierwsza cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x00, 1, (uint8_t*)&no1, sizeof(no1), HAL_MAX_DELAY);
	HAL_Delay(10);
	no1 = 0x39 - (no1 - 0x30);
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x06, 1, (uint8_t*)&no1, sizeof(no1), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Druga cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x01, 1, (uint8_t*)&no2, sizeof(no2), HAL_MAX_DELAY);
	HAL_Delay(10);
	no2 = 0x39 - (no2 - 0x30);
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x07, 1, (uint8_t*)&no2, sizeof(no2), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Trzecia cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x02, 1, (uint8_t*)&no3, sizeof(no3), HAL_MAX_DELAY);
	HAL_Delay(10);
	no3 = 0x39 - (no3 - 0x30);
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x08, 1, (uint8_t*)&no3, sizeof(no3), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Czwarta cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x03, 1, (uint8_t*)&no4, sizeof(no4), HAL_MAX_DELAY);
	HAL_Delay(10);
	no4 = 0x39 - (no4 - 0x30);
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x09, 1, (uint8_t*)&no4, sizeof(no4), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Pi¹ta cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x04, 1, (uint8_t*)&no5, sizeof(no5), HAL_MAX_DELAY);
	HAL_Delay(10);
	no5 = 0x39 - (no5 - 0x30);
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x0a, 1, (uint8_t*)&no5, sizeof(no5), HAL_MAX_DELAY);
	HAL_Delay(10);

	// Szosta cyfra + dopelnienie do 9
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x05, 1, (uint8_t*)&no6, sizeof(no6), HAL_MAX_DELAY);
	HAL_Delay(10);
	no6 = 0x39 - (no6 - 0x30);
	HAL_I2C_Mem_Write(&i2c, eeprom24C16_ADDR, 0x0b, 1, (uint8_t*)&no6, sizeof(no6), HAL_MAX_DELAY);
	HAL_Delay(10);

	 printf("\nWgrywanie zakonczone!\n");
}

int main(void)
{
	SystemCoreClock = 8000000; // taktowanie 8Mhz
	HAL_Init();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_I2C2_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pin = GPIO_PIN_11 | GPIO_PIN_10;		// SCL, SDA
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gpio);

	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_2;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Mode = GPIO_MODE_AF_INPUT;
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);

	i2c.Instance             = I2C2;
	i2c.Init.ClockSpeed      = 100000;
	i2c.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	i2c.Init.OwnAddress1     = 0xff;
	i2c.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c.Init.OwnAddress2     = 0xff;
	i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&i2c);

	uart.Instance = USART2;
	uart.Init.BaudRate = 115200;
	uart.Init.WordLength = UART_WORDLENGTH_8B;
	uart.Init.Parity = UART_PARITY_NONE;
	uart.Init.StopBits = UART_STOPBITS_1;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart.Init.OverSampling = UART_OVERSAMPLING_16;
	uart.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&uart);

	//uint8_t test = 0x00;
	//uint8_t ile = 0x00;
	int i = 0;
	uint8_t tab[6] = {0};
	char choice = '0';

	 infoMenu();

	 while (1) {

		while(choice == '0')
		{
			if (__HAL_UART_GET_FLAG(&uart, UART_FLAG_RXNE) == SET)
				HAL_UART_Receive(&uart, (uint8_t*)&choice, 1, 100);
		}

		 switch (choice)
		 {
		 case '1':							 //Odczytaj ADMO BLACK
			 //printf("1\n");
			 testADMOBlack();
			 infoMenu();
			 choice = '0';
		 	 break;
		 case '2':							 //Wyczysc ADMO BLACK
			 //printf("2\n");
			 clearADMO();
			 infoMenu();
			 choice = '0';
		 	 break;
		 case '3':							 //Wgraj ADMO BLACK
			 //printf("3\n");
			 i = 0;
			 printf("Wprowadz 6 cyfr numeru ADMO lub  q  aby anulowac\n");
			 while(i < 6)
			 {
			 tab[i] = 'x';
			 while(tab[i] == 'x')
				{
					if (__HAL_UART_GET_FLAG(&uart, UART_FLAG_RXNE) == SET)
						HAL_UART_Receive(&uart,  &tab[i], 1, 100);

					if(tab[i] == 'q')
						break;
				}
				if(tab[i] == 'q')
					break;

			 printf("%d\t",tab[i]-48);
			 	i++;
			 }
			if(tab[i] == 'q')
			{
				printf("Anulowano\n");
				infoMenu();
				choice = '0';
				break;
			}

			 writeAdmo(tab);
			 infoMenu();
			 choice = '0';
		 	 break;
		 case '4':							 //Sprawdzenie ca³ej pamieci eeprom ADMO BLACK
			 //printf("4\n");
			 testBlack();
			 infoMenu();
			 choice = '0';
		 	 break;
		 case '5':							//Sprawdzenie ca³ej pamieci eeprom ADMO BLACK (in progress...)
			 //printf("5\n");
			 printf("in progress...\n");
			 infoMenu();
			 choice = '0';
		 	 break;
		 case '6':							//Wyczysc ADMO BLACK (in progress...)
			 //printf("6\n");
			 printf("in progress...\n");
			 infoMenu();
			 choice = '0';
		 	 break;
		 default:
			 choice = '0';
		 	 break;
		 }
	 }
}
