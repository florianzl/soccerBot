/*
 * motor vorne rechts 1, hinten rechts 2, hinten links 3, vorne links 4
 *

 *
 * kein delay im main Programm
 * -> bei warte scannt bot weiter
 */

#include <CAN.h>
#include <Pixy2I2C.h>
#include <Wire.h>
#include <elapsedMillis.h>
#include <EEPROM.h>
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

elapsedMillis deathTime;
elapsedMillis waitTime;

class Bot {
 private:
  byte epromByte[4] = {0, 0, 0, 0};
 private:
  int speed;

 private:
  int compass, compassHead;

 private:
  bool compassEnabled;

 private:
  int ballDirection;

 private:
  bool ballSeen;

 private:
  int goalDirection, goalDistance;

 private:
  bool portEnabled[] = {false, false, false, false, false, false, false, false};

 private:
  bool button1Array[] = {false, false, false, false, false, false, false, false};

 private:
  bool button2Array[] = {false, false, false, false, false, false, false, false};

 private:
  int buttonLedId[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};

 private:
  int led1Array[] = {0, 0, 0, 0, 0, 0, 0, 0};

 private:
  int led2Array[] = {0, 0, 0, 0, 0, 0, 0, 0};

 private:
  bool soccer, hasPixy, pixyBlind;

 private:
  int lastCompassOutput;

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

  void setupBot(bool pixy, bool soccer) {
    init();
    epromInit();
    this.hasPixy = pixy;
    this.soccer = soccer;
    blinkAll(GREEN);
  }

 private:
  void epromInit() {
    int x = 0; //platzhalter variable
    readEEPROM();
    compassHead = epromByte[1] * 265 + epromByte[2];  //  hightbyte,lowbyte
    if (epromByte[0] == x) {
      //NOTHING
    }
    if (epromByte[1] == x) {
      //NOTHING
    }
    if (epromByte[2] == x) {
      //NOTHING
    }
    if (epromByte[3] == x) {
      //NOTHING
    }
  }

  void readEprom() {
    for (int i = 0; i < 4; i++) {
      epromByte[i] = (byte(EEPROM.read(i)));
    }
  }

  void writeEprom() {
    epromByte[0] = modus;
    epromByte[1] = head / 256;  // highbyte
    epromByte[2] = head % 256;  // lowbyte
    epromByte[3] = start;

    for (int i = 0; i < 4; i++) {
      EEPROM.write(i, epromByte[i]);
    }
    EEPROM.commit();
  }

