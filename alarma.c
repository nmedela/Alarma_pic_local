///////////////////////////////////////////
/// ALARMA PARA ELECTRONICA BUENOS AIRES///
///////////////////////////////////////////
//Usa lcd 4bit 16x2
//pines del puerto D definidos como salida para controlar lcd
// d0= Enable ; D1= RS ; d4-d7 = datos lcd; ;
//////////////////////////////////////////////
// D2: led verde ;; d3: led rojo
///////////////////////////////////////////////
//A0= sensor temperaratura
//A1=OK ; ; A2= + ; ; A3= - ; ; A4= Cancelar ; ;
//E0= Sensor puerta ; ; E1= Sensor movimiento
//C0= SIRENA ; ; C1= Luces ; ;
//
///////////////////////////////////////////////


#include <16f877a.h>
#device ADC=10;
#include <stdio.h>

#include <STRING.h>
#use delay (clock=4000000)

//#use rs232 (baud=2400,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)
#fuses XT,NOWDT,PUT
#byte porta=0x05
#byte portd=0x08
#byte portb=0x06
#byte portc=0x07
#byte trisa=0x85
#byte trisd=0x88
#use fast_io(a)
#use fast_io(b)
#use fast_io(c)
#use fast_io(d)

#define LCD_E           PIN_D0                                    ////
#define LCD_RS          PIN_D1                                    ////
#define LCD_D4          PIN_D4                                    ////
#define LCD_D5          PIN_D5                                    ////
#define LCD_D6          PIN_D6                                    ////
#define LCD_D7          PIN_D7
#define LED_VERDE       PIN_D2
#define LED_ROJO        PIN_D3

#define BTN_OK          PIN_A1
#define BTN_MAS         PIN_A2
#define BTN_MENOS       PIN_A3
#define BTN_CANCELAR    PIN_A4

#define  SENSOR_MAG     PIN_E0
#define  SENSOR_MOV     PIN_E1

#define SIRENA2         PIN_C3
#define  SIRENA         PIN_B0
#define  LUCES          PIN_B1



#define LCD_CHR = 1
#define LCD_CMD = 0

#define LCD_LINE_1 = 0x80 //LCD RAM address for the 1st line
#define LCD_LINE_2 = 0xC0 //LCD RAM address for the 2nd line

//E_PULSE = 0.0005
//E_DELAY = 0.0005
void lcd_init();//inicializa lcd
void lcd_bytes(int bits,int mode);//manda los datos como comando o caracter 0: comando ; 1:caracter
void lcd_toggle_enable(); //Hace el cambio de activado y desactiva con su tiempo correspondiente
void lcd_string(char mensaje[],int line,  int t);// manda los mensajes indicando la posicion y la cantidad de caracteres
void pantalla_inicial(); //Muestra electronica bs as y alarma desactivada con la plantillas de temperatura
int chequearbotones();
void chequeartiempo();
void activarconteo();
void sonando();
int chequeosensores();
                //"0123456789ABCDEF";
char mensaje1[16]="Electronica BsAs"; // 1
char mensaje2[16]="Desactivada   °C";
char mensaje3[16]="Activada      °C";
char borrar[16]  ="                ";
char mensaje5[16]="Cambiar password"; //2
char mensaje6[16]="Contra. antigua ";
char mensaje7[16]="Nueva contrasena";
char mensaje8[16]="Pass. cambiada  ";
char mensaje9[16]="        SEGUNDOS";
char mensaje10[16]="    SONANDO     ";

int16 conteo3=0;
int pantalla=0; //indica en que pantalla esta
int estadoalarma=0;
int16 muestratemp=0;
int conteo=0;
int conteo1=0;
int valor=0;
int armada=0;
int alarma=0;
int sonar=0;
int posicion=0;
char clave1='1';
char clave2='1';
char clave3='1';
char clave4='1';
char numero='1';
char numero1='1';
char numero2='1';
char numero3='1';
char numero4='1';

