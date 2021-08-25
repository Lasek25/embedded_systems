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
    
    lcd_init(); //Inicjalizacja wyswietlacza
    lcd_cmd(L_CLR); //Czyszczenie wyswietlacza
    
    int i = 0;
    int m = 0;
    int s1 = 0;
    int s2 = 0;
    //lista mocy mikrofalówki
    char* moc[] = {"800", "600", "350", "200"};
    while(1)
    {
        //Czyszczenie wyswietlacza
        lcd_cmd(L_CLR);
        //przej?cie do pierwszej linii
        lcd_cmd(L_L1);
        //wypisanie aktualnej mocy na wy?wietlaczu
        lcd_str("Moc ");
        lcd_str(moc[i]);
        lcd_str("W        ");
        //przej?cie do drugiej linii
        lcd_cmd(L_L2);
        //wypisanie czasu na wy?wietlaczu
        lcd_str("Czas ");
        //48 to kod dla zera
        lcd_dat(48);
        lcd_dat(m + 48);
        lcd_str(":");
        lcd_dat(s1 + 48);
        lcd_dat(s2 + 48);
        lcd_str("      ");
        delay(200);
        
        //ustawianie mocy za pomoc? przycisku
        if(PORTBbits.RB5 == 0)
        {              
            (i == 3) ? i = 0 : i++;
        }
        //ustawianie liczby minut za pomoc? przycisku
        if(PORTBbits.RB4 == 0)
        {              
            (m == 9) ? m = 0 : m++;
        }
        //ustaiwanie liczby sekund za pomoc? przycisku
        if(PORTBbits.RB3 == 0)
        {              
            (s1 == 5) ? (s1 = 0, (m == 9) ? m = 0 : m++) : s1++;
        }
        //start mikrofali za pomoc? przycisku
        if(PORTBbits.RB2 == 0)
        {              
            //dopóki czas jest wi?kszy od zera 
            //i nie jest przyci?ni?ty przycisk resetu
            while((m > 0 || s1 > 0 || s2 > 0) && PORTBbits.RB1)
            {
                //kolejne warunki odpowiedzialne za zmniejszanie czasu zegara
                if(s1 == 0 && s2 == 0)
                {
                    if(m > 0)
                    {
                        m--;
                        s1 = 5;
                        s2 = 9;
                    }
                }
                else
                {
                      if(s2 == 0)
                      {
                          s1--;
                          s2 = 9;
                      }
                      else
                          s2--;
                }
                //zamiast 1s jest podane 0.75s opoznienia, 
                //dzieki temu rzeczywisty czas jest zblizony do 1s
                delay(740);
                lcd_cmd(L_L2);
                //wypisywanie czasu na wy?wietlaczu
                lcd_str("Czas ");
                lcd_dat(48);
                lcd_dat(m + 48);
                lcd_str(":");
                lcd_dat(s1 + 48);
                lcd_dat(s2 + 48);
                lcd_str("      ");
                delay(10);
                //przycisk stop/start
                if(PORTBbits.RB2 == 0)
                {
                    delay(500);
                    while(PORTBbits.RB2 && PORTBbits.RB1)
                    {
                        continue;
                    }
                        
                } 
            }
        }
        //reset mocy, minut i sekund po naci?ni?ciu przycisku
        if(PORTBbits.RB1 == 0)
        {
            i = 0;
            m = 0;
            s1 = 0;
            s2 = 0;
        }
    }
    
    return;
}
