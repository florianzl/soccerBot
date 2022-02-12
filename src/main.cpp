#include <Arduino.h>
#include <header.h>

Bot bot;
bool start;

void setup() {
  bot.setupBot(true, true);
  start = true;
}

void play(bool s) {
  bool hasKickoff = s;

  if (bot.ballVisible()) {
    bot.led(0, 2, OFF);

    if (hasKickoff) {
      // nur effektiv wenn bot seitlich gestellt wird, damit er eine kurve um den gegner macht
      bot.drive(bot.directionBehindBall(), 75, bot.getGoalDirection() / -5);
      bot.wait(1000);
      hasKickoff = false;
    }
    // bot hat am anfang nicht den Ball
    else {
      if (bot.isInCorner()) {
        bot.getOutOfCorner();
      } else {
        if (bot.hasBall()) {
          // abfrage ob bot in optimaler entfernung mit ball aufs tor schaut um zu schießen
          if (bot.getGoalDirection() < 3 && bot.getGoalDirection() > -3 && bot.getGoalDistance < 20) {
            bot.strike();
          } else {
            // bot hat ball, guckt aber nicht direkt zum tor oder ist weit weg
          }
        } else {
          bot.drive(directionBehindBall(), bot.getSpeed(), bot.getGoalDirection() / -5);
        }
      }
    }
  } else {
    bot.led(0, 2, RED);
    bot.drive(0, 0, 0);
  }
}

void preparationMode() {
  if (bot.button(0, 2))
    bot.kick(30);
  else if (bot.button(7, 1))
    bot.setCompass();
}

void gameMode() {
  if (bot.button(0, 2))
    bot.pauseBot();
  else if (bot.button(7, 1))
    play(false);
  else if (bot.button(7, 2))
    play(true);
}

void loop() {
  bot.wait(5);
  if (bot.button(0, 1)) bot.changeMode();

  switch (bot.getMode()) {
    case 0:
      preparationMode();
      bot.led(0, 1, YELLOW);
      return;
    case 1:
      gameMode();
      bot.led(0, 1, GREEN);
      return;
  }
}

/*
void loop() {
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
        if(bot.pixyBlind && bot.goalDirection() < 5 && bot.goalDirection() > -5) {


        }
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
*/