void main(){
SET_TRIS_A(0x1f); //las primeras 5 bits menores como entrada
SET_TRIS_B(0x00);
SET_TRIS_C(0x00);
SET_TRIS_D(0x00); // lo pongo todo como salida del 4 al 7 datos
SET_TRIS_E(0x03); // los pirmero 2 bits como entradas
portb=0;
portd=0;
portc=0;
int boton=0;
lcd_init();//inicializa lcd
pantalla_inicial();
setup_timer_0(RTCC_INTERNAL|RTCC_DIV_256);
enable_interrupts(INT_TIMER0);
enable_interrupts(GLOBAL);       //Habilita interrupcion global
setup_adc_ports(AN0);// PUERTO A0 COMO ANAL
setup_adc(ADC_CLOCK_INTERNAL); // configuro ADC
set_TIMER0(50); //inicializa el timer0 esta configuraacion con el timer 217 da aprox 10ms
muestratemp=0;
while(true){
   boton = chequearbotones();
   switch(boton){
      case 0: 
      break;  
      case 2:
      if(armada ==1 && pantalla!=1){
         if (numero<'4'){
            numero=numero+1;
         }else{
            numero='1';
         }
         if (posicion==1){
            lcd_bytes(0x86,0);
         }
         if (posicion==2){
            lcd_bytes(0x87,0);
         }
         if (posicion==3){
            lcd_bytes(0x88,0);
         }
         if (posicion==4){
            lcd_bytes(0x89,0);
         }
         lcd_bytes(numero,1);
      }    
      break;
      
      case 3:
         if(armada ==1 && pantalla!=1){
            if (numero>'1'){
               numero=numero-1;
            }else{
               numero='4';
            }
            if (posicion==1){
               lcd_bytes(0x86,0);
            }
            if (posicion==2){
               lcd_bytes(0x87,0);
            }
            if (posicion==3){
              lcd_bytes(0x88,0);
            }
            if (posicion==4){
               lcd_bytes(0x89,0);
            }
            lcd_bytes(numero,1);
         }
      break;
      
      case 4:
         if(armada ==1 && pantalla!=1){
            if (posicion>1){
               posicion=posicion-1;
               if(posicion==3){
                  numero=numero3;
                  lcd_bytes(0x89,0);
                  lcd_bytes('-',1); 
               }
               if(posicion==2){
                  numero=numero2;
                  lcd_bytes(0x88,0);
                  lcd_bytes('-',1);  
               }
               if(posicion==1){
                  numero=numero1;
                  lcd_bytes(0x87,0);
                  lcd_bytes('-',1);
               }
            }
         }
         if(pantalla==0 && armada==0){
            lcd_string(borrar,0xc0,16);
            lcd_string(mensaje2,0xc0,16);
            pantalla=1;
            armada=0;
            OUTPUT_HIGH(LED_VERDE);
            OUTPUT_LOW(LED_ROJO); 
         }
      break;
      
      case 1:
        if(pantalla==1 && armada==0){
         activarconteo();
        }
        if((pantalla==0 || alarma==1) && armada==1){
         switch (posicion){
            case 1:
               numero1=numero;
               posicion=2;
               lcd_bytes(0x87,0);
               lcd_bytes(numero,1);
            break;
            case 2:
               numero2=numero;
               posicion=3;
               lcd_bytes(0x88,0);
               lcd_bytes(numero,1);
            break;
            case 3:
               numero3=numero;
               posicion=4;
               lcd_bytes(0x89,0);
               lcd_bytes(numero,1);
            break;
            case 4:
               numero4=numero;
               if(numero1==clave1){
                  if(numero2==clave2){
                     if(numero3==clave3){
                        if(numero4==clave4){
                           alarma=0;
                           armada=0;
                           OUTPUT_LOW(SIRENA);
                           OUTPUT_LOW(LUCES);
                           pantalla=1;
                           pantalla_inicial();
                       }
                     }
                  }       
               }
            break;
         }
        }
      break;
   }  
}
}
void activarconteo(){
   lcd_string(borrar,0xc0,16);
   lcd_string(mensaje9,0xc0,16);
   pantalla=0;
   valor=60;
   conteo=100;
   conteo1=50;
}
int chequearbotones(){
   if(!input(BTN_OK)){
      delay_ms(50);
      if(!input(BTN_OK)){
         while(!input(BTN_OK)){}
            return 1;
      }
   }
   if(!input(BTN_MAS)){
      delay_ms(50);
      if(!input(BTN_MAS)){
         while(!input(BTN_MAS)){}
            return 2;
      }
   }
   if(!input(BTN_MENOS)){
      delay_ms(50);
      if(!input(BTN_MENOS)){
         while(!input(BTN_MENOS)){}
            return 3;
      }
   }
   if(!input(BTN_CANCELAR)){
      delay_ms(50);
      if(!input(BTN_CANCELAR)){
         while(!input(BTN_CANCELAR)){}
            return 4;
      }
   }
   return 0;
   
}

void pantalla_inicial(){
   lcd_string(mensaje1,0x80,16);
   lcd_string(mensaje2,0xC0,16);
   pantalla=1;
   OUTPUT_HIGH(LED_VERDE);
   OUTPUT_LOW(LED_ROJO);
}
//Inicia display
void lcd_init(){
  lcd_bytes(0x33,0);   // 0011 0011 Initialise
  lcd_bytes(0x32,0); // 0011 0010 Initialise
  lcd_bytes(0x06,0); // 0000 0110 Cursor move direction
  lcd_bytes(0x0C,0); // 0000 1100 Display On,Cursor Off, Blink Off
  lcd_bytes(0x28,0); // 0010 1000 Data length, number of lines, font size
  lcd_bytes(0x01,0); // 0000 0001 Clear display
  delay_ms(10);
}
void lcd_bytes(int bits,int mode){
   int dato;
   //chequea si es un caracter o instruccion
   if (mode == 0) {
      OUTPUT_LOW(LCD_RS);
   }else{
      OUTPUT_HIGH(LCD_RS);
   }
//envio primero el nibble de mayor peso
   dato = bits & 0xf0;
   portd &= 0x0f;
   portd = portd | dato;
   lcd_toggle_enable();
// muevo 4 posiciones los bits y mando el otro nibble
   dato = bits << 4 ;
   dato = dato & 0xf0;
   portd &= 0x0f;
   portd = portd | dato;
   lcd_toggle_enable();
}

