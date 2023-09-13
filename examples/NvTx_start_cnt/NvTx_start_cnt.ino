#include "NvTx.h"
#include <Arduino.h>

//
// Transactional data storage usage example
//

// Changing the instance id invalidates all saved values.
// You can change it for ex. on updating to new version incompatible with old stored data.
#define MY_INST_ID 0x111

unsigned cnt1;
uint8_t  cnt[2];
//
// Assign addresses to storage locations
// where cnt1 and cnt2 will be permanently stored
//
NvPlace(cnt1, 0x10, MY_INST_ID); // place at explicitly defined address
NvTagAfter(cnt2, cnt[0], cnt1); // place cnt2 right after cnt1
NvTagAfter(cnt3, cnt[1], cnt2); // place cnt2 right after cnt1

// Try to place cnt3 to the same location as cnt2 - wont compile
//NvTagAfter(cnt3, cnt[1], cnt1);

void setup()
{
	Serial.begin(9600);

	if (NvTxGet(cnt1))
		++cnt1;
	else
		cnt1 = 1;

	NvTxPut(cnt1);

	if (NvTxTagGet(cnt2, cnt[0]))
		++cnt[0];
	else
		cnt[0] = 1;

	if (!(cnt1 % 4))
		// Simulate bad transaction.
		// The new value will not be stored.
		// Previous value will be read instead.
		NvTxTagPutAt_(cnt3, cnt[0], NvAddr(cnt2), MY_INST_ID);
	else
		NvTxTagPut(cnt2, cnt[0]);
}

void loop()
{	
	if (NvTxGet(cnt1)) {
		Serial.print("cnt1=");
		Serial.println(cnt1);
	}
	if (NvTxTagGet(cnt2, cnt[0])) {
		Serial.print("cnt2=");
		Serial.println(cnt[0]);
	}
	if (NvTxTagGet(cnt3, cnt[1])) {
		Serial.print("cnt3=");
		Serial.println(cnt[1]);
	}
	delay(1000);
}
