#include <avr/sleep.h>
#include <Servo.h>

volatile const int buttonPin  = 2;
volatile const int sensorPin  = 3; 
volatile const int LEDPin     = 7;

volatile const int armPin     = 9;
volatile const int lidPin     = 10;
volatile const int mosfetPin  = 8;


volatile bool lidActive = false;
volatile bool armActive  = false;

// Saves positions of servos
volatile int pos = 0;
volatile int lidPos = 110;

// Create servos
Servo armServo;
Servo lidServo;

// Lid
// Closed       = 110
// Fully open   = 10

// Arm
// Closed = 0
// Touch  = 120??

void setup() {

  //Inputs
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(sensorPin, INPUT);
  //Outputs
  pinMode(LEDPin, OUTPUT);
  pinMode(mosfetPin, OUTPUT);

  //Servo initialization
  digitalWrite(mosfetPin, LOW);   // Turn on servos
  
  armServo.attach(armPin);        // Attach arm
  armServo.write(0);              // Closed value

  lidServo.attach(lidPin);        // Attach lid 
  lidServo.write(110);            //   Closed value
  
  delay(1000);                    // Give some time
  digitalWrite(mosfetPin, HIGH);  // Turn off servos
  //

   
  // If button is active from beginning, run program
  if(digitalRead(buttonPin) == LOW) {
    digitalWrite(LEDPin, HIGH); 

  // If Sensor is active from beginning, run lid
  } else if(digitalRead(sensorPin) == HIGH) {
    digitalWrite(LEDPin, HIGH); 
    
  } else {
      // None active, goto sleep!
     sleepNow();
  }

  
}

void loop() {

  // Button has been pushed
  if(armActive) {
    digitalWrite(LEDPin, HIGH); 
    digitalWrite(mosfetPin, LOW);
    switch (random(1, 4+1)) {
                  case 1:
                    run1();
                    break;
            
                  case 2:
                    run2();
                    break;
            
                 case 3:
                    run3();
                    break;
            
                 case 4:
                    run4();
                    break;
            
                  default:
                    break;
         }  


  // Movement detected
  }else if(lidActive && !armActive) {
      digitalWrite(LEDPin, HIGH); 
      digitalWrite(mosfetPin, LOW);
      switch (random(1, 2+1)) {
                  case 1:
                    lid1();
                    break;
            
                  case 2:
                    lid2();
                    break;  
            
                  default:
                    break;
         };
      
  } else {
    // None active, goto sleep! 
    sleepNow();
  }

      
}

void armSleep() {
  armActive = false;
}
void lidSleep() {
  lidActive = false;
}


// Interrupt routine for button
void buttonInt() { 
  sleep_disable(); //Disable sleep mode.
  detachInterrupt(digitalPinToInterrupt(buttonPin)); //Remove interrupt from button 
  attachInterrupt(digitalPinToInterrupt(buttonPin), armSleep, HIGH); // Enable sleep interrupt
  armActive = true; 
}

// Interrupt routine for sensor
void sensorInt() { 
  sleep_disable(); //Disable sleep mode.
  detachInterrupt(digitalPinToInterrupt(sensorPin)); //Remove interrupt from sensor
  attachInterrupt(digitalPinToInterrupt(sensorPin), lidSleep, FALLING); // Enable sleep interrupt
  lidActive = true;
}

void sleepNow()
{
  digitalWrite(LEDPin, LOW);      // Turn off LED
  digitalWrite(mosfetPin, HIGH);  // Turn off servos

  sleep_enable();  
  
  detachInterrupt(digitalPinToInterrupt(buttonPin)); // Remove sleep interrupt buttonPin
  detachInterrupt(digitalPinToInterrupt(sensorPin)); // Remove sleep interrupt sensorPin
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInt, LOW); // Add button wakeup interrupt
  attachInterrupt(digitalPinToInterrupt(sensorPin), sensorInt, RISING); // Add sensor wakeup interrupt
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu(); // Active sleep!!!! zZzZzzzZ 

}

////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////   PROGRAMS   ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
// Tease
void run1() {
  openLid(30, 30);
  forward(123, 30);
  hold(2000);
  backEnd(0, 30); 
  closeLid(110, 10);
  hold(2000); 
  forward(145, 0);
  
  while (armActive) {}
  backEnd(0, 0); 
  delay(360); // NECCESARY !!!!!!!!!!!!!!!
}
 
//Quick
void run2() {
  openLid(30, 10);
  forward(145, 1);

  while (armActive) {}
  backEnd(0, 10); 
  closeLid(110, 10);
}

//Really quick
void run3() {  
  forward(145, 0);

  while (armActive) {} 
  backEnd(0, 0); 
  delay(360); // NECCESARY !!!!!!!!!!!!!!!
}

//Funny
void run4() { 
  openLid(30, 5);
  closeLid(110,5);
  openLid(30, 5);
  closeLid(110,5);
  openLid(30, 5);
  closeLid(110,5); 
  hold(3000);

  openLid(30, 5);
  forward(145, 1);
  
  while (armActive) {}
  backEnd(0, 10);  
  closeLid(110, 10);
}



////////////////////////// SENSOR //////////////////////////////
//Lid - Just open
void lid1() {  
  openLid(70, 0); 
  while (lidActive) {} 
  if(digitalRead(buttonPin) == HIGH) {
    closeLid(110,0);   
    delay(300); // NECCESARY !!!!!!!!!!!!!!!
  }
}

// Keep button unavaible to be pushed
void lid2() {  
  forward(135, 0);
  delay(5000); // Wait 7 sec before going down
  while (lidActive) {} 
  if(digitalRead(buttonPin) == HIGH) {
    backEnd(0, 0); 
    delay(360); // NECCESARY !!!!!!!!!!!!!!!
  }
} 


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////   FUNCTIONALITY   /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void openLid(int newPos, int speed) {
  for (unsigned int i = lidPos; i > newPos && (armActive || lidActive); i -= 2) {
    lidPos -= 2;
    lidServo.write(i);
    delay(speed);
  }

} 

void closeLid(int newPos, int speed) {
  for (unsigned int i = lidPos; i < newPos; i += 2) {
    lidPos += 2;
    lidServo.write(i);
    delay(speed);
  }
}

void forward(int newPos, int speed) {

  for (unsigned int i = pos; i < newPos && (armActive ||  lidActive); i += 2) {
    pos += 2;
    armServo.write(i);
    delay(speed);
  }
}

void back(int newPos, int delayTime) {
  for (unsigned int i = pos; i > newPos && armActive == 1; i -= 2) {
    pos -= 2;
    armServo.write(pos);
    delay(delayTime);
  }

}

void backEnd(int newPos, int delayTime) {
  for (unsigned int i = pos; i > newPos; i -= 2) {
    pos -= 2;
    armServo.write(pos);
    delay(delayTime);
  }
  pos = newPos;
}

void hold(int ms) {
  for (unsigned int i = 0; i <= 100 && armActive == 1; i += 1) {
    delay(ms / 100);
  }
}
