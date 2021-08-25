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

unsigned char Wezyk(unsigned char input, char kierunek)
{
    if(kierunek == 'L')
        return input << 1;
    else
        return input >> 1;
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
    
    unsigned char display;
    unsigned int tmp = 0;
    char kierunek;
    int program = 0; 
    
    while(1)
    {   
        //switch umo?liwiaj?cy wybór podprogramu
        switch(program)
        {
            //8 bitowy licznik binarny zliczaj?cy w gór? (0...255) 
            case 0:
                display = 0;
                while(1)
                {
                    //Odczyt z potencjometru P2, dzielimy przedzia? na 5 czesci, 
                    //a 1024/5=204.8
                    tmp = ((unsigned int)adc(1) / 205);
                    //Wyswietlenie na diodach rejestru D
                    PORTD = display;
                    //uzale?nienie czasu wy?wietlania danej warto?ci za pomoc?
                    //diod od warto?ci ustawienia potencjometru P2 
                    //z wykorzystaniem funkcji opó?nienia
                    delay(tmp * 200 + 200);
                    //zwi?kszanie warto?ci wy?wietlanego licznika
                    display++;
                    //p?tla umo?liwiaj?ca ?atwiejsze przechwycenie 
                    //naci?ni?cia przycisku
                    unsigned int i = 11000;
                    while(PORTBbits.RB4 && PORTBbits.RB3 && i > 0)
                        i--;
                    //je?li zostanie naci?ni?ty przycisk RB3 lub RB4 
                    //prze??cz na kolejny program
                    if(PORTBbits.RB3 == 0 || PORTBbits.RB4 == 0)
                    {
                        program = 1;
                        break;
                    }   
                }
                break;
                
            //wezyk
            case 1:
                //warto?? w??yka jest równa 7 
                //(2**0 + 2**1 + 2**2 = 7, 3 kolejne zapalone diody)
                display = 7;
                while(1)
                {
                    //Odczyt z potencjometru P2, dzielimy przedzia? na 5 czesci, 
                    //a 1024/5=204.8
                    tmp=((unsigned int)adc(1) / 205);
                    //Wyswietlenie na diodach rejestru D
                    PORTD = display;
                    //uzale?nienie czasu wy?wietlania danej warto?ci za pomoc?
                    //diod od warto?ci ustawienia potencjometru P2 
                    //z wykorzystaniem funkcji opó?nienia
                    delay(tmp * 200 + 200);
                    //je?li w??yk 'dojedzie do lewej ?ciany' 
                    //to zmie? kierunek ruchu na prawo
                    if(display == 224)
                        kierunek = 'P';
                    //je?li w??yk 'dojedzie do prawej ?ciany' 
                    //to zmie? kierunek ruchu na lewo
                    else if(display == 7)
                        kierunek = 'L';
                    //wykonaj ruch w??yka 
                    //o jeden bit w aktualnym kierunku
                    display = Wezyk(display, kierunek);
                    //p?tla umo?liwiaj?ca ?atwiejsze przechwycenie 
                    //naci?ni?cia przycisku
                    unsigned int i = 11000;
                    while(PORTBbits.RB4 && PORTBbits.RB3 && i > 0)
                        i--;
                    //je?li zostanie naci?ni?ty przycisk RB3 lub RB4 
                    //prze??cz na kolejny program
                    if(PORTBbits.RB3 == 0 || PORTBbits.RB4 == 0)
                    {
                        program = 0;
                        break;
                    }   
                }
                break;
        }
    }
    
    return;
}
