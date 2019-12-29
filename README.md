# NvTx
Transactional non volatile storage for Arduino

Transactional non-volatile storage uses EEPROM for reliably storing
client's data. It provides the following useful properties:
 - client will not read the data it have never written
 - client will not read the data corrupted for any reason
 - the data update will succeed either entirely or not at all, in the latter
   case old data will be read
 - the data is bound to the semantic tag (the string) provided by the client so
   the data originally bound to the different tag will not be written to avoid
   erroneous interpretation of the semantically different data
 
To provide the above guarantees the data is saved in 2 copies in 2 adjacent
storage 'cells'. Each copy is protected by 2 checksums calculated based on the
semantic tag and the content of the data.

## Author

Oleg Volkov (olegv142@gmail.com)
