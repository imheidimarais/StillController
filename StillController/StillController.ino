#include <OneWire.h>
#include <DallasTemperature.h>

#define BAUDRATE 9600
#define MEASUREMENTFAIL -126.0f  // functions will return -127 for an error
#define N_MAX 3 // total number of measurements
#define MEASUREMENT_LOOP_DELAY 1000

//defines for DS18B20
#define TEMPERATURE_BUS_WPS1 22
#define SENSOR_RESOLUTION 11

//global variables
//WPS1 = waterproof sensors still 1
DeviceAddress BUS_WPS1_Sensors[3] = { {0x28, 0xFF, 0xE1, 0x5E, 0x60, 0x17, 0x05, 0x8A}, //probe1
                                       {0x28, 0xFF, 0x72, 0x74, 0x30, 0x17, 0x04, 0x3D}, //probe2
                                       {0x28, 0xFF, 0xA6, 0x45, 0x60, 0x17, 0x03, 0x43} }; //probe3

OneWire oneWire(TEMPERATURE_BUS_WPS1);
DallasTemperature DS18B20_WPS1_sensors(&oneWire);

//function prototypes
byte measurement_loop();
byte measure_sensor(byte N);
float measure_temperature_DS18B20(byte N);

void setup() {
  // initialisation step
  Serial.begin(BAUDRATE);
  DS18B20_WPS1_sensors.begin();

  //set resolution for DS18B20 WP S1
  for(byte i = 0; i < 3 ; i+=1 ){
    DS18B20_WPS1_sensors.setResolution(BUS_WPS1_Sensors[i], SENSOR_RESOLUTION);
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
  
  if(0 <= N < 3){
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
  Serial.println("Error");
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
  if(0 <= N && N < 3){
    DS18B20_WPS1_sensors.requestTemperatures();
    return DS18B20_WPS1_sensors.getTempC(BUS_WPS1_Sensors[N]);
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



