#include <OneWire-master/OneWire.h>
#include "SevSeg-master/SevSeg.h"
#include "DallasTemp/DallasTemperature.h"
#include "virtualwire/VirtualWire.h"

SevSeg sevseg;

float temp = 0.0f;
uint8_t len = sizeof(temp);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	
	byte numDigits = 4;
	byte digitPins[] = { A2, A1, A0, A3 };
	byte segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins,false,true,false);
	sevseg.setChars("----");

	vw_set_rx_pin(10);
	vw_set_ptt_inverted(true);
	vw_set_ptt_pin(13);
	vw_setup(2000);
	vw_rx_start();
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (vw_get_message((uint8_t *)&temp, &len))
	{
		sevseg.setNumber(temp, 2);
	}
	sevseg.refreshDisplay();
}



/*
/*
Showing number 0-9 on a Common Anode 7-segment LED display
Displays the numbers 0-9 on the display, with one second inbetween.
A
---
F |   | B
| G |
---
E |   | C
|   |
---
D
This example code is in the public domain.


// Pin 2-8 is connected to the 7 segments of the display.

int pinA = 2;
int pinB = 3;
int pinC = 4;
int pinD = 5;
int pinE = 6;
int pinF = 7;
int pinG = 8;
int pinH = 9;
int D1 = 10;
int D2 = 11;
int D3 = 12;
int D4 = 13;

// the setup routine runs once when you press reset:
void setup() {
	// initialize the digital pins as outputs.
	pinMode(pinA, OUTPUT);
	pinMode(pinB, OUTPUT);
	pinMode(pinC, OUTPUT);
	pinMode(pinD, OUTPUT);
	pinMode(pinE, OUTPUT);
	pinMode(pinF, OUTPUT);
	pinMode(pinG, OUTPUT);
	pinMode(pinH, OUTPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
	digitalWrite(D1, LOW);
	digitalWrite(D2, HIGH);
	digitalWrite(D3, HIGH);
	digitalWrite(D4, HIGH);
	//0
	digitalWrite(pinA, HIGH);
	digitalWrite(pinB, HIGH);
	digitalWrite(pinC, HIGH);
	digitalWrite(pinD, HIGH);
	digitalWrite(pinE, HIGH);
	digitalWrite(pinF, HIGH);
	digitalWrite(pinG, HIGH);
	digitalWrite(pinH, HIGH);
	delay(1);
}

*/