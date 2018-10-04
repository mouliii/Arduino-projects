#include <Manchester.h>


#define RX_PIN 7 //pin where your transmitter is connected


uint8_t data[5] = { 4, 0, 0, 0, 0 };
float c1 = 0.0f;
float c2 = 0.0f;
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	man.setupReceive(RX_PIN, MAN_1200);
	man.beginReceiveArray(sizeof(data), data);
}

void loop() {
	if (man.receiveComplete()) {
		Serial.println("asd?");
		uint8_t receivedSize = data[0];
		//do something with the data in 'buffer' here before you start receiving to the same buffer again
		c1 = data[1];
		c2 = data[2];
		if (data[3] == 1)
		{
			c1 -= 100;
		}
		float c3 = c1 + c2 / 100.f;
		man.transmitArray(sizeof(data), data);
		Serial.println( c3 );

		man.beginReceiveArray(sizeof(data), data);
	}
}
