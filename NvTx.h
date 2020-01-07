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

// The following helper macro get variable name as the parameter. The only other parameter required is EEPROM base address.
#define NvTxGetAt(var, addr) nv_tx_get(#var, 0, &var, sizeof(var), addr)
#define NvTxPutAt(var, addr) nv_tx_put(#var, 0, &var, sizeof(var), addr)

// Same as above but with instance id parameter
#define NvTxGetAt_(var, addr, inst_id) nv_tx_get(#var, inst_id, &var, sizeof(var), addr)
#define NvTxPutAt_(var, addr, inst_id) nv_tx_put(#var, inst_id, &var, sizeof(var), addr)

// Calculates the amount of storage is takes on EEPROM to save the value of the variable
#define NvTxSize(var) (2 * (4 + sizeof(var)))

//
// The following set of macro provides the means for automatically assigning addresses
// to storage locations at compile time
//

// We are using separate enum to declare each var storage location
#define NvAddr(var) var##_eeprom_addr_

// The next free EEPROM address after var's storage location
#define NvNextAddr(var) (NvAddr(var) + NvTxSize(var))

// Assert the var storage location does fit onto the EEPROM
#define NvAssertFit(var) STATIC_ASSERT2(NvNextAddr(var) <= E2END + 1, var##_fit_EEPROM)

// Declare variable's storage address explicitly.
// Typically used for declaring first address of the sequence of storage locations.
#define NvPlace(var, addr, inst_id) enum {var##_eeprom_addr_ = addr, var##_eeprom_iid_ = inst_id}; NvAssertFit(var)

// Assign the address of 'var's storage next to the 'prev's storage location
#define NvAfter(var, prev) enum {\
		var##_eeprom_addr_ = NvNextAddr(prev), var##_eeprom_iid_ = prev##_eeprom_iid_, prev##_eeprom_has_next_ = 1\
	}; NvAssertFit(var)

// Get var's instance id
#define NvInstId(var) var##_eeprom_iid_

// Get/put variable using the previously declared address
#define NvTxGet(var) NvTxGetAt_(var, NvAddr(var), NvInstId(var))
#define NvTxPut(var) NvTxPutAt_(var, NvAddr(var), NvInstId(var))
