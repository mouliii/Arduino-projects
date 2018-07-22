// Button.h

#ifndef _BUTTON_h
#define _BUTTON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Button
{
public:
	Button(const int pin)
		:
		pin(pin)
	{
		pinMode(pin, INPUT_PULLUP);
	}
private:
	const int pin;
	bool prevButtonState = true; // true - HIGH | false - LOW
};

#endif

