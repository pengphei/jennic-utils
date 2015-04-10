#ifndef UTIL_CRC_H
#define UTIL_CRC_H

unsigned char util_crc8(unsigned char* ucs, unsigned char ucslen, unsigned char crc);
unsigned char util_crc8_by_table(unsigned char* ucs, unsigned char ucslen, unsigned char crc);

#endif // UTIL_CRC_H
