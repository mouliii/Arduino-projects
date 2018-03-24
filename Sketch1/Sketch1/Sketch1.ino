

// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress = 76;


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

int potValue = 0;


unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4; // send once per second


void setup() {

	Serial.begin(9600);

	Serial.println("SimpleTx Starting");

	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	radio.setRetries(3, 5); // delay, count
	radio.setAutoAck(false);
	radio.setPALevel(RF24_PA_MAX);
	radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		potValue = analogRead(A0);
		send();
		prevMillis = millis();
	}
}

//====================

void send() {

	radio.write(&potValue, sizeof(potValue));
	// Always use sizeof() as it gives the size as the number of bytes.
	// For example if dataToSend was an int sizeof() would correctly return 2
	Serial.print("Pot Value ");
	Serial.println(potValue);

}
