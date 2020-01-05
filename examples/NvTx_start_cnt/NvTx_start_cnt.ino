#include "NvTx.h"
#include <Arduino.h>

//
// Transactional data storage usage example
//

// Changing the instance id invalidates all saved values.
// You can change it for ex. on updating to new version incompatible with old stored data.
#define MY_INST_ID 0x123

unsigned cnt1;
uint8_t  cnt2;
uint8_t  cnt3;
//
// Assign addresses to storage locations
// where cnt1 and cnt2 will be permanently stored
//
NvPlace(cnt1, 0x10, MY_INST_ID); // place at explicitly defined address
NvAfter(cnt2, cnt1); // place cnt2 right after cnt1
// Try to place cnt3 to the same location as cnt2 - wont compile
// NvAfter(cnt3, cnt1);

void setup()
{
	Serial.begin(9600);

	if (NvTxGet(cnt1))
		++cnt1;
	else
		cnt1 = 1;

	NvTxPut(cnt1);

	if (NvTxGet(cnt2))
		++cnt2;
	else
		cnt2 = 1;

	if (!(cnt1 % 10))
		// Simulate bad transaction.
		// The new value will not be stored.
		// Previous value will be read instead.
		NvTxPutAt(cnt3, NvAddr(cnt2));
	else
		NvTxPut(cnt2);
	
}

void loop()
{	
	if (NvTxGet(cnt1)) {
		Serial.print("cnt1=");
		Serial.println(cnt1);
	}
	if (NvTxGet(cnt2)) {
		Serial.print("cnt2=");
		Serial.println(cnt2);
	}
	delay(1000);
}
