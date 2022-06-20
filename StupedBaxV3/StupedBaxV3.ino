#include <avr/sleep.h>
#include <Servo.h>

volatile const int wakePin = 2; // pin used for waking up
volatile const int LEDPin = 7;
volatile const int armPin = 9;
volatile const int lidPin = 10;
volatile const int mosfetPin = 8;

volatile int isActive;
volatile int pos = 0;
volatile int lidPos = 110;
volatile int LA = 0;
volatile int AA = 0;

//Arm
// Hit = 145
// Close = 0

//Lid
// Open = 0 / 10 (fully) (30 - exact)
// Closed = approx 110
 
//Create servo
Servo armServo;
Servo lidServo;

///////////////////////////////////////////////////////////////////////////////////
void setup() {

  ///// Settings //////
  LA = 0; // Lid offset
  AA = -13; // Arm offset
  
  pinMode(wakePin, INPUT_PULLUP); 
  pinMode(mosfetPin, OUTPUT);

  /////////////////////////////////
  digitalWrite(mosfetPin, LOW);
  armServo.attach(armPin);
  armServo.write(0);

  lidServo.attach(lidPin);
  lidServo.write(lidPos);
  delay(1000);
  digitalWrite(mosfetPin, HIGH);
  /////////////////////////////////

  // Check init values to ensure correct sleep
  if (digitalRead(wakePin) == LOW) { // If switch is turned on enable sleep interrupt
    isActive = 1;
    attachInterrupt(0, setSleep, HIGH);
  } else {
    isActive = 0;
    sleepNow(); // If switch is off go to sleep
  }

}

///////////////////////////////////////////////////////////////////////////////////
void loop() {
  if ((digitalRead(wakePin) == LOW) && (isActive == 1)) { 
    digitalWrite(mosfetPin, LOW);
 
    switch (random(1, 6+1)) {

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
        
     case 5:
        run5();
        break;
        
     case 6:
        run6();
        break;

      default:
        break;

    }

    delay(500);
    sleepNow();

  } else {
    sleepNow();
  }

}

///////////////////////////////////////////////////////////////////////////////////

void sleepNow()
{ 
  digitalWrite(mosfetPin, HIGH);

  sleep_enable();
  detachInterrupt(0); // Remove the sleep interrupt
  attachInterrupt(0, wakeUpNow, LOW); // Add wakeUp interrupt
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_cpu();//activating sleep mode   

}

///////////////////////////////////////////////////////////////////////////////////
void setSleep() { // Here goes the interrupt when going to sleep
  isActive = 0;
}


void wakeUpNow() // Here goes the interrupt when getting awake
{
  sleep_disable(); //Disable sleep mode.
  detachInterrupt(0); //Remove interrupt from pin2
  attachInterrupt(0, setSleep, HIGH); // Enable sleep interrupt
  isActive = 1;
}



///////////////////////////////////////////////////////////////////////////////////
// Tease
void run1() {
  openLid(30, 30);
  forward(134, 30);
  hold(2000);
  back(50, 30);
  forward(155, 0);

  while (isActive) {}
  backEnd(0, 20);
  closeLid(110, 20);
}
 
//Quick
void run2() {
  openLid(30, 10);
  forward(155, 1);

  while (isActive) {}
  backEnd(0, 10); 
  closeLid(110, 10);
}

//Really quick
void run3() { 
  openLid(30, 5);
  forward(155, 0);

  while (isActive) {}
  backEnd(0, 10);  
  closeLid(110, 1);
}

//Really really quick
void run4() {  
  forward(155, 0);

  while (isActive) {}
  backEnd(0, 0);   
}

//Funny open/close
void run5() { 
  openLid(55, 5);
  closeLid(110,5);
  openLid(55, 5);
  closeLid(110,5);
  openLid(55, 5);
  closeLid(110,5); 
  hold(3000);

  openLid(30, 5);
  forward(155, 1);
  
  while (isActive) {}
  backEnd(0, 10);  
  closeLid(110, 10);
}

//Open lid, look
void run6() { 
  openLid(80, 2);
  hold(3000);
  closeLid(110,2);
  hold(2000);
  forward(155, 0);

  while (isActive) {}
  backEnd(0, 0);   
}


void openLid(int newPos, int speed) {

  for (unsigned int i = lidPos; i > newPos && isActive == 1; i -= 2) {
    lidPos -= 2;
    lidServo.write(i+LA);
    delay(speed);
  }

}

void closeLid(int newPos, int speed) {
  for (unsigned int i = lidPos; i < newPos; i += 2) {
    lidPos += 2;
    lidServo.write(i+LA);
    delay(speed);
  }
}


void forward(int newPos, int speed) {

  for (unsigned int i = pos; i < newPos && isActive == 1; i += 2) {
    pos += 2;
    armServo.write(i+AA);
    delay(speed);
  }
}

void back(int newPos, int delayTime) {
  for (unsigned int i = pos; i > newPos && isActive == 1; i -= 2) {
    pos -= 2;
    armServo.write(pos+AA);
    delay(delayTime);
  }

}

void backEnd(int newPos, int delayTime) {
  for (unsigned int i = pos; i > newPos; i -= 2) {
    pos -= 2;
    armServo.write(pos+AA);
    delay(delayTime);
  }
  pos = newPos;
}

void hold(int ms) {
  for (unsigned int i = 0; i <= 100 && isActive == 1; i += 1) {
    delay(ms / 100);
  }
}
