// TODO TAI MUISTUTKSIA
// 
// 
// BATTERY COMPENSATION
///////////////////////////////////////////////


#include "Gyro.h"
#include <Wire.h>
#include <Servo.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte radioAddress = 76;
const char maxAngle = 400;

RF24 radio(CE_PIN, CSN_PIN);
Servo esc1;	
Servo esc2;
Servo esc3;
Servo esc4;
Gyro gyro;

struct Input
{
	int thrust = 0;
	int pitch = 0;
	int roll = 0;
	int yaw = 0;
	bool STOP = false;
};
Input inputs;

long loop_timer;
//////////////// PID CONSTANTS ////////////////
float kp = 0.0f;
float ki = 0.0f;
float kd = 20.0f;
//////////////// //////////// ////////////////
float pid_p = 0.0f;
float pid_i = 0.0f;
float pid_d = 0.0f;
float prevErrorRoll = 0.0f;
float prevErrorPitch = 0.0f;
float error = 0.0f;
int pid_roll = 0;
int pid_pitch = 0;
int pid_yaw = 0;
//////////////////////////////////////////////

void setup() {
	Serial.begin(9600);
	pinMode(7, OUTPUT);
	// esc
	esc1.attach(2); // top left
	esc2.attach(3); // top right
	esc3.attach(4); // bottom left
	esc4.attach(5); // bottom right
	esc1.writeMicroseconds(1000);
	esc2.writeMicroseconds(1000);
	esc3.writeMicroseconds(1000);
	esc4.writeMicroseconds(1000);
	// gyro
	Wire.begin();
	gyro.setup_mpu_6050_registers();
	gyro.Init();
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
	gyro.read_mpu_6050_data();
	if (inputs.thrust > 1150)
	{
		CalculatePID();
	}
	else
	{
		pid_roll = 0;
		pid_pitch = 0;
		pid_yaw = 0;
	}
	WriteToMotors();
	//showData(); // <- debug

	if (micros() - loop_timer > 4000)
	{
		digitalWrite(7, HIGH);
		loop_timer = micros();
	}
	else
	{
		digitalWrite(7, LOW);
	}

	while (micros() - loop_timer < 4000);                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
	{
		loop_timer = micros();                                           //Reset the loop timer
	}
}

void GetTransmitterData() {
	if (radio.available()) {
		radio.read(&inputs, sizeof(inputs));
		// joystick raw input zeroed //
		inputs.roll -= 527; //522
		inputs.roll *= -1;
		inputs.pitch -= 529; //525
		// mapping
		inputs.thrust = map(inputs.thrust, 673, 1023, 1000, 2000);
		if (inputs.thrust < 1000)
		{
			inputs.thrust = 1000;
		}
		inputs.roll = map(inputs.roll, -496, 527, -maxAngle, maxAngle);
		inputs.pitch = map(inputs.pitch, -529, 494, -maxAngle, maxAngle);
	}
}

void CalculatePID()
{
	// ROLL ///////////////////////////////
	error = gyro.anglePitch() - inputs.roll; // anglePitch() ON OIKEASTI angleRoll() !!!!!!!!!!!!!!!!!!!
	// kp
	pid_p = kp * error + 0.5f;
	// ki and limit checks
	pid_i += ki * error + 0.5f;
	if (pid_i > maxAngle)
	{
		pid_i = maxAngle;
	}
	else if (pid_i < -maxAngle)
	{
		pid_i = -maxAngle;
	}
	// kd
	pid_d = kd * (error - prevErrorRoll) + 0.5f;
	// total roll pid
	pid_roll = (pid_p + pid_i + pid_d);
	// total limit check
	if (pid_roll > maxAngle)
	{
		pid_roll = maxAngle;
	}
	else if (pid_roll < -maxAngle)
	{
		pid_roll = -maxAngle;
	}
	prevErrorRoll = error; 
	// PITCH  ///////////////////////////////
	error = gyro.angleRoll() - inputs.pitch; // angleRoll() ON OIKEASTI anglePitch() !!!!!!!!!!!!!!!!!!!
	// kp
	pid_p = kp * error + 0.5f;
	// ki and limit checks
	pid_i += ki * error + 0.5f;
	if (pid_i > maxAngle)
	{
		pid_i = maxAngle;
	}
	else if (pid_i < -maxAngle)
	{
		pid_i = -maxAngle;
	}
	// kd
	pid_d = kd * (error - prevErrorPitch) + 0.5f;
	// total pitch pid
	pid_pitch = (pid_p + pid_i + pid_d);
	// total limit check
	if (pid_pitch > maxAngle)
	{
		pid_pitch = maxAngle;
	}
	else if (pid_pitch < -maxAngle)
	{
		pid_pitch = -maxAngle;
	}
	prevErrorPitch = error;
	// YAW  /////////////////////////////////
	inputs.yaw *= maxAngle;
}

void WriteToMotors()
{	// m1, m2, m3, m4
	int m[] =
	{
		inputs.thrust + pid_roll + pid_pitch - pid_yaw,
		inputs.thrust - pid_roll + pid_pitch + pid_yaw,
		inputs.thrust + pid_roll - pid_pitch + pid_yaw,
		inputs.thrust - pid_roll - pid_pitch - pid_yaw 
	};

	for (int i = 0; i < 4; i++)
	{
		if (m[i] < 1000)
		{
			m[i] = 1000;
		}
		else if (m[i] > 2000)
		{
			m[i] = 2000;
		}
	}
	esc1.writeMicroseconds(m[0]);
	esc2.writeMicroseconds(m[1]);
	esc3.writeMicroseconds(m[2]);
	esc4.writeMicroseconds(m[3]);
}

void showData()
{
	//debug only
	//Serial.println(gyro.angleRoll() );
	//Serial.println(pid_pitch);
	//Serial.println("=========");
}

/*
PITCH JA ROLL VÄÄRINPÄIN
*/