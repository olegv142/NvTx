#include "crc16.h"

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	// Both should print 47933
	Serial.println(crc16_str("123456789"));
	Serial.println(crc16("123456789", 9));
	delay(1000);
}
