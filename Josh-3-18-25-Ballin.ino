#include <Servo.h>

Servo fl, fr, m, bl, br, intake, magazine;
//  Apparently the Arduino Servo library uses
// *software*-generated PWM, so you can use any Digital pins :D (not just "PWM" pins)
// edit: nvm it was vibing wrong
// const int FL_PIN = 2,
// FR_PIN = 3,
// M_PIN = 4,
// BL_PIN = 5,
// BR_PIN = 6,
const int FL_PIN = 3,
          FR_PIN = 5,
          M_PIN = 6,
          BL_PIN = 9,
          BR_PIN = 10,
          INTAKE_PIN = 11,
          MAGAZINE_PIN = 12;



// Note: Arduino `int` and `long` are 16 and 32 bits.
// If you want human-predictable datatype widths, use int8_t and similar types.

// [ x, y, rotation, intake, magazine ]
const int BUF_SIZE = 6;
// We haven't decided yet if the intake and magazine will be analog or not though...
uint8_t buf[BUF_SIZE];


void setup() {
  // delay(5000);

  Serial.begin(115200);

  fl.attach(FL_PIN);
  fr.attach(FR_PIN);
  m.attach(M_PIN);
  bl.attach(BL_PIN);
  br.attach(BR_PIN);
  // intake.attach(INTAKE_PIN);
  // magazine.attach(MAGAZINE_PIN);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // holonomic_drive(0, 255, 0);

  // run_systems_test();
  run_from_serial();
  delay(10);
}

void run_systems_test() {
  // speed out of 0-255, above 127 is forwards
  // const int test_speed = 200, delay_time_ms = 2000;

  // auto stop = []() {
  //   holonomic_drive(127, 127, 127);
  // };

  // auto test_move = [&](int horizontal, int forward, int rotation) {
  //   stop();
  //   blink_led();
  //   delay(delay_time_ms);
  //   holonomic_drive(horizontal, forward, rotation);
  //   delay(delay_time_ms);
  //   stop();
  // };

  // delay(5000);
  // test_move(127, test_speed, 127);
  // test_move(127, -test_speed, 127);

  // test_move(test_speed, 127, 127);
  // test_move(-test_speed, 127, 127);

  // test_move(127, 127, test_speed);
  // test_move(127, 127, -test_speed);


  // sys test v2 lol
  // Servo motors[] = { fr, fl, m, bl, br };
  // for (auto motor : motors) {
  //   blink_led();
  //   // motor.write(180);
  //   // delay(2000);
  //   // motor.write(90);
  //   // delay(2000);

  //   // motor.write(127);
  //   // for (int i = 90; i < 180; i++) {
  //   //   motor.write(i);
  //   //   delay(3);
  //   // }
  //   // for (int i = 180; i > 0; i--) {
  //   //   motor.write(i);
  //   //   delay(3);
  //   // }
  //   // for (int i = 0; i < 90; i++) {
  //   //   motor.write(i);
  //   //   delay(3);
  //   // }
  //   // motor.write(90);

  //   blink_led();
  //   delay(1000);
  // }
  // exit(0);
}


void blink_led() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}

void run_from_serial() {
  if (Serial.available() > 0) {

    // Read incoming data
    Serial.readBytesUntil('\n', buf, BUF_SIZE);

    // Parse incoming data -- this is effectively the "protocol do"
    int horizontal = buf[0];  // Not used for driving
    int forward = buf[1];
    int rotation = buf[2];
    // Control scheme is thus: LeftY = fwd/bkwd; RightX = turn
    // It is the Pi's responsibility to send Y Positive UP!
    // (This is not the norm for HID controllers, which send Y positive *down*)

    // Drive! :)
    holonomic_drive(horizontal, forward, rotation);
    set_intake(buf[3]);
  }

  // Wait for 10 milliseconds before repeating the loop.
  delay(10);
}


void holonomic_drive(int horizontal, int forward, int rotation) {
  // dear god he modified the arguments
  horizontal = map(horizontal, 0, 255, 0, 180);
  forward = map(forward, 0, 255, 0, 180);
  rotation = map(rotation, 0, 255, 0, 180);

  fl.write(forward + rotation);
  bl.write(forward + rotation);

  fr.write(forward - rotation);
  br.write(forward - rotation);

  m.write(horizontal);
}



void tank_drive(int left, int right, int horizontal) {
  // dear god he modified the arguments
  horizontal = map(horizontal, 0, 255, 0, 180);
  left = map(left, 0, 255, 0, 180);
  right = map(right, 0, 255, 0, 180);

  fl.write(left);
  bl.write(left);

  fr.write(right);
  br.write(right);

  m.write(horizontal);
}


void set_intake(int value) {
  // intake.write(map(value, 0, 255, 0, 180));
}
