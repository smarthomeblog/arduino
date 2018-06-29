#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_NODE_ID 5

#define CHILD_ID_DOOR 1
#define CHILD_ID_VOLTAGE 2
#define DOOR_SENSOR_PIN 3

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point

#include <MyConfig.h>
#include <MySensors.h>

 

/**************************************************/
/****************** CONSTANTS *********************/
/**************************************************/

static const uint64_t UPDATE_INTERVAL = 1800000;
int oldBatteryPcnt = 0;
int oldValue = 0;
float VBAT_PER_BITS = 0.0032551319648094;
float VMIN = 1.9;
float VMAX = 3.24;

/**************************************************/
/****************** VARIABLES *********************/
/**************************************************/


/**************************************************/
/****************** MESSAGES **********************/
/**************************************************/


MyMessage msgDoor(CHILD_ID_DOOR,V_TRIPPED);
MyMessage msgVBat(CHILD_ID_VOLTAGE,V_CUSTOM);


void presentation()  
{
present(CHILD_ID_DOOR, S_DOOR);
}


void setup()
{

// use the 1.1 V internal reference
#if defined(__AVR_ATmega2560__)
    analogReference(INTERNAL1V1);
#else
    analogReference(INTERNAL);
#endif
  
pinMode(DOOR_SENSOR_PIN,INPUT);
}


void loop()      
{    
    sleep(digitalPinToInterrupt(DOOR_SENSOR_PIN),CHANGE, UPDATE_INTERVAL);
    motion_on();
    getBatteryLevel();
}



/**************************************************/
/**************** AUX. FUNCTIONS ******************/
/**************************************************/
void getBatteryLevel()
{
  // Battery monitoring reading
  int sensorValue = analogRead(BATTERY_SENSE_PIN);
  delay(500);

  // Calculate the battery in %
  float Vbat = sensorValue * VBAT_PER_BITS;
  send(msgVBat.set(Vbat, 3));
  int batteryPcnt = static_cast<int>(((Vbat - VMIN) / (VMAX - VMIN))*100.);
  
    if (batteryPcnt > 100) {
      batteryPcnt = 100;
    }
    
    sendBatteryLevel(batteryPcnt);
}

void motion_on()
{
   
   bool tripped = digitalRead(DOOR_SENSOR_PIN) == LOW;
   send(msgDoor.set(tripped?"0":"1"));  // Send tripped value to gw
}


