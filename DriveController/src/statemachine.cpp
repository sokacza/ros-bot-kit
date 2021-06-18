#include "statemachine.h"



/*--------------------------------------------------------------------------------------------------------------------------------------
          Constructor
---------------------------------------------------------------------------------------------------------------------------------------*/
SM_Manager::SM_Manager(void (*_decision_fcn)(volatile geometry_msgs::Twist *twist), sm_funcType * _state_fcn_list)
{
  //Default Values
  this->numStates = (size_t)0;
  this->stateIndex = 1U;

  //Connecting Methods
  (this->decision_fcn) = _decision_fcn; 
  (this->state_fcn_list) = _state_fcn_list;

  (void) this->UpdateNumStates();
  
#ifdef DEBUGPRINT
  Serial.print("Func: Constructor, size of fcn list is: ");
  Serial.println(this->numStates);
#endif 

}
//---------------------------------------------------------------------------------------------------------------------------------------

/*--------------------------------------------------------------------------------------------------------------------------------------
          Update Number of States
---------------------------------------------------------------------------------------------------------------------------------------*/
void SM_Manager::UpdateNumStates(){
  this->numStates = (size_t)0;
  for(size_t i = 0; this->state_fcn_list[i] != nullptr; i++){
    this->numStates ++;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------


/*--------------------------------------------------------------------------------------------------------------------------------------
          Update Number of States
---------------------------------------------------------------------------------------------------------------------------------------*/
void SM_Manager::RunState(void* _state_param){

  #ifdef DEBUGPRINT
  Serial.println("Inside of RUNSTATE");
  #endif

  (*state_fcn_list[this->stateIndex])(_state_param);//(_state_param);

}
//---------------------------------------------------------------------------------------------------------------------------------------


/*--------------------------------------------------------------------------------------------------------------------------------------
          Getters/Setters
---------------------------------------------------------------------------------------------------------------------------------------*/
uint8_t SM_Manager::getStateIndex(){
  return this->stateIndex; 
}
size_t SM_Manager::getNumStates(){ 
  return this->numStates; 
};
void SM_Manager::setStateIndex(uint8_t _stateIndex){
  (this->stateIndex) = _stateIndex;

  #ifdef DEBUGPRINT
  Serial.print("StateIndex: ");
  Serial.println(this->stateIndex);
  #endif
}
//---------------------------------------------------------------------------------------------------------------------------------------





