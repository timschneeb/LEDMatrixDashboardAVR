#ifndef BMP280_H_ 
#define BMP280_H_
bool initBMP280();
float readTemperature();
float readPressure();
float readAltitude();
#endif