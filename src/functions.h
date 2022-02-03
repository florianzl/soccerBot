/*
  ausgelagerte Funktionen vom main programm
*/

#include <Arduino.h>

#include "bohlebots.h"
BohleBots bot;

// lässt jede led in bestimmter farbe für 0.5s leuchten
void led(string color) {
  bot.led(0, 1, color);
  bot.led(0, 2, color);
  bot.led(7, 1, color);
  bot.led(7, 2, color);
  bot.warte(500);
  bot.led(0, 1, OFF);
  bot.led(0, 2, OFF);
  bot.led(7, 1, OFF);
  bot.led(7, 2, OFF);
}

void startBot() {
  bot.setSoccer(true);
  bot.setPixy(true);
  bot.init();
  bot.setze_kompass();
  Serial.println("Kompass gesetzt");
  led("GREEN");
}

//bot beschleunigt und kickt
int shoot() {
  // Zahl muss rausgefunden werden
  if (bot.goalDistance < 10) {
    bot.kick(30);
    bot.fahre(0,0,0);
  }
  else 
    bot.fahre(0,100,0);
}

// bot bleibt vor Tor stehen
int speed(int s) {
  return bot.goalDistance > 5 ? s : 0;
}

// passt rotationsgeschwindigkeit an winkel zum Tor an, damit bot nicht überdreht
int rotate() {
  // TODO test ob bestimmte geschwindigkeit?
  if (bot.goalDirection() > 5) {
    if (bot.goalDirection() > 150)
      return -24;
    else if (bot.goalDirection() > 115 && bot.goalDirection() < 150)
      return -21;
    else if (bot.goalDirection() > 80 && bot.goalDirection() < 115)
      return -17;
    else if (bot.goalDirection() > 50 && bot.goalDirection() < 80)
      return -14;
    else if (bot.goalDirection() > 20 && bot.goalDirection() < 50)
      return -12;
    else if (bot.goalDirection() > 3 && bot.goalDirection() < 20)
      return -10;
  } else if (bot.goalDirection() < -5) {
    if (bot.goalDirection() < -150)
      return 24;
    else if (bot.goalDirection() < -115 && bot.goalDirection() > -150)
      return 21;
    else if (bot.goalDirection() < -80 && bot.goalDirection() > -115)
      return 17;
    else if (bot.goalDirection() < -50 && bot.goalDirection() > -80)
      return 14;
    else if (bot.goalDirection() < -20 && bot.goalDirection() > -50)
      return 12;
    else if (bot.goalDirection() < -3 && bot.goalDirection() > -20)
      return 10;
  }
  return 0;
}

// TODO individuelle rotationsgeschwindigkeit
int turnToBall() {
  int rotationSpeed = 0;

  if (bot.ballDirection() < 0) {
    if (bot.ballDirection() > -3)
      rotationSpeed = -9;

    else
      rotationSpeed = -15;
  }

  else if (bot.ballDirection() > 0) {
    if (bot.ballDirection() < 3)
      rotationSpeed = 9;

    else
      rotationSpeed = 15;
  }

  return rotationSpeed;
}

// bot geht auf standby
void stopBot() {
  bot.drive(0, 0, 0);
  led("RED");
}

// bot fährt 2 richungen weiter als Ball bis er frontal zum Ball ist
int directionBehindBall() {
  if (bot.ballDirection() < 0) {
    if (bot.ballDirection() == -6)
      return 8;
    else if (bot.ballDirection() == -7)
      return 7;
    else
      return bot.ballDirection() - 2;
  }

  else if (bot.ballDirection() > 0) {
    if (bot.ballDirection() == 7)
      return -7;
    else if (bot.ballDirection() == 8)
      return -6;
    else
      return bot.ballDirection() + 2;
  }

  return 0;
}