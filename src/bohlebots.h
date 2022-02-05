/*
 * motor vorne rechts 1, hinten rechts 2, hinten links 3, vorne links 4
 * Logik : vorne rechts ist 1 und dann im Uhrzeigersinn
 *
 * Richtung von -179 bis 180, 0 nach vorne - wird gequantelt
 * Geschw von -100 bis +100
 * dreh (von -100 bis + 100 , rechts rum positiv
 *
 * kein delay im main Programm
 * -> bei warte scannt bot weiter
 */

#include <CAN.h>
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
#define CYAN 5
#define MAGENTA 6
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

int compassInt;
int head;
bool compassEna = false;

int ballDirectionVar = 0;
int ballMax = 0;

int goalDirectionVar = 0;
int goalDistanceVar = 0;

boolean soccer = false;
boolean hasPixy = false;

bool portena[] = {false, false, false, false, false, false, false, false};
int buttonLedId[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
bool button1Array[] = {false, false, false, false, false, false, false, false};
bool button2Array[] = {false, false, false, false, false, false, false, false};

int led1Array[] = {0, 0, 0, 0, 0, 0, 0, 0};
int led2Array[] = {0, 0, 0, 0, 0, 0, 0, 0};

elapsedMillis deathTime;
elapsedMillis waitTime;

class BohleBots {
 public:
  BohleBots() {
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

    Serial.print("Warte auf Canbus");
    delay(100);
    if (!CAN.begin(500E3)) {
      Serial.print("...failed!");
      while (1)
        ;
    }  // start the CAN bus at 500 kbps
    else
      Serial.println("...succeed");
  }

  void init() {
    digitalWrite(DRIVE_DIS, HIGH);
    delay(100);
    deathTime = 1000;
    for (int i = 0; i < 8; i++) {
      Wire.beginTransmission(buttonLedId[i]);
      byte error = Wire.endTransmission();
      if (error == 0)
        portena[i] = true;
      Serial.print("LED_Tast : " + String(i) + " : ");
      if (error == 0)
        Serial.println("true");
      else
        Serial.println("false");
    }
    delay(100);
    Wire.beginTransmission(compassAddress);
    byte error = Wire.endTransmission();
    if (error == 0)
      compassEna = true;
    if (error == 0)
      Serial.println("Kompass true");
    else
      Serial.println("Kompass false");

    delay(100);
    if (hasPixy) {
      Serial.print("Warte auf Pixy2 auf i2c 0x54...");
      pixy.init(0x54);
      Serial.println("done");
    }
  }

  void warte(int time) {
    waitTime = 0;
    i2csync();
    if (soccer)
      Can();
    if (hasPixy)
      readPixy();
    while (waitTime < time) {
      if ((waitTime % 10) == 0)
        i2csync();
      else
        delay(1);
    }
  }

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

  void drive(int direction, int speed, int rotate) {
    // direction = direction / 45;
    int maxs = abs(speed) + abs(rotate);
    if (maxs > 100) {
      speed = speed * 100 / maxs;
      rotate = rotate * 100 / maxs;
    }
    // Serial.println("Richtung: "+String(richtung)+" geschw: "+String(geschw)+" Dreh: "+dreh);
    if (direction == 0)  // geradeaus
    {
      motor(1, -speed + rotate);
      motor(2, -speed + rotate);
      motor(3, speed + rotate);
      motor(4, speed + rotate);
    }

    if (direction == 1)  // 45 Grad rechts
    {
      motor(1, +rotate);
      motor(2, -speed + rotate);
      motor(3, +rotate);
      motor(4, speed + rotate);
    }
    if (direction == 2)  // rechts
    {
      motor(1, speed + rotate);
      motor(2, -speed + rotate);
      motor(3, -speed + rotate);
      motor(4, speed + rotate);
    }

    if (direction == 3)  // 135 Grad rechts
    {
      motor(1, speed + rotate);
      motor(2, +rotate);
      motor(3, -speed + rotate);
      motor(4, +rotate);
    }

    if (direction == 4)  // zurück
    {
      motor(1, speed + rotate);
      motor(2, speed + rotate);
      motor(3, -speed + rotate);
      motor(4, -speed + rotate);
    }

    if (direction == -3)  // 135 Grad links
    {
      motor(1, +rotate);
      motor(2, speed + rotate);
      motor(3, +rotate);
      motor(4, -speed + rotate);
    }
    if (direction == -2)  // links
    {
      motor(1, -speed + rotate);
      motor(2, speed + rotate);
      motor(3, speed + rotate);
      motor(4, -speed + rotate);
    }

    if (direction == -1)  // 45 Grad links
    {
      motor(1, -speed + rotate);
      motor(2, +rotate);
      motor(3, speed + rotate);
      motor(4, +rotate);
    }
  }

  void setSoccer(boolean t) {
    soccer = t;
  }

  void setPixy(boolean t) {
    hasPixy = t;
  }

  void servo(int pos)  // sinnvollerweise von -100 bis 100
  {
    if (pos > 100)
      pos = 100;
    if (pos < -100)
      pos = -100;
    ledcWrite(6, 5000 + pos * 30);
  }

  void ena(bool isena) {
    digitalWrite(DRIVE_DIS, !isena);
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

  int compass() {
    return compassInt;
  }

  void setze_kompass() {
    Wire.beginTransmission(compassAddress);
    byte error = Wire.endTransmission();
    if (error == 0) {
      head = kompass_org();
    }
  }

  bool button(int device, int nr)  // liefert von device Taster nr (1:links, 2:rechts)
  {
    if (device < 0)
      return false;
    if (device > 7)
      return false;
    // portena[device] = true;
    if (nr == 1)
      return button1Array[device];
    if (nr == 2)
      return button2Array[device];
    return false;
  }

  int ballDirection() {
    return ballDirectionVar;
  }

  int ball() {
    if (ballMax > 0)
      return true;
    return false;
  }

  int goalDirection() {
    readPixy();
    return goalDirectionVar;
  }

  int goalDistance() {
    return goalDistanceVar;
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

    // portena[device] = true;

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
    for (int i = 40; i < 100; i = i + 3) {
      drive(0, i, 0);
      delay(1);
    }
    drive(0, 100, 0);
    delay(20);
    kick(40);
    drive(0, -100, 0);
    delay(10);
    drive(0, 0, 0);
  }

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

  int kompass_org() {
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

  int kompass_lesen() {
    return ((((kompass_org() - head) + 180 + 360) % 360) - 180);
  }

  void set_heading() {
    head = kompass_org();
  }

  void i2csync() {
    for (int i = 0; i < 8; i++) {
      if (portena[i]) {
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
    }  // ENDE TastLED
    if (compassEna == true) {
      compassInt = kompass_lesen();
    }
  }

  void Can() {
    elapsedMillis echtzeit = 0;

    int time = 0;
    int irpaket;
    CAN.beginPacket(0x03, 1, true);  // sendet RTR und will 1 byte
    if (CAN.endPacket() == 1) {
      // Serial.println("done1 :"+String(time));
      while (!CAN.parsePacket()) {
        delayMicroseconds(1);
        time++;
      }
      // Serial.println("done2 :"+String(time));
      // Serial.println("echtzeit :"+String(echtzeit));
      while (CAN.available()) {
        irpaket = CAN.read();
        ballDirectionVar = (irpaket / 16) - 7;
        int zone = irpaket % 16;
        if (zone < 1) {
          ballMax = 0;
        } else
          ballMax = 1;
        if (ballMax < 1)
          ballDirectionVar = 0;
      }

      // Serial.println("done :"+String(time));
    } else
      Serial.println("IR antwortet nicht");
  }

  void evaluatePixy()  // wird nur aufgerufen, wenn die Pixy überhaupt etwas sieht
  {
    int goal = 1;  // pixy signatur, die benutzt wird im code
    int pixyBlocks = pixy.ccc.blocks[0].m_signature;

    if (pixyBlocks == goal) {
      goalDirectionVar = -(pixy.ccc.blocks[0].m_x - 158) / 2;
      int goalWidth = pixy.ccc.blocks[0].m_width;
      int goalHeight = pixy.ccc.blocks[0].m_height;
      int goalDistanceRaw = pixy.ccc.blocks[0].m_y;
      goalDistanceVar = (goalDistanceRaw - goalHeight) / 4;  //-abs(tor_richtung)/10;
      if (goalDistanceVar < 0)
        goalDistanceVar = 0;
      if (goalDistanceVar > 63)
        goalDistanceVar = 63;
    }
  }

 public:
  void readPixy() {
    pixy.ccc.getBlocks();

    if (pixy.ccc.numBlocks) {
      evaluatePixy();
    } else {
      goalDirectionVar = compass();
    }
  }

 public:
  boolean hasBall() {
    if (input(3) > 0) {
      return false;
    }
    else return true;
  }
};