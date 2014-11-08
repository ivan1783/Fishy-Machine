

int numberOfRevolutions =16;          // how many steps are between two feeder holes
int stepsPerRevolution = 12;          // how many steps are in one revolution - set per motor type

// Pin names to physical location
int homeLED = 11;
int readyLED = 12;
int runLED = 13;
int coilOne = 2;
int coilTwo = 14;
int enable = 10;
int homeButton = 5;
int startButton = 8;

int homeButtonValue = 0;

//limit swtich pin number
int limitSwitch = 9;

//button to start feeding
int feed = 0;

int stepTimeInput = 2;
//optimal value for 12v on EM-319 motor is 2.4ms between steps
float stepTime = 2500;

//initial value for limit switch 8(allows home process to start
int limitSwitchValue = 0;

// number of times to run Stepper function
int numberOfSteps = numberOfRevolutions * stepsPerRevolution;

//value for number of ms in 30s
unsigned long feedRate = 30000;

//position of feeder
int position = 0;

//is the machine homed??
int homed = 0;

void setup()
{

  //pin configuration
  pinMode(coilOne, OUTPUT); 
  pinMode(coilTwo, OUTPUT); 
  pinMode(enable, OUTPUT);
  pinMode(homeButton, INPUT_PULLUP);
  pinMode(startButton, INPUT_PULLUP);
  digitalWrite(coilOne, LOW);
  digitalWrite(coilTwo, LOW);
  digitalWrite(enable, LOW);
  pinMode(limitSwitch, INPUT);
  //pin config run led
  pinMode(runLED, OUTPUT);
  digitalWrite(runLED, LOW);
  //pin config home led
  pinMode(homeLED, OUTPUT);
  digitalWrite(homeLED, LOW);
  //pin config ready led
  pinMode(readyLED, OUTPUT);
  digitalWrite(readyLED, LOW);
}

void loop()
{

  //check status of home button
  homeButtonValue = digitalRead(homeButton);
  //check status of start button
  feed = digitalRead(startButton);
  //if the machined is not homed, set the home led to on, else set the homed led to off and turn the ready led on
  if (homed == 0) {
    digitalWrite(homeLED, HIGH);
  } else {
     digitalWrite(homeLED, LOW);
     digitalWrite(readyLED, HIGH);
  }   
  delay(100);
  //home routine
  if (homeButtonValue == LOW) {
   SteppingHome(numberOfSteps, stepTime);
   homed = 1; 
  }  
  else{

  }
  //feed routine, if the machine is homed and the start button is pressed start feeding
  if (feed == LOW && homed == 1) {
    digitalWrite(readyLED, LOW);
    feedLoop(position, feedRate, numberOfSteps, stepTime);
  }
  else {
    
    }
}

int feedLoop(int position, long feedRate, int numberOfSteps, float stepTime){

  digitalWrite(runLED, HIGH);
  unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

    while(position < 14){
      
      digitalWrite(runLED, HIGH);
      delay(1000);
      digitalWrite(runLED, LOW);
      delay(1000);
      
      currentMillis = millis();
      
      //if position is before the end and the current time is larger than the previous time minus the feedRate, execute a feed
      if ((unsigned long)(currentMillis - previousMillis) >= feedRate && position <= 14) {
         //feed 
         SteppingBack(numberOfSteps, stepTime);
         //track position, if condition will not be reached if feeding machine is at the end
         position ++;
         previousMillis = currentMillis;
      }  
        else{
      }
    }
    homed=0;
    digitalWrite(runLED, LOW);
    digitalWrite(readyLED, LOW);
    digitalWrite(homeLED, HIGH);
}
//function SteppingHome is to find the home position of the machine
int SteppingHome(int numberOfSteps, float stepTime){

  //turn on output stages
  digitalWrite(enable, HIGH);
  delay(10);
  stepTime = stepTime + 3000;
  int acceleration = 160;
  int accelerationSteps = 40;
  //this is called when the "home" button is pressed, runs until limit switch is triggered
  while(limitSwitchValue == 0) {
    
    if (accelerationSteps > 0){
      stepTime = stepTime - acceleration;
      acceleration = acceleration - 4;
      accelerationSteps--;
    }
    
    //step 1
    digitalWrite(coilOne, LOW);
    digitalWrite(coilTwo, LOW);
    delayMicroseconds(stepTime);
    //step 2
    digitalWrite(coilOne, HIGH);
    digitalWrite(coilTwo, LOW);
    delayMicroseconds(stepTime);
    //step 3
    digitalWrite(coilOne, HIGH);
    digitalWrite(coilTwo, HIGH);
    delayMicroseconds(stepTime);
    //step 4
    digitalWrite(coilOne, LOW);
    digitalWrite(coilTwo, HIGH);
    delayMicroseconds(stepTime);
   
    limitSwitchValue = digitalRead(limitSwitch);
  }

  
int deccelerationSteps = 45;
int decceleration = 4;

   while(deccelerationSteps > 0) {
    
    if (deccelerationSteps > 0 ) {
       stepTime = stepTime + decceleration;
       decceleration = decceleration + 4;
        deccelerationSteps--;
    }
    //step 1
    digitalWrite(coilOne, LOW);
    digitalWrite(coilTwo, LOW);
    delayMicroseconds(stepTime);
    //step 2
    digitalWrite(coilOne, HIGH);
    digitalWrite(coilTwo, LOW);
    delayMicroseconds(stepTime);
    //step 3
    digitalWrite(coilOne, HIGH);
    digitalWrite(coilTwo, HIGH);
    delayMicroseconds(stepTime);
    //step 4
    digitalWrite(coilOne, LOW);
    digitalWrite(coilTwo, HIGH);
    delayMicroseconds(stepTime);
  }
  //turn off output stage
  digitalWrite(enable, LOW);
  //reset limit switch variable to 0 (SteppingHome can run again)
  limitSwitchValue = 0;
  //delay before moving SteppingBack in order to take up slack in feed carrier
  delay(100);
  SteppingBack(170, 2500);
}

int SteppingBack(int numberOfSteps, float stepTime){

  //turn on output stages
  digitalWrite(enable, HIGH);
  delay(10);

  stepTime = stepTime + 3000;
  unsigned int acceleration = 160;
  unsigned int decceleration = 4;
  
  for(numberOfSteps; numberOfSteps > 0; numberOfSteps--) {
    
    //step 4
    digitalWrite(coilOne, LOW);
    digitalWrite(coilTwo, HIGH);
    delayMicroseconds(stepTime);
    //step 3
    digitalWrite(coilOne, HIGH);
    digitalWrite(coilTwo, HIGH);
    delayMicroseconds(stepTime);
    //step 2
    digitalWrite(coilOne, HIGH);
    digitalWrite(coilTwo, LOW);
    delayMicroseconds(stepTime);
    //step 1
    digitalWrite(coilOne, LOW);
    digitalWrite(coilTwo, LOW);
    delayMicroseconds(stepTime);
    
    if (acceleration > 0) {
    stepTime = stepTime - acceleration;
    
    acceleration = acceleration - 4;
    }
    
    if (numberOfSteps < 45) {
    stepTime = stepTime + decceleration;
    
    decceleration = decceleration + 4;
    }

  }
  //turn off output stages
  digitalWrite(enable, LOW);
  
}

