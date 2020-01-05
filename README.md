# NvTx
Transactional non volatile storage for Arduino

Transactional non-volatile storage uses EEPROM for reliably storing
client's data. It provides the following useful properties:
 - client will not read the data it have never written
 - client will not read the data corrupted for any reason
 - the data update will succeed either entirely or not at all, in the latter
   case old data will be read
 - the data is bound to the string tag and numerical instance id provided by the
   client so the data originally bound to the different tag or having different instance id will not
   be read to avoid erroneous interpretation of the semantically different data
 
To provide the above guarantees the data is saved in 2 copies in 2 adjacent
storage 'cells'.

![NvTx cell format](https://github.com/olegv142/NvTx/blob/master/doc/NvTx.jpg)


## Author

Oleg Volkov (olegv142@gmail.com)
