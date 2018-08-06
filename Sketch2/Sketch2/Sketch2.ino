#include <OneWire.h>
#include <DallasTemp/DallasTemperature.h>
#include "virtualwire/VirtualWire.h"

#define ONE_WIRE_BUS 7

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp = 0;
float currentTime = 0.0f;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	sensors.begin();
	sensors.setResolution(12);

	vw_set_tx_pin(6);
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_setup(2000);	 // Bits per sec

	currentTime = millis();
}

// the loop function runs over and over again until power down or reset
void loop() {

	if (millis() - currentTime > 2500)
	{
		sensors.setResolution(12);
		sensors.setWaitForConversion(true);
		sensors.requestTemperatures();
		temp = sensors.getTempCByIndex(0);
		vw_send((uint8_t*) &temp, sizeof(temp));
		currentTime = millis();
		Serial.println(temp);
	}
}
