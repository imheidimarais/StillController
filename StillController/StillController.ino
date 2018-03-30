#include <OneWire.h>
#include <DallasTemperature.h>

#define BAUDRATE 9600
#define MEASUREMENTFAIL -126.0f  // functions will return -127 for an error
#define MEASUREMENT_LOOP_DELAY 1000

// the order of the sensors is predefined, if adding sensors change relevant num constant
// take care to add the sensors to the bus associated with correct grouping of sensors
#define NUM_WPS1_SENSORS 3
#define WPS1_MININDEX 0
#define WPS1_MAXINDEX 2 // WPS1_MININDEX+NUM_WPS1_SENSORS-1

#define NUM_CHIPS1_SENSORS 5
#define CHIPS1_MININDEX 3 // WPS1_MAXINDEX+1
#define CHIPS1_MAXINDEX 7 // CHIPS1_MININDEX+NUM_CHIPS1_SENSORS-1

#define NUM_WPS2_SENSORS 5
#define WPS2_MININDEX (CHIPS1_MAXINDEX+1)
#define WPS2_MAXINDEX 12 // WPS2_MININDEX+NUM_WPS2_SENSORS-1

#define NUM_CHIPS2_SENSORS 7
#define CHIPS2_MININDEX 13 // WPS2_MAXINDEX+1
#define CHIPS2_MAXINDEX 19 // CHIPS2_MININDEX+NUM_CHIPS2_SENSORS-1

//will change when all DHT added
#define N_MAX  NUM_WPS1_SENSORS+NUM_CHIPS1_SENSORS+NUM_WPS2_SENSORS+NUM_CHIPS2_SENSORS// total number of measurements

//defines for DS18B20
#define TEMPERATURE_BUS_WPS1 22
#define TEMPERATURE_BUS_CHIPS1 23
#define TEMPERATURE_BUS_WPS2 24
#define TEMPERATURE_BUS_CHIPS2 25
#define SENSOR_RESOLUTION 11

//global variables
//WPS1 = waterproof sensors still 1
//CHIPS1 = chip (not waterproof) still 1
DeviceAddress BUS_WPS1_Sensors[3] = { {0x28, 0xFF, 0xE1, 0x5E, 0x60, 0x17, 0x05, 0x8A}, //probe1
                                       {0x28, 0xFF, 0x72, 0x74, 0x30, 0x17, 0x04, 0x3D}, //probe2
                                       {0x28, 0xFF, 0xA6, 0x45, 0x60, 0x17, 0x03, 0x43} }; //probe3

DeviceAddress BUS_WPS2_Sensors[5] = { {0x28, 0xFF, 0x5F, 0x89, 0x60, 0x17, 0x05, 0xE0}, //probe4
                                       {0x28, 0xFF, 0x5A, 0xDE, 0x80, 0x14, 0x02, 0x7D}, //probe5
                                       {0x28, 0xFF, 0x88, 0x43, 0x60, 0x17, 0x03, 0x10}, //probe6
                                       {0x28, 0xFF, 0x93, 0x49, 0x60, 0x17, 0x03, 0x07}, //probe7
                                       {0x28, 0xFF, 0x3C, 0x32, 0x25, 0x17, 0x03, 0x63}}; //probe8
                                       
DeviceAddress BUS_CHIPS1_Sensors[5] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }; //dummy address

DeviceAddress BUS_CHIPS2_Sensors[7] = { {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, //dummy address
                                        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}; //dummy address

OneWire oneWire_WPS1(TEMPERATURE_BUS_WPS1);
OneWire oneWire_WPS2(TEMPERATURE_BUS_WPS2);
DallasTemperature DS18B20_WPS1_sensors(&oneWire_WPS1);
DallasTemperature DS18B20_WPS2_sensors(&oneWire_WPS2);

//function prototypes
byte measurement_loop();
byte measure_sensor(byte N);
float measure_temperature_DS18B20(byte N);

void setup() {
  // initialisation step
  Serial.begin(BAUDRATE);
  DS18B20_WPS1_sensors.begin();
  DS18B20_WPS2_sensors.begin();

  //set resolution for DS18B20 WP S1
  for(byte i = 0; i < NUM_WPS1_SENSORS ; i+=1 ){
    DS18B20_WPS1_sensors.setResolution(BUS_WPS1_Sensors[i], SENSOR_RESOLUTION);
  }
  //set resolution for DS18B20 WP S2
  for(byte i = 0; i < NUM_WPS2_SENSORS ; i+=1 ){
    DS18B20_WPS2_sensors.setResolution(BUS_WPS2_Sensors[i], SENSOR_RESOLUTION);
  }
}

void loop() {
  //1. wait for data on the serial bus
  Serial.println("Input start command");
  while(!Serial.available()){
    delay(200);
  }
  
  byte cmd;

  cmd = Serial.read();
  // ascii code for zero is 48
  switch(cmd-48){
    case 0:
      // do thing associated with cmd0
      break;
    case 1:
      // start measurement loop
      measurement_loop();
      break;
    default:
      Serial.println("not a valid command");
      return;
  }
}

byte measurement_loop(){
  // check for data on serial bus
  // if no data coms false, if data coms true
  for(byte N = 0; ; N+=1){
    delay(MEASUREMENT_LOOP_DELAY);
    byte coms;
    coms = false;
  
    if(coms){
      // handle this
      break;
    }
    else{
      
      // work through sensors
      measure_sensor(N);
      if(N == N_MAX-1){
        N = -1;
      }
    }
  }
}

byte measure_sensor(byte N){
  // example structure
  float measurement;
  
  if(WPS1_MININDEX <= N && N <= CHIPS2_MAXINDEX){  //2nd condition will depend on number of DS18B20 sensors in use which depends on number of stills
    measurement = measure_temperature_DS18B20(N);
    Serial.print("Sensor ");
    Serial.print(N);
    Serial.print(": ");
    Serial.println(measurement);
  }
//  else if(3 <= N < 6){
//    measurement = measure_temperature_DHT22(N);
//  }

 // log
 if(measurement <= MEASUREMENTFAIL){
  // call log error
 }
 else{
  // call log measurement
 }
 // return to measurement loop
 return 0;
}


//DS18B20 temperature measurement
float measure_temperature_DS18B20(byte N){
  //request T for all sensors on bus
  if(WPS1_MININDEX <= N && N <= WPS1_MAXINDEX){
    DS18B20_WPS1_sensors.requestTemperatures();
    return DS18B20_WPS1_sensors.getTempC(BUS_WPS1_Sensors[N]);
  }
  else if(CHIPS1_MININDEX <= N && N <= CHIPS1_MAXINDEX){
    return -127.0f;
  }
  else if(WPS2_MININDEX <= N && N <= WPS2_MAXINDEX){
    DS18B20_WPS2_sensors.requestTemperatures();
    return DS18B20_WPS2_sensors.getTempC(BUS_WPS2_Sensors[N-WPS2_MININDEX]);
  }
  else if(CHIPS2_MININDEX <= N && N <= CHIPS2_MAXINDEX){
    return -127.0f;
  }
  else { 
    return -127.0f;
    // N invalid
  }
  
}

//byte log_measurement(){
//  
//}
//
//byte log_error(){
//  
//}



