#include <IRremote.h>

/*
vaatii testaamista
*/


#define pon 20L
#define poff 32L
#define chd 27L
#define chu 28L
#define vold 37L
#define volu 36L

IRsend irsend;
unsigned long com = 0L;
void Send(unsigned long command);

void setup()
{
	for (int i = 0; i < 3; i++) {
		irsend.sendRC5(20, 12);
		delay(40);
	}
	delay(10000);
}

void loop() 
{
	Send(chd);
	delay(500);
}

void Send(unsigned long command)
{
	if (command != com)
	{
		com = command;
		for (int i = 0; i < 3; i++)
		{
			irsend.sendRC5(command, 12);
			delay(40);
		}
	}
	else
	{
		command += 2048;
		for (int i = 0; i < 3; i++)
		{
			irsend.sendRC5(command, 12);
			delay(40);
		}
	}
	
}