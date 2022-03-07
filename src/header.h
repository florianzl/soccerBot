/*
 * motor vorne rechts 1, hinten rechts 2, hinten links 3, vorne links 4
 *

 *
 * kein delay im main Programm
 * -> bei warte scannt bot weiter
 */
#include <Arduino.h>
#include <CAN.h>
#include <EEPROM.h>
#include <Pixy2I2C.h>
#include <Wire.h>
#include <elapsedMillis.h>
Pixy2I2C pixy;

// Farben der LEDs
#define OFF 0
#define GREEN 1
#define RED 2
#define YELLOW 3
#define BLUE 4
#define WHITE 7

#define SDA 21
#define SCL 22

#define DRIVE_DIS 2
#define DRIVE1_PWM 12
#define DRIVE1_DIR 13
#define DRIVE3_PWM 27
#define DRIVE3_DIR 14
#define DRIVE4_PWM 26
#define DRIVE4_DIR 25
#define DRIVE2_PWM 19
#define DRIVE2_DIR 18

#define INPUT1 35
#define INPUT2 34
#define INPUT3 39
#define INPUT4 36

#define io1 2
#define io2 16
#define io3 17
#define io4 18
#define io5 19

#define kicker 15
#define dribbler 23
#define compassAddress 0x60  // cmps11, cmps12, cmps14
#define ANGLE_8 1
#define EPROM_SIZE 64

#define right 1
#define left 0
#define frontal -1
#define blue 000
#define yelllow 001

elapsedMillis deathTime;
elapsedMillis waitTime;
elapsedMillis cornerTime;
elapsedMillis sameBallDirection;

class Bot {
 private:
  byte epromByte[4] = {0, 0, 0, 0};

  int speed, mode;

  int compass, compassHead;
  bool compassEnabled;

  int ballDirection, lastBallDirection;
  bool ballVisibility;

  int goalDirection, lastGoalDirection, goalDistance, goal;

