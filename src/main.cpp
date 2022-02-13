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
    else {
      // bot hat gerade keinen Anstoß
      if (bot.isInCorner()) {
        bot.getOutOfCorner();
      } 
      else {
        if (bot.hasBall()) {
          if (bot.getGoalDirection() < 3 && bot.getGoalDirection() > -3 && bot.getGoalDistance() < 20) {
            //bot ist in optimaler entfernung mit ball zum Tor gerichtet
            bot.strike();
          } else {
            // bot hat ball, guckt aber nicht direkt zum tor oder ist weit weg
            bot.drive(0,bot.getSpeed(),bot.getGoalDirection() / -5);
          }
        } else {
          bot.drive(bot.directionBehindBall(), bot.getSpeed(), bot.getCompass() / -5);
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
    bot.kick(40);
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
}
*/