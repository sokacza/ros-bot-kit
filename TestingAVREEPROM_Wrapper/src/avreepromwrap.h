#ifndef _AVREEPROMWRAP_H_
#define _AVREEPROMWRAP_H_


#include "inttypes.h"
#include "arduino.h"
#include <avr/eeprom.h>

#define EEPROM_SIZE 1000


typedef struct EEPROM_Data EEPROM_Data;
struct EEPROM_Data{
    float data1;
};


extern EEPROM_Data eeprom_data;

typedef struct EEPROM_Manager EEPROM_Manager;
struct EEPROM_Manager{
    //Variables
    uint32_t tableIndex;
    size_t tableSize;
    uint32_t * tablePtr;
    uint32_t tableStartPoint;
    //Methods
    void (*reinit)(EEPROM_Manager * const me, uint32_t eepromStartPoint);
    void (*clear)(EEPROM_Manager * const);
    uint32_t (*push)(EEPROM_Manager * const me, EEPROM_Data * const);
    EEPROM_Data (*pop)(EEPROM_Manager * const);
    void (*resetStartingPoint)(EEPROM_Manager * const, uint32_t tableStartPoint);
    uint32_t (*getTablePtr)(EEPROM_Manager * const);
    uint32_t (*getTableIndex)(EEPROM_Manager * const);
};

void EEPROM_Manager_Constructor(EEPROM_Manager * const me, uint32_t);
void EEPROM_Clear(EEPROM_Manager * const me);
uint32_t EEPROM_Push(EEPROM_Manager * const me, EEPROM_Data * const _eeprom_data);
EEPROM_Data EEPROM_Pop(EEPROM_Manager * const me);
void EEPROM_CurrentSize(EEPROM_Manager * const me);
void EEPROM_ResetStartingPoint(EEPROM_Manager* const me, uint32_t);
inline uint32_t* EEPROM_getTablePtr(EEPROM_Manager * const me){ return me->tablePtr; };
inline uint32_t EEPROM_getTableIndex(EEPROM_Manager * const me){ return me->tableIndex; };
#endif //_AVREEPROMWRAP_H_