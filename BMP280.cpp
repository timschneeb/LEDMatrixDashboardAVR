#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <Arduino.h>

#define BMP_SCK 52
#define BMP_SDO 50
#define BMP_SDI 51 
#define BMP_CS 53

Adafruit_BMP280 bmp280(BMP_CS, BMP_SDI, BMP_SDO,  BMP_SCK);

bool initBMP280(){
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  return bmp280.begin();
}
float readTemperature(){
    return bmp280.readTemperature();
}
float readPressure(){
    return bmp280.readPressure();
}
float readAltitude(){
    return bmp280.readAltitude(1013.25);
}