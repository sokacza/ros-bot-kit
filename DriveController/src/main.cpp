//#define DEBUGPRINT
//usually used for boards other than arduino mega because of hardware programmer (i think) but im using a third-party mega
//#define USE_USBCON 

#include "inttypes.h"
#include "ros.h"
#include "geometry_msgs/Twist.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "ros/time.h"
#include "statemachine.h"
#include <Servo.h>

/*--------------------------------------------------------------------------------------------------------------------------------------
          Global Variables
---------------------------------------------------------------------------------------------------------------------------------------*/
static volatile uint8_t global_requestUpdateFlag;
static volatile geometry_msgs::Twist global_twist; 
static const uint32_t global_refreshTime = 500; //ms
bool firstRun = true;
//---------------------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------------------------------------

          Actuation
          
-----------------------------------------------------------------------------------------------------------------------------------------*/
Servo driveServo;
typedef struct ActuationControl ActuationControl;
struct ActuationControl{
    //Servo
    float maxDeg = 180.0f, minDeg = 0.0f;
    float centrePosition = 90.0f;
    float newPosition = 90.0f;

    //Drive Motor
    float maxPWM = 5.0f, minPWM = 0.0f;
    float newPWM = 2.5f;
};
ActuationControl driveControl;
//-----------------------------------------------------------------------------------------------------------------------------------------


/*--------------------------------------------------------------------------------------------------------------------------------------
          Function Prototypes
---------------------------------------------------------------------------------------------------------------------------------------*/
void Wait(void* param);
void TeleopVelocityTwist(void* param);
void Initialize(void* param);
void SerialReconnect(void* param);
void DebugChannel(void* param);
geometry_msgs::Twist TwistCopy(volatile geometry_msgs::Twist *_twist);
void Refresh(volatile geometry_msgs::Twist*);
//---------------------------------------------------------------------------------------------------------------------------------------


/*--------------------------------------------------------------------------------------------------------------------------------------
          ROS Pub/Sub
---------------------------------------------------------------------------------------------------------------------------------------*/
ros::NodeHandle nh;
std_msgs::String _status;
ros::Publisher chatter("chatter", &_status); 

void msg_cb(const geometry_msgs::Twist& twistMsg);
ros::Subscriber<geometry_msgs::Twist> sub ("cmd_vel", msg_cb);
//---------------------------------------------------------------------------------------------------------------------------------------

/*--------------------------------------------------------------------------------------------------------------------------------------
          State Machine Configuration
---------------------------------------------------------------------------------------------------------------------------------------*/
#define S_WAIT 0U
#define S_INITIALIZE 1U
#define S_TELEOP 2U
#define S_SERIALRECONNECT 3U
#define S_DEBUGCHANNEL 4U
sm_funcType state_list[] = {&Wait, &Initialize, &TeleopVelocityTwist, &SerialReconnect, &DebugChannel,  nullptr}; //Jump Vector Table

SM_Manager sm(&Refresh, state_list);  //CONSTRUCTOR
//-----------------------------------------------------------------------------------------------------------------------------------------


/*--------------------------------------------------------------------------------------------------------------------------------------
          Decision Function
---------------------------------------------------------------------------------------------------------------------------------------*/
void Refresh(volatile geometry_msgs::Twist *_twist){
  (void) _twist;

  if(firstRun == true){
    Serial.println(firstRun);
    sm.setStateIndex(S_INITIALIZE);
    firstRun = false;
    Serial.println(firstRun);
  }
  else if (!nh.connected())
    sm.setStateIndex(S_SERIALRECONNECT);
 
  else if(global_requestUpdateFlag == 1){
    sm.setStateIndex(S_TELEOP);
    global_requestUpdateFlag = 0;
  }
  else{
    sm.setStateIndex(S_WAIT);
  }
  
}
//-----------------------------------------------------------------------------------------------------------------------------------------




