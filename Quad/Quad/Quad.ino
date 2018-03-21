#include <Servo.h>

Servo esc;
int throttlePin = A0;

void setup() {
	esc.attach(8);
}

void loop() {
	int throttle = analogRead(throttlePin);
	throttle = map(throttle, 0, 1023, 0, 255);
	esc.write(throttle);
}