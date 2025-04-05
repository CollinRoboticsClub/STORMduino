#include <Servo.h>

Servo fl, fr, m, bl, br, intake, magazine;
//  Apparently the Arduino Servo library uses
// *software*-generated PWM, so you can use any Digital pins :D (not just "PWM" pins)
// edit: nvm it was vibing wrong
const int FL_PIN = 3,
          FR_PIN = 5,
          M_PIN = 6,
          BL_PIN = 9,
          BR_PIN = 10,
          INTAKE_PIN = 11,
          MAGAZINE_PIN = 12;

// Note: Arduino `int` and `long` are 16 and 32 bits.
// If you want human-predictable datatype widths, use int8_t and similar types.

// Motor flips: if true, the drive() functions will flip the direction!
const bool FL_FLIPPED = false,
           FR_FLIPPED = false,
           M_FLIPPED = true,
           BL_FLIPPED = false,
           BR_FLIPPED = false,
           INTAKE_FLIPPED = false,
           MAGAZINE_FLIPPED = true;


// [ x, y, rotation, intake, magazine ]
// const int BUF_SIZE = 4;
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
  intake.attach(INTAKE_PIN);
  magazine.attach(MAGAZINE_PIN);

  pinMode(LED_BUILTIN, OUTPUT);
  blink_led();  // to show that it's booted up
}

void loop() {
  // holonomic_drive(0, 255, 0);

  // run_systems_test();
  run_from_serial();
  // delay(10);
}

void run_systems_test() {
  // speed out of 0-255, above 127 is forwards
  const int test_speed = 160, delay_time_ms = 2000;

  auto stop = []() {
    holonomic_drive(127, 127, 127);
  };

  auto test_move = [&](int horizontal, int forward, int rotation) {
    stop();
    blink_led();
    delay(delay_time_ms);
    holonomic_drive(horizontal, forward, rotation);
    delay(delay_time_ms);
    stop();
  };

  delay(5000);
  test_move(127, test_speed, 127);
  test_move(127, -test_speed, 127);

  test_move(test_speed, 127, 127);
  test_move(-test_speed, 127, 127);

  test_move(127, 127, test_speed);
  test_move(127, 127, -test_speed);


  // // sys test v2 lol
  // Servo motors[] = { fl, fr, m, bl, br, intake, magazine };
  // for (auto motor : motors) {
  //   // blink_led();
  //   // motor.write(180);
  //   // delay(2000);
  //   // motor.write(90);
  //   // delay(2000);

  //   motor.write(90);
  //   for (int i = 90; i < 180; i++) {
  //     motor.write(i);
  //     delay(3);
  //   }
  //   for (int i = 180; i > 0; i--) {
  //     motor.write(i);
  //     delay(3);
  //   }
  //   for (int i = 0; i < 90; i++) {
  //     motor.write(i);
  //     delay(3);
  //   }
  //   motor.write(90);

  //   blink_led();
  //   // delay(1000);
  // }
  // exit(0);
}


void blink_led() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void run_from_serial() {
  if (Serial.available() > 0) {
    // Read incoming data
    // buf[0] = 127;
    // buf[1] = 230;
    // buf[2] = 127;
    // buf[3] = 127;
    // buf[4] = 127;
    Serial.readBytesUntil('\n', buf, BUF_SIZE);

    // Parse incoming data -- this is effectively the "protocol"
    // int horizontal = map(buf[0], 0, 255, 0, 180);
    // int forward = map(buf[1], 0, 255, 0, 180);
    // int rotation = map(buf[2], 0, 255, 0, 180);
    // Control scheme is thus: LeftY = fwd/bkwd; RightX = turn
    // It is the Pi's responsibility to send Y Positive UP!
    // (This is not the norm for HID controllers, which send Y positive *down*)

    // Drive! :)
    // holonomic_drive(horizontal, forward, rotation);
    // holonomic_drive(horizontal, 150, rotation);

    int left = map(buf[0], 0, 255, 0, 180);
    int right = map(buf[1], 0, 255, 180, 0);
    int middle = map(buf[2], 0, 255, 0, 180);

    tank_drive(left, right, middle); // wtaf

    int intake_speed = map(buf[3], 0, 255, 0, 180);
    set_intake(intake_speed);

    int magazine_speed = map(buf[4], 0, 255, 0, 180);
    set_magazine(magazine_speed);
  }

  // Wait for 10 milliseconds before repeating the loop.
  delay(10);
}


void holonomic_drive(int horizontal, int forward, int rotation) {
  int left_motors = constrain(forward + rotation, 0, 180);
  fl.write(!FL_FLIPPED ? left_motors : map(left_motors, 0, 180, 180, 0));
  bl.write(!BL_FLIPPED ? left_motors : map(left_motors, 0, 180, 180, 0));

  int right_motors = constrain(forward - rotation, 0, 180);
  fr.write(!FR_FLIPPED ? right_motors : map(right_motors, 0, 180, 180, 0));
  br.write(!BR_FLIPPED ? right_motors : map(right_motors, 0, 180, 180, 0));

  m.write(!M_FLIPPED ? horizontal : map(horizontal, 0, 180, 180, 0));
}



void tank_drive(int left, int right, int horizontal) {
  fl.write(!FL_FLIPPED ? left : map(left, 0, 180, 180, 0));
  bl.write(!BL_FLIPPED ? left : map(left, 0, 180, 180, 0));

  fr.write(!FR_FLIPPED ? right : map(right, 0, 180, 180, 0));
  br.write(!BR_FLIPPED ? right : map(right, 0, 180, 180, 0));

  m.write(!M_FLIPPED ? horizontal : map(horizontal, 0, 180, 180, 0));
}


void set_intake(int speed) {
  intake.write(speed);
}

void set_magazine(int speed) {
  magazine.write(speed);
}