void lcd_toggle_enable(){
   delay_ms(10);
   OUTPUT_HIGH(LCD_E);
   delay_ms(10);
   OUTPUT_LOW(LCD_E);
   delay_ms(10);
}
//manda los mensajes y deja el cursor en la posicion siguiente
void lcd_string(char mensaje[],int line,int t){
lcd_bytes(line,0);
int i=0;
while(i<t){
lcd_bytes(mensaje[i],1);
i++;
}
}




//interrupcion por timer.. para chequear temperatura y hacer prender o apagar los leds de indicadores
#int_TIMER0
void  TIMER0_isr(void) 
{
if ( pantalla==1){
int temper;
int8 decimal;
int8 unidad;
int medicion;
muestratemp=muestratemp + 1;
if (muestratemp == 200){
set_adc_channel (0);                // Elige canal a medir RA0
delay_us (20);
medicion=read_adc ();              // Hace conversión AD 
temper=medicion*(0.48852);     // Pasa binario a °C
decimal=(temper/10)& 0b00001111;
unidad=((temper - (decimal*10)) & 0b00001111);
decimal= decimal | 0x30;
unidad= unidad | 0x30;
lcd_bytes(0x0C,0);
lcd_bytes(0xcc,0);
lcd_bytes(decimal,1);
lcd_bytes(0xcd,0);
lcd_bytes(unidad,1);
muestratemp=0;
}
}
if(armada==1){
     if (conteo==0){
     chequeosensores();
      conteo=50;
     }else{
     conteo=conteo-1;
     }
}
if(alarma==1){
sonando();
}
if(pantalla==0){
   int decimal=0;
   int unidad=0;
   if (conteo1==0){
   if (armada==0){
   output_toggle(LED_VERDE);}else{
   output_toggle(LED_ROJO);
   }
   conteo1=50;
   }else{
   conteo1=conteo1-1;
   }
   
   if(conteo == 0){
      valor=valor-1;
      decimal = (valor/10)& 0b00001111;
      unidad= ((valor-(decimal*10))& 0b00001111);
      decimal = decimal | 0x30;
      unidad = unidad | 0x30;
      lcd_bytes(0xc5,0);
      lcd_bytes(decimal,1);
      lcd_bytes(0xc6,0);
      lcd_bytes(unidad,1);
      if(valor==0){
         if(armada==0){
            pantalla=1;
            lcd_string(mensaje3,0xc0,16);
            OUTPUT_LOW(LED_VERDE);
            armada=1;
            OUTPUT_HIGH(LUCES);
            delay_ms(1000);
            OUTPUT_LOW(LUCES);
            valor=60;
            OUTPUT_HIGH(LED_ROJO);
         }else{
            pantalla=3;
            alarma=1;
            conteo3= 12000;
            conteo1=50;
            OUTPUT_HIGH(LUCES);
            OUTPUT_HIGH(SIRENA);
            lcd_string(borrar,0xc0,16);
            lcd_string(mensaje10,0xc0,16);
         }
      }
      conteo=100;
   }else{
   conteo=conteo-1;  
   }
}

set_TIMER0(217); //inicializa el timer0 ese
}

int chequeosensores(){
if(!input(SENSOR_MAG)){
      delay_ms(50);
      if(!input(SENSOR_MAG)){
        lcd_string(borrar,0xc0,16);
        lcd_string(mensaje9,0xc0,16);
        lcd_string(borrar,0x80,16);
        lcd_bytes(0x86,0);
        lcd_bytes('1',1);
        lcd_bytes('-',1);
        lcd_bytes('-',1);
        lcd_bytes('-',1);
        pantalla=0;
        posicion=1;
        numero1='1';
        
      }
   }
    return 0;  
}
void sonando(){
if(alarma==1){
   if(conteo3 == 0){
   OUTPUT_LOW(SIRENA);
   OUTPUT_LOW(LUCES);
   if (conteo1==0){
   output_toggle(LED_VERDE);
   output_toggle(LED_ROJO);
   conteo1=50;
   
   }else{
   conteo1=conteo1-1;
   }
     
   }else{
   conteo3=conteo3-1;
   
   }
}
}
