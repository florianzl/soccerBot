#include <Arduino.h>
#include <header.h>

Bot bot;
int game;
int start;

void setup() {
  bot.setupBot(true, true);
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
      bot.drive(0, 75, bot.getGoalDirection() / -2);
      bot.wait(500);
    }

    else {
      if (bot.IsInCorner()) {
        bot.led(7, 2, RED);
        bot.getOutOfCorner();
      } else {
        bot.led(7, 2, WHITE);
        if (bot.hasBall()) {
          if (bot.getGoalDirection() < 10 && bot.getGoalDirection() > -10 && bot.hasBall() && !bot.getPixyBlind()) {
            // bot ist mit ball zum Tor gerichtet
            bot.strike();
          } else {
            // bot hat ball, guckt aber nicht direkt zum tor oder ist weit weg
            bot.drive(0, 60, bot.getGoalDirection() / -2);
          }
        } else {
          // ball nicht in Ballschale
          if (bot.getBallDirection() == 1 || bot.getBallDirection() == -1) {
            bot.drive(bot.directionBehindBall(), 30, bot.getBallDirection() * -15);
          } else {
            bot.drive(bot.directionBehindBall(), 30, bot.getCompass() / -5);
          }
        }
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
    bot.drive(bot.getBallDirection(), 20, bot.getCompass() / -5);
    Serial.println("preparation" + String(bot.getBallDirection()));
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
