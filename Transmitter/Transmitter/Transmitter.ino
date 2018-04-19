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

struct Input
{
	int thrust = 0;
	int pitch = 0;
	int roll = 0;
	int yaw = 0;
	bool STOP = false;
};

Input inputs;

void setup() {

	Serial.begin(9600);

	Serial.println("SimpleTx Starting");

	pinMode(2, INPUT_PULLUP); // right
	pinMode(3, INPUT_PULLUP); // left

	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	radio.setRetries(3, 5); // delay, count
	radio.setAutoAck(false);
	radio.setPALevel(RF24_PA_MIN);
	radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		if (!inputs.STOP)
		{
			if (digitalRead(2) == LOW && digitalRead(3) == LOW)
			{
				inputs.STOP = true;
				inputs.thrust = 0;
				inputs.pitch = 0;
				inputs.roll = 0;
				inputs.yaw = 0;
			}
			inputs.thrust = analogRead(A0);
			inputs.pitch = analogRead(A3);
			inputs.roll = analogRead(A1);
			if (digitalRead(2) == LOW)
			{
				inputs.yaw = 1;
			}
			else if (digitalRead(3) == LOW)
			{
				inputs.yaw = -1;
			}
			else
			{
				inputs.yaw = 0;
			}
		}
		else
		{
			inputs.STOP = true;
		}
		send();
		prevMillis = millis();
	}
}

//====================

void send() {

	radio.write(&inputs, sizeof(inputs));
	// Always use sizeof() as it gives the size as the number of bytes.
	// For example if dataToSend was an int sizeof() would correctly return 2
	Serial.println(inputs.thrust);
}