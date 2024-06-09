#include <Servo.h>
Servo myservo;

//FUNCTION DECLARATIONS
  void read_buttons();
  void freeflow();
  int cup_selection();
  int size_selection();
//PIN ASSIGNMENT
  int cup_leds[] = {23, 25, 27, 29};
  int cup_buttons[] = {22, 24, 26, 28};
  int size_leds[] = {31, 33, 35, 37};
  int size_buttons[] = {30, 32, 34, 36};

  int ff_button = 40;
  int ff_led = 41;
  int start_button = 38;
  int start_led = 39;

  int pump = 50;
  const int servo = 10;

//BUTTON STATES
  int buttonState[10];
  int buttonValue[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//LED STATES
  int ledstates[] = {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW};

//STATE INDICES
  int cup1_state = 0, cup2_state = 1, cup3_state = 2, cup4_state = 3, size1_state = 4, size2_state = 5, size3_state = 6, size4_state = 7, start_state = 8, ff_state = 9;

//AVAILABLE CUP SIZES
  int sizes[] = {8, 10, 12, 16};


void setup() {
  Serial.begin(9600);
  myservo.attach(servo);

  for(int i=0; i<4; i++){
  pinMode(cup_leds[i], OUTPUT);
  pinMode(size_leds[i], OUTPUT);
  pinMode(cup_buttons[i], INPUT);
  pinMode(cup_buttons[i], INPUT);
  }
  pinMode(ff_led, OUTPUT);
  pinMode(ff_button, INPUT);
  pinMode(start_led, OUTPUT);
  pinMode(start_button, INPUT);

  pinMode(pump, OUTPUT);

  }
//_________________________________________________________________________________________________

void loop() {
  int cupNumber;
  int cupSizes[4];
  myservo.write(0);

  read_buttons();   //checking button states for the blinking sequence (the while loop that follows)
  Serial.flush();

  //INITIAL BLINKING SEQUENCE
   while((buttonValue[cup1_state] != 1) && (buttonValue[cup2_state] != 1) && (buttonValue[cup3_state] != 1) && (buttonValue[cup4_state] != 1) && (buttonValue[ff_state] != 1)){
    read_buttons();
    for(int i=0; i<4; i++)
      digitalWrite(cup_leds[i], HIGH); 
    digitalWrite(ff_led, HIGH);  
    delay(50);
    read_buttons();
    for(int i=0; i<4; i++)
      digitalWrite(cup_leds[i], LOW);    
    digitalWrite(ff_led, LOW); 
    delay(50);  
    }
  
  
  //FREE FLOW PROCEDURE
    if(buttonValue[ff_state] == 1){
      freeflow_V2();
      resetEverything();
      digitalWrite(pump, LOW);
      delay(1000);
      return;
      }
  
  // Cup(s) selection
    cupNumber = cup_selection();

  // Size selection(s)
    for(int i=0; i < cupNumber; i++){
      delay(750);
      digitalWrite(cup_leds[i], HIGH);
      cupSizes[i] = size_selection();

      digitalWrite(cup_leds[i], LOW);
      digitalWrite(cup_leds[cupNumber-1], HIGH);
      delay(250);
      resetButtons();
    }


  // Wait for start button
    waitForStartButton();
    Serial.print("Begin pouring \t |||");
  
  // Add servo and pump control logic here
  Serial.print ("cup itteration \n"); ///
    for(int i=0; i<cupNumber; i++){
      Serial.print ("Sport Control \n"); ////
      spoutControl(i);
      delay(500);
      Serial.print ("Pump control \n"); ////
      pumpControl(cupSizes[i]);
    }

  resetEverything();
  myservo.write(0);
  delay(1000);
  Serial.print("--------- \n");
}

//_________________________________________________________________________________________________

void read_buttons(){
  for (int i = 0; i < 4; i++) {
    buttonState[i] = digitalRead(cup_buttons[i]);
    if (buttonState[i] == HIGH) {
      buttonValue[i] = 1;
    }
    buttonState[i + 4] = digitalRead(size_buttons[i]);
    if (buttonState[i + 4] == HIGH) {
      buttonValue[i + 4] = 1;
    }  

  buttonState[ff_state] = digitalRead(ff_button);
      if(buttonState[ff_state] == HIGH)  
        buttonValue[ff_state] = 1;  
  buttonState[start_state] = digitalRead(start_button);
      if(buttonState[start_state] == HIGH)  
        buttonValue[start_state] = 1;            
  }
  delay(100);
  }

void freeflow_V2(){
  resetButtons();
  read_buttons();
   
  digitalWrite(ff_led, HIGH);
  ledstates[ff_state] = HIGH;
  
  waitForStartButton();
  while (digitalRead(start_button) != HIGH) {
    read_buttons();
    digitalWrite(start_led, HIGH);
    myservo.write(0);
    digitalWrite(pump, HIGH);
  }
  delay(100);
  if(digitalRead(start_button) == HIGH && ledstates[start_state] == HIGH){
    resetButtons();
    digitalWrite(start_led, LOW);
    digitalWrite(ff_led, LOW);
    delay(1000);
    
    return;
  }

}

void turnOffleds(){
  for(int i=0; i<4; i++){
    digitalWrite(cup_leds[i], LOW);
    digitalWrite(size_leds[i], LOW);
  }
  digitalWrite(ff_led, LOW);
  digitalWrite(start_led, LOW);
  for(int i=0; i<10; i++){
    ledstates[i] = LOW;
  }
  }
void resetButtons(){
  for(int i = 0; i<10; i++)     //RESET ALL OF THE BUTTON STATE VALUES
    buttonValue[i] = 0;
 }

int cup_selection(){
  int cupNum = 0;
  read_buttons();
  for (int i = 0; i < 4; i++) {
    if (buttonValue[i] == 1) {
      cupNum = i + 1;
      for (int j = 0; j < 4; j++) {
        digitalWrite(cup_leds[j], (i == j) ? HIGH : LOW);
      }
      break;
    }
  }
  Serial.print("Cup Number: ");
    Serial.print(cupNum);
    Serial.print("\t");
  return cupNum;
  }


int size_selection(){
  int cupSize = 0; 
  digitalWrite(ff_led, LOW);
  digitalWrite(start_led, LOW); 
  ledstates[ff_state] = LOW;
  ledstates[start_state] = LOW;
  buttonValue[start_state] = buttonValue[ff_state] = 0;
  read_buttons();
  
  //INITIAL BLINKING SEQUENCE
   while((buttonValue[size1_state] != 1) && (buttonValue[size2_state] != 1) && (buttonValue[size3_state] != 1) && (buttonValue[size4_state] != 1)){
    read_buttons();
    for(int i=0; i<4; i++)
    digitalWrite(size_leds[i], HIGH);    
    delay(50);
    
    read_buttons();
    for(int i=0; i<4; i++)
    digitalWrite(size_leds[i], LOW);
    delay(50); 
    }  
  for (int i = 0; i < 4; i++) {
    if (buttonValue[i+4] == 1) {  
      for (int j = 0; j < 4; j++) 
        digitalWrite(size_leds[j], (i == j) ? HIGH : LOW);
      cupSize = sizes[i];
      break;
    }
  }
  Serial.print("Cup Size: ");
    Serial.print(cupSize);
    Serial.print("\t");
  delay(100);
  return cupSize;
  }



void waitForStartButton() {
  while (digitalRead(start_button) != HIGH) {
    digitalWrite(start_led, HIGH);
    delay(150);
    digitalWrite(start_led, LOW);
    delay(150);
  }
  delay(250);
  }
void resetEverything(){
  turnOffleds();
  resetButtons();
}
void spoutControl(int cupNum){
  int position[] = {0, 18, 38, 55};

  myservo.write(position[cupNum]);
  Serial.print("Position at ");
    Serial.print(position[cupNum]);
    Serial.print(" degrees. \n");

  delay(2000);



}
void pumpControl(int cupSize){
  int flowRate = 1;  //Units: fl oz/second
  int pouringTimes[] = {8*flowRate, 10*flowRate, 12*flowRate, 16*flowRate}; //How long to keep the pump on for each cup size
  int timeIndex;
  
  if(cupSize == 8)
    timeIndex = 0;
  if(cupSize == 10)
    timeIndex = 1;
  if(cupSize == 12)
    timeIndex = 2;
  if(cupSize == 16)
    timeIndex = 3;    



  digitalWrite(pump, HIGH);
  delay(pouringTimes[timeIndex]*1000);
  digitalWrite(pump, LOW);
  
  Serial.print("Pump on for ");
    Serial.print(pouringTimes[timeIndex]);
    Serial.print(" seconds. \n");
}

/*
void freeflow(){
  resetButtons();
  read_buttons();
  //BLINKING SEQUENCE FOR FF AND START LED
    while(buttonValue[ff_state] != 1 && buttonValue[start_state] != 1){   
      //read_buttons();
      resetButtons();
      digitalWrite(start_led, HIGH);  
      digitalWrite(ff_led, HIGH);  
      delay(150);
      // read_buttons();
      digitalWrite(start_led, LOW);    
      digitalWrite(ff_led, LOW); 
      delay(150); 
      read_buttons(); 
     }
    read_buttons();    
  //SELECTING MANUAL FREE FLOW
    if(buttonValue[ff_state] == 1 && buttonValue[start_state] == 0){
      digitalWrite(ff_led, HIGH);
      digitalWrite(start_led, LOW); 
      ledState_ff = HIGH;
      ledState_start = LOW;     
      buttonValue[start_state] = 0;

      for(int i=0; i<9; i++){
        ledstates[i] = LOW;
        buttonstates[i] = LOW;
        } 
       
       digitalWrite(leds[i], LOW);
       resetButtons();
       }

  //SELECTING AUTOMATIC FREE FLOW
    if(buttonValue[ff_state] == 0 && buttonValue[start_state] == 1){    
      while(buttonValue[ff_state] != 1 && buttonValue[start_state] != 0){
        digitalWrite(ff_led, HIGH);
        digitalWrite(start_led, HIGH); 
        ledState_ff = HIGH;
        ledState_start = HIGH;     
        buttonValue[ff_state] = 1;
        buttonValue[start_state] = 1;
        read_buttons();
        if(ledState_start == HIGH & buttonValue[start_state] == 1){   //TURN OFF CONDITION
          digitalWrite(ff_led, LOW);
          digitalWrite(start_led, LOW); 
          ledState_ff = LOW;
          ledState_start = LOW;     
          buttonValue[ff_state] = buttonValue[start_state] = 0;
          delay(1000);
          loop();
        }

        for(int i=0; i<8; i++){
          digitalWrite(leds[i], LOW);
          buttonValue[i] = 0;
          ledstates[i] = LOW;
        }
        buttonValue[ff_state] = 0;

      }
    }
  
  resetButtons();
 // goto restart;
  }
*/