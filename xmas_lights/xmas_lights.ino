#include <avr/sleep.h>

/*
 * BLUE
 * RED
 * GREEN
 * GROUND
*/


/*
RED 
Ground
Green
Blue
*/


int red_pin = 3;
int green_pin = 5;
int blue_pin = 6;
const int STEP = 2;
const int DELAY_TIME = 50;
const int MIN = 0;
const int MAX = 255;
const int STROBE = 100; //miliseconds
int mode = 0;

//Non blocking delay, a little better
//Time in milliseconds
void wait(int time) {
  int start = millis();
  while(millis() - start <= time);
}

void brighten(int pin, int time, int step) {
  int i = MIN; 
  while (i < 256) {
    analogWrite(pin, i);
    delay(time);
    i += step;
  }
}

void fade(int pin, int time, int step) {
  int i = MAX;
  while (i >= 0) {
    analogWrite(pin, i);
    delay(time);
    i -= step;
  }
}

//Red and green fade 
//Very slowly
//TODO: replace with something more different than dynamic fade
//like dynamic flash
//TODO need formula to calculate loop speed
void rg_fade() {
  brighten(red_pin, 250, 1);
  fade(red_pin, 250, 1);
  brighten(green_pin, 250, 1);
  fade(green_pin, 250, 1);
}

//brighten red, fade green
void rg_fade_inverse() {
  brighten(red_pin, 100, 5);
  digitalWrite(red_pin, LOW);
  digitalWrite(green_pin, HIGH);
  fade(green_pin, 100, 5);
}

//brighten green, fade red
void gr_fade_inverse() {
  brighten(green_pin, 100, 5);
  digitalWrite(green_pin, LOW);
  digitalWrite(red_pin, HIGH);
  fade(red_pin, 100, 5);
}

//Speed of delay and step are changed
void dynamic_fade() {
  for (int delay = 100; delay < 1000; delay += 100) {
    for (int step = 15; step <= 60; step += 15) {
      brighten(red_pin, delay, step);
      fade(red_pin, delay, step);
      digitalWrite(red_pin, LOW);
      brighten(green_pin, delay, step);
      fade(green_pin, delay, step);
      digitalWrite(green_pin, LOW);
    }
  }
}

//red, yellow, green flashing
void rgy() {
  for (int time = 1000; time < 10000; time += 500) {
    digitalWrite(red_pin, HIGH);
    delay(time);
    analogWrite(green_pin, 70);
    delay(time);
    digitalWrite(green_pin, HIGH);
    digitalWrite(red_pin, LOW);
    delay(time);
    digitalWrite(green_pin, LOW);
  }
}


//red green blue yellow purple
void rgbyp() {
  //red
  digitalWrite(red_pin, HIGH);
  delay(2000);
  //purple
  digitalWrite(blue_pin, HIGH);
  delay(2000);
  //blue
  digitalWrite(red_pin, LOW);
  delay(2000);
  //green
  digitalWrite(blue_pin, LOW);
  digitalWrite(green_pin, HIGH);
  delay(2000);
  //yellow
  digitalWrite(red_pin, HIGH);
  delay(2000);
  digitalWrite(green_pin, LOW);
}

//Strobe && fade
//Red && green
void fade_strobe() {
  for (int j = 0; j < 3; ++j) {
    for(int i = MIN; i < MAX; i += STEP) {
      digitalWrite(green_pin, LOW);
      analogWrite(red_pin, i);
      delay(STROBE);
      digitalWrite(red_pin, LOW);
      analogWrite(green_pin, i);
      delay(STROBE);
    }
    for(int i = MAX; i >= MIN; i -= STEP) {
      digitalWrite(green_pin, LOW);
      analogWrite(red_pin, i);
      delay(STROBE);
      digitalWrite(red_pin, LOW);
      analogWrite(green_pin, i);
      delay(STROBE);
    }
  }
}

// Turn off RGB 
void reset_pins() {
  digitalWrite(red_pin, LOW);
  digitalWrite(green_pin, LOW);
  digitalWrite(blue_pin, LOW);
}

