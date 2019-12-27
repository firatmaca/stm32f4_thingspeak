#include "stm32f4xx_conf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define ESP8266BUFFER_LENGHT 500
char transmitdata[100];
char transmitconf[100];
char g_arrui8ESP8266Buf[ESP8266BUFFER_LENGHT];
static uint16_t ESPWriteIndex=0;
uint16_t potdegeri;
int length;
int say=0;
int say2=0;
int say3=0;
void adc_configure(void);
uint16_t Read_ADC(void);

void Delay(uint32_t a){
	  while(a--){
			
		}
}
	
void USART_puts(USART_TypeDef* USARTx,volatile char *s) // char karakter sayisi kadar döndürüyor
{
 while(*s)
 {
  while(!(USARTx ->SR & 0x00000040));
	 USART_SendData(USARTx,*s);
	 *s++;
 }	 
}	

//bufferi temizler
static void Clear_ESPBuffer(void)
{
    uint16_t i;

    for(i=0;i<ESP8266BUFFER_LENGHT;i++)
        g_arrui8ESP8266Buf[i] = 0;

    ESPWriteIndex=0;
}

//uart init
void Usart_InitializeHW(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;

// B Portuna Clock Verilir
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
/**
 * PB6 ve PB7 Pinleri alternatif fonksiyon olarak çalisacagi tanimlanir
 * @onemli Usartaktif etmeden önce çagrilmalidir.!
 */
 GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1); // TX()
 GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1); // RX()
 // Initialize pins as alternating function
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_Init(GPIOB, &GPIO_InitStruct);

GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // TX()
 GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // RX()
 // Initialize pins as alternating function
 GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_Init(GPIOA, &GPIO_InitStruct);

GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOB, &GPIO_InitStruct);

GPIO_ResetBits(GPIOB,GPIO_Pin_13);
}


void Usart_InitializeAp(void)
{
 USART_InitTypeDef USART_InitStruct;
 NVIC_InitTypeDef NVIC_InitStructure;
 /**
 * USART1 modülüne Clock verilir.
 */
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
 /**
 * USART3 modülüne Clock verilir.
 */
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

 USART_InitStruct.USART_BaudRate = 115200;
 USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
 USART_InitStruct.USART_Parity = USART_Parity_No;
 USART_InitStruct.USART_StopBits = USART_StopBits_1;
 USART_InitStruct.USART_WordLength = USART_WordLength_8b;
 USART_Init(USART1, &USART_InitStruct);

USART_InitStruct.USART_BaudRate = 115200;
 USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART_InitStruct.USART_Mode = USART_Mode_Tx ;
 USART_InitStruct.USART_Parity = USART_Parity_No;
 USART_InitStruct.USART_StopBits = USART_StopBits_1;
 USART_InitStruct.USART_WordLength = USART_WordLength_8b;
 USART_Init(USART2, &USART_InitStruct);
 USART_Cmd(USART2, ENABLE); // USART2 aktif edilir.

/* USART1 Rx interrupt aktif ediliyor
 * USART'dan herhangi bir data geldiginde
 * USART1_IRQHandler() adiyla isimlendirdigimiz fonksiyona gider
 */
 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // USART1 Rx interrupt aktif ediliyor

NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; // USART1 interrupt ina ince ayar vermek istiyoruz.
 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// Öncelik olarak bu interrupt kaynagina 0 veriyoruz. ( 0 yazarak En öncelikli kaynak yapiyoruz )
 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // Kendi bagli oldugu vektordeki alt grup içinde de en öncelikli olarak kurduk
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // USART1 interrupt kanali aktif edilir.
 NVIC_Init(&NVIC_InitStructure); // Yaptigimiz ayarlari NVIC birimine yüklüyoruz.

USART_Cmd(USART1, ENABLE); // Nihayetinde USART1 aktif edilir.
}


