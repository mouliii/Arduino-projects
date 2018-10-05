#include <OneWire-master/OneWire.h>
#include "SevSeg-master/SevSeg.h"
#include <VirtualWire.h>

#define RX_PIN 10 //pin where your transmitter is connected

SevSeg sevseg;

float temp = 0.0f;
uint8_t len = sizeof(temp);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	// Initialise the IO and ISR
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_set_rx_pin(10);
	vw_setup(2000); // Bits per sec
	vw_rx_start(); // Start the receiver PLL running
	// screen
	byte numDigits = 4;
	byte digitPins[] = { A0, A1, A2, A3 };
	byte segmentPins[] = { 2, 3, 4, 5, 6, 7, 8, 9 };
	sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins,false,true,true);
	sevseg.setBrightness(47);
	sevseg.setChars("----");

}

// the loop function runs over and over again until power down or reset
void loop() {
	
	if (vw_get_message((uint8_t *)&temp, &len))
	{
		Serial.println(temp);
		if (temp > 0.0f)
		{
			sevseg.setNumber(temp, 2);
		}
		else if (temp < 0.0f && temp > -10.0f)
		{
			//offset leading zeroes ?
			sevseg.setNumber(temp, 2);
			sevseg.setChars("-");
		}
		else if (temp < -10.0f)
		{
			//offset ^ ^ ^ ^ ?
			sevseg.setNumber(temp, 1);
			sevseg.setChars("-");
		}
	}

	sevseg.refreshDisplay();
}



/*

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

*/