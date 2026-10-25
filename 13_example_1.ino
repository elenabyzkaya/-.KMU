#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9
#define PIN_TRIG  12  // sonar sensor TRIGGER (not used in this task)
#define PIN_ECHO  13  // sonar sensor ECHO    (not used in this task)
#define PIN_SERVO 10

// Servo duty values -> replace with YOUR calibrated values
#define _DUTY_MIN 1000  // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1500  // servo neutral position (90 degree)
#define _DUTY_MAX 2000  // servo full counter-clockwise position (180 degree)

// Challenge 13C02: two experiments
#define _SPEED_PHASE1 3.0   // experiment 1: 3 deg/sec   -> 180 degrees in 60 sec
#define _SPEED_PHASE2 0.3   // experiment 2: 0.3 deg/sec -> 90 degrees in 300 sec

// Intervals (unit: msec)
#define _INTERVAL_SERVO   20
#define _INTERVAL_SERIAL  200

// global variables
Servo myservo;

unsigned long last_sampling_time_servo;   // unit: msec
unsigned long last_sampling_time_serial;  // unit: msec
unsigned long phase_start_time;           // start time of the current phase (msec)

bool event_servo, event_serial;           // event triggered?

float servo_speed = _SPEED_PHASE1;  // unit: degree/sec
float duty_target = _DUTY_MAX;
float duty_curr   = _DUTY_MIN;      // float, so that sub-usec steps are not truncated

int phase = 1;  // 1: speed 3, 2: speed 0.3, 3: finished

// reset all timers to the current time
void resync_timers() {
  unsigned long t = millis();
  last_sampling_time_servo  = t;
  last_sampling_time_serial = t;
  phase_start_time = t;
}

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, 1);

  // move servo to the start position
  myservo.attach(PIN_SERVO);
  duty_curr = _DUTY_MIN;
  myservo.writeMicroseconds((int)duty_curr);

  // initialize serial port
  Serial.begin(57600);
  delay(1500);  // give the servo time to reach the start position

  resync_timers();
}

void loop() {
  unsigned long time_curr = millis();

  // wait until next event time
  if (time_curr >= (last_sampling_time_servo + _INTERVAL_SERVO)) {
    last_sampling_time_servo += _INTERVAL_SERVO;
    event_servo = true;
  }
  if (time_curr >= (last_sampling_time_serial + _INTERVAL_SERIAL)) {
    last_sampling_time_serial += _INTERVAL_SERIAL;
    event_serial = true;
  }

  if (event_servo) {
    event_servo = false;

    if (phase != 3) {
      // duty change per interval (usec)
      //   = duty range * (speed / 180 deg) * interval (sec)
      float step = (float)(_DUTY_MAX - _DUTY_MIN)
                   * (servo_speed / 180.0)
                   * ((float)_INTERVAL_SERVO / 1000.0);

      if (duty_curr < duty_target) {
        duty_curr += step;
        if (duty_curr >= duty_target) {
          duty_curr = duty_target;

          if (phase == 1) {
            // experiment 1 finished -> return to start, begin experiment 2
            myservo.writeMicroseconds((int)duty_curr);
            delay(2000);  // short pause to separate the two experiments in the video

            phase = 2;
            servo_speed = _SPEED_PHASE2;
            duty_curr = _DUTY_MIN;
            duty_target = _DUTY_NEU;  // 90 degree
            myservo.writeMicroseconds((int)duty_curr);
            delay(1500);              // wait for the servo to return to start

            resync_timers();
          } else {
            phase = 3;  // experiment 2 finished
          }
        }
      }
      myservo.writeMicroseconds((int)duty_curr);
    }
  }

  if (event_serial) {
    event_serial = false;
    float elapsed = (millis() - phase_start_time) / 1000.0;
    float angle = (duty_curr - _DUTY_MIN) * 180.0 / (_DUTY_MAX - _DUTY_MIN);

    Serial.print("phase:");   Serial.print(phase);
    Serial.print(",speed:");  Serial.print(servo_speed);
    Serial.print(",time_s:"); Serial.print(elapsed, 1);
    Serial.print(",duty:");   Serial.print((int)duty_curr);
    Serial.print(",angle:");  Serial.println(angle, 1);
  }
}
