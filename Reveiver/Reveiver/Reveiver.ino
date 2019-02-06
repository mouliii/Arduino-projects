#include <MPU6050_tockn.h>
#include "Wire.h"
#include <Servo.h>
#include <RF24.h>
#include "Gyro.h"


/*
    timer ei radio yhteyttö esim 300 ms  <----- miten aktivointi
	kaasu 1000 + 100% * 10 ( max 70% ? )
	turn rate ? max 10 astetta ?
*/

#define CE_PIN   9
#define CSN_PIN 10

const int ledPin = 7;
const byte radioAddress = 76;
RF24 radio(CE_PIN, CSN_PIN);

MPU6050 mpu6050(Wire);
Servo esc1;
Servo esc2;
Servo esc3;
Servo esc4;
// controller inputs
int inputs[5] = { 0,0,0,0,0 };
// PID //
const float kp = 1.0f;
const float ki = 0.002f;
const float kd = 0.15f;
const float limit = 200.0f;
struct PID
{
	void ResetValues()
	{
		p = 0.0f;
		i = 0.0f;
		d = 0.0f;
		pid = 0.0f;
		error = 0.0f;
		prevError = 0.0f;
	}
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
long radioSilenceTimer = 0.0f;

void CalculatePID(PID& pid);

int counter = 0;

void setup() {
	Serial.begin(115200);
	pinMode(ledPin, OUTPUT);
	// esc
	
	esc1.attach(2);
	esc1.writeMicroseconds(1000);
	esc2.attach(3);
	esc2.writeMicroseconds(1000);
	esc3.attach(4);
	esc3.writeMicroseconds(1000);
	esc4.attach(5);
	esc4.writeMicroseconds(1000);
	
	// radio
	Serial.println("Receiver starting");
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setPALevel(RF24_PA_MIN);
	radio.setAutoAck(false);
	radio.openReadingPipe(1, radioAddress);
	radio.startListening();
	// setup gyro
	digitalWrite(ledPin, HIGH);
	mpu6050.begin();
	//mpu6050.calcGyroOffsets(true);
	mpu6050.setGyroOffsets(4.0f, 47.0f, 0.0f);
	digitalWrite(ledPin, LOW);
	//Reset the loop timer
	loop_timer = micros();
}

void loop() {

	GetTransmitterData();
	mpu6050.update();
	// pid
	if (inputs[0] > 1300)
	{
		pidRoll.error = inputs[2] - mpu6050.getAngleX();
		pidPitch.error = inputs[1] - mpu6050.getAngleY();
		CalculatePID(pidRoll);
		CalculatePID(pidPitch);
	}
	else
	{
		pidRoll.ResetValues();
		pidPitch.ResetValues();
	}
	
	WriteToMotors();
	//showData(); // <- debug
	
	while (micros() - loop_timer < 4000);  // check 4 ms                 //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
	{
		loop_timer = micros();                                           //Reset the loop timer
	}
}	

void GetTransmitterData() {
	
	if (radio.available() )
	{
		radio.read(&inputs, sizeof(inputs));
		inputs[0] = inputs[0] * 10 + 1000;
		inputs[1] /= 10;
		inputs[2] /= 10 * -1;
		radioSilenceTimer = 0.0f;
	}
	else
	{
		radioSilenceTimer += millis();
		
		if (radioSilenceTimer - millis() > 300)  // 300 ms
		{
			while (!radio.available() )
			{
				esc1.write(1000);
				esc2.write(1000);
				esc3.write(1000);
				esc4.write(1000);
			}

			radio.read(&inputs, sizeof(inputs));
			inputs[0] = inputs[0] * 10 + 1000;
			inputs[1] /= 10;
			inputs[2] /= 10 * -1;
			radioSilenceTimer = 0.0f;
		}
		
	}
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
	int motorSpeed[4] = {
		inputs[0] + pidRoll.pid - pidPitch.pid,
		inputs[0] - pidRoll.pid - pidPitch.pid,
		inputs[0] + pidRoll.pid + pidPitch.pid,
		inputs[0] - pidRoll.pid + pidPitch.pid
	};
	for (int i = 0; i < 4; i++)
	{
		if (motorSpeed[i] < 1000)
		{
			motorSpeed[i] = 1000;
		}
		else if (motorSpeed[i] > 1900)
		{
			motorSpeed[i] = 1900;
		}
	}

	esc1.writeMicroseconds(motorSpeed[0]);
	esc2.writeMicroseconds(motorSpeed[1]);
	esc3.writeMicroseconds(motorSpeed[2]);
	esc4.writeMicroseconds(motorSpeed[3]);
}

void showData()
{
	/*
	Serial.print(pidRoll.pid);
	Serial.print("       ");
	Serial.println(pidPitch.pid);

	Serial.print(mpu6050.getAngleX());
		Serial.print("       ");
		Serial.print(mpu6050.getAngleY());
		Serial.print("       ");
		Serial.println(mpu6050.getAngleZ());
	*/
	if (++counter > 10)
	{
		Serial.print(inputs[0] - pidRoll.pid - pidPitch.pid);
		Serial.print("   ");
		Serial.print(inputs[0] + pidRoll.pid - pidPitch.pid);
		Serial.print("   ");
		Serial.print(inputs[0] - pidRoll.pid + pidPitch.pid);
		Serial.print("   ");
		Serial.println(inputs[0] + pidRoll.pid + pidPitch.pid);

		counter = 0;
	}
	
	
	/*
	Serial.print(inputs[0] - pidRoll.pid - pidPitch.pid);
	Serial.print("   ");
	Serial.print(inputs[0] + pidRoll.pid - pidPitch.pid);
	Serial.print("   ");
	Serial.print(inputs[0] - pidRoll.pid + pidPitch.pid);
	Serial.print("   ");
	Serial.println(inputs[0] + pidRoll.pid + pidPitch.pid);
	*/

	//Serial.println(inputs[2]);
}