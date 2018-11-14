// Markos Ferro 2018-11-14
// Configuracion ajustada para TFT LCD Tactil de 2.4" de Asterkat para Arduino Mega 2560 https://www.ebay.es/itm/LCD-tactil-2-4-Pantalla-COLOR-TFT-Display-TOUCH-SCREEN-Shield-ARDUINO-MEGA/223177201325?hash=item33f665d6ad:g:j-wAAOSwQSNamcEs:rk:1:pf:0

// Modified TFT_paint sketch from Adafruit_TFTLCD Library for
// TFT shield LCD 2.4" Chip ILI9341
// http://www.electronicavm.net
// @iPadNanito

#include <Adafruit_GFX.h>    // Libreria de graficos
#include <Adafruit_TFTLCD.h> // Libreria de LCD 
#include <TouchScreen.h>     // Libreria del panel tactil

 // Pines necesarios para los 4 pines del panel tactil
#define YP A2 // Pin analogico A1 para ADC
#define XM A1 // Pin analogico A2 para ADC
#define YM 6 
#define XP 7 

// Definimos la presion máxima y minima que podemos realizar sobre el panel
#define MINPRESSURE 1
#define MAXPRESSURE 1000

// Para mejor precision de la presion realizada, es necesario 
// medir la resistencia entre los pines X+ y X-.
// En Shield TFT 2.4" LCD se mide entre los pines A2 y 6
// Instancia del panel tactil (Pin XP, YP, XM, YM, Resistencia del panel) 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364); 

short TS_MINX = 175; // Coordenadas del panel tactil para delimitar
short TS_MINY = 165; // el tamaño de la zona donde podemos presionar
short TS_MAXX = 881; // y que coincida con el tamaño del LCD
short TS_MAXY = 915; 

#define LCD_CS A3   // Definimos los pines del LCD
#define LCD_CD A2   // para poder visualizar elementos graficos
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define	BLACK   0x0000  // Definimos los colores 
#define	BLUE    0x001F  // que utilizaremos para 
#define	RED     0xF800  // el texto y los elementos graficos
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); // Instancia LCD 

int X; // Variables que almacenaran la coordenada
int Y; // X, Y donde presionemos y la variable Z 
int Z; // almacenara la presion realizada

int a = 0; // variable "flag" para control rebotes

#define ledA 21
#define ledK 20

void setup(void) 
{ 
  tft.begin(0x9341); // Iniciamos el LCD especificando el controlador ILI9341. 
  
  tft.fillScreen(CYAN); // Pintamos la pantalla de Rojo

  pinMode(13, OUTPUT);
  pinMode(21, OUTPUT); // ANODO DEL LED
  pinMode(20, OUTPUT); // CATODO DEL LED
  
  tft.setRotation(0); // Establecemos la posición de la pantalla Vertical u Horizontal
  
  tft.drawRect(40, 20, 160, 60, RED); // Dibujamos un "boton"
  
  tft.setCursor(60,35);  // Colocamos el cursor
  tft.setTextSize(4);    // Especificamos el tamaño del texto
  tft.setTextColor(RED); // Definimos el color del texto 
  tft.println("BOTON"); // Escribimos por pantalla     
}

void loop()
{          
    lecturaPanel(); // Realizamos lectura del panel para detectar presion y coordenadas
      
    // Si la pulsación del eje X se produce entre los puntos 40 y 160
    // Y la pulsacion del eje Y se produce entre los puntos 20 y 60
    // Y la presión realizada esta entre el margen determinado
    if((X > 40 && X < 200) && (Y > 20 && Y < 60) && (Z > MINPRESSURE && Z < MAXPRESSURE)) 
    {
      if (a == 0) // Si la variable flag esta en 0
      {
        tft.fillRect(40, 20, 160, 60, RED); // Dibujamos nuestro boton relleno de color
        
        tft.setCursor(70,25); // Colocamos el cursor
        tft.setTextSize(3);   // Especificamos el tamaño del texto
        tft.setTextColor(WHITE); // Definimos el color del texto 
        tft.println("BOTON"); // Escribimos por pantalla
        tft.setCursor(60,50);  // Colocamos el cursor
        tft.println("PULSADO"); // Escribimos por pantalla   
     
        digitalWrite(ledA, HIGH);
        digitalWrite(ledK, LOW);

        a = 1; // Ponemos la variable flag en 1
        delay(150);
      }
      else if (a == 1) // Si la variable flag esta en 1
      {
         tft.fillRect(41, 21, 158, 58, CYAN); // Dibujamos el fondo de nuestro boton en cyan
         
         tft.setCursor(60,35); // Colocamos el cursor
         tft.setTextSize(4); // Especificamos el tamaño del texto
         tft.setTextColor(RED); // Definimos el color del texto 
         tft.println("BOTON"); // Escribimos por pantalla
         
        digitalWrite(ledA, LOW);
        digitalWrite(ledK, LOW);
         
         a = 0; // Ponemos la variable flag en 0 para evitar los rebotes
         delay(150);
      }
    }    
}

void lecturaPanel()
{
    digitalWrite(13, HIGH); 
    TSPoint p = ts.getPoint(); // Realizamos lectura de las coordenadas
    digitalWrite(13, LOW);
  
    pinMode(XM, OUTPUT); // La librería utiliza estos pines como entrada y salida
    pinMode(YP, OUTPUT); // por lo que es necesario declararlos como salida justo
                         // despues de realizar una lectura de coordenadas.    
  
    // Mapeamos los valores analogicos leidos del panel tactil (0-1023)
    // y los convertimos en valor correspondiente a la medida del LCD 320x240
    X = map(p.x, TS_MAXX, TS_MINX, tft.width(), 0);
    Y = map(p.y, TS_MAXY, TS_MINY, tft.height(), 0);
    Z = p.z;
}



