/*
 * OHNE KICKER
 */

#include <Arduino.h>
#include "functions.h"
//#include "my_eprom.h"




void loop() {
  bot.warte(5);

  if (bot.button(0, 1)) {
    // kicker test
    bot.setze_kompass();
    Serial.println("Kompass gesetzt");
    bot.warte(100);
  }

  else if (bot.button(0, 2)) {
    modus = 10;
    //writeEEPROM();
  }
    

  else if (bot.button(7, 1)) {
    modus = 2;
    start = true;
    //writeEEPROM();
  }

  else if (bot.button(7, 2)) {
    stopBot();
    modus = 0;
    //writeEEPROM();
  }

  // wenn ball mind. richtung 3 ist fährt bot hinter ball, sonst zum ball
  if (modus == 1) {
    if (bot.ball()) {
      bot.drive(directionBehindBall() / 2, speed, bot.goalDirection() / -5);
    } else
      bot.drive(0, 0, bot.goalDirection() / -5);
  }

  else if (modus == 2) {
    if (bot.ball()) {
      if (start) {
        bot.drive(directionBehindBall() / 2, 75, bot.goalDirection() / -5);
        bot.warte(1000);
        start = false;
      } else {
        bot.drive(directionBehindBall() / 2, speed, bot.goalDirection() / -5);
      }
    } else 
      bot.drive(0, 0, 0);

  }

  // testen der rotation
  else if (modus == 10) {
    // bot.drive(0, speed(30), rotate());
    // bot.drive(0,30,bot.goalDirection() / -5);
    // Serial.println("richtung: " + String(bot.ballDirection()) + " fahre " + String(directionBehindBall()));
    //led(WHITE);
    Serial.println(bot.input(3));
  }
}
