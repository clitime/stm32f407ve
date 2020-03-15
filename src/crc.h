#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>


/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021    x^16 + x^12 + x^5 + 1
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 byte (32767 bit)
*/
uint16_t calcCrc16CCITT(const uint8_t *data, uint16_t len);


#endif
