#include <OneWire.h>
#include <DallasTemp/DallasTemperature.h>
#include "arduino-libs-manchester-master/Manchester.h"

#define ONE_WIRE_BUS 8
#define TX_PIN 7 //pin where your transmitter is connected

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp = 0.0f;
float currentTime = 0.0f;
uint8_t data[5] = { 4, 0, 0, 0, 0 };

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	sensors.setResolution(12);
	sensors.begin();

	//man.workAround1MhzTinyCore(); //add this in order for transmitter to work with 1Mhz Attiny85/84
	man.setupTransmit(TX_PIN, MAN_1200);

	currentTime = millis();
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - currentTime > 3000)
	{
		//read temp
		sensors.requestTemperatures();
		temp = sensors.getTempCByIndex(0);
		// check if negative
		if (temp < 0.0f)
		{
			temp += 100.0f;
			data[3] = 1;
		}
		else
		{
			data[3] = 0;
		}
		// limits
		float decs = temp;
		fmod(decs, 10);
		decs * 1000;
		fmod(decs, 10);
		if (decs > 5.0f)
		{
			temp += 0.01f;
		}
		// math
		data[1] = (int)temp;
		data[2] = (int)fmod(temp*100,100);
		for (int i = 0; i < 2; i++)
		{
			Serial.println(data[i+1]);
		}
		Serial.println(data[1] + data[2] / 100.f);
		// send
		for (size_t i = 0; i < 1; i++)
		{
			man.transmitArray(sizeof(data), data);
			delay(200);
		}
		
		currentTime = millis();
	}
}
