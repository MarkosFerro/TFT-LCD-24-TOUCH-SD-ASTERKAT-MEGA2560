// Markos Ferro 2018-11-14
// Configuracion ajustada para TFT LCD Tactil de 2.4" de Asterkat para Arduino Mega 2560 https://www.ebay.es/itm/LCD-tactil-2-4-Pantalla-COLOR-TFT-Display-TOUCH-SCREEN-Shield-ARDUINO-MEGA/223177201325?hash=item33f665d6ad:g:j-wAAOSwQSNamcEs:rk:1:pf:0

// This sketch has been Refurbished by BUHOSOFT
// BMP-loading example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftbmp_shield.pde sketch instead!
// If using an Arduino Mega make sure to use its hardware SPI pins, OR make
// sure the SD library is configured for 'soft' SPI in the file Sd2Card.h.
//No DEBUG Mode: Sketch uses 30.070 bytes (93%) of program storage space. Maximum is 32.256 bytes.
//DEBUG Mode: Sketch uses 31.046 bytes (96%) of program storage space. Maximum is 32.256 bytes.

#define DEBUG
#include <Adafruit_GFX.h>    // Libreria de graficos
#include <Adafruit_TFTLCD.h> // Libreria de LCD 
#include <SD.h>              // Libreria de tarjeta SD
#include <SPI.h>             // Libreria bus SPI 

#define LCD_CS A3   // Definimos los pines del LCD
#define LCD_CD A2   // para poder visualizar elementos graficos
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#if defined __AVR_ATmega2560__ // Para Arduino Uno/Duemilanove, conectamos la tarjeta SD en los pines del puerto SPI
#define SD_SCK 13              // que se correspondencon los pines MOSI -> 11, MISO -> 12 y SCK -> 13 
#define SD_MISO 12  
#define SD_MOSI 11
#endif

// Chip Select del bus SPI correspondiente a la conexion con la tarjeta SD
#define SD_CS 10  

// En la tarjeta SD debemos colocar imagenes en formato BMP de 24 Bits!
// Otro tipo de formato de imagen no se puede visualizar por pantalla. 

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, A4); // Instancia LCD 

void setup()
{
  tft.reset();
  
  tft.begin(0x9341); // Iniciamos el LCD especificando el controlador ILI9341. 
                     // (Otros controladores disponibles: HX8357D, HX8347G, ILI9328, ILI9325)

  tft.fillScreen(0); // Pintamos la pantalla de negro para el fondo.

  #if defined __AVR_ATmega2560__               
  if (!SD.begin(SD_CS, SD_MOSI, SD_MISO, SD_SCK )) 
  {
    tft.println(F("failed!"));       // Definimos los pines de la tarjeta SD para MEGA
    return;
  }
  #else
  if (!SD.begin(SD_CS)) {
    tft.println(F("failed!"));
    return;
  }
  #endif

}

void loop()
{
   tft.setRotation(0); // Establecemos la posicion de la pantalla Vertical
   
   bmpDraw("1v.bmp", 0, 0); // Mostramos una imagen en las coordenadas 0,0
  
   delay(5000);
   
   bmpDraw("2v.bmp",0,0); // // Mostramos otra imagen en las coordenadas 0,0
   
   delay(5000);
   
   tft.setRotation(3); // Establecemos la posicion de la pantalla Horizontal
   
   bmpDraw("1h.bmp", 0, 0); // Mostramos una imagen en las coordenadas 0,0
   
   delay(5000);
   
   bmpDraw("2h.bmp", 0, 0); // Mostramos otra imagen en las coordenadas 0,0
   
   delay(5000);
   
   bmpDraw("3h.bmp", 0, 0); // Mostramos una imagen en las coordenadas 0,0
   
   delay(5000);
   
   bmpDraw("4h.bmp", 0, 0); // Mostramos otra imagen en las coordenadas 0,0
   
   delay(5000);
}

// Esta funcion abre un archivo Windows bitmap (BMP) y lo muestra por 
// pantalla en las coordenadas especificadas. Se puede acelerar el 
// proceso de muestreo leyendo muchos pixeles a la vez en lugar de 
// leer pixel a pixel, incrementando el tamaño de la siguiente variable 
// BUFFPIXEL, utilizaremos mas memoria RAM del Arduino pero se realizará
// la carga de la imagen mas rapido.
// Un buffer de 20 pixeles es un valor equilibrado. 

#define BUFFPIXEL 20

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

#ifdef DEBUG
  tft.fillScreen(0);
  tft.setCursor(0,0);
  tft.print(F("Loading image '"));
  tft.print(filename);
  tft.println('\'');
#endif
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    #ifdef DEBUG
    tft.println(F("File not found"));
    #endif
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) {
      // BMP signature
    #ifdef DEBUG
    tft.println(F("File size: ")); tft.println(read32(bmpFile));
    #else
        read32(bmpFile);
    #endif
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    #ifdef DEBUG
    tft.print(F("Image Offset: ")); tft.println(bmpImageoffset, DEC);
    // Read DIB header
    tft.print(F("Header size: ")); tft.println(read32(bmpFile));
    #else
    read32(bmpFile);
    #endif

    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      #ifdef DEBUG
     // Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      #endif
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        #ifdef DEBUG
        tft.print(F("Image size: "));
        tft.print(bmpWidth);
        tft.print('x');
        tft.println(bmpHeight);
        //delay(3000);
        #endif*/
        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        }

      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) tft.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