static void ESP8266_Init(void)
{
 static uint8_t ESPInitCase=0;

  GPIO_SetBits(GPIOB,GPIO_Pin_13);

 switch(ESPInitCase)
 {
 case 0:
	 USART_puts(USART2,"case0");
 // Standart AT Komutu gonderilir , Cevabi OK olmalidir.
 USART_puts(USART1,"AT\r\n");
 // 1 saniye gecikme koyuyoruz.
 Delay(1000000L);
 ESPInitCase = 1;
 break;

case 1:
	USART_puts(USART2,"case1");
 // STRSTR fonksiyonu metin icinde metin arayan bir fonksiyondur
 // Eger aranilan metni bulabilirse pointer adresi ile geri donus yapar
 // Bulamaz ise NULL dondurur. Bizde null'dan farkli mi diye sorariz.
 if (strstr(g_arrui8ESP8266Buf,"OK") != NULL)
 {
 Clear_ESPBuffer();
 USART_puts(USART2,"Module Erisildi\n");
 ESPInitCase = 2;
 }
 else
 {
 Clear_ESPBuffer();
 USART_puts(USART2,"Modul Bulunamadi, Tekrar Deneniyor\n");
 ESPInitCase = 0;
 }
 break;

case 2:
	USART_puts(USART2,"case2");
 // Modulun 3 modu var gerekli bilgi datasheet'de biz 1 olmasini istiyoruz
 USART_puts(USART1,"AT+CWMODE?\r\n");
 // 1 saniye gecikme koyuyoruz.
 Delay(1000000L);
 ESPInitCase = 3;
 break;

case 3:
	USART_puts(USART2,"case3");
 // sprintf(str,"Firatmaca \r \n");
	      //   USART_SendData(USART2,*str); //cevap mode 1 'mi
 if (strstr(g_arrui8ESP8266Buf,"+CWMODE:1") != NULL)
 {
 Clear_ESPBuffer();
 USART_puts(USART2,"MODE Ayar Dogru\n");
 ESPInitCase = 4;
 }
 else
 {
 // Fabrika ayarlari olarak 2 geliyor biz onu 1 yapip reset komutu ile tamamlariz.
 USART_puts(USART1,"AT+CWMODE=1\r\n");
 //USART_puts(USART1,"AT+RST\r\n");
 Delay(1000000L);

 Clear_ESPBuffer();
 USART_puts(USART2,"MOD Degistirilir.\n");
 ESPInitCase = 0;
 }
 break;

case 4:
	USART_puts(USART2,"case4");
 // Baglanilacak olan wifi agina ait kullanici adi ve sifre girisi yapilir.
  USART_puts(USART1,"AT+CWJAP=\"kullanıcı adı\",\"şifre\"\r\n");
 // 1 saniye gecikme koyuyoruz.
 Delay(2000000L);
 ESPInitCase = 5;
 break;

case 5:
	USART_puts(USART2,"case5");
 // Baglanti saglandiginde OK Cevabi alinir
 if (strstr(g_arrui8ESP8266Buf,"OK") != NULL)
 {
 Clear_ESPBuffer();
 USART_puts(USART2,"Modeme Baglanti yapildi\n");
 ESPInitCase = 6;
 }
 else
 {
 Delay(1000000L);
 USART_puts(USART2,"Modeme Baglanti Bekleniyor.\n");
	 
 Delay(5000000L);
 say2++;
 if(say2==3)
 {
 USART_puts(USART2,"Modeme baglanilamadi\n");
 ESPInitCase=8;
 say2=0;
 }
	 
 }
 break;

case 6:
	USART_puts(USART2,"case6");
 // IP adresi nedir diye soruyoruz ?
 USART_puts(USART1,"AT+CIFSR\r\n");
 // 1 saniye gecikme koyuyoruz.
 Delay(1000000L);
 ESPInitCase = 7;
 break;

case 7:
	USART_puts(USART2,"case7");
 // IP alana kadar error bilgisi gonderir. Onu ayiririz.
 if (strstr(g_arrui8ESP8266Buf,"ERROR") == NULL)
 {
 USART_puts(USART2,"Alinan IP = \n");
 // Gelen bilginin 11.karakterinden itibaren IP adresi yaziyor.
 USART_puts(USART2,&g_arrui8ESP8266Buf[11]);
 Clear_ESPBuffer();
 ESPInitCase=8;
 }
 else
 {
 // ERROR der ise tekrar dene
 Delay(1000000L);
 USART_puts(USART2,"Tekrar Dene.\n");
 Clear_ESPBuffer();
 ESPInitCase=6;
 }

break;

case 8:
	USART_puts(USART2,"case8");
 // Baglanilacak siteye ait bilgiler girildi.
 USART_puts(USART1,"AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n");
 // 1 saniye gecikme koyuyoruz.
 Delay(1000000L);
 ESPInitCase = 9;
 break;

case 9:
	USART_puts(USART2,"case9");
 // Baglanti kuruldugunda Linked diye bir cevap aliriz.
  if (strstr(g_arrui8ESP8266Buf,"CONNECT") != NULL)
//if (strstr(g_arrui8ESP8266Buf,"Linked") != NULL)
 {
 Clear_ESPBuffer();
 USART_puts(USART2,"Site ile baglanti kuruldu\n");
 ESPInitCase = 10;
 }
 else
 {
 // Cevap gelene kadar bekler
 Delay(1000000L);
 USART_puts(USART2,"siteye Baglanti Bekleniyor.\n");
 Delay(10000000L);
	 
 say++;
 if(say==3)
 {
 USART_puts(USART2,"Siteye Baglanti kurulamadi.\n");
 ESPInitCase=8;
 say=0;
 } 
 }
 break;

case 10:
	USART_puts(USART2,"case10");
 potdegeri=Read_ADC(); // adc okunuyor 
 
 
 sprintf(transmitdata, "GET https://api.thingspeak.com/update?api_key=DWIK4ZSUYD76X8AI&field1=%d\r\n",potdegeri);
 
 length=strlen(transmitdata); // gönderilecek datanin uzunlugu
 length=length+2;
 sprintf(transmitconf,"AT+CIPSEND=%d\r\n",length);
 
 USART_puts(USART1,transmitconf);
 // Komutu verdikten sonra bize '>' bilgisi geliyor.
 Delay(10000000L);
 
 if (strstr(g_arrui8ESP8266Buf,">") != NULL)
 {
 Clear_ESPBuffer();
 USART_puts(USART2,"Gönderilen data uzunlugu kabul edildi\n");
 USART_puts(USART1,transmitdata);
 USART_puts(USART2,"Data gönderildi\n");
 Delay(2000000L);
 ESPInitCase=10;
 }
 else
 {
 // Cevap gelene kadar bekler
 Delay(3000000L);
 USART_puts(USART2,"gönderilen data uzunlugu kabul edilmedi\n");
 ESPInitCase=8;
 USART_puts(USART2,"Siteye yeniden baglanilmaya çalisiliyor\n");
	 
 say3++;
 if(say3==3)
 {
 USART_puts(USART2,"Baglanti kurulamadi.\n Ayarlar kontrol edilecek\n");
 ESPInitCase=0;
 say3=0;
 }
	 
 }


 break;

}
}

