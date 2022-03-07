#include <Arduino.h>
#include <header.h>

#define blue 000;
#define yellow 001;
Bot bot;
int game;
int start;

void setup() {
  bot.setupBot(true, true, blue);
  game = 0;
  start = 0;
}

void play(bool s) {
  bool KickOff = s;

  if (bot.ballVisible()) {
    bot.led(0, 2, OFF);

    if (KickOff) {
      // nur effektiv wenn bot seitlich gestellt wird, damit er eine kurve um den gegner macht
      KickOff = false;
      bot.drive(0, 75, bot.getGoalDirection() / -3);
      bot.wait(500);
    }

    else {
      if (bot.IsInCorner()) {
        bot.led(7, 2, RED);
        bot.getOutOfCorner();
      } else {
        bot.led(7, 2, WHITE);
        if (bot.hasBall()) {
          if (bot.getGoalDirection() < 3 && bot.getGoalDirection() > -3 && bot.getBallDirection() == 0 && !bot.getPixyBlind()) {
            // bot ist mit ball zum Tor gerichtet
            bot.strike();
          } else {
            // bot hat ball, guckt aber nicht direkt zum tor oder ist weit weg
            bot.drive(0, 60, bot.getGoalDirection() / -2);
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
  if (bot.button(0, 2)) {
    bot.kick(45);
  } else if (bot.button(7, 1)) {
    bot.setCompass();
  } else if (bot.button(7, 2)) {
    Serial.println(bot.getCompass());
  }
}

void gameMode() {
  if (bot.button(0, 2)) {
    bot.pauseBot();
    game = 0;
    start = 0;
  } else if (bot.button(7, 1)) {
    game = 1;
    start = 1;
  } else if (bot.button(7, 2)) {
    game = 1;
    start = 0;
  }
}

void loop() {
  bot.wait(5);
  if (bot.button(0, 1)) {
    bot.changeMode();
    bot.wait(100);
  }

  switch (bot.getMode()) {
    case 0:
      bot.led(0, 1, YELLOW);
      preparationMode();
      break;
    case 1:
      bot.led(0, 1, GREEN);
      gameMode();
      break;
  }

  switch (game) {
    case 1:
      switch (start) {
        case 1:
          play(true);
          break;
        case 0:
          play(false);
          break;
      }
  }
}
