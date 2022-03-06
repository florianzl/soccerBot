#include <Arduino.h>
#include <header.h>

Bot bot;
int action;
int start;
int defense;

void setup() {
  bot.setupBot(true, true);
  action = 0;
  start = 0;
  defense = 0;
}

void play(bool s) {
  bool KickOff = s;

  if (bot.ballVisible()) {
    bot.led(0, 2, OFF);

    if (KickOff) {
      // nur effektiv wenn bot seitlich gestellt wird, damit er eine kurve um den gegner macht
      KickOff = false;
      bot.drive(bot.directionBehindBall(), 75, bot.getGoalDirection() / -5);
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
    defense = 0;
    bot.kick(45);
  } else if (bot.button(7, 1)) {
    defense = 0;
    bot.setCompass();
  } else if (bot.button(7, 2)) {
    defense = 1;
  }
}

void gameMode() {
  if (bot.button(0, 2)) {
    bot.pauseBot();
    action = 0;
    start = 0;
  } else if (bot.button(7, 1)) {
    action = 1;
    start = 1;
  } else if (bot.button(7, 2)) {
    action = 1;
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

  switch (action) {
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
  switch (defense) {
    case 1:
      bot.defense();
  }
}
