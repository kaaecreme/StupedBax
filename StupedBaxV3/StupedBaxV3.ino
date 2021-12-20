#include <avr/sleep.h>
#include <Servo.h>

volatile const int wakePin = 2;                 // pin used for waking up
volatile const int LEDPin = 13;
volatile const int servoPin = 9;
volatile const int servoLidPin = 10;
volatile const int mosfetPin = 6;

volatile int isActive;
volatile int pos = 0;
volatile int lidPos = 0;

//Create servo
Servo myservo;
Servo myLidservo;

void setup()
{
  pinMode(wakePin, INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);
  pinMode(mosfetPin, OUTPUT);
  
/////////////////////////////////
  digitalWrite(mosfetPin, HIGH);
  myservo.attach(servoPin);
  myservo.write(0);

  myLidservo.attach(servoLidPin);
  myLidservo.write(0);
  delay(1000);
  digitalWrite(mosfetPin, LOW);
/////////////////////////////////
  Serial.begin(9600);

  // Check init values to ensure correct sleep
  if (digitalRead(0) == LOW) { // If switch is turned on enable sleep interrupt
    isActive = 1;
    attachInterrupt(0, setSleep, LOW);
  } else {
    isActive = 0;
    sleepNow(); // If switch is off go to sleep
  }

}

void setSleep() { // Here goes the interrupt when going to sleep
  isActive = 0;
}


void wakeUpNow() // Here goes the interrupt when getting awake
{
  isActive = 1;
}

void sleepNow()
{
  isActive = 0;
  digitalWrite(LEDPin, LOW);
  digitalWrite(mosfetPin, LOW);


  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  detachInterrupt(0); // Remove the sleep interrupt
  attachInterrupt(0, wakeUpNow, LOW); // Add wakeUp interrupt
  sleep_mode();   // SLEEP

  sleep_disable();         // WAKE UP.
  detachInterrupt(0);
  attachInterrupt(0, setSleep, HIGH); // Enable sleep interrupt
}

void loop()
{
  if ((digitalRead(2) == LOW) && (isActive == 1)) {
    Serial.println("Awake");
    digitalWrite(LEDPin, HIGH);
    digitalWrite(mosfetPin, HIGH);


    //    switch (random(1, 3 + 1)) {
    //
    //      case 1:
    //        run1();
    //        break;
    //
    //      case 2:
    //        run2();
    //        break;
    //
    //      case 3:
    //        run3();
    //        break;
    //
    //
    //      default:
    //        break;
    //
    //    }

    run1();
    Serial.println("SLEEP");
    delay(500);
    sleepNow();

  }

}



void run1() {

  openLid(170, 30); 
  forward(150, 30);
  hold(2000);
  back(50, 30);
  forward(174, 0);

  while (isActive) {}
  backEnd(0, 20);
  closeLid(0, 20);
}

void run2() {
  openLid(180, 0);
  forward(174, 0);
  while (isActive) {}
  backEnd(0,10);
  closeLid(0,10);
}

void run3() {
  hold(3000);
  forward(174, 0);
  while (isActive) {}
}



void openLid(int newPos, int speed) {
  for (unsigned int i = lidPos; i < newPos && isActive == 1; i += 2) {
    lidPos += 2;
    myLidservo.write(i);
    delay(speed);
  }
   
}

void closeLid(int newPos, int speed) {
  for (unsigned int i = lidPos; i > newPos; i -= 2) {
    lidPos -= 2;
    myLidservo.write(i);
    delay(speed);
  } 
}


void forward(int newPos, int speed) {

  for (unsigned int i = pos; i < newPos && isActive == 1; i += 2) {
    pos += 2;
    myservo.write(i);
    delay(speed);
  }
}

void back(int newPos, int delayTime) {
  for (unsigned int i = pos; i > newPos && isActive == 1; i -= 2) {
    pos -= 2;
    myservo.write(pos);
    delay(delayTime);
  }

}

void backEnd(int newPos, int delayTime) {
  for (unsigned int i = pos; i > newPos; i -= 2) {
    pos -= 2;
    myservo.write(pos);
    delay(delayTime);
  }
  pos = newPos;
}

void hold(int ms) {
  for (unsigned int i = 0; i <= 100 && isActive == 1; i += 1) {
    delay(ms / 100);
  }
}
