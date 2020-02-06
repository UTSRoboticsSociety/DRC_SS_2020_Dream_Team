#include <Firmata.h>
#include <Servo.h>

#define PWM1_PIN 7
#define PWM2_PIN 8
#define SERVO_PIN 3
#define MAX_AVERAGE 7
#define MIN_ANGLE_DIFF 5
Servo myservo;  // create servo object to control a servo

bool ch1_first_zero_set = false;
bool started_timer1 = false;
bool ch2_first_zero_set = false;
bool started_timer2 = false;
int ch1_reading_check, ch2_reading_check, pin_reading1, pin_reading2, timer1, timer2;
int ch1_reading[MAX_AVERAGE];
int ch2_reading[MAX_AVERAGE];
int sum, i;
float ch1_average_reading;
float ch2_average_reading;
int count1 = 0;
int count2 = 0;
int servo_position, previous_servo_position = 0;
bool MANUAL_MODE = false;

void stringCallback(char *myString)
{
  if (!MANUAL_MODE) {
    myservo.write(atoi(myString)); 
  }
}

void setup ()
{ 
  Firmata.setFirmwareVersion(FIRMATA_FIRMWARE_MAJOR_VERSION, FIRMATA_FIRMWARE_MINOR_VERSION);
  Firmata.attach(STRING_DATA, stringCallback);
  myservo.attach(SERVO_PIN);  // attaches the servo signal pin to the servo object
  pinMode (PWM1_PIN, INPUT_PULLUP);
  pinMode (PWM2_PIN, INPUT_PULLUP);
  pinMode (5, OUTPUT);
  digitalWrite(5, LOW);
  Firmata.begin(57600);
}  

void loop ()
{   
  pin_reading1 = digitalRead(PWM1_PIN);
  if (!pin_reading1 && !ch1_first_zero_set) {
    ch1_first_zero_set = true;
  }
  if ((pin_reading1 && ch1_first_zero_set) && !started_timer1) {
    timer1 =  micros();
    started_timer1 = true;
  }
  if ((!pin_reading1 && ch1_first_zero_set) && started_timer1) {
    ch1_reading_check = micros() - timer1;  
    if (ch1_reading_check>0) {
      if (count1<MAX_AVERAGE) {
        ch1_reading[count1] =  ch1_reading_check;
        count1++;
      }
      else {
        ch1_reading[0] =  ch1_reading_check;
        count1 = 1;
      }
    }
    started_timer1 = false;
  }

  if (count1 == MAX_AVERAGE) {
    sum = 0;
    for (i=0;i<MAX_AVERAGE;i++) {
      sum+=ch1_reading[i];
    }
    ch1_average_reading = ((float)sum)/((float)MAX_AVERAGE);
    if (abs(ch1_average_reading-1500.0)<27) {
      digitalWrite(5, HIGH);
      MANUAL_MODE = false;
    }
    else {
      digitalWrite(5, LOW);
      MANUAL_MODE = true;
    }
  }


  
  pin_reading2 = digitalRead(PWM2_PIN);
  if (!pin_reading2 && !ch2_first_zero_set) {
    ch2_first_zero_set = true;
  }
  if ((pin_reading2 && ch2_first_zero_set) && !started_timer2) {
    timer2 =  micros();
    started_timer2 = true;
  }
  if ((!pin_reading2 && ch2_first_zero_set) && started_timer2) {
    ch2_reading_check = micros() - timer2;  
    if (ch2_reading_check>0) {
      if (count2<MAX_AVERAGE) {
        ch2_reading[count2] =  ch2_reading_check;
        count2++;
      }
      else {
        ch2_reading[0] =  ch2_reading_check;
        count2 = 1;
      }
    }
    started_timer2 = false;
  }

  if ((count2 == MAX_AVERAGE) && MANUAL_MODE) {
    sum = 0;
    for (i=0;i<MAX_AVERAGE;i++) {
      sum+=ch2_reading[i];
    }
    ch2_average_reading = ((float)sum)/((float)MAX_AVERAGE);
    servo_position = ((ch2_average_reading-1000.0)/1000.0)*180.0;
    if (servo_position<0) {
      servo_position = 0;
    }
    if (servo_position>180) {
      servo_position = 180;
    }
    if (abs(servo_position-previous_servo_position)>MIN_ANGLE_DIFF) {
      myservo.write(servo_position);
    } 
    previous_servo_position = servo_position; 
  }




  

  if (Firmata.available()) {
    Firmata.processInput();
  }

  
}
