// TODO TAI MUISTUTKSIA
// 
// TODO YAW PID
// BATTERY COMPENSATION
///////////////////////////////////////////////

#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <VirtualWire.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte radioAddress = 76;

RF24 radio(CE_PIN, CSN_PIN);

long loop_timer;
int data;
uint8_t buf = sizeof(data);

void setup() {

	Serial.begin(9600);
	// radio
	Serial.println("SimpleRx Starting");

	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	digitalWrite(2, LOW);
	digitalWrite(3, LOW);
	/*
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setPALevel(RF24_PA_MIN);
	radio.setAutoAck(false);
	radio.openReadingPipe(1, radioAddress);
	radio.startListening();
	*/
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_set_rx_pin(13);
	vw_setup(1000);	 // Bits per sec

	vw_rx_start();       // Start the receiver PLL running

	//Reset the loop timer
	loop_timer = micros();
}

void loop() {
	GetTransmitterData();
	while (micros() - loop_timer < 4000);                                //Wait until the loop_timer reaches 4000us (250Hz) before starting the next loop
	{
		loop_timer = micros();                                           //Reset the loop timer
	}
}

void GetTransmitterData() {
	/*
	if (radio.available()) {
		radio.read(&data, sizeof(data));
		digitalWrite(2, HIGH);
		Serial.println(data);
	}
	else
	{
		digitalWrite(2, LOW);
	}
	*/
	if (vw_get_message((uint8_t*)&data, &buf))
	{
		digitalWrite(2, HIGH);
		Serial.println(data);
	}
	else
	{
		digitalWrite(2, LOW);
	}
}
