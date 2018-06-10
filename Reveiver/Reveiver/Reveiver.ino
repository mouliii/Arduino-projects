#include <Wire.h>
#include <Servo.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte radioAddress = 76;
RF24 radio(CE_PIN, CSN_PIN);
//  KAASU MAX 100 <<-----
const int maxAngle = 10;

Servo esc1;
Servo esc2;
Servo esc3;
Servo esc4;
Servo servo;

int inputs[2] = { 0,0 };

long loop_timer;

void setup() {
	Serial.begin(9600);
	pinMode(7, OUTPUT);
	// esc
	esc1.attach(2);
	esc1.write(1000);
	esc2.attach(3);
	esc2.write(1000);
	esc3.attach(4);
	esc3.write(1000);
	esc4.attach(5);
	esc4.write(1000);
	servo.attach(6);
	servo.write(90);
	
	// radio
	Serial.println("SimpleRx Starting");
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

	servo.write(inputs[1]);

	esc1.write(inputs[0]);
	esc2.write(inputs[0]);
	esc3.write(inputs[0]);
	esc4.write(inputs[0]);

	showData(); // <- debug
	
	while (micros() - loop_timer < 4000);                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
	{
		loop_timer = micros();                                           //Reset the loop timer
	}
}
	

void GetTransmitterData() {
	
	if (radio.available() )
	{
		ListenRadio();
	}
	else
	{	
		while (!radio.available() )
		{
			if (micros() - loop_timer > 500000)
			{
				esc1.write(1000);
				esc2.write(1000);
				esc3.write(1000);
				esc4.write(1000);
			}
		}
		ListenRadio();
		loop_timer = micros();
	}
	
}

void ListenRadio()
{
	radio.read(&inputs, sizeof(inputs));
	// joystick raw input zeroed //
	inputs[1] -= 532; //522
	// mapping
	inputs[0] = 1000 + map(inputs[0], 662, 1023, 0, 100);
	inputs[1] = 1 + map(inputs[1], -500, 520, 70, 120);
}

void showData()
{

}