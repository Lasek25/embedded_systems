// CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = ON        // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF

#include <xc.h>

#define LENA  PORTEbits.RE1
#define LDAT  PORTEbits.RE2
#define LPORT PORTD

#define L_ON	0x0F
#define L_OFF	0x08
#define L_CLR	0x01
#define L_L1	0x80
#define L_L2	0xC0
#define L_CR	0x0F		
#define L_NCR	0x0C	

#define L_CFG   0x38

void delay(unsigned int ms)
{
    unsigned int i;
    unsigned char j;
    
 for (i =0; i< ms; i++)
 {
 
  for (j =0 ; j < 200; j++)
   {
      Nop();
      Nop();
      Nop();
      Nop();
      Nop();
   }
 }
}

unsigned int adc(unsigned char kanal)
{
    switch(kanal)
    {
        case 0: ADCON0=0x01; break; //P1
        case 1: ADCON0=0x05; break; //P2
        case 2: ADCON0=0x09; break; 
    }
    
    ADCON0bits.GO=1;
    while(ADCON0bits.GO == 1);

   return ((((unsigned int)ADRESH)<<2)|(ADRESL>>6));
}

void lcd_wr(unsigned char val)
{
  LPORT=val;
}

void lcd_cmd(unsigned char val)
{
	LENA=1;
        lcd_wr(val);
        LDAT=0;
        //delay(3);
        LENA=0;
        //delay(3);
	LENA=1;
}
 
void lcd_dat(unsigned char val)
{
	LENA=1;
        lcd_wr(val);
        LDAT=1;
        //delay(3);
        LENA=0;
        //delay(3);
	LENA=1;
}

void lcd_init(void)
{
	LENA=0;
	LDAT=0;
	delay(20);
	LENA=1;
	
	lcd_cmd(L_CFG);
	delay(5);
	lcd_cmd(L_CFG);
        delay(1);
	lcd_cmd(L_CFG); //configura
	lcd_cmd(L_OFF);
	lcd_cmd(L_ON); //liga
	lcd_cmd(L_CLR); //limpa
	lcd_cmd(L_CFG); //configura
        lcd_cmd(L_L1);
}

void lcd_str(const char* str)
{
 unsigned char i=0;
  
 while(str[i] != 0 )
 {
   lcd_dat(str[i]);
   i++;
 }  
}

//funkcja do wy?wietlania czasu na wy?wietlaczu
void lcd_time(int m11, int m21, int s11, int s12, int s21, int s22)
{
    //Czyszczenie wyswietlacza
    lcd_cmd(L_CLR);
    //przej?cie do pierwszej linii
    lcd_cmd(L_L1);
    //wy?wietlenie czasu, 48 to kod dla zera
    lcd_dat(48);
    lcd_dat(m11 + 48);
    lcd_str(":");
    lcd_dat(s11 + 48);
    lcd_dat(s12 + 48);
    lcd_str("      ");
    lcd_dat(48);
    lcd_dat(m21 + 48);
    lcd_str(":");
    lcd_dat(s21 + 48);
    lcd_dat(s22 + 48);
}

