/*
  * OHNE KICKER
*/

#include <Arduino.h>
#include "functions.h"

int modus;

void setup() {
  modus = 0;
  bot.setSoccer(true);
  bot.setPixy(true);
  bot.init();
  Serial.print("start");
}

void loop() {
  bot.warte(5);

  if (bot.button(0, 1)) {
    startBot();
  }

  if (bot.button(0, 2))
    modus = 2;

  if (bot.button(7, 1))
    modus = 1;

  if (bot.button(7, 2)) {
    stopBot();
    modus = 0;
  }

  if (modus == 2) {
    bot.readPixy();
    if (bot.ball() > 0) {

      if (!bot.readLightSensor()) {

          
        //kicker

        //fahre ins tor
        //höhere geschwindigkeit
        bot.drive(0, 30, rotate());
      }

      //ball nicht in Ballschranke
      else {

        if ((bot.ballDirection() < 3 && bot.ballDirection() > 0) || (bot.ballDirection() > -3 && bot.ballDirection() < 0)) {

          bot.drive(0, 30, turnToBall());
        }

        else {

          
          bot.drive(directionBehindBall() / 2, 30, rotate());
        }
      }
    }
    else {
      bot.drive(0,0, rotate());
    }
  }


//testen der rotation
  if (modus == 1) {
    bot.drive(0,30,0);
    //bot.drive(0,30,rotate());
    Serial.println(rotate());
  }
}
