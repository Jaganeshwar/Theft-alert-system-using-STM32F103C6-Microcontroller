#include<stm32f10x.h>
char a;
void delay(unsigned int a)
{
	while(a--);
}
void enable()
{
	GPIOC->ODR |= 1<<14;
	delay(100);
	GPIOC->BRR = 1<<14;
	delay(100);
}
void lcd(char rs,char dta)
{
	GPIOC->ODR = rs<<15;
	GPIOA->ODR = (dta&0XF0)>>4;
	enable();
	GPIOA->ODR = (dta&0X0F);
	enable();
}
void lcd1(char *dta)
{
	while(*dta)
	{
		lcd(1,*dta++);
	}
}
void tx(char *ptr)
{
	while(*ptr)
	{
	  USART1->DR = *ptr++;
		while(!(USART1->SR&1<<7));
		delay(10000);
	}
}
void USART1_IRQHandler()
{
	while(!(USART1->SR&1<<5));
	a = USART1->DR;
	delay(10000);
	USART1->SR = 0<<5;
	tx("ATH\r");
	if(a=='s')
	{
		GPIOB->ODR = 0<<8;
		GPIOB->ODR = 0<<9;
		lcd(0,0XC0);
		lcd1("MESSAGE RECIEVED..");
	}
}
void gsm()
{
	int i;
	tx("AT\r");
	for(i=0;i<40;i++)
	{
		delay(10000);
	}
	tx("AT+CMGF=1\r");
	for(i=0;i<40;i++)
	{
		delay(10000);
	}   
	tx("AT+CMGS=\"+918838706469\"\r");
	for(i=0;i<40;i++)
	{
		delay(10000);
	}
	
}
int main()
{
	RCC->CR = 0X00000083;
	RCC->CFGR = 0X00000000;
	RCC->APB2ENR = 1<<0|1<<2|1<<3|1<<4|1<<14;
	GPIOA->CRL = 0X00222222;
	GPIOA->CRH = 0X04A0;
	GPIOB->CRL = 0X08;
	GPIOB->CRH = 0X22;
	GPIOC->CRH = 0X22000000;
	USART1->CR1 = 1<<13|1<<3|1<<2|1<<5|1<<8;
	USART1->BRR = 0X341;
	USART1->CR2 = 0;
	NVIC_EnableIRQ(USART1_IRQn);
	
	lcd(0,0X02);
	lcd(0,0X28);
	lcd(0,0X0E);
	lcd(0,0X80);
	//lcd(1,'A');
	
	while(1)
	{
		if(GPIOB->IDR&1<<0)
		{
			while(GPIOB->IDR&1<<0);
			GPIOB->ODR = 1<<8|1<<9;
			lcd(0,0X80);
			lcd1("ALERT....");
			gsm();
		}
	}
}
