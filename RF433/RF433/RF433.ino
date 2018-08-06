#include <rc-switch/RCSwitch.h>
// 1 - lyhyt on 2 - pitkä on 3 - lyhyt off 4 - pitkä off
 char* key = "23141414232314141414232314231423232314141414232314142323142314142314142314231414142314232314232323232323141423232314232323142314141";
 
RCSwitch radio;
// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	pinMode(8, INPUT_PULLUP);
	pinMode(7, OUTPUT);
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	if (digitalRead(8) == LOW )
	{
		Serial.println("works");
		for (int i = 0; i < 4; i++)
		{
			for (int i = 0; i < 12; i++)
			{
				digitalWrite(7, HIGH);
				delayMicroseconds(362);
				digitalWrite(7, LOW);
				delayMicroseconds(362);
			}
			delayMicroseconds(3310);

			for (int i = 0; i < 132; i++)
			{
				switch (key[i])
				{
				case '1':
					digitalWrite(7, HIGH);
					delayMicroseconds(362);
					break;
				case '2':
					digitalWrite(7, HIGH);
					delayMicroseconds(725);
					break;
				case '3':
					digitalWrite(7, LOW);
					delayMicroseconds(362);
					break;
				case '4':
					digitalWrite(7, LOW);
					delayMicroseconds(725);
					break;
				default:
					digitalWrite(7, LOW);
					break;
				}
			}
			digitalWrite(7, LOW);
			delayMicroseconds(5000);
		}
		delay(1000);
	}

}
