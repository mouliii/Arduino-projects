// SimpleTx - the master or the transmitter
#include <RF24_config.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress = 76;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4; // send once per 4ms
//
int inputs[5] = { 0,0,0,0,0 };

void setup() {

	Serial.begin(9600);

	pinMode(2, INPUT_PULLUP); // right
	pinMode(3, INPUT_PULLUP); // left


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
		
		inputs[0] = map(analogRead(A0), 661, 1023, 0, 100); // kaasu
		inputs[1] = map(analogRead(A1), 1024, 0, -100, 100 + 2); // y
		inputs[2] = map(analogRead(A2), 1024, 0, -100, 100 + 2); // x
		// inputs[3] = digitalRead(3); // left shoulder  
		if (digitalRead(3) == 0)
		{
			inputs[3] = 1;
		}
		else
		{
			inputs[3] = 0;
		}
		// inputs[4] = digitalRead(2); // right shoulder  // buttons inverted
		if (digitalRead(2) == 0)
		{
			inputs[4] = 1;
		}
		else
		{
			inputs[4] = 0;
		} 
		// send inputs
		radio.write(&inputs, sizeof(inputs));
		// update time
		prevMillis = millis();
	}

	//Serial.println(map(inputs[0],661,1023,0,100));  // 661 - 1023 -> 0% - 100%
	//Serial.println(map(inputs[1], 1024, 0, -100, 100) + 2); // y-akseli  1022 - 522 - 0  -> -98% - 102% 
	//Serial.println(map(inputs[2], 1024, 0, -100, 100) + 2); // x-akseli
	//Serial.println(digitalRead(2));

}