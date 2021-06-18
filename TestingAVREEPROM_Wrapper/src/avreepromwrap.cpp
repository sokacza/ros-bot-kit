#include "avreepromwrap.h"


void EEPROM_Manager_Constructor(EEPROM_Manager * const me, uint32_t _tableStartPoint){
  Serial.println("Constructor called");
   //Attach methods
   me->reinit = &EEPROM_Manager_Constructor;
   me->clear = &EEPROM_Clear;
   me->push = &EEPROM_Push;
   me->pop = &EEPROM_Pop;
   me->resetStartingPoint = &EEPROM_ResetStartingPoint;
   //me->tableSize = &EEPROM_getTableSize;
   me->getTableIndex = &EEPROM_getTableIndex;
   Serial.println("Done attaching methods");
   
   //Set default values   
   me->tableStartPoint = _tableStartPoint;
   me->resetStartingPoint(me, _tableStartPoint);
   Serial.println("Done reseting starting point");
}

void EEPROM_Clear(EEPROM_Manager * const me){
    Serial.println("Inside clear method");
    for(uint16_t i = 0; i < EEPROM_SIZE; i++){
      eeprom_write_block( NULL, (void*)i, sizeof(uint8_t));
    }
  Serial.println("Done loop, resetting starting point");
  me->resetStartingPoint(me, me->tableStartPoint);
}

uint32_t EEPROM_Push(EEPROM_Manager * const me, EEPROM_Data * const _eeprom_data){
  //Validating the function parameters
  if(((me->tableSize) + sizeof(_eeprom_data)) > ((size_t) EEPROM_SIZE))
    return (uint32_t)0; //FAILURE, ran out of room in the EEPROM
  
  //Push to new block in EEPROM
  eeprom_update_block((const void *) _eeprom_data, /*double check*/(void*)(size_t)(me->tableSize + (uint32_t)1), sizeof(_eeprom_data));
  me->tableIndex ++;

  return (uint32_t)1;
}

EEPROM_Data EEPROM_Pop(EEPROM_Manager * const me){
  EEPROM_Data val; 
  eeprom_read_block((void*) &val, (const void*)(me->tableIndex), sizeof(EEPROM_Data));
  //Pushing NULL to the end block (LIFO format)
  eeprom_update_block((const void*)0, (void*) (me->tableIndex), sizeof(EEPROM_Data));
  me->tableIndex --;

  return val;
}

void EEPROM_ResetStartingPoint(EEPROM_Manager* const me, uint32_t * tableStartPoint){
  me->tableIndex = 0;
  me->tablePtr = (uint32_t*)(tableStartPoint + (uint32_t)2);
  eeprom_write_block(&tableStartPoint, &tableStartPoint, sizeof(uint32_t));
  eeprom_write_block(&(me->tableIndex), (&tableStartPoint + sizeof(uint32_t)), sizeof(uint32_t));

}
