
#include <Wire.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte radioAddress = 76;

RF24 radio(CE_PIN, CSN_PIN);

long loop_timer;
int data;

void setup() {

	Serial.begin(9600);
	// radio
	Serial.println("SimpleRx Starting");

	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);

	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setPALevel(RF24_PA_MIN);
	radio.setAutoAck(false);
	radio.openReadingPipe(1, radioAddress);
	radio.startListening();

	//Reset the loop timer
	loop_timer = micros();
}

void loop() {
	GetTransmitterData();
}

void GetTransmitterData() {

	if (radio.available()) {
		radio.read(&data, sizeof(data));
		Serial.println(data);
	}
}