// crazy functions.. played every 10 min
// Doesn't turn off all the way 
void glow(pin) {
    int x = 25;
    int step = 5;
    int time = 10;
    for(int i = x; i < MAX; i += step) {
      analogWrite(pin, i);
      delay(time);
    }
    for(int i = MAX; i >= x; i -= step) {
      analogWrite(pin, i);
      delay(time);
    }
}

// color flash


/*
Assume start arduino at 6
Do new task every 15 minutes or something
Do task at new hour
Run from 6pm-12am
*/
unsigned long int time = 0;
unsigned long int seconds = 0;
unsigned long int minutes = 0;
unsigned long int hours = 0;
unsigned long int days = 0;

unsigned long int ms_to_seconds(int milliseconds) {
  return milliseconds / (unsigned long) 1000;
}

unsigned long int ms_to_minutes(int milliseconds) {
  return ms_to_seconds(milliseconds) / (unsigned long) 60; 
}

unsigned long int ms_to_hours(int milliseconds) {
  return ms_to_minutes(milliseconds) / (unsigned long) 60;
}

unsigned long int ms_to_days(int milliseconds) {
  return ms_to_hours(milliseconds) / (unsigned long) 24;
}

void set_time(int ms) {
  time = ms;
  seconds = ms_to_seconds(ms);
  minutes = ms_to_minutes(ms);
  hours = ms_to_hours(ms);
  days = ms_to_days(ms);
}

void setup() {
  // setup rgb pins
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  reset_pins();
  // Get current time
  Serial.begin(9600);
  /*
  Serial.print("CMOOONN");
  Serial.println();
  Serial.println("AYYYYY");
  */
  time = millis();
}

unsigned long int minute = 60000;
unsigned long int hour = 3600000;
unsigned long int mode_interval = 10 * minute;
unsigned long int prev_millis = 0;
unsigned long int cur_millis = 0;
unsigned long int on_interval = hour; //hour * 6
unsigned long int prev_day = 0;
void loop() {
  cur_millis = millis();
  while(1) {

  }
  
  /*
    Will finish after 6 hours plus some change
    Calculate the offset then apply to 18 hours
    So we start on time the next day at 5
  */
  if (cur_millis - prev_day >= on_interval) {
    //End on a bang
    Serial.print("Sleepy time, ");
    Serial.print(cur_millis);
    Serial.print(", ");
    Serial.println(prev_day);
    //delay(64800000 - (cur_millis-prev_day)); //delay by 18 hours or until 6pm next day
    delay(10000);
    prev_day = cur_millis;
    mode = 0;
  }

  //Run each for 10 minutes
  //At end of all six hit em with strobe
  if (cur_millis - prev_millis >= mode_interval) {
    Serial.print("New mode, ");
    Serial.print(cur_millis);
    Serial.print(", ");
    Serial.print(prev_millis);
    Serial.print(", ");
    Serial.println(mode);

    prev_millis = cur_millis;
    reset_pins();
    mode += 1;
  }

  if (mode == 0) {
    //Serial.println("rg_fade");
    /*
    digitalWrite(red_pin, HIGH);
    delay(1000);
    digitalWrite(red_pin, LOW);
    delay(1000);
    */
    rg_fade();
  } else if(mode == 1) {
    //Serial.println("rg_fade_inverse");
    //digitalWrite(green_pin, HIGH);
    rg_fade_inverse();
  } else if(mode == 2) {
    //Serial.println("gr_fade_inverse");
    //digitalWrite(blue_pin, HIGH);
    gr_fade_inverse();
  } else if(mode == 3) {
    //Serial.println("dynamic_fade");
    dynamic_fade();
  } else if(mode == 4) {
    //Serial.println("rgy");
    //digitalWrite(red_pin, HIGH);
    //digitalWrite(blue_pin, HIGH);
    rgy();
  } else if(mode == 5) {
    //Serial.println("rgbyp");
    //digitalWrite(green_pin, HIGH);
    //digitalWrite(blue_pin, HIGH);
    rgbyp();
  } else {
    //Serial.println("fade_strobe");
    //digitalWrite(red_pin, HIGH);
    //digitalWrite(green_pin, HIGH);
    //digitalWrite(blue_pin, HIGH);
    fade_strobe();
    mode = 0;
  }

  


  /*
   *orange
  analogWrite(red_pin, MAX);
  analogWrite(green_pin, 1 * (MAX/12)); //3/4
*/  
  
}
