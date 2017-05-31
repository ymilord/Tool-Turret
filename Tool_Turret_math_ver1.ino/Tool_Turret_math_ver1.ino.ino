#define ENABLEPIN 9 // must be LOW in order for driver to be turned on
#define M0PIN 8
#define M1PIN 7
#define M2PIN 6
#define RESETPIN 5  // must be HIGH in order for driver to be turned on
#define SLEEPPIN 4  // must be HIGH in order for driver to be turned on
#define STEPPIN 3   // moves the motor one microstep per pulse
#define DIRPIN 2    // specifies which direction the motor will turn 

int step_size = 2;
int res = 4;

int M0_state;
int M1_state;
int M2_state;

float pos = 0;
float vel = 0;

float accel = 1440;   // the constant acceleration during the trapezoidal velocity profile in deg/s^2
float vel_max = 360;    // the max velocity during the trapezoidal velocity profile in deg/s^2


float pos_target = 0;
float pulse_timer = 0;
long pulse_count = 0;
long pulse_target = 0;

void setup() {
  
  Serial.begin(115200);
  Serial.setTimeout(10);
  Serial.println("Connected");
  
  pinMode(ENABLEPIN, OUTPUT);
  pinMode(M0PIN, OUTPUT);
  pinMode(M1PIN, OUTPUT);
  pinMode(M2PIN, OUTPUT);
  pinMode(RESETPIN, OUTPUT);
  pinMode(SLEEPPIN, OUTPUT);
  pinMode(STEPPIN, OUTPUT);
  pinMode(DIRPIN, OUTPUT);
  digitalWrite(DIRPIN, HIGH);
  
  digitalWrite(ENABLEPIN, LOW);
  digitalWrite(RESETPIN, HIGH);
  digitalWrite(SLEEPPIN, HIGH);
  
  M0_state = bitRead(step_size, 2);
  M1_state = bitRead(step_size, 1);
  M2_state = bitRead(step_size, 0);
  
  digitalWrite(M0PIN, M0_state);
  digitalWrite(M1PIN, M1_state);
  digitalWrite(M2PIN, M2_state);

  accel *= res;
  vel_max *= res;
}

void loop() {
  serial_read();

  if (pulse_target != pulse_count){
          
    float t1 = vel_max/accel;
    float p1 = accel*pow(t1,2)/2;
    float p2 = pulse_target - p1;

    if (p1 > pulse_target/2){
      p1 = pulse_target/2;
      p2 = pulse_target/2;
    }
    
    long timer = micros();
    long pulse_timer = micros();

    Serial.println(p1);
    Serial.println(p2);

    pulse();
    while(pulse_count < pulse_target){

      if (pulse_count < p1){
        vel = accel  * sqrt(2*pulse_count/accel);
      }
      if (pulse_count > p2){
        vel = accel * sqrt(2*(pulse_target-pulse_count)/accel);
      }
 
      if (micros() - pulse_timer > 1000000/vel){
        pulse();
        pulse_timer = micros();
//        Serial.print("position: ");
//        Serial.print(pulse_count);
//        Serial.print("  velocity: ");
//        Serial.print(vel);
//        Serial.print('\n');
      } 

//      delayMicroseconds(100);
    }

    pos = 0;
    pulse_count = 0;
    pulse_target = 0;
    pos_target = 0;
    vel = 0;
    
  }
  

  
//  delayMicroseconds(2);
//  digitalWrite(STEPPIN, LOW);
//  Serial.println("step");
//  delayMicroseconds(2);
//  digitalWrite(STEPPIN, HIGH);
//  Serial.println("step");
  
}

void pulse(){
  digitalWrite(STEPPIN, HIGH);
  delayMicroseconds(2);
  digitalWrite(STEPPIN, LOW);
  delayMicroseconds(2);
  pos += 1.8/res;
  pulse_count += 1;
}

void serial_read(){
  
  while (Serial.available() > 0) {      // if there are bytes available in the serial port
    String incomingByte = Serial.readStringUntil('\n');     // read the line in the serial port as a string until '\n'       
    int spacelocation = incomingByte.indexOf(' ');      // find the location of the comma
    String command = incomingByte.substring(0,spacelocation);      // assign the command to a string
    String value = incomingByte.substring(spacelocation+1);      // assign the value to a float
    Serial.print(command);
    Serial.print(" ");
    Serial.print(value);
    Serial.print('\n');

    if (command == "step"){
      step_size = value.toFloat();
      M0_state = bitRead(step_size, 2);
      M1_state = bitRead(step_size, 1);
      M2_state = bitRead(step_size, 0);
  
      digitalWrite(M0PIN, M0_state);
      digitalWrite(M1PIN, M1_state);
      digitalWrite(M2PIN, M2_state);
    }

    if (command == "pos"){
      pos_target = value.toFloat();
      pulse_target = pos_target * res / 1.8;
    }
  }



  

}
