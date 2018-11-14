// Markos Ferro 2018-11-14
// Configuracion ajustada para TFT LCD Tactil de 2.4" de Asterkat para Arduino Mega 2560 https://www.ebay.es/itm/LCD-TFT-2-4-Display-Touch-240x320-SPI-Pantalla-Arduino-UNO-Mega-Raspberry-P001/201591428229?hash=item2eefc91485:g:Gk8AAOSwcgNZDEi0:rk:2:pf:0

// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

#include <Adafruit_GFX.h>    // Libreria de graficos
#include <Adafruit_TFTLCD.h> // Libreria de LCD 
#include <stdint.h>
#include "TouchScreen.h"

 // Pines necesarios para los 4 pines del panel tactil
#define YP A2  // Pin analogico A1 para ADC
#define XM A1  // Pin analogico A2 para ADC
#define YM 6 
#define XP 7 

short TS_MINX = 175; // Coordenadas del panel tactil para delimitar
short TS_MINY = 165; // el tamaño de la zona donde podemos presionar
short TS_MAXX = 881; // y que coincida con el tamaño del LCD
short TS_MAXY = 915; 

// Definimos la presion máxima y minima que podemos realizar sobre el panel
#define MINPRESSURE 1
#define MAXPRESSURE 1000

// Para mejor precision de la presion realizada, es necesario 
// medir la resistencia entre los pines X+ y X-.
// En Shield TFT 2.4" LCD se mide entre los pines A2 y 6
// Instancia del panel tactil (Pin XP, YP, XM, YM, Resistencia del panel) 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364); 

#define	BLACK   0x0000  // Definimos los colores 
#define	BLUE    0x001F  // que utilizaremos para 
#define	RED     0xF800  // el texto y los elementos graficos
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define LCD_CS A3   // Definimos los pines del LCD
#define LCD_CD A2   // para poder visualizar elementos graficos
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); // Instancia LCD 

#define BOXSIZE 40 // Tamaño de los cajetines de colores
#define BOXSIZEY 30 // Tamaño de los cajetines de colores
#define PENRADIUS 2 // Tamaño del cursor a la hora de pintar 

int oldcolor, currentcolor; // Colores del cursor

void setup(void) 
{
  tft.begin(0x9341); // Iniciamos el LCD especificando el controlador ILI9341. 

  tft.fillScreen(BLACK); // Pintamos el fondo de negro

  tft.fillRect(0, 10, BOXSIZE, BOXSIZEY, RED);  // Dibujamos los cajetines de los colores
  tft.fillRect(BOXSIZE, 10, BOXSIZE, BOXSIZEY, YELLOW);
  tft.fillRect(BOXSIZE*2, 10, BOXSIZE, BOXSIZEY, GREEN);
  tft.fillRect(BOXSIZE*3, 10, BOXSIZE, BOXSIZEY, CYAN);
  tft.fillRect(BOXSIZE*4, 10, BOXSIZE, BOXSIZEY, BLUE);
  tft.fillRect(BOXSIZE*5, 10, BOXSIZE, BOXSIZEY, MAGENTA);
  tft.fillRect(0, 0, 240, 10, WHITE);
  tft.drawRect(0, 10, BOXSIZE, BOXSIZEY, WHITE);
  
  currentcolor = RED; // Color seleccionado 
 
  pinMode(13, OUTPUT);
}


void loop()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint(); // Obtenemos la lectura del panel
  digitalWrite(13, LOW);
  
  pinMode(XM, OUTPUT); 
  pinMode(YP, OUTPUT);

  // Si se detecta presion sobre el panel
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
  { 
    // Si la pulsacion es el en cuadro blanco, 
    if ((p.y > TS_MAXY-20) && (p.x > TS_MINX && p.x < TS_MAXX)) 
    {                    
      // Pintamos la pantalla de negro para "borrar" el dibujo
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZEY, BLACK); 
    }
  
    // Mapeamos los valores analogicos leidos del panel tactil (0-1023)
    // y los convertimos en valor correspondiente a la medida del LCD 320x240
    p.x = map(p.x, TS_MAXX, TS_MINX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    if (p.y < BOXSIZE) // Si pulsamos en un cajetin, asignamos el color de dicho cajetin 
    {                  // al color actual seleccionado
    
       oldcolor = currentcolor;

       if (p.x < BOXSIZE) 
       { 
         currentcolor = RED; 
         tft.drawRect(0, 10, BOXSIZE, BOXSIZEY, WHITE);
       } 
       else if (p.x < BOXSIZE*2) 
       {
         currentcolor = YELLOW;
         tft.drawRect(BOXSIZE, 10, BOXSIZE, BOXSIZEY, WHITE);
       } 
       else if (p.x < BOXSIZE*3) 
       {
         currentcolor = GREEN;
         tft.drawRect(BOXSIZE*2, 10, BOXSIZE, BOXSIZEY, WHITE);
       }
       else if (p.x < BOXSIZE*4) 
       {
         currentcolor = CYAN;
         tft.drawRect(BOXSIZE*3, 10, BOXSIZE, BOXSIZEY, WHITE);
       } 
       else if (p.x < BOXSIZE*5) 
       {
         currentcolor = BLUE;
         tft.drawRect(BOXSIZE*4, 10, BOXSIZE, BOXSIZEY, WHITE);
       } 
       else if (p.x < BOXSIZE*6) 
       {
         currentcolor = MAGENTA;
         tft.drawRect(BOXSIZE*5, 10, BOXSIZE, BOXSIZEY, WHITE);
       }

       if (oldcolor != currentcolor) 
       {
          if (oldcolor == RED) tft.fillRect(0, 10, BOXSIZE, BOXSIZEY, RED);
          if (oldcolor == YELLOW) tft.fillRect(BOXSIZE, 10, BOXSIZE, BOXSIZEY, YELLOW);
          if (oldcolor == GREEN) tft.fillRect(BOXSIZE*2, 10, BOXSIZE, BOXSIZEY, GREEN);
          if (oldcolor == CYAN) tft.fillRect(BOXSIZE*3, 10, BOXSIZE, BOXSIZEY, CYAN);
          if (oldcolor == BLUE) tft.fillRect(BOXSIZE*4, 10, BOXSIZE, BOXSIZEY, BLUE);
          if (oldcolor == MAGENTA) tft.fillRect(BOXSIZE*5, 10, BOXSIZE, BOXSIZEY, MAGENTA);
       }
    }
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) 
    {
     tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor); // Dibujamos circulos con el color y tamaño seleccionado
    }
  }
}
