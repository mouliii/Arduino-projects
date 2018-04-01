// TODO SHITTII TAI MUISTUTKSIA
// 
// TODO YAW PID
// MOOTTORI()
// 
///////////////////////////////////////////////


#include "Gyro.h"
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte thisSlaveAddress = 76;
const char maxAngle = 20;

RF24 radio(CE_PIN, CSN_PIN);
Servo esc1;	
Servo esc2;
Servo esc3;
Servo esc4;
Gyro gyro;
bool newData = false; // <- debug

struct Input
{
	int thrust = 0;
	int pitch = 0;
	int roll = 0;
	int yaw = 0;
};
Input inputs;

long loop_timer;
//////////////// PID CONSTANTS ////////////////
const float kp = 0.0f;
const float ki = 0.0f;
const float kd = 0.0f;
//////////////// //////////// ////////////////
float pid_p = 0.0f;
float pid_i = 0.0f;
float pid_d = 0.0f;
float error = 0.0f; 
float prevError = 0.0f;
float pid_roll = 0.0f;
float pid_pitch = 0.0f;
float pid_yaw = 0.0f;
//////////////////////////////////////////////

void setup() {

	Serial.begin(9600);
	// gyro
	Wire.begin();
	gyro.setup_mpu_6050_registers();
	// radio
	Serial.println("SimpleRx Starting");
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setPALevel(RF24_PA_MIN);
	radio.setAutoAck(false);
	radio.openReadingPipe(1, thisSlaveAddress);
	radio.startListening();
	// esc
	esc1.attach(2); // top left
	esc2.attach(3); // top right
	esc3.attach(4); // bottom left
	esc4.attach(5); // bottom right

	//Reset the loop timer
	loop_timer = micros();
}

//=============

void loop() {
	GetTransmitterData();
	GetGyroData();
	CalculatePID();
	WriteToMotors();
	showData(); // <- debug
	if (micros() - loop_timer > 4000)
	{
		Serial.println("loop timer too high!!!!");
	}

	while (micros() - loop_timer < 4000);                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
	{
		loop_timer = micros();                                               //Reset the loop timer
	}
}

//==============

void GetTransmitterData() {
	if (radio.available()) {
		radio.read(&inputs, sizeof(inputs));
		// joystick raw input zeroed //
		inputs.roll -= 522;
		inputs.roll *= -1;
		inputs.pitch -= 525;
		// mapping
		inputs.roll = map(inputs.roll, -522, 501, -maxAngle, maxAngle);
		inputs.pitch = map(inputs.pitch, -525, 498, -maxAngle, maxAngle);

		newData = true; // <- debug
	}
}

void GetGyroData()
{
	gyro.read_mpu_6050_data();
}

void CalculatePID()
{
	// ROLL ///////////////////////////////
	error = gyro.angleRoll() - inputs.roll;
	// kp
	pid_p = kp * error;
	// ki and limit checks
	pid_i += ki * error;
	if (pid_i > maxAngle)
	{
		pid_i = maxAngle;
	}
	else if (pid_i < -maxAngle)
	{
		pid_i = -maxAngle;
	}
	// kd
	pid_d = kd * (error - prevError);
	// total roll pid
	pid_roll = pid_p + pid_i + pid_d;
	// total limit check
	if (pid_roll > maxAngle)
	{
		pid_roll = maxAngle;
	}
	else if (pid_roll < -maxAngle)
	{
		pid_roll = -maxAngle;
	}

	// PITCH  ///////////////////////////////
	error = gyro.anglePitch() - inputs.pitch;
	// kp
	pid_p = kp * error;
	// ki and limit checks
	pid_i += ki * error;
	if (pid_i > maxAngle)
	{
		pid_i = maxAngle;
	}
	else if (pid_i < -maxAngle)
	{
		pid_i = -maxAngle;
	}
	// kd
	pid_d = kd * (error - prevError);
	// total roll pid
	pid_pitch = pid_p + pid_i + pid_d;
	// total limit check
	if (pid_pitch > maxAngle)
	{
		pid_pitch = maxAngle;
	}
	else if (pid_pitch < -maxAngle)
	{
		pid_pitch = -maxAngle;
	}
	// YAW  /////////////////////////////////

	// TODO
	prevError = error;
}

void WriteToMotors()
{
	//esc1.writeMicroseconds();
}


void showData() {
	if (newData == true) {
		Serial.println(inputs.thrust + 1000); // TODO
		newData = false;
	}
}