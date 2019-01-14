

const int ledPin = 4;
const int mosfPin = 2;
const int voltPin = 3;


// the setup function runs once when you press reset or power the board
void setup() {

	pinMode(ledPin, OUTPUT); // led
	pinMode(mosfPin, OUTPUT); // mosfet
	pinMode(voltPin, INPUT);  // battery level

	digitalWrite(mosfPin, HIGH);
}

// the loop function runs over and over again until power down or reset
void loop() {
	float voltage = analogRead(voltPin) * 0.0049f; //read the A0 pin value
	
	if (voltage < 4.0f)
	{
		digitalWrite(mosfPin, LOW);
	}

	if (voltage < 4.45f) // 4.45  ~ 20% ( 11.1v )
	{

		if (digitalRead(ledPin) == LOW)
		{
			digitalWrite(ledPin, HIGH);
		}
		else
		{
			digitalWrite(ledPin, LOW);
		}
	}

	delay(2500);
}


// 4v   -> 10.05v
// 4.2v -> 10.5v
// 4.4v -> 11.0v