  void pauseBot() {
    drive(0, 0, 0);
    blinkAll(Red);
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
  void drive(int direction, int speed, int rotate) {
    int maxs = abs(speed) + abs(rotate);
    if (maxs > 100) {
      speed = speed * 100 / maxs;
      rotate = rotate * 100 / maxs;
    }
    // geradeaus
    if (direction == 0) {
      motor(1, -speed + rotate);
      motor(2, -speed + rotate);
      motor(3, speed + rotate);
      motor(4, speed + rotate);
    }

    // 45 Grad rechts
    if (direction == 1) {
      motor(1, +rotate);
      motor(2, -speed + rotate);
      motor(3, +rotate);
      motor(4, speed + rotate);
    }

    // rechts
    if (direction == 2) {
      motor(1, speed + rotate);
      motor(2, -speed + rotate);
      motor(3, -speed + rotate);
      motor(4, speed + rotate);
    }

    // 135 Grad rechts
    if (direction == 3) {
      motor(1, speed + rotate);
      motor(2, +rotate);
      motor(3, -speed + rotate);
      motor(4, +rotate);
    }
    // zurück
    if (direction == 4) {
      motor(1, speed + rotate);
      motor(2, speed + rotate);
      motor(3, -speed + rotate);
      motor(4, -speed + rotate);
    }

    // 135 Grad links
    if (direction == -3) {
      motor(1, +rotate);
      motor(2, speed + rotate);
      motor(3, +rotate);
      motor(4, -speed + rotate);
    }

    // links
    if (direction == -2) {
      motor(1, -speed + rotate);
      motor(2, speed + rotate);
      motor(3, speed + rotate);
      motor(4, -speed + rotate);
    }

    // 45 Grad links
    if (direction == -1) {
      motor(1, -speed + rotate);
      motor(2, +rotate);
      motor(3, speed + rotate);
      motor(4, +rotate);
    }
  }

  int directionBehindBall() {
    speed = 50;
    switch (ballDirection) {
      case 1:
        return 2;
      case -1:
        return -2;

      case 2:
        return 4;
      case -2:
        return -4;

      case 3:
        return 5;
      case -3:
        return -5;

      case 4:
        return 6;
      case -4:
        return -6;

      case 5:
        speed = 55;
        return 7;
      case -5:
        speed = 55;
        return -7;

      case 6:
       speed = 60;
        return 8;
      case -6:
        speed = 60;
        return 8;

      case 7:
        speed = 55;
        return -7;
      case -7:
        speed = 55;
        return 7;

      case 8:
        return -6;
    }
  }

  void getOutOfCorner() {
    switch (siteOfBot()) {
      case "right":
        // bot ist in der rechten ecke
        if (hasBall()) {
          drive(0, 40, -10);
        } else {
          drive(-3, 70, 0);
        }
      case "left":
        // bot ist in der linken ecke
        if (hasBall()) {
          drive(0, 40, 10);
        } else {
          drive(3, 70, 0);
        }
      default:
        Serial.println("bot faces the goal head on")
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

  bool button(int device, int nr)  // liefert von device Taster nr (1:links, 2:rechts)
  {
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

  int ball() {
    if (ballSeen)
      return true;
    return false;
  }

  bool hasBall() {
    if (input(3) > 0) {
      return false;
    } else
      return true;
  }

  int getGoalDirection() {
    readPixy();
    return goalDirection;
  }

  int getGoalDistance() {
    return goalDirection;
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

  // lässt jede led in bestimmter farbe für 0.5s leuchten
  void blinkAll(int color) {
    bot.led(0, 1, color);
    bot.led(0, 2, color);
    bot.led(7, 1, color);
    bot.led(7, 2, color);
    bot.warte(200);
    bot.led(0, 1, OFF);
    bot.led(0, 2, OFF);
    bot.led(7, 1, OFF);
    bot.led(7, 2, OFF);
  }

  void kick(int time) {
    if (deathTime < 1000)
      return;
    if (time > 40)
      time = 40;
    digitalWrite(kicker, HIGH);
    delay(time);
    digitalWrite(kicker, LOW);
    deathTime = 0;
  }

  void strike() {
    for (int i = 50; i < 100; i = i + 3) {
      drive(0, i, getGoalDirection() / 5);
      delay(1);
    }
    drive(0, 100, getGoalDirection() / 5);
    delay(20);
    kick(40);
    drive(0, -100, 0);
    delay(10);
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

 private:
  int spdToPWM(int speed) {
    if (speed < 0)
      speed *= -1;
    return ((speed * 255) / 100);
  }

  int getCompass() {
    return compass;
  }

  void setCompass() {
    Wire.beginTransmission(compassAddress);
    byte error = Wire.endTransmission();
    if (error == 0) {
      compassHead = CompassOrg();
    }
  }

 private:
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

 private:
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

 private:
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
          ballSeen = false;
        } else
          ballSeen = true;
        if (!ballSeen)
          ballDirection = 0;
      }
    } else
      Serial.println("IR doesn't answer");
  }

 private:
  void evaluatePixy() {
    int signature = 1;
    int pixyBlocks = pixy.ccc.blocks[0].m_signature;

    if (pixyBlocks == signature) {
      goalDirection = -(pixy.ccc.blocks[0].m_x - 158) / 2;
      lastCompassOutput = getCompass();
      // int goalWidth = pixy.ccc.blocks[0].m_width;
      int goalHeight = pixy.ccc.blocks[0].m_height;
      int goalDistanceRaw = pixy.ccc.blocks[0].m_y;
      goalDistance = (goalDistanceRaw - goalHeight) / 4;
      if (goalDistance < 0)
        goalDistance = 0;
      if (goalDistance > 63)
        goalDistance = 63;
    }
  }

 private:
  void readPixy() {
    pixy.ccc.getBlocks();

    if (pixy.ccc.numBlocks) {
      evaluatePixy();
      pixyBlind = false;
    } else {
      goalDirection = getCompass();
      pixyBlind = true;
    }
  }

  bool getPixyBlind() {
    return pixyBlind;
  }

 private:
  string siteOfBot() {
    if (lastCompassOutput != 0)
      return lastCompassOutput < 0 ? "right" : "left";
    else
      return "frontal";
  }
};