#include <Arduino.h>
#include <header.h>

Bot bot;
int game;
int start;
int challenge;

void setup() {
  bot.setupBot(true, true);
  game = 0;
  start = 0;
  challenge = 0;
}

void play() {
  if (bot.ballVisible()) {
    bot.led(0, 2, OFF);

    // roboter hat keinen anstoß
    if (bot.IsInCorner()) {
      bot.led(7, 2, RED);
      bot.getOutOfCorner();
    } else {
      // roboter ist nicht in der ecke
      bot.led(7, 2, WHITE);

      if (bot.hasBall()) {
        if (bot.getGoalDirection() < 10 && bot.getGoalDirection() > -10 && !bot.getPixyBlind()) {
          // bot ist mit ball zum Tor gerichtet
          bot.strike();
        } else {
          // bot hat ball, guckt aber nicht direkt zum tor
          bot.drive(0, 60, bot.getGoalDirection() / -1.7);
        }

      } else {
        // ball ist nicht in Ballschale
        bot.drive(bot.directionBehindBall(), bot.getSpeed(), bot.getCompass() / -5);
      }
    }

  } else {
    // ball wird nicht gesehen
    bot.led(0, 2, RED);
    bot.drive(0, 0, bot.getGoalDirection() / -1);
  }
}

void preparationMode() {
  if (bot.button(0, 2)) {
    bot.kick(45);
  } else if (bot.button(7, 1)) {
    bot.setCompass();
  } else if (bot.button(7, 2)) {
    Serial.println(bot.getGoalDistance());
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

      switch (game) {
        case 1:
          switch (start) {
            case 1:
              play();
              break;
            case 0:
              play();
              break;
          }
      }
      break;
  }
}
