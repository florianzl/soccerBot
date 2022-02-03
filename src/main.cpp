/*
 * OHNE KICKER
 */

#include <Arduino.h>

#include "functions.h"

int modus;

void setup() {
  modus = 0;
  startBot();
}

void loop() {
  bot.warte(5);

  if (bot.button(0, 1)) {
    // kicker test
    bot.kick(30)
  }

  else if (bot.button(0, 2))
    modus = 1;

  else if (bot.button(7, 1))
    modus = 2;

  else if (bot.button(7, 2)) {
    stopBot();
    modus = 0;
  }

  // wenn ball mind. richtung 3 ist fährt bot hinter ball, sonst zum ball
  if (modus == 1) {
    if (bot.ball() > 0) {
      if (bot.hasBall()) {
        if (bot.goalDirection() < 3 && bot.goalDirection() > -3) {
          shoot();
        }

        bot.drive(0, 30, rotate());
      } else {
        if (bot.ballDirection() < 3 && bot.ballDirection() > -3)
          bot.drive(0, 30, turnToBall());
        else
          bot.drive(directionBehindBall() / 2, 30, rotate());
      }
    } else
      bot.drive(0, 0, rotate());
  }

  else if (modus == 2) {
    if (bot.ball()) {
      if (bot.hasball()) {
        if (bot.goalDirection() < 3 && bot.goalDirection() > -3) {
          shoot();
        }

        bot.drive(0, 30, rotate());
      } else {
        bot.drive(directionBehindBall / 2, 30, rotate());
      }
    } else
      bot.drive(0, 0, rotate());
  }

  // testen der rotation
  else if (modus == 10) {
    bot.drive(0, 30, 0);
    // bot.drive(0,30,rotate());
    Serial.println(rotate());
  }
}
