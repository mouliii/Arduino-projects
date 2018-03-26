// SimpleTx - the master or the transmitter
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress = 76;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4; // send once per second

struct Input
{
	int thrust = 0;
	int pitch = 0;
	int roll = 0;
	int yaw = 0;
};

Input inputs;


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
		inputs.thrust = analogRead(A0);
		inputs.pitch = analogRead(A1);
		inputs.roll = analogRead(A2);
		send();
		prevMillis = millis();
	}
}

//====================

void send() {

	radio.write(&inputs, sizeof(inputs));
	// Always use sizeof() as it gives the size as the number of bytes.
	// For example if dataToSend was an int sizeof() would correctly return 2
	Serial.print("thrust ");
	Serial.println(inputs.thrust);
	Serial.print("roll ");
	Serial.println(inputs.roll);
	Serial.print("pitch ");
	Serial.println(inputs.pitch);

}
