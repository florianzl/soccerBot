/*
 * OHNE KICKER
 */

#include <Arduino.h>

#include "functions.h"

int modus;

void setup() {
  modus = 0;
  speed = 70;
  startBot();
}

void loop() {
  bot.warte(5);

  if (bot.button(0, 1)) {
    // kicker test
    bot.setze_kompass();
    Serial.println("Kompass gesetzt");
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
      if (bot.ballDirection() < 3 && bot.ballDirection() > -3)
        bot.drive(0, speed, bot.ballDirection() * 5);
      else
        bot.drive(directionBehindBall() / 2, speed, bot.goalDirection() / -5);
    } else
      bot.drive(0, 0, bot.goalDirection() / -5);
  }

  else if (modus == 2) {
    if (bot.ball()) {
      bot.drive(directionBehindBall() / 2, speed, bot.goalDirection() / -5);
    } else
      bot.drive(0, 0, bot.goalDirection() / -5);
  }

  // testen der rotation
  else if (modus == 10) {
    // bot.drive(0, speed(30), rotate());
    // bot.drive(0,30,bot.goalDirection() / -5);
    // Serial.println("richtung: " + String(bot.ballDirection()) + " fahre " + String(directionBehindBall()));
    bot.drive(0, 0, bot.ballDirection() * 5);
  }
}