int main(void)
{
	
	//USART_puts(USART2,"firat");
 // System Clock Init Edilir.
 SystemInit();
 // Usart modullerinin portlarinin hardware ayarlari yapilir.
 Usart_InitializeHW();
 // Usart modullerinin clock , baud ve interrupt ayarlari yapilir.
 Usart_InitializeAp();
 adc_configure();
 // Giris metni
 USART_puts(USART2," esp8266 deneme!!! \r\n");
 // Komut gondermeden once module ait bufferi temizleriz.
 Clear_ESPBuffer();

	 
while(1)
 {

 ESP8266_Init();
 }


}

// USART1 interrupt Alt Programi
void USART1_IRQHandler(void){
 // USART1 RX interrupt flag kontrol edilir.
 if( USART_GetITStatus(USART1, USART_IT_RXNE) )
 {
 uint8_t Received_Byte = USART1->DR; // Gelen bilgi degisken içine alinir.
 USART2->DR = Received_Byte; // Debug monitor icin ayni bilgi usart3'e gonderilir.

// Strstr fonksiyonu için eklendi, modülden null karakteri gelebiliyordu , onu engellemis olduk.
 if(Received_Byte != 0)
 {
 g_arrui8ESP8266Buf[ESPWriteIndex] = Received_Byte;
 ESPWriteIndex++;
 }

}
}

void adc_configure()
{
 ADC_CommonInitTypeDef ADC_CommonInitStructure;
 ADC_InitTypeDef ADC_init_structure; 
 GPIO_InitTypeDef GPIO_InitStructure;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; // potansiyometrenin data pini A0 pinine baglanacak
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_Init(GPIOA, & GPIO_InitStructure);

ADC_DeInit();
 
 ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
 ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; // 4/8/16.. vs olabilir kristalin 4 de 1 i seçtik
 ADC_CommonInit(& ADC_CommonInitStructure);
 
 
 ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right; 
 ADC_init_structure.ADC_Resolution = ADC_Resolution_12b; 
 ADC_init_structure.ADC_ContinuousConvMode = ENABLE; 
 ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
 ADC_init_structure.ADC_NbrOfConversion = 1;
 ADC_init_structure.ADC_ScanConvMode = DISABLE;
 ADC_Init(ADC1,&ADC_init_structure);
 
 //Enable ADC conversion
 ADC_Cmd(ADC1,ENABLE);
 
 
}


uint16_t Read_ADC(void)
{
 ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1, ADC_SampleTime_56Cycles); /*hesaplama islemi tek cycle da hesaplanmaz,genis sürede daha az ak1m ceker,
 tepki süresi önemli ise kisa tutulabilir */
 ADC_SoftwareStartConv(ADC1); // adc1 üzerinde yazilimsal dönüsümü baslat 
 while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET); /*icerideki sart saglandikca assagiya inme while noktali virgül ile bitiyorsa,
 her dönüsüm sonrasi bayrak sifirlandi*/
 return ADC_GetConversionValue(ADC1); 
}
