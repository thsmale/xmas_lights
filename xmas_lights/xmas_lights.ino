#include <avr/sleep.h>

int red_pin = 5;
int green_pin = 3;
int blue_pin = 6;
const int STEP = 2;
const int DELAY_TIME = 50;
const int MIN = 0;
const int MAX = 255;
const int STROBE = 100; //miliseconds
int mode = 0;

/*
 * pin: 3, 5, 6, 9, 10, 11 as they support PWM
 * time: (milliseconds) how long to wait until the brightness is increased 
 * step: [0, 255] how much to increase the brightness by
*/
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
void rgbyp(int time) {
  //red
  digitalWrite(red_pin, HIGH);
  delay(time);
  //purple
  digitalWrite(blue_pin, HIGH);
  delay(time);
  //blue
  digitalWrite(red_pin, LOW);
  delay(time);
  //green
  digitalWrite(blue_pin, LOW);
  digitalWrite(green_pin, HIGH);
  delay(time);
  //yellow
  digitalWrite(red_pin, HIGH);
  delay(time);
  digitalWrite(green_pin, LOW);
}

//Strobe && fade
//Red && green
// TODO: increase speed and brightness porpotinally 
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
void glow(int pin) {
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

void rg_glow() {
  for (int i = 0; i < 100; ++i) {
    glow(red_pin);
    digitalWrite(red_pin, LOW);
    glow(green_pin);
    digitalWrite(green_pin, LOW);    
  }
}

// Fibonacci sequence up to MAX
void fibonacci() {
  for (int t = 0; t < 10; t++) {
    int i = 0, j = 1, k = 0;
    int time = 250;
    while(k <= MAX) {
      digitalWrite(green_pin, LOW);
      analogWrite(red_pin, i);
      delay(time);
      digitalWrite(red_pin, LOW);
      analogWrite(green_pin, j);
      delay(time);
      k = i + j;
      i = j;
      j = k;
    }
    // traverse
    digitalWrite(red_pin, LOW);
    digitalWrite(green_pin, LOW);
    delay(1000);
  }
}

// Gaussian distribution
// Adjust the mean, stdev, etc

void rando() {
  randomSeed(420);
  for(int i = 0; i < 100; ++i) {
    digitalWrite(red_pin, LOW);
    digitalWrite(green_pin, LOW);
    int brightness = random(MIN, MAX);
    int time = random(100, 2000);
    int pin = random(0, 3);
    if (pin == 0) {
      pin = red_pin;
    } else if (pin == 1) {
      pin = green_pin;
    } else {
      //yellow
      digitalWrite(red_pin, HIGH);
      analogWrite(green_pin, 70);
      delay(time);
      continue;
    }
    analogWrite(pin, brightness);
    delay(time);
  }  
}

// merry christmas in morse code 
// fastest way you can analog while human can see

void rgbyp_flash() {
  //rgbyp
}

// color flash


/*
Assume start arduino at 6
Do new task every 15 minutes or something
Do task at new hour
Run from 6pm-12am
*/

void setup() {
  // setup rgb pins
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  reset_pins();
  Serial.begin(9600); // for sending print statements to console
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
    rg_fade();
  } else if(mode == 1) {
    rg_fade_inverse();
  } else if(mode == 2) {
    gr_fade_inverse();
  } else if(mode == 3) {
    dynamic_fade();
  } else if(mode == 4) {
    rgy();
  } else if(mode == 5) {
    rgbyp(2000);
  } else {
    fade_strobe();
    for(int i = 0; i < 10; i++) {
      fibonacci();
    }
    mode = 0;
  }

  /*
   *orange
  analogWrite(red_pin, MAX);
  analogWrite(green_pin, 1 * (MAX/12)); //3/4
*/  
  
}
