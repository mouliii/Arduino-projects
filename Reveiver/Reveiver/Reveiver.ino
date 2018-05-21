// TODO TAI MUISTUTKSIA
// 
// 
// BATTERY COMPENSATION
///////////////////////////////////////////////


#include "Gyro.h"
#include <Wire.h>
#include <ServoTimer2\ServoTimer2.h>
#include <VirtualWire\VirtualWire.h>

const int maxAngle = 100;
int maxTurn = maxAngle / 10;

ServoTimer2 esc1;
ServoTimer2 esc2;
ServoTimer2 esc3;
ServoTimer2 esc4;
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
uint8_t len = sizeof(inputs);

long loop_timer;
//////////////// PID CONSTANTS ////////////////
float kp = 4.5f;
float ki = 1.0f;
float kd = 0.5f;
//////////////// //////////// ////////////////
float pid_p = 0.0f;
float pid_i = 0.0f;
float pid_d = 0.0f;
float prevErrorRoll = 0.0f;
float prevErrorPitch = 0.0f;
float error = 0.0f;
float pid_roll = 0;
float pid_pitch = 0;
float pid_yaw = 0;
//////////////////////////////////////////////

void setup() {
	//Serial.begin(9600);
	pinMode(7, OUTPUT);
	// esc
	esc1.attach(2); // top left
	esc2.attach(3); // top right
	esc3.attach(4); // bottom left
	esc4.attach(5); // bottom right
	
	esc1.write(1000);
	esc2.write(1000);
	esc3.write(1000);
	esc4.write(1000);
	
	// gyro
	Wire.begin();
	gyro.setup_mpu_6050_registers();
	gyro.Init();
	// radio
	Serial.println("SimpleRx Starting");
	vw_set_rx_pin(11);
	vw_set_ptt_pin(true);
	vw_setup(1000);
	vw_rx_start();

	//Reset the loop timer
	loop_timer = micros();
}

void loop() {
	if (!inputs.STOP)
	{
		GetTransmitterData();
		gyro.read_mpu_6050_data();
		if (inputs.thrust > 1050)
		{
			WriteToMotors();
		}
		else
		{
			esc1.write(1000);
			esc2.write(1000);
			esc3.write(1000);
			esc4.write(1000);
		}
		
		if (inputs.thrust > 1100)
		{
			CalculatePID();
		}
		else
		{
			pid_roll = 0;
			pid_pitch = 0;
			pid_yaw = 0;
		}
		
		//showData(); // <- debug

		if (micros() - loop_timer > 4000)
		{
			digitalWrite(7, HIGH);
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
	else
	{
		inputs.thrust = 0;
		inputs.pitch = 0;
		inputs.roll = 0;
		inputs.yaw = 0;

		esc1.write(1000);
		esc2.write(1000);
		esc3.write(1000);
		esc4.write(1000);
	}
}

void GetTransmitterData() {
	
	if (vw_have_message())
	{
		if (vw_get_message((uint8_t*)&inputs, &len))
		{
			// joystick raw input zeroed //
			inputs.roll -= 533; //522
			//inputs.roll *= -1;
			inputs.pitch -= 535; //525
			// mapping
			inputs.thrust = 1000 + map(inputs.thrust, 662, 1023, 0, 1000);
			inputs.roll = map(inputs.roll, -501, 522, -maxTurn, maxTurn);
			inputs.pitch = map(inputs.pitch, -524, 500, -maxTurn, maxTurn);
		}
	}
	else
	{
		
		while (!vw_have_message() )
		{
			if (micros() - loop_timer > 500000)
			{
				esc1.write(1000);
				esc2.write(1000);
				esc3.write(1000);
				esc4.write(1000);
			}
		}
		if (vw_get_message((uint8_t*)&inputs, &len))
		{
			// joystick raw input zeroed //
			inputs.roll -= 533; //522
			inputs.roll *= -1;
			inputs.pitch -= 535; //525
								 // mapping
			inputs.thrust = 1000 + map(inputs.thrust, 662, 1023, 0, 1000);
			inputs.roll = map(inputs.roll, -501, 522, -maxTurn, maxTurn);
			inputs.pitch = map(inputs.pitch, -524, 500, -maxTurn, maxTurn);
		}
		loop_timer = micros();
		
	}
}

void CalculatePID()
{
	// ROLL ///////////////////////////////
	error = gyro.anglePitch() - inputs.roll; // anglePitch() ON OIKEASTI angleRoll() !!!!!!!!!!!!!!!!!!!
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
	pid_d = kd * (error - prevErrorRoll);
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
	pid_d = kd * (error - prevErrorPitch);
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
	pid_yaw = inputs.yaw * maxTurn / 2;

}

void WriteToMotors()
{	// m1, m2, m3, m4
	int m[] =
	{
		inputs.thrust - pid_roll + pid_pitch + pid_yaw,
		inputs.thrust + pid_roll + pid_pitch - pid_yaw,
		inputs.thrust - pid_roll - pid_pitch - pid_yaw,
		inputs.thrust + pid_roll - pid_pitch + pid_yaw 
	};

	for (int i = 0; i < 4; i++)
	{
		if (m[i] < 1000)
		{
			m[i] = 1000;
		}
		else if (m[i] > 1850)
		{
			m[i] = 1850;
		}
	}
	esc1.write(m[0]);
	esc2.write(m[1]);
	esc3.write(m[2]);
	esc4.write(m[3]);
	/*
	Serial.print( m[0] );
	Serial.print("   ");
	Serial.print( m[1] );
	Serial.print("   ");
	Serial.print(m[2]);
	Serial.print("   ");
	Serial.println(m[3]);
	*/
}

void showData()
{
	
	//debug only
	/*
	Serial.print(gyro.anglePitch() );
	Serial.print("   ");
	Serial.println(gyro.angleRoll() );
	
	Serial.print( pid_roll );
	Serial.print("   ");
	Serial.print( pid_pitch );
	Serial.print("   ");
	Serial.println( "" );
	*/
	//Serial.println("=========");
	

}

/*
PITCH JA ROLL VÄÄRINPÄIN
*/