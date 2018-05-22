// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress = 76;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4; // send once per second


int msg = 0;

void setup() {

	Serial.begin(9600);

	Serial.println("SimpleTx Starting");

	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setRetries(3, 5); // delay, count
	radio.setAutoAck(false);
	radio.setPALevel(RF24_PA_MAX);
	radio.openWritingPipe(slaveAddress);


}

//====================

void loop() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		send();
		prevMillis = millis();
	}
}

//====================

void send() {
	if (++msg > 9)
	{
		msg = 0;
	}
	radio.write(&msg, sizeof(msg));
	Serial.println(msg);
}