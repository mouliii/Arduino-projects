// SimpleRx - the slave or the receiver

#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte thisSlaveAddress = 76;

RF24 radio(CE_PIN, CSN_PIN);
Servo esc1;
Servo esc2;
Servo esc3;
Servo esc4;

bool newData = false; // <- debug

struct Input
{
	int thrust = 0;
	int pitch = 0;
	int roll = 0;
	int yaw = 0;
};

Input inputs;

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
	esc1.attach(2);
	esc2.attach(3);
	esc3.attach(4);
	esc4.attach(5);
}

//=============

void loop() {
	getData();
	showData(); // <- debug
}

//==============

void getData() {
	if (radio.available()) {
		radio.read(&inputs, sizeof(inputs));
		// joystick raw input zeroed //
		inputs.roll -= 522;
		inputs.pitch -= 525;
		// mapping
		inputs.roll = map(inputs.roll, -522, 501, -20, 20);
		inputs.pitch = map(inputs.pitch, -525, 498, -20, 20);
		inputs.thrust = map(inputs.thrust, 0, 1023, 0, 255);
		// half values
		byte rollHalf = inputs.roll / 2;
		byte pitchHalf = inputs.pitch / 2;
		// calculate and write motors true values
		esc1.write(inputs.thrust + rollHalf + pitchHalf);
		esc2.write(inputs.thrust - rollHalf + pitchHalf);
		esc3.write(inputs.thrust + rollHalf - pitchHalf);
		esc4.write(inputs.thrust - rollHalf - pitchHalf);
		
		newData = true; // <- debug
	}
}

void showData() {
	if (newData == true) {
		Serial.print("Data received ");
		Serial.println(); // TODO
		newData = false;
	}
}
