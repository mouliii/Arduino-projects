

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

int potValue = 0; // this must match dataToSend in the TX
bool newData = false;

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
	esc.attach(2);
}

//=============

void loop() {
	getData();
	showData();
}

//==============

void getData() {
	if (radio.available()) {
		radio.read(&potValue, sizeof(potValue));
		potValue = map(potValue, 0, 1023, 0, 255);
		esc.write(potValue);
		newData = true;
	}
}

void showData() {
	if (newData == true) {
		Serial.print("Data received ");
		Serial.println(potValue);
		newData = false;
	}
}
