#include "NvTx.h"
#include <Arduino.h>

//
// Transactional data storage usage example
//

unsigned cnt1;
uint8_t  cnt2;
uint8_t  cnt3;

#define ADDR1 0x10
#define ADDR2 (ADDR1 + NvTxSize(cnt1))

void setup()
{
	Serial.begin(9600);

	if (NvTxGet(cnt1, ADDR1))
		++cnt1;
	else
		cnt1 = 1;

	NvTxPut(cnt1, ADDR1);

	if (NvTxGet(cnt2, ADDR2))
		++cnt2;
	else
		cnt2 = 1;

	if (!(cnt1 % 10))
		// Simulate bad transaction.
		// The new value will not be stored.
		// Previous value will be read instead.
		NvTxPut(cnt3, ADDR2);
	else
		NvTxPut(cnt2, ADDR2);
	
}

void loop()
{	
	if (NvTxGet(cnt1, ADDR1)) {
		Serial.print("cnt1=");
		Serial.println(cnt1);
	}
	if (NvTxGet(cnt2, ADDR2)) {
		Serial.print("cnt2=");
		Serial.println(cnt2);
	}
	delay(1000);
}