  bool portEnabled[8] = {false, false, false, false, false, false, false, false};
  bool button1Array[8] = {false, false, false, false, false, false, false, false};
  bool button2Array[8] = {false, false, false, false, false, false, false, false};
  int buttonLedId[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

  int led1Array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int led2Array[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  bool soccer, hasPixy, pixyBlind;

 public:
  Bot() {
    pinMode(kicker, OUTPUT);
    digitalWrite(kicker, LOW);
    Serial.begin(115200);
    delay(200);
    Wire.begin(SDA, SCL);

    pinMode(DRIVE_DIS, OUTPUT);
    digitalWrite(DRIVE_DIS, LOW);
    pinMode(DRIVE1_DIR, OUTPUT);
    pinMode(DRIVE2_DIR, OUTPUT);
    pinMode(DRIVE3_DIR, OUTPUT);
    pinMode(DRIVE4_DIR, OUTPUT);

    pinMode(INPUT1, INPUT);
    pinMode(INPUT2, INPUT);
    pinMode(INPUT3, INPUT);
    pinMode(INPUT4, INPUT);

    ledcAttachPin(DRIVE1_PWM, 1);
    ledcSetup(1, 1000, 8);
    ledcWrite(1, 0);
    ledcAttachPin(DRIVE2_PWM, 2);
    ledcSetup(2, 1000, 8);
    ledcWrite(2, 0);
    ledcAttachPin(DRIVE3_PWM, 3);
    ledcSetup(3, 1000, 8);
    ledcWrite(3, 0);
    ledcAttachPin(DRIVE4_PWM, 4);
    ledcSetup(4, 1000, 8);
    ledcWrite(4, 0);
    ledcAttachPin(io1, 6);
    ledcSetup(6, 50, 16);
    ledcWrite(6, 0);

    // start the CAN bus at 500 kbps
    Serial.print("Warte auf Canbus");
    delay(100);
    if (!CAN.begin(500E3)) {
      Serial.print("...failed!");
      while (1)
        ;
    } else
      Serial.println("...succeed");
  }

  void init() {
    digitalWrite(DRIVE_DIS, HIGH);
    delay(100);
    deathTime = 1000;
    pixyBlind = false;
    lastBallDirection = 0;
    for (int i = 0; i < 8; i++) {
      Wire.beginTransmission(buttonLedId[i]);
      byte error = Wire.endTransmission();
      if (error == 0)
        portEnabled[i] = true;
      Serial.print("Button : " + String(i) + " : ");
      if (error == 0)
        Serial.println("true");
      else
        Serial.println("false");
    }

    delay(100);
    Wire.beginTransmission(compassAddress);
    byte error = Wire.endTransmission();
    if (error == 0) {
      compassEnabled = true;
      Serial.println("Compass true");
    } else {
      compassEnabled = false;
      Serial.println("Compass false");
    }

    delay(100);
    if (hasPixy) {
      Serial.print("Wait for Pixy2 on i2c 0x54...");
      pixy.init(0x54);
      Serial.println("done");
    }
  }

 public:
  void setupBot(bool pixy, bool soccer, int goal) {
    this->hasPixy = pixy;
    this->soccer = soccer;
    this->goal = goal;
    mode = 0;
    init();
    epromInit();
    blinkAll(GREEN);
  }

  void pauseBot() {
    drive(0, 0, 0);
    blinkAll(RED);
  }

  void changeMode() {
    switch (mode) {
      case 0:
        mode = 1;
        return;
      case 1:
        mode = 0;
        return;
    }
  }

  bool getMode() {
    return mode;
  }

 private:
  void epromInit() {
    int x = 0;  // platzhalter variable
    readEprom();
    compassHead = epromByte[1] * 265 + epromByte[2];  //  hightbyte,lowbyte
    if (epromByte[0] == x) {
      // NOTHING
    }
    if (epromByte[1] == x) {
      // NOTHING
    }
    if (epromByte[2] == x) {
      // NOTHING
    }
    if (epromByte[3] == x) {
      // NOTHING
    }
  }

 public:
  void readEprom() {
    for (int i = 0; i < 4; i++) {
      epromByte[i] = (byte(EEPROM.read(i)));
    }
  }

  void writeEprom() {
    // epromByte[0] = ;
    epromByte[1] = compassHead / 256;  // highbyte
    epromByte[2] = compassHead % 256;  // lowbyte
    // epromByte[3] = ;

    for (int i = 0; i < 4; i++) {
      EEPROM.write(i, epromByte[i]);
    }
    EEPROM.commit();
  }

  void wait(int time) {
    waitTime = 0;
    i2cSync();
    if (soccer)
      Can();
    if (hasPixy)
      readPixy();
    while (waitTime < time) {
      if ((waitTime % 10) == 0)
        i2cSync();
      else
        delay(1);
    }
  }

 private:
  void motor(int number, int speed) {
    // Speed wird bei 100 und -100 gekappt
    if (speed > 100)
      speed = 100;
    if (speed < -100)
      speed = -100;
    int pwm = spdToPWM(speed);
    int dir;
    if (speed < 0)
      dir = LOW;
    else
      dir = HIGH;

    if (number == 1)
      digitalWrite(DRIVE1_DIR, dir);
    if (number == 2)
      digitalWrite(DRIVE2_DIR, dir);
    if (number == 3)
      digitalWrite(DRIVE3_DIR, dir);
    if (number == 4)
      digitalWrite(DRIVE4_DIR, dir);

    if ((number > 0) && (number < 5))
      ledcWrite(number, pwm);
  }

  /*
   Direction -3 bis 4, 0 vorne und 4 zurück, rechts positiv
   speed     -100 bis 100,
   rotate    -100 bis 100, rechts positiv
   */
 public:
  void drive(int direction, int speed, int rotate) {
    direction = direction / 2;
    if (sameBallDirection > 1000) {
      direction = direction * -1;
    }
    int maxs = abs(speed) + abs(rotate);
    if (maxs > 100) {
      speed = speed * 100 / maxs;
      rotate = rotate * 100 / maxs;
    }

    switch (direction) {
      // geradeaus
      case 0:
        motor(1, -speed + rotate);
        motor(2, -speed + rotate);
        motor(3, speed + rotate);
        motor(4, speed + rotate);
        return;

      // 45 Grad rechts
      case 1:
        motor(1, rotate);
        motor(2, -speed + rotate);
        motor(3, rotate);
        motor(4, speed + rotate);
        return;

      // 45 Grad links
      case -1:
        motor(1, -speed + rotate);
        motor(3, speed + rotate);
        motor(4, rotate);
        motor(2, rotate);
        return;

      // rechts
      case 2:
        motor(1, speed + rotate);
        motor(2, -speed + rotate);
        motor(3, -speed + rotate);
        motor(4, speed + rotate);
        return;

      // links
      case -2:
        motor(1, -speed + rotate);
        motor(2, speed + rotate);
        motor(3, speed + rotate);
        motor(4, -speed + rotate);
        return;

      // 135 Grad rechts
      case 3:
        motor(1, speed + rotate);
        motor(2, rotate);
        motor(3, -speed + rotate);
        motor(4, rotate);
        return;

      // 135 Grad links
      case -3:
        motor(1, rotate);
        motor(2, speed + rotate);
        motor(3, rotate);
        motor(4, -speed + rotate);
        return;

      // zurück
      case 4:
        motor(1, speed + rotate);
        motor(2, speed + rotate);
        motor(3, -speed + rotate);
        motor(4, -speed + rotate);
        return;
    }
  }

  int directionBehindBall() {
    speed = 50;
    if (ballDirection != lastBallDirection) {
      lastBallDirection = ballDirection;
      sameBallDirection = 0;
    }
    switch (ballDirection) {
      case 0:
        speed = 65;
        return 0;

      case 1:
        speed = 50;
        return 2;
      case -1:
        speed = 50;
        return -2;

      case 2:
        speed = 50;
        return 2;
      case -2:
        speed = 50;
        return -2;

      case 3:
        speed = 50;
        return 4;
      case -3:
        speed = 50;
        return -4;

      case 4:
        speed = 43;
        return 6;
      case -4:
        speed = 43;
        return -6;

      case 5:
        speed = 43;
        return 6;
      case -5:
        speed = 43;
        return -6;

      case 6:
        speed = 65;
        return 8;
      case -6:
        speed = 65;
        return 8;

      case 7:
        speed = 65;
        return -6;
      case -7:
        speed = 65;
        return 6;

      case 8:
        speed = 60;
        switch (siteOfBot()) {
          case right:
            // links über mitte fahren
            return -6;
          case left:
            // rechts über mitte fahren
            return 6;
        }
    }
  }

 private:
  void servo(int pos) {
    if (pos > 100)
      pos = 100;
    if (pos < -100)
      pos = -100;
    ledcWrite(6, 5000 + pos * 30);
  }

 public:
  int siteOfBot() {
    readPixy();
    if (hasBall()) {
      // fährt nach pixy
      if (compass != 0)
        return compass < 0 ? right : left;
      else
        return frontal;
    } else {
      // fährt nach kompass
      if (lastGoalDirection != 0)
        return lastGoalDirection < 0 ? right : left;
      else
        return frontal;
    }
  }

  int input(int number) {
    if (number == 1)
      return (analogRead(INPUT1));
    else if (number == 2)
      return (analogRead(INPUT2));
    else if (number == 3)
      return (analogRead(INPUT3));
    else if (number == 4)
      return (analogRead(INPUT4));
    return 0;
  }

  // liefert von device Taster nr (1:links, 2:rechts)
  bool button(int device, int nr) {
    if (device < 0)
      return false;
    if (device > 7)
      return false;
    // portEnabled[device] = true;
    if (nr == 1)
      return button1Array[device];
    if (nr == 2)
      return button2Array[device];
    return false;
  }

  int getBallDirection() {
    return ballDirection;
  }

  int ballVisible() {
    return ballVisibility;
  }

  bool hasBall() {
    return (input(3) > 0 && ballDirection == 0);
  }

  int getGoalDirection() {
    readPixy();
    return goalDirection;
  }

  int getGoalDistance() {
    readPixy();
    return goalDistance;
  }

  int getLastGoalDistance() {
    readPixy();
    return lastGoalDistance;
  }

  int getLastGoalDirection() {
    readPixy();
    if (lastGoalDirection < 0) {
      return -1;
    }
    if (lastGoalDirection > 0) {
      return 1;
    } else
      return 0;
  }

  int getCompass() {
    return compass;
  }

  bool getPixyBlind() {
    return pixyBlind;
  }

  void setCompass() {
    Wire.beginTransmission(compassAddress);
    byte error = Wire.endTransmission();
    if (error == 0) {
      compassHead = compassOrg();
    }
  }

  int getSpeed() {
    return speed;
  }

  void led(int device, int nr, int color) {
    if (color < 0)
      return;
    if (color > 7)
      return;
    if (device < 0)
      return;
    if (device > 7)
      return;

    if (nr == 1) {
      color = color * 2;
      led1Array[device] = color;
    }
    if (nr == 2) {
      color = color * 16;
      if (color > 63)
        color = color + 64;
      led2Array[device] = color;
    }
  }

  // lässt jede led in bestimmter farbe für 0.05s leuchten
  void blinkAll(int color) {
    led(0, 1, color);
    led(0, 2, color);
    led(7, 1, color);
    led(7, 2, color);
    wait(50);
    led(0, 1, OFF);
    led(0, 2, OFF);
    led(7, 1, OFF);
    led(7, 2, OFF);
  }

  void kick(int time) {
    if (deathTime < 1000)
      return;
    if (time > 45)
      time = 45;
    digitalWrite(kicker, HIGH);
    delay(time);
    digitalWrite(kicker, LOW);
    deathTime = 0;
  }

  void strike() {
    for (int i = speed; i < 100; i += 3) {
      drive(0, i, getGoalDirection() / -5);
      delay(1);
    }
    drive(0, 100, getGoalDirection() / -5);
    delay(10);
    kick(45);
    drive(0, 0, 0);
  }

 private:
  bool digit(int number) {
    if (number == 1)
      return (analogRead(INPUT1) < 2048);
    if (number == 2)
      return (analogRead(INPUT2) < 2048);
    if (number == 3)
      return (analogRead(INPUT3) < 2048);
    if (number == 4)
      return (analogRead(INPUT4) < 2048);
  }

  int spdToPWM(int speed) {
    if (speed < 0)
      speed *= -1;
    return ((speed * 255) / 100);
  }

  int compassOrg() {
    unsigned char high_byte, low_byte, angle8;
    unsigned int angle16;
    Wire.beginTransmission(compassAddress);
    Wire.write(ANGLE_8);
    Wire.endTransmission();
    Wire.requestFrom(compassAddress, 3);
    while (Wire.available() < 3)
      ;
    angle8 = Wire.read();  // Read back the 5 bytes
    high_byte = Wire.read();
    low_byte = Wire.read();
    angle16 = high_byte;  // Calculate 16 bit angle
    angle16 <<= 8;
    angle16 += low_byte;
    return angle16 / 10;
  }

  int readCompass() {
    return ((((compassOrg() - compassHead) + 180 + 360) % 360) - 180);
  }

  void setCompassHeading() {
    compassHead = compassOrg();
  }

  void i2cSync() {
    for (int i = 0; i < 8; i++) {
      if (portEnabled[i]) {
        int ledwert = 255 - led1Array[i] - led2Array[i];
        Wire.beginTransmission(buttonLedId[i]);
        Wire.write(ledwert);
        ;
        Wire.endTransmission();

        Wire.requestFrom(buttonLedId[i], 1);
        if (Wire.available()) {
          int tread = 255 - Wire.read();
          tread = tread % 128;
          if (tread > 63)
            button2Array[i] = true;
          else
            button2Array[i] = false;
          tread = tread % 2;
          if (tread > 0)
            button1Array[i] = true;
          else
            button1Array[i] = false;
        }
      }
    }
    if (compassEnabled == true) {
      compass = readCompass();
    } else
      Serial.println("Compass disabled");
  }

  void Can() {
    elapsedMillis echtzeit = 0;
    int time = 0;
    int irpaket;
    CAN.beginPacket(0x03, 1, true);  // sendet RTR und will 1 byte
    if (CAN.endPacket() == 1) {
      while (!CAN.parsePacket()) {
        delayMicroseconds(1);
        time++;
      }
      while (CAN.available()) {
        irpaket = CAN.read();
        ballDirection = (irpaket / 16) - 7;
        int zone = irpaket % 16;
        if (zone < 1) {
          ballVisibility = false;
        } else
          ballVisibility = true;
        if (!ballVisibility)
          ballDirection = 0;
      }
    } else
      Serial.println("IR doesn't answer");
  }

  void evaluatePixy() {
    switch (goal) {
      case yellow:
        int signature = 1;
        led(7, 1, YELLOW);
        break;
      case blue:
        int signature = 2;
        led(7, 1, BLUE);
        break;
    }
    int pixyBlocks = pixy.ccc.blocks[0].m_signature;

    if (pixyBlocks == signature) {
      goalDirection = -(pixy.ccc.blocks[0].m_x - 158) / 2;
      lastGoalDirection = goalDirection;
    }
  }

  void readPixy() {
    pixy.ccc.getBlocks();

    if (pixy.ccc.numBlocks) {
      evaluatePixy();
      if (!hasBall())
        cornerTime = 0;
      pixyBlind = false;
    } else {
      goalDirection = compass;
      pixyBlind = true;
    }
  }

 public:
  bool IsInCorner() {
    readPixy();
    return (ballDirection == 0 && cornerTime > 1000);
  }

  void getOutOfCorner() {
    if (lastGoalDirection > 0) {
      if (compass > -20) {
        drive(0, 0, getLastGoalDirection() * -30);
      } else {
        drive(0, 70, getLastGoalDirection() * -20);
      }
    } else {
      if (compass < 20) {
        drive(0, 0, getLastGoalDirection() * -30);
      } else {
        drive(0, 70, getLastGoalDirection() * -20);
      }
    }
  }
};