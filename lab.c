/*
 * Archivo: lab.c
 * Dispositivo: PIC16F887
 * Compilador:  XC8, MPLABX v5.40
 * Autor: José Fernando de León González
 * Programa: Contadores utilizando pots con el ADC
 * Hardware: pots en PORTA y leds y resistencias en PORTC & PORTB
 * 
 * Creado: 18/04/22
 * Última modificación: 18/04/22
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

/*------------------------------------------------------------------------------
 * Prototipos de funciones
------------------------------------------------------------------------------*/
void setup (void);


/*------------------------------------------------------------------------------
 * Interrupciones
------------------------------------------------------------------------------*/
void __interrupt() isr (void)
{
    if (PIR1bits.ADIF)  // Interrupción del ADC
    {
        if (ADCON0bits.CHS == 0b0000)
            PORTC = ADRESH;
        else
            PORTB = ADRESH;
 
        PIR1bits.ADIF = 0;
    }
    
}
/*------------------------------------------------------------------------------
 * Ciclo principal
------------------------------------------------------------------------------*/
void main(void) {
    setup();
    ADCON0bits.GO = 1;      // primera conversión del ADC
    
    while(1){
    
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
    
    TRISB = 0;                  // PORTB como salida
    PORTB = 0;                  // limpiamos PORTB
    
    
    // Configuración del oscilador
    OSCCONbits.IRCF = 0b0110;    // IRCF <2:0> -> 110 4 MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    
    //COnfiguración del ADC
    ADCON1bits.ADFM = 0;        // Ajustado a la izquierda
    ADCON1bits.VCFG0 = 0;       // Referencia en VDD
    ADCON1bits.VCFG1 = 0;       // Referencia en VSS
    
    ADCON0bits.ADCS = 0b01;     // ADCS <1:0> -> 01 FOSC/8
    ADCON0bits.CHS = 0b0000;    // CHS  <3:0> -> 0000 AN0
    
    ADCON0bits.ADON = 1;        // Encender ADC
    __delay_us(50);
    
    //Configuración de las interrupciones
    INTCONbits.GIE = 1;         // Habilitamos interrupciones globales
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones de los puertos
    
    PIE1bits.ADIE = 1;          // Habilitamos interrupciones del ADC
    PIR1bits.ADIF = 0;          // Flag del ADC en 0
    
    
   
}
/*------------------------------------------------------------------------------
 * Funciones
------------------------------------------------------------------------------*/
