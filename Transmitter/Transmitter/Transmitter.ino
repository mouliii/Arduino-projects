// SimpleTx - the master or the transmitter
#include <SPI.h>
#include <VirtualWire/VirtualWire.h>



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

	vw_set_tx_pin(7);
	vw_setup(2000);
	vw_set_ptt_inverted(true);
}

//====================

void loop() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		inputs.thrust = analogRead(A0);
		inputs.pitch = analogRead(A4);
		inputs.roll = analogRead(A6);
		send();
		prevMillis = millis();
	}
}

//====================

void send() {

	vw_send((uint8_t*)&inputs, sizeof(inputs));
	Serial.println(inputs.thrust);
}
