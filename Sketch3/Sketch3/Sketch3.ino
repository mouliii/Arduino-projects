char foo;
#include <OneWire.h>
#include <DallasTemp/DallasTemperature.h>
#include "arduino-libs-manchester-master/Manchester.h"

#define ONE_WIRE_BUS 1
#define TX_PIN 0 //pin where your transmitter is connected

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp = 0.0f;
float currentTime = 0.0f;
uint8_t data[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

// the setup function runs once when you press reset or power the board
void setup() {




	sensors.setResolution(12);
	sensors.begin();

	man.workAround1MhzTinyCore(); //add this in order for transmitter to work with 1Mhz Attiny85/84
	man.setupTransmit(TX_PIN, MAN_1200);

	currentTime = millis();
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - currentTime > 30000)
	{
		sensors.setResolution(12);
		sensors.requestTemperatures();
		temp = sensors.getTempCByIndex(0);
		for (size_t i = 0; i < 3; i++)
		{
			man.transmitArray(sizeof(temp), (uint8_t *)&temp);
			delay(200);
		}

		currentTime = millis();
	}
}
