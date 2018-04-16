// SimpleTx - the master or the transmitter

#include <SPI.h>
#include <RF24.h>
#include <VirtualWire.h>

#define CE_PIN   9
#define CSN_PIN 10

const byte slaveAddress = 76;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 4; // send once per second


int msg = 20;

void setup() {

	Serial.begin(9600);

	Serial.println("SimpleTx Starting");
	/*
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	//radio.setRetries(3, 5); // delay, count
	radio.setAutoAck(false);
	radio.setPALevel(RF24_PA_MAX);
	radio.openWritingPipe(slaveAddress);
	*/
	vw_set_ptt_inverted(true); // Required for DR3100
	vw_set_tx_pin(7);
	vw_setup(1000);	 // Bits per sec
	
}

//====================

void loop() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		send();
		prevMillis = millis();
	}
}

//====================

void send() {
	//radio.write(&msg, sizeof(msg));
	vw_send((uint8_t *)&msg, sizeof(msg));
}
