#include <Arduino.h>

#include "bohlebots.h"
BohleBots bot;

void startBot() {
  bot.led(0, 1, WHITE);
  bot.setze_kompass();
  Serial.println("Kompass gesetzt");
  bot.warte(500);
  bot.led(0, 1, OFF);
}

int rotate() {
  int t = 0;
  if (bot.goalDirection() > 5) {
    //TODO test ob bestimmte geschwindigkeit?
    if (bot.goalDirection() > 150)
      t = -24;
    else if (bot.goalDirection() > 115 && bot.goalDirection() < 150)
      t = -21;
    else if (bot.goalDirection() > 80 && bot.goalDirection() < 115)
      t = -17;
    else if (bot.goalDirection() > 50 && bot.goalDirection() < 80)
      t = -14;
    else if (bot.goalDirection() > 20 && bot.goalDirection() < 50)
      t = -12;
    else if (bot.goalDirection() > 3 && bot.goalDirection() < 20)
      t = -10;
  } else if (bot.goalDirection() < -5) {
    if (bot.goalDirection() < -150)
      t = 24;
    else if (bot.goalDirection() < -115 && bot.goalDirection() > -150)
      t = 21;
    else if (bot.goalDirection() < -80 && bot.goalDirection() > -115)
      t = 17;
    else if (bot.goalDirection() < -50 && bot.goalDirection() > -80)
      t = 14;
    else if (bot.goalDirection() < -20 && bot.goalDirection() > -50)
      t = 12;
    else if (bot.goalDirection() < -3 && bot.goalDirection() > -20)
      t = 10;
  }
  return t;
}

//TODO individuelle rotationsgeschwindigkeit
int turnToBall() {
  int rotationSpeed = 0;

  if (bot.ballDirection() < 0) {
    if (bot.ballDirection() > -3)
      rotationSpeed = -9;

    else
      rotationSpeed = -15;
  }

  else if (bot.ballDirection() > 0) {
    if (bot.ballDirection() < 3)
      rotationSpeed = 9;

    else
      rotationSpeed = 15;
  }

  return rotationSpeed;
}

void stopBot() {
  bot.led(0, 1, WHITE);
  bot.led(0, 2, WHITE);
  bot.led(7, 1, WHITE);
  bot.led(7, 2, WHITE);
  bot.drive(0, 0, 0);
  bot.warte(500);
  bot.led(0, 1, OFF);
  bot.led(0, 2, OFF);
  bot.led(7, 1, OFF);
  bot.led(7, 2, OFF);
}

int directionBehindBall() {
  // bot fährt 2 richungen weiter als Ball bis er frontal zum Ball ist

  if (bot.ballDirection() < 0) {
    if (bot.ballDirection() == -6)
      return 8;
    else if (bot.ballDirection() == -7)
      return 7;
    else
      return bot.ballDirection() - 2;
  }

  else if (bot.ballDirection() > 0) {
    if (bot.ballDirection() == 7)
      return -7;
    else if (bot.ballDirection() == 8)
      return -6;
    else
      return bot.ballDirection() + 2;
  }
  
  return 0;
}