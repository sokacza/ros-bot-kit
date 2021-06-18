#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__
//#include "main.h"
//#define DEBUGPRINT

#include "ros.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"


typedef void (*sm_funcType)(void*);
typedef enum {
  FAILURE = 0, SUCCESS
} sm_ret_t;

class SM_Manager{

private:
  size_t numStates;
  uint8_t stateIndex;
  sm_funcType *state_fcn_list;
  void (*eeprom_fcn)(void * val);

public: 

  //Getters
  uint8_t getStateIndex();
  size_t getNumStates();
  //Setters
  void setStateIndex(uint8_t _stateIndex);

  //Methods
  void (*decision_fcn)(volatile geometry_msgs::Twist *); 
  void RunState(void* _state_param);
  void UpdateNumStates();

  //Constructor
  SM_Manager(void (*_decision_fcn)(volatile geometry_msgs::Twist *), sm_funcType *);
};







#endif //__STATEMACHINE_H__