#include <Arduino.h>
#include "avreepromwrap.h"


EEPROM_Manager eeprom;
EEPROM_Data data = {
  .data1 = 69.0f
};

void setup() {
  Serial.begin(57600);
  EEPROM_Manager_Constructor(&eeprom, 0);
  Serial.println("HELLO");
  Serial.print("Items: ");
  Serial.println(eeprom.tableSize);
  



}

void loop(){
  for(int i = 0; i< 50; i++){
    data.data1 = (float) i;
    Serial.print("Pushing value to table: ");
    Serial.print("(");
    Serial.print(data.data1);
    Serial.println(")");
    eeprom.push(&eeprom, &data);
    delay(200);
  }
  delay(2000);
  Serial.print("Index: ");
  Serial.println(eeprom.getTableIndex(&eeprom));
  delay(2000);
  Serial.println("Popping");
  eeprom.pop(&eeprom);
  delay(2000);
  Serial.println("__POPPED__");
  Serial.println(eeprom.getTableIndex(&eeprom));


  for(uint32_t i = 0; i< eeprom.getTableIndex(&eeprom); i++){
    uint32_t * ptr = (uint32_t *) ((size_t)i * sizeof(EEPROM_Data)) + (uint32_t*) (eeprom.tablePtr);
    eeprom_read_block(&data, ptr, sizeof(EEPROM_Data));
    Serial.println(data.data1);
  }

}