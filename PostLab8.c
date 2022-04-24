/*
 * File:   PostLab8.c
 * Author: jorge
 *
 * Created on 23 de abril de 2022, 12:10 PM
 */
// PIC16F887 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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
#include <stdint.h>             // int8_t, unit8_t
#define _tmr0_value 217         // Valor para 10 ms
#define _XTAL_FREQ 4000000

int convvoltaje;
uint8_t bandera;
uint8_t display1;
uint8_t display2;
uint8_t display3;
uint8_t digito1;
uint8_t digito2;
uint8_t digito3;
uint8_t splitarray[3];
uint8_t tabla[10] = {0b00111111,       // 0
                     0b00000110,       // 1
                     0b01011011,       // 2
                     0b01001111,       // 3
                     0b01100110,       // 4
                     0b01101101,       // 5
                     0b01111101,       // 6
                     0b00000111,       // 7
                     0b01111111,       // 8
                     0b01101111};      // 9
uint8_t tabla1[10] = {0b10111111,       // 0
                      0b10000110,       // 1
                      0b11011011,       // 2
                      0b11001111,       // 3
                      0b11100110,       // 4
                      0b11101101,       // 5
                      0b11111101,       // 6
                      0b10000111,       // 7
                      0b11111111,       // 8
                      0b11101111};      // 9

void __interrupt() isr (void){
    if(PIR1bits.ADIF){                  // Interrupción por ADC
        if(ADCON0bits.CHS == 0){        // Interrupción por AN0
            PORTC = ADRESH;             // Mostrar los bits más signifiativos
        }
        else if (ADCON0bits.CHS == 1){ // Interrupción por AN1
            convvoltaje = 100*ADRESH/51;
        }
        PIR1bits.ADIF = 0;              // Limpiar la bandera de ADC
    }
    if(INTCONbits.T0IF){        // Verificar si se activa la bandera de interrupcion de TMR0
        PORTE = 0;
        if(bandera == 0){        
            PORTD = display1;
            PORTE = 0b00000001;
            bandera = 1; 
        }
        else if(bandera == 1){        
            PORTD = display2;
            PORTE = 0b00000010;
            bandera = 2; 
        }
        else if(bandera == 2){        
            PORTD = display3;
            PORTE = 0b00000100;
            bandera = 0; 
        }
        INTCONbits.T0IF = 0;// Limpiar la bandera de TMR0
        TMR0 = _tmr0_value; // Valor de retraso para reinicio de TMR0
    }
    return;
}

void tmr0_setup(void){
    OPTION_REGbits.T0CS = 0;     // Temporizador
    OPTION_REGbits.PSA = 0;      // Prescaler a TMR0
    OPTION_REGbits.PS0 = 1;
    OPTION_REGbits.PS1 = 1;
    OPTION_REGbits.PS2 = 1;      // Prescaler 1:256
    INTCONbits.T0IF = 0;         // Limpiar la bandera de TMR0
    TMR0 = _tmr0_value;          // Valor de retraso
    return;
}

void setup(void){
    ANSEL = 0b00000011;         // AN0 y AN1 como entradas analógicas
    ANSELH = 0;                 // I/O digitales
    
    TRISA = 0b00000011;         // RA0 y RA1 como entradas analógicas y demás como salidas
    TRISC = 0x00;               // PORTC como salida
    TRISD = 0x00;               // PORTD como salida
    TRISE = 0x00;               // PORTE como salida
    PORTA = 0x00;               // Se limpia PORTA
    PORTC = 0x00;               // Se limpia PORTC
    PORTD = 0x00;               // Se limpia PORTD
    PORTE = 0x00;               // Se limpia PORTE
    
    // Configuración de interrupciones
    PIR1bits.ADIF = 0;          // Limpiar la bandera de ADC
    PIE1bits.ADIE = 1;          // Habilitar interrupciones de ADC
    INTCONbits.PEIE = 1;        // Habilitar interrupciones de periféricos
    INTCONbits.GIE = 1;         // Habilitar interrupciones globales
    INTCONbits.T0IE = 1;        // Habilitar interrupciones de TMR0
    INTCONbits.T0IF = 0;        // Limpiar la bandera de TMR0
    
    // Configuración del oscilador
    OSCCONbits.IRCF = 0b0110;   // 4MHz
    OSCCONbits.SCS = 1;         // Oscilador interno
    
    // Configuración ADC
    ADCON0bits.ADCS = 0b00000001;   // FOSC/8
    ADCON1bits.VCFG0 = 0;           // VDD
    ADCON1bits.VCFG1 = 0;           // VSS
    ADCON0bits.CHS = 0b0000;        // Selecciona el AN0
    ADCON1bits.ADFM = 0;            // Justificador a la izquierda
    ADCON0bits.ADON = 1;            // Habilitar modulo ADC
    __delay_us(40);                 // Sample time
    
    return;
}

struct digitos {                 // Estructura contenedora de digitos de numero de 3 digitos de largo
    int digito1;
    int digito2;
    int digito3;
};


struct digitos function(int num){   // Función de separación de dígitos
    int splitarray[3];              // Array que almacena los digitos
    struct digitos D1;              // Estructura temporal para asignar los digitos y devolverlos

    int digit = num%10;             // Se toma el ultimo digito del numero que entra a la función
    splitarray[0] = digit;          // Se almacena el digito en la primera posición del array
    
    num = num/10;                   // Se divide el valor que entra de la función en 10 y se actualiza
    digit = num%10;                 // Se toma el ultimo digito del numero actualizado
    splitarray[1] = digit;          // Se almacena el digito en la segunda posición del array

    num = num/10;                   // Se divide el valor que entra de la función en 10 y se actualiza
    splitarray[2] = num;            // Se almacena el digito en la tercera  posición del array

    // Almacenamiento de digitos (Se hace de forma inversa debido a que "digit" extrae los digitos inversamente)
    D1.digito3 = splitarray[0];     // Unidades
    D1.digito2 = splitarray[1];     // Decenas
    D1.digito1 = splitarray[2];     // Centenas
    
    // Regresamos la structure
    return D1;
}

void main(void){
    setup();
    tmr0_setup();
    while(1){
        struct digitos result = function(convvoltaje);    // Se corre la funcion para extraer los digits
        display3 = tabla[result.digito3];           // Mostrar el valor de Unidades en display3
        display2 = tabla[result.digito2];           // Mostrar el valor de Decenas en display2
        display1 = tabla1[result.digito1];           // Mostrar el valor de Centenas en display1
            
        if (ADCON0bits.GO == 0){
            if(ADCON0bits.CHS == 0){        // Interrupción por AN0
                ADCON0bits.CHS = 0b0001;    // Cambio de AN0 a AN1
            }
            else if (ADCON0bits.CHS == 1){  // Interrupción por AN1
                ADCON0bits.CHS = 0b0000;    // Cambio de AN1 a AN0         
            }
             __delay_us(40);                // Sample time
            ADCON0bits.GO = 1;              // On
        }        
    }
    return;
}

