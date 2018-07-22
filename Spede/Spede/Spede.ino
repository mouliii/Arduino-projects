
#include "Button.h"
unsigned long currentMillis;
float period = 1000;
bool gameIsRunning = true;
bool isPressed = true;
int led;
int prevLed;
bool buttonStates[] = {1,1,1,1};

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);

	pinMode(2, INPUT_PULLUP);
	pinMode(3, INPUT_PULLUP);
	pinMode(4, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	randomSeed(analogRead(0));

	led = random(4) + 6;
	do
	{
		prevLed = random(4) + 6;
	} while (prevLed == led);

	for (int i = 6; i <= 9; i++)
	{
		digitalWrite(i, HIGH);
	}
	delay(1000);
	for (int i = 6; i <= 9; i++)
	{
		digitalWrite(i, LOW);
	}

	currentMillis = millis();  //initial start time
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	for (int i = 0; i < 4; i++)
	{
		if (digitalRead(i + 2) == HIGH)
		{
			buttonStates[i] = true;
		}
	}

	if (gameIsRunning)
	{
		// timer to light new led
		if (millis() - currentMillis >= period)  //test whether the period has elapsed
		{
			//check if button is pressed
			if (!isPressed)
			{
				//gameIsRunning = false;
				for (int i = 6; i <= 9; i++)
				{
					digitalWrite(i, HIGH);
				}
			}
			else
			{	// roll new led
				digitalWrite(prevLed, LOW);
				digitalWrite(led, HIGH);
				isPressed = false;
			}

			currentMillis = millis();
		}
		else
		{	// check buttons
			if (!isPressed)
			{

				// This if statement will only fire on the falling edge of the button input
				for (int i = 0; i < 4; i++)
				{
					if (digitalRead(i + 2) == LOW && buttonStates[i] == true) {
						// reset the button low flag
						buttonStates[i] = false;

						isPressed = true;
						prevLed = led;
						do
						{
							led = random(4) + 6;
						} while (led == prevLed);
						period *= 0.95f;
					}
					else
					{
						Serial.println(led);
						Serial.println(i + 4);
						Serial.println(period);
						isPressed = false;
						gameIsRunning = false;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 6; i <= 9; i++)
		{
			digitalWrite(i, HIGH);
		}
	}
	
}