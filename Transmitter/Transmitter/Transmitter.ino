// SimpleTx - the master or the transmitter
#include <VirtualWire\VirtualWire.h>

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4; // send once per 4ms

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

	vw_set_tx_pin(12);
	vw_set_ptt_pin(true);
	vw_setup(1000);

}

//====================

void loop() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		if (!inputs.STOP)
		{
			inputs.thrust = analogRead(A0);
			inputs.pitch = analogRead(A1);
			inputs.roll = analogRead(A2);

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
			// HÄTÄ SEIS
			if (digitalRead(2) == LOW && digitalRead(3) == LOW)
			{
				inputs.STOP = true;
				inputs.thrust = 0;
				inputs.pitch = 0;
				inputs.roll = 0;
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

	vw_send((uint8_t*)&inputs, sizeof(inputs));

	Serial.println(inputs.pitch);
	// Always use sizeof() as it gives the size as the number of bytes.
	// For example if dataToSend was an int sizeof() would correctly return 2

}