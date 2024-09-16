#include <REGX51.H>

sbit Relay = P2^7;			// Chan kich tin hieu cho relay 24V
sbit CS = P2^0;					// Chan chon thiet bi
sbit SCK = P2^1;				// Chan xung dieu khien
sbit MISO = P2^2;				// Chan truyen du lieu toi 8051 tu ADC
sbit MOSI = P2^3;				// Chan truyen du lieu toi ADC tu 8051

void initialize_SPI();				// Khai bao ham khoi tao SPI
unsigned char interface_SPI (unsigned char Data);	// Khai bao ham truyen va nhan du lieu qua SPI
unsigned char read_ADC();													// Khai bao ham doc gia tri cua ADC
void control_Carlight();													// Khai bao ham dieu khien LED 24V
void delay_Time(unsigned int ms);									// Khai bao ham delay su dung Timer 8051

void main()
{
    initialize_SPI();  				// Khoi tao giao thuc SPI
    while (1) {
        control_Carlight();		// Goi ham dieu khien LED 24V
				delay_Time(1000);			// Delay 1s
    }
}

void initialize_SPI()		// Khoi tao giao thuc SPI
{
    CS = 1;  						// Ban dau keo CS len 1 --> khong giao tiep SPI
    MOSI = 0; 					// Khong gui du lieu
    SCK = 0;   					// Chua su dung SPI
}

unsigned char interface_SPI (unsigned char Data)		// Tao ham truyen va nhan du lieu SPI
{
    unsigned int i;
    unsigned int received_Data = 0;

    for (i = 0; i < 8; i++) {		// Gui tung bit cua 1 byte du lieu
        if (Data & 0x80) {			// Kiem tra bit cao nhat MSB de bat dau gui tung bit mot bat dau tu MSB --> LSB
            MOSI = 1;  
        } else {
            MOSI = 0;  
        }
				
        SCK = 1;  							// Tao xung de bat dau truyen
        Data <<= 1;  						// Dich 1 bit sang trai de chuan bi gui bit tiep theo
        received_Data <<= 1;		// Dich 1 bit sang trai de chuan bi nhan bit tu ADC
        if (MISO) 							// MISO = 1 thi them bit LSB la 1
				{						
            received_Data |= 0x01;
        }
        SCK = 0;  							// Hoan thanh truyen 1 bit
    }
		
    return received_Data;				// Tra ve gia tri
		
}

unsigned char read_ADC()				// Tao ham doc gia tri tu ADC
{
    unsigned char MSB;
    unsigned int result;

    CS = 0;  										// Keo chan CS = 0 bat dau giao tiep
    interface_SPI(0xD0);  			// Doc du lieu tu kenh 0 cua ADC
    MSB = interface_SPI(0x00);  // Gui gia tri trong vào ADC va bat dau nhan gia tri ADC tu kenh 0
    CS = 1;  										// Dung giao tiep giua 8051 voi ADC

    result = MSB;								// Luu gia tri tu ADC
	
    return result;							// Tra ve gia tri cho ham
}

void control_Carlight() 		// Tao ham dieu khien den o to
{
    unsigned int light_Value = read_ADC();	// Khoi tao gia tri anh sang = gia tri do tu ADC
    
    if (light_Value < 110) 	// Neu gia tri anh sang nho hon 110 thi bat den, lon hon thi tat den
		{		
        Relay = 1;
    } 
		
		else 
			
		{  
        Relay = 0; 	
    }
}

void delay_Time(unsigned int ms)	// Khoi tao ham delay su dung bo Timer/Counter 8051
{
    unsigned int i;
	
    for (i = 0; i < ms; i++) 
		{
				TMOD = 0x01;  				// Chon Timer 0, su dung thanh ghi TMOD - chon che do Mode 1: 16 bit
        TH0 = 0xFC;   				// TL0 dem tu 0x67 --> 0xFC va tran ve 0x67
        TL0 = 0x67;   
        TR0 = 1;     					// Timer0 bat dau dem
        while (TF0 == 0);  		// Kiem tra xem Timer0 da dem xong hay chua
        TR0 = 0;      				// Dung dem tai Timer0
        TF0 = 0;    					// Dat co tran TF ve 0 --> chuan bi dem tiep  
    }
}