#include <Wire.h>
#include <OneWire.h>
#include <DallasTemp/DallasTemperature.h>
#include <Arduino-LiquidCrystal-I2C-library-master/LiquidCrystal_I2C.h>

#define ONE_WIRE_BUS 13

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float celcius = 0;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	sensors.begin();
	sensors.setResolution(10);


}

// the loop function runs over and over again until power down or reset
void loop() {
	sensors.requestTemperatures();
	celcius = sensors.getTempCByIndex(0);
	delay(1000);
	Serial.println(celcius);
}
