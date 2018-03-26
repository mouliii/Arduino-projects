

// SimpleRx - the slave or the receiver

#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte thisSlaveAddress = 76;

RF24 radio(CE_PIN, CSN_PIN);
Servo esc;

bool newData = false; // <- debug

struct Input
{
	int thrust = 0;
	int pitch = 0;
	int roll = 0;
	int yaw = 0;
};

Input inputs;

//===========

void setup() {

	Serial.begin(9600);
	// radio
	Serial.println("SimpleRx Starting");
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setPALevel(RF24_PA_MIN);
	radio.setAutoAck(false);
	radio.openReadingPipe(1, thisSlaveAddress);
	radio.startListening();
	// esc
	//esc.attach(2);
}

//=============

void loop() {
	getData();
	showData(); // <- debug
}

//==============

void getData() {
	if (radio.available()) {
		radio.read(&inputs, sizeof(inputs));
		// TODO // 1023 / 2 == 511,5 -> 512 middle
		// roll - 521 center value
		// pitch - 525 center value
		inputs.roll -= (9 - 512);
		inputs.pitch -= (13 - 512); /*TEST*/

		//potValue = map(potValue, 0, 1023, 0, 255);
		//esc.write(potValue);
		
		newData = true; // <- debug
	}
}

void showData() {
	if (newData == true) {
		Serial.print("Data received ");
		Serial.println(); // TODO
		newData = false;
	}
}
