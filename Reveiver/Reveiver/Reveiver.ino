#include <Wire.h>
#include <Servo.h>
#include <RF24.h>
#include "Gyro.h"

#define CE_PIN   9
#define CSN_PIN 10

const byte radioAddress = 76;
RF24 radio(CE_PIN, CSN_PIN);

Gyro gyro;
Servo esc1;
Servo esc2;
Servo esc3;
Servo esc4;
// controller inputs
int inputs[3] = { 1000,0,0 };
// PID //
const float kp = 1.0f;
const float ki = 0.002f;
const float kd = 0.15f;
const float limit = 200.0f;
struct PID
{
	float p = 0.0f;
	float i = 0.0f;
	float d = 0.0f;
	float pid = 0.0f;
	float error = 0.0f;
	float prevError = 0.0f;
};
PID pidRoll;
PID pidPitch;
//////////
long loop_timer;

void CalculatePID(PID& pid);

void setup() {
	Serial.begin(9600);
	pinMode(7, OUTPUT);  // LED ON PINNI 7 !!!!!!!!!!!
	// gyro
	gyro.setup_mpu_6050_registers();
	// esc
	/*
	esc1.attach(2);
	esc1.write(1000);
	esc2.attach(3);
	esc2.write(1000);
	esc3.attach(4);
	esc3.write(1000);
	esc4.attach(5);
	esc4.write(1000);
	*/
	// radio
	Serial.println("SimpleRx Starting");
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setPALevel(RF24_PA_MIN);
	radio.setAutoAck(false);
	radio.openReadingPipe(1, radioAddress);
	radio.startListening();
	// setup gyro
	gyro.Init();
	//Reset the loop timer
	loop_timer = micros();
}

void loop() {

	//GetTransmitterData();
	gyro.read_mpu_6050_data();
	// pid
	pidRoll.error = inputs[1] - gyro.anglePitch();
	pidPitch.error = inputs[2] - gyro.angleRoll();
	CalculatePID(pidRoll);
	CalculatePID(pidPitch);
	
	//WriteToMotors();
	
	showData(); // <- debug
	
	while (micros() - loop_timer < 40000);                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
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

void CalculatePID(PID& pid)
{
	/*
	pitch - roll
	float error
	p = error * kp
	i += i + ( error * ki )   limit
	d = ( error - edelError ) * kd
	prevError = error
	*/

	pid.p = pid.error * kp;
	pid.i = pid.i + (pid.error * ki);
	if (pid.i > limit)
	{
		pid.i = limit;
	}
	else if (pid.i < -limit)
	{
		pid.i = -limit;
	}
	pid.d = (pid.error - pid.prevError) * kd;
	pid.prevError = pid.error;

	pid.pid = pid.p + pid.i + pid.d;

	if (pid.pid > limit)
	{
		pid.pid = limit;
	}
	else if (pid.pid < -limit)
	{
		pid.pid = -limit;
	}
}

void WriteToMotors()
{
	esc1.write(inputs[0] - pidRoll.pid - pidPitch.pid);
	esc2.write(inputs[0] + pidRoll.pid - pidPitch.pid);
	esc3.write(inputs[0] - pidRoll.pid + pidPitch.pid);
	esc4.write(inputs[0] + pidRoll.pid + pidPitch.pid);

}

void showData()
{
	/*
	Serial.print(pidPitch.pid);
	Serial.print("       ");
	Serial.println(pidRoll.pid);
	*/
	Serial.print(gyro.anglePitch());
	Serial.print("       ");
	Serial.println(gyro.angleRoll());
	
	/*
	Serial.print(inputs[0] - pidRoll.pid - pidPitch.pid);
	Serial.print("   ");
	Serial.print(inputs[0] + pidRoll.pid - pidPitch.pid);
	Serial.print("   ");
	Serial.print(inputs[0] - pidRoll.pid + pidPitch.pid);
	Serial.print("   ");
	Serial.println(inputs[0] + pidRoll.pid + pidPitch.pid);
	*/
}