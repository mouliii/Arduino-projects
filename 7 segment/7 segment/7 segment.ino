#include <OneWire-master/OneWire.h>
#include "SevSeg-master/SevSeg.h"
#include "DallasTemp/DallasTemperature.h"
#include <arduino-libs-manchester-master/Manchester.h>

#define RX_PIN 10 //pin where your transmitter is connected

SevSeg sevseg;

float temp = 0.0f;
uint8_t data[5] = { 4, 0, 0, 0, 0 };
float c1 = 0.0f;
float c2 = 0.0f;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	man.setupReceive(RX_PIN, MAN_1200);
	man.beginReceiveArray(sizeof(data), data);
	byte numDigits = 4;
	byte digitPins[] = { A0, A1, A2, A3 };
	byte segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins,false,true,true);
	sevseg.setBrightness(47);
	sevseg.setChars("----");

}

// the loop function runs over and over again until power down or reset
void loop() {
	if (man.receiveComplete()) {
		Serial.println("msg");
		Serial.println(data[0]);
		Serial.println(data[1]);
		Serial.println(data[2]);
		uint8_t receivedSize = data[0];
		//do something with the data in 'buffer' here before you start receiving to the same buffer again
		c1 = data[1];
		c2 = data[2];
		if (data[3] == 1)
		{
			c1 -= 100;
		}
		float c3 = c1 + c2 / 100.f;
		Serial.println(c3);
		if (c3 > 0.0f)
		{
			sevseg.setNumber(c3, 2);
		}
		else if (c3 < 0.0f && c3 > -10.0f)
		{
			//offset leading zeroes ?
			sevseg.setNumber(c3, 2);
			sevseg.setChars("-");
		}
		else if (c3 < -10.0f)
		{
			//offset ^ ^ ^ ^ ?
			sevseg.setNumber(c3, 1);
			sevseg.setChars("-");
		}
		
		man.beginReceiveArray(sizeof(data), data);
	}

	sevseg.refreshDisplay();
}
