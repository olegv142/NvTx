#pragma once

#include "StaticAssert.h"

/*
 Transactional non-volatile storage uses EEPROM for reliably storing
 client's data. It provides the following useful properties:
 - client will not read the data it have never written
 - client will not read the data corrupted for any reason
 - the data update will succeed either entirely or not at all, in the latter
   case old data will be read
 - the data is bound to the string tag and numerical instance id provided by
   the client so the data originally bound to the different tag or having different
   instance id will not be read to avoid erroneous interpretation of the semantically 
   different data

We use either variable name as the string tag or separate tag provided by the caller.
So we have separate set of macro for the second case.
*/

// Read the stored value. Returns true if the value was successfully read. This function should always be called
// before nv_tx_put to format storage even in case you are not interested in the current value.
bool nv_tx_get(
		const char* tag, // the value tag is the string name of the value that serves to identify its semantic
		unsigned inst_id,// to discriminate instances with the same tag 
		void* val,       // pointer to the memory location where the value will be copied should the read succeed
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value is stored
	);

// Write new value. The caller is expected to call nv_tx_get first.
void nv_tx_put(
		const char* tag, // the value tag is the string name of the value that serves to identify its semantic
		unsigned inst_id,// to discriminate instances with the same tag 
		void const* val, // pointer to the memory location from where the value will be copied to the EEPROM
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value will be stored
	);

// Erase stored value so that subsequent nv_tx_get will return false.
void nv_tx_erase(
		unsigned size,   // the value size in bytes
		unsigned addr    // the EEPROM base address where the value will be stored
	);

// Read/write variable from/to the particular storage location
#define NvTxTagGetAt_(tag, var, addr, inst_id) nv_tx_get(#tag, inst_id, &var, sizeof(var), addr)
#define NvTxTagPutAt_(tag, var, addr, inst_id) nv_tx_put(#tag, inst_id, &var, sizeof(var), addr)

// Calculates the amount of storage is takes on EEPROM to save the value of the variable
#define NvTxSize(var) (2 * (4 + sizeof(var)))

//
// The following set of macro provides the means for automatically assigning addresses
// to storage locations at compile time
//

// We are using separate enum to declare each var storage location
#define NvAddr(tag) tag##_eeprom_addr_

// The next free EEPROM address after var's storage location
#define NvNextTagAddr(tag, var) (NvAddr(tag) + NvTxSize(var))
#define NvNextAddr(var) NvNextTagAddr(var, var)

// Assert the var storage location does fit onto the EEPROM
#define NvAssertFit(tag) STATIC_ASSERT2(tag##_eeprom_next_addr_ <= E2END + 1, tag##_fit_EEPROM)

// Declare variable's storage address explicitly.
// Typically used for declaring first address of the sequence of storage locations.
#define NvPlaceTag(tag, var, addr, inst_id) enum {tag##_eeprom_addr_ = addr, tag##_eeprom_next_addr_ = NvNextTagAddr(tag, var), tag##_eeprom_iid_ = inst_id}; NvAssertFit(tag)
#define NvPlace(var, addr, inst_id) NvPlaceTag(var, var, addr, inst_id)

// Assign the address of 'var's storage next to the 'prev's storage location
#define NvTagAfter(tag, var, prev_tag) enum {\
		tag##_eeprom_addr_ = prev_tag##_eeprom_next_addr_, tag##_eeprom_iid_ = prev_tag##_eeprom_iid_,\
		tag##_eeprom_next_addr_ = NvNextTagAddr(tag, var),\
		prev_tag##_eeprom_has_next_ = 1\
	}; NvAssertFit(tag)
#define NvAfter(var, prev) NvTagAfter(var, var, prev)

// Get var's instance id
#define NvInstId(tag) tag##_eeprom_iid_

// Get/put variable using the previously declared address
#define NvTxTagGet(tag, var) NvTxTagGetAt_(tag, var, NvAddr(tag), NvInstId(tag))
#define NvTxTagPut(tag, var) NvTxTagPutAt_(tag, var, NvAddr(tag), NvInstId(tag))
#define NvTxGet(var) NvTxTagGet(var, var)
#define NvTxPut(var) NvTxTagPut(var, var)

// Erase storage location so subsequent calls to NvTxTagGet / NvTxGet will return false
#define NvTxTagErase(tag, var) nv_tx_erase(sizeof(var), NvAddr(tag))
#define NvTxErase(var) NvTxTagErase(var, var)