/*--------------------------------------------------------------------------------------------------------------------------------------
          Methods
---------------------------------------------------------------------------------------------------------------------------------------*/
//#define SIMPLE_DRIVE
void TeleopVelocityTwist(void* param){
  (void) param;
  #ifdef DEBUGPRINT
  Serial.println("2_Func:Teleop");
  #endif
#ifdef SIMPLE_DRIVE
  driveControl.newPosition = global_twist.angular.z;
  if(driveControl.newPosition > 0.1)
    driveServo.write(driveControl.maxDeg);
  else if(driveControl.newPosition < -0.1)
    driveServo.write(driveControl.minDeg);
  else
    driveServo.write(driveControl.centrePosition);

  if(driveControl.newPWM  > 0.0f){
    analogWrite(A0, driveControl.maxPWM);
  }
  else{
    analogWrite(A0, driveControl.minPWM);
  }
#endif //SIMPLE_DRIVE

#ifndef SIMPLE_DRIVE
  //Update Servo Position
  driveControl.newPosition = (global_twist.angular.z + abs(global_twist.angular.z)); //Make range from [-x, +x] to [0, 2x]
  driveControl.newPosition *= (driveControl.maxDeg - driveControl.minDeg);
  driveServo.write(driveControl.newPosition + driveControl.minDeg);

  //Update Drive Motor PWM
  driveControl.newPWM = global_twist.linear.x * 255;
  //Clamp PWM
  if(driveControl.newPWM > 255.0f)
    driveControl.newPWM = 255.0f;
  else if (driveControl.newPWM < 0.0f)
    driveControl.newPWM = 0.0f;
  
  analogWrite(12, (int)driveControl.newPWM);
#endif // NOT SIMPLE_DRIVE
}

void Wait(void* param){
  (void) param;
  #ifdef DEBUGPRINT
  Serial.println("0_Func:Wait");
  #endif
  digitalWrite(13, LOW);
  digitalWrite(12, LOW);

}

void Initialize(void* param){
  #ifdef DEBUGPRINT
  Serial.println("1_Func:Initialize");
  #endif

  //Default Global Values
  global_twist.linear.x = 0.0f;
  global_twist.linear.y = 0.0f;
  global_twist.linear.z = 0.0f;
  global_twist.angular.x = 0.0f;
  global_twist.angular.y = 0.0f;
  global_twist.angular.z = 0.0f;
  global_requestUpdateFlag = 0;
  //Init IO
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  driveServo.attach(7);

  SerialReconnect(param);

}

void SerialReconnect(void* param){
  (void) param;
  #ifdef DEBUGPRINT
  Serial.println("3_Func:SerialReconnect");
  #endif

  //Turn drive motor off for safety
  if(analogRead(12) > 0)
    analogWrite(12, 0);

  while(!nh.connected()){
    #ifdef DEBUGPRINT
    Serial.println("Reconnection Loop");
    #endif
    //nh.getHardware()->setBaud(57600);
    nh.initNode();
    nh.advertise(chatter);
    nh.subscribe(sub);
    nh.spinOnce(); //REQUIRED FOR IT TO SYNC WITH ROSSERIAL NODE
    digitalWrite(13, LOW);
    delay(2000);
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
    digitalWrite(13, HIGH);
    delay(200);
  }
}

void DebugChannel(void* param){
  (void) param;
#ifdef DEBUGPRINT
  Serial.println("4_Func:Debug");
#endif

  String a = ("%d" + sm.getStateIndex());
  memcpy(&(_status.data), &a, sizeof(a));
  chatter.publish(&_status);
}


//-----------------------------------------------------------------------------------------------------------------------------------------



/*--------------------------------------------------------------------------------------------------------------------------------------
          Message Callback
---------------------------------------------------------------------------------------------------------------------------------------*/
void msg_cb(const geometry_msgs::Twist& twistMsg){
  global_twist.linear.x = twistMsg.linear.x;
  global_twist.linear.y = twistMsg.linear.y;
  global_twist.linear.z = twistMsg.linear.z;
  global_twist.angular.x = twistMsg.angular.x;
  global_twist.angular.y = twistMsg.angular.y;
  global_twist.angular.z = twistMsg.angular.z;
  global_requestUpdateFlag = 1; 

#ifdef DEBUGPRINT
  Serial.println("Callback Called");
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------------------


geometry_msgs::Twist TwistCopy(volatile geometry_msgs::Twist *_twist){
  geometry_msgs::Twist _twist_cpy;

  //geometry_msgs::Twist doesnt want to be copied directly or through initializer list
  _twist_cpy.linear.x = _twist->linear.x; 
  _twist_cpy.linear.y = _twist->linear.y; 
  _twist_cpy.linear.z = _twist->linear.z; 
  _twist_cpy.angular.x = _twist->angular.x;
  _twist_cpy.angular.y = _twist->angular.y;
  _twist_cpy.angular.z = _twist->angular.z;

  //_twist_cpy = (const geometry_msgs::Twist) _twist_cpy;

  return _twist_cpy;
}

void setup() {
  #ifdef DEBUGPRINT
  Serial.begin(115200);
  #endif
  
  while(1){  
   sm.RunState(nullptr);
   nh.spinOnce();  
   delay(global_refreshTime);
   sm.decision_fcn(&global_twist);
  }
  
}
void loop(){delay(5000);}  //Serial1.println("Something Messed Up");  
  