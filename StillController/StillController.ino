#define BAUDRATE 9600
#define MEASUREMENTFAIL -126F  // functions will return -127 for an error
#define N_MAX 10 // total number of measurements

byte measurement_loop();
byte measure_sensor(byte N);

void setup() {
  // initialisation step
  Serial.begin(BAUDRATE)
}

void loop() {
  // put your main code here, to run repeatedly:
  //1. wait for data on the serial bus

  byte cmd;
  switch(cmd){
    case 0:
      // do thing associated with cmd0
    case 1:
      // start measurement loop
      measurement_loop();
  }
}

byte measurement_loop(){
  // check for data on serial bus
  // if no data coms false, if data coms true
  for(byte N = 0; ; N+=1){
    byte coms;
    coms = false;
  
    if(coms){
      // handle this
      break;
    }
    else{
      
      // work through sensors
      measure_sensor(N);
      if(N == N_MAX){
        N = 0;
      }
    }
  }
}

byte measure_sensor(N){
  // example structure
  float measurement;
  
  if(1 <= N <= 4){
    measurement = measure_temperature_type1(N);
    
  }
  else if(4 < N <=6){
    measurement = measure_temperature_type2(N);
  }

 // log
 if(measurement <= MEASUREMENTFAIL){
  // call log error
 }
 else{
  // call log measurement
 }
 // return to measurement loop
}

byte log_measurement(){
  
}

byte log_error(){
  
}