void main(void) {
    
    //Inicjalizacja konwertera analogowo cyfrowego
    ADCON0=0x01;
    ADCON1=0x0B;
    ADCON2=0x01;
    
    TRISA=0xC3;
    TRISB=0x3F;   
    TRISC=0x01;
    TRISD=0x00;
    TRISE=0x00;
    
    lcd_init(); //Inicjalizacja wy?wietlacza
    lcd_cmd(L_CLR); //Czyszczenie wy?wietlacza
    unsigned int tmp = 0;
    while(1)
    {
       //zmienne opisuj?ce minuty i sekundy zegarów obu graczy
       int m11 = 0;
       int s11 = 0;
       int s12 = 0;
       int m21 = 0;
       int s21 = 0;
       int s22 = 0;
       //odczyt z potencjometru P2, dzielimy przedzia? na 3 cz??ci, 
       //a 1024/3=341.33
       tmp = ((unsigned int)adc(1) / 341);

       //ustawienie warto?ci minut zale?ne od odczytu z potencjometru
       if(tmp == 0)
       {
           m11 = 5;
           m21 = 5;
       }
       else if (tmp == 1)
       {
           m11 = 3;
           m21 = 3;
       }
       else
       {
           m11 = 1;
           m21 = 1;
       }
       //wyswietlenie czasów obu graczy na wy?wietlaczu
       lcd_time(m11, m21, s11, s12, s21, s22);
       delay(100);
       
       //aby rozpocz?? odliczanie czasu nale?y nacisn?? przycisk
       //pierwszego lub drugiego gracza
        if(PORTBbits.RB3 == 0 || PORTBbits.RB5 == 0)
        {
            //p?tla dzia?ania zegarów, ko?czy dzia?anie po naci?ni?ciu resetu, 
            //jest to mo?liwe po up?yni?ciu jednego z czasów
            while(PORTBbits.RB4)
            {
                //zegar drugiego gracza
                if(PORTBbits.RB3 == 0)
                {
                    //dopóki nie jest naci?ni?ty przycisk drugiego gracza
                    while(PORTBbits.RB5)
                    {
                        //kolejne warunki odpowiedzialne za zmniejszanie czasu
                        if(s21 == 0 && s22 == 0)
                        {
                            if(m21 > 0)
                            {
                                m21--;
                                s21 = 5;
                                s22 = 9;
                            }
                        }
                        else
                        {
                            if(s22 == 0)
                            {
                                s21--;
                                s22 = 9;
                            }
                            else
                                s22--;
                        }
                        //zamiast 1s jest podane 0.75s opoznienia, 
                        //dzieki temu rzeczywisty czas jest zblizony do 1s
                        delay(750);
                        //wyswietlenie czasów obu graczy na wy?wietlaczu
                        lcd_time(m11, m21, s11, s12, s21, s22);
                        //sprawdzenie warunku ko?ca czasu
                        if(m21 == 0 && s21 == 0 && s22 == 0)
                        {
                            //wy?wietlenie komunikatu ko?ca czasu
                            lcd_cmd(L_CLR);
                            lcd_cmd(L_L1);
                            lcd_str("Koniec czasu!   ");
                            lcd_cmd(L_L2);
                            lcd_str("Wygrywa gracz 1 ");
                            //oczekiwanie na naci?ni?cie przycisku resetu,
                            //aby przygotowa? zegary do kolejnego odmierzania
                            while(PORTBbits.RB4)
                                continue;
                            break;
                        }
                    }
                }
                //zegar pierwszego gracza
                if(PORTBbits.RB5 == 0)
                {
                    //dopóki nie jest naci?ni?ty przycisk pierwszego gracza
                    while(PORTBbits.RB3)
                    {
                        //kolejne warunki odpowiedzialne za zmniejszanie czasu
                        if(s11 == 0 && s12 == 0)
                        {
                            if(m11 > 0)
                            {
                                m11--;
                                s11 = 5;
                                s12 = 9;
                            }
                        }
                        else
                        {
                            if(s12 == 0)
                            {
                                s11--;
                                s12 = 9;
                            }
                            else
                                s12--;
                        }
                        //zamiast 1s jest podane 0.75s opoznienia, 
                        //dzieki temu rzeczywisty czas jest zblizony do 1s
                        delay(750);
                        //wyswietlenie czasów obu graczy na wy?wietlaczu
                        lcd_time(m11, m21, s11, s12, s21, s22);
                        //sprawdzenie warunku ko?ca czasu
                        if(m11 == 0 && s11 == 0 && s12 == 0)
                        {
                            //wy?wietlenie komunikatu ko?ca czasu
                            lcd_cmd(L_CLR);
                            lcd_cmd(L_L1);
                            lcd_str("Koniec czasu!   ");
                            lcd_cmd(L_L2);
                            lcd_str("Wygrywa gracz 2 ");
                            //oczekiwanie na naci?ni?cie przycisku resetu,
                            //aby przygotowa? zegary do kolejnego odmierzania
                            while(PORTBbits.RB4)
                                continue;
                            break;
                        }
                    }
                }
            }
        }
    }
    return;
}
