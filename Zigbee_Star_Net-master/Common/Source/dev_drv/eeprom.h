/*
 * eeprom.h
 *
 *  Created on: 2017-7-13
 *      Author: xubov
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "node.h"

void EEPROM_init(void);
int EEPROM_deleteAllRecord(void);
int EEPROM_write(uint8* data, uint16 len, uint8 section, uint16 offset);
int EEPROM_read(uint8* data, uint16 len, uint8 section, uint16 offset);
int EEPROM_deleteRecord(uint8 section);
int EEPROM_saveNodeParm(NODE_tsData *NODE_sData);
int EEPROM_readNodeParm(NODE_tsData *NODE_sData);
int EEPROM_cleanNodeParm(void);
void EEPROM_WR_test(void);
#endif /* EEPROM_H_ */
