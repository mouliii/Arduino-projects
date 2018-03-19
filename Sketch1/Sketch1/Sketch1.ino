#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

DHT dht(7,DHT11);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Serial.println("testi");

	lcd.begin(16, 2);
	lcd.print("Hello world!");
	lcd.clear();
}

// the loop function runs over and over again until power down or reset
void loop() {

	lcd.setCursor(0, 0);
	float h = dht.readHumidity();
	lcd.print("humi: ");
	lcd.print(h);
	lcd.setCursor(0, 1);
	float t = dht.readTemperature();
	lcd.print("temp: ");
	lcd.print(t);

	Serial.println("humidity: " + (String)h);
	Serial.println("temp: " + (String)t);

	delay(2000);
}
