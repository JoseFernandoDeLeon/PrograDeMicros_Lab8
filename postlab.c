/*
 * Archivo: postlab.c
 * Dispositivo: PIC16F887
 * Compilador:  XC8, MPLABX v5.40
 * Autor: José Fernando de León González
 * Programa: DAC y desplegado de voltaje producido por pots con el módulo ADC
 * Hardware: pots en PORTA y leds, resistencias y voltímetro en PORTB, displays y transistores en PORTC & PORTD  
 * Creado: 23/04/22
 * Última modificación- 23/04/22
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>



/*------------------------------------------------------------------------------
 * Constantes
------------------------------------------------------------------------------*/
#define _XTAL_FREQ 4000000      // Oscilador de 4 MHz


/*------------------------------------------------------------------------------
 * Variables
------------------------------------------------------------------------------*/
int ADC_disp_val;
int ADC_voltage;

uint8_t centenas;
uint8_t decenas;
uint8_t unidades;
uint8_t residuo;

uint8_t flags = 0;
uint8_t display_val [3]; 

uint8_t tabla [16] = {0b00111111,       //[0] 0
                      0b00000110,       //[1] 1
                      0b01011011,       //[2] 2
                      0b01001111,       //[3] 3
                      0b01100110,       //[4] 4
                      0b01101101,       //[5] 5
                      0b01111101,       //[6] 6
                      0b00000111,       //[7] 7
                      0b01111111,       //[8] 8
                      0b01101111,       //[9] 9
                      0b01110111,       //[10] A
                      0b01111100,       //[11] B
                      0b00111001,       //[12] C
                      0b01011110,       //[13] D
                      0b01111001,       //[14] E
                      0b01110001};      //[15] F
/*------------------------------------------------------------------------------
 * Prototipos de funciones
------------------------------------------------------------------------------*/
void setup (void);
void displays (int num);

/*------------------------------------------------------------------------------
 * Interrupciones
------------------------------------------------------------------------------*/
void __interrupt() isr (void)
{
    if (PIR1bits.ADIF)  // Interrupción del ADC
    {
        if (ADCON0bits.CHS == 0b0000)
            ADC_disp_val = ADRESH;
        else
            PORTB = ADRESH;
 
        PIR1bits.ADIF = 0;
    }
     if (INTCONbits.T0IF)  // Interrupción del TIMER0
    {
        PORTD = 0;
        
        if (flags == 0)
        {
            PORTC = display_val[2];
            PORTD = 1;
            flags = 1;
        }
        else if (flags == 1)
        {
            PORTC = display_val[1];
            PORTD = 2;
            flags = 2;
        }
        else if (flags == 2)
        {
            PORTC = display_val[0];
            PORTD = 4;
            flags = 0;
        }
        TMR0 = 6;
        INTCONbits.T0IF = 0;
    }
    return;
}
/*------------------------------------------------------------------------------
 * Ciclo principal
------------------------------------------------------------------------------*/
void main(void) {
    setup();
    ADCON0bits.GO = 1;      // primera conversión del ADC
    
    while(1){
    
    ADC_voltage =  (int) (ADC_disp_val*((float )5/255)*(100));
    
    displays(ADC_voltage);
    
    display_val [0] = tabla[unidades];
    display_val [1] = tabla[decenas];
    display_val [2] = tabla[centenas];    
    
    if (ADCON0bits.GO == 0) {
        if (ADCON0bits.CHS == 0b0000)
            ADCON0bits.CHS = 0b0001;
        else
            ADCON0bits.CHS = 0b0000;
        __delay_us(1000);
        ADCON0bits.GO = 1;
        
    } 

    }        
    return;
}

/*------------------------------------------------------------------------------
 * Configuración
------------------------------------------------------------------------------*/
void setup (void){
    
    // Configuración de los puertos
    ANSEL = 0b00000011;          //AN0 & AN1 como entradas analógicas
    ANSELH = 0;
    
    TRISA = 0b00000011;         // PORTA como salida, RA0 & RA1 como entradas 
    PORTA = 0;                  // Limpiamos PORTA 
    
    TRISC = 0;                  // PORTC como salida
    PORTC = 0;                  // Limpiamos PORTC 
    
    TRISD = 0;                  // PORTD como salida
    PORTD = 0;                  // Limpiamos PORTD 
    
    TRISB = 0;                  // PORTB como salida
    PORTB = 0;                  // limpiamos PORTB
    
    
    // Configuración del oscilador
    OSCCONbits.IRCF = 0b0110;    // IRCF <2:0> -> 110 4 MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    
    //Configuración del ADC
    ADCON1bits.ADFM = 0;        // Ajustado a la izquierda
    ADCON1bits.VCFG0 = 0;       // Referencia en VDD
    ADCON1bits.VCFG1 = 0;       // Referencia en VSS
    
    ADCON0bits.ADCS = 0b01;     // ADCS <1:0> -> 01 FOSC/8
    ADCON0bits.CHS = 0b0000;    // CHS  <3:0> -> 0000 AN0
    
    ADCON0bits.ADON = 1;        // Encender ADC
    __delay_us(50);
    
    // Configuración del TIMER0
    OPTION_REGbits.T0CS = 0;    // TIMER0 como temporizador
    OPTION_REGbits.PSA = 0;     // Prescaler a TIMER0
    OPTION_REGbits.PS = 0b001;  // PS<2:0> -> 001 Prescaler 1:4
    
    TMR0 = 6;
    
    //Configuración de las interrupciones
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones de los puertos
    
    PIE1bits.ADIE = 1;          // Habilitamos interrupciones del ADC
    INTCONbits.TMR0IE = 1;     // activamos interrupciones del TIMER0
    
    PIR1bits.ADIF = 0;          // Flag del ADC en 0
    INTCONbits.T0IF = 0;        // Flag del TIMER0 en 0
    
    
   
}
/*------------------------------------------------------------------------------
 * Funciones
------------------------------------------------------------------------------*/

void displays (int num)
{

    centenas =  (uint8_t)(num/100);
    residuo =   num%100;
    
    decenas =   residuo/10;
    residuo =   residuo%10;
    
    unidades =  residuo;
    
    return;
}