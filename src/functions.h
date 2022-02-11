/*
  ausgelagerte Funktionen vom main programm
*/

#include <Arduino.h>

#include "header.h"



// lässt jede led in bestimmter farbe für 0.5s leuchten
void led(int color) {
  bot.led(0, 1, color);
  bot.led(0, 2, color);
  bot.led(7, 1, color);
  bot.led(7, 2, color);
  bot.warte(200);
  bot.led(0, 1, OFF);
  bot.led(0, 2, OFF);
  bot.led(7, 1, OFF);
  bot.led(7, 2, OFF);
}

void startBot() {
  soccer = true;
  pixy = true;
  bot_init()
  bot.init();
  led(GREEN);
}


// bot beschleunigt und kickt
int shoot() {
  // Zahl muss rausgefunden werden
  if (bot.goalDistance() < 10) {
    bot.kick(30);
    bot.drive(0, 0, 0);
  } else
    bot.drive(0, 100, 0);
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
  led(RED);
}

// bot fährt 2 richungen weiter als Ball bis er frontal zum Ball ist
int directionBehindBall() {
  if (bot.ballDirection() < 0) {
    speed = 50;
    if (bot.ballDirection() == -1)
      return -2;
    else if (bot.ballDirection() == -2)
      return -4;

    else if (bot.ballDirection() == -3)
      return -5;

    else if (bot.ballDirection() == -4)
      return -6;

    else if (bot.ballDirection() == -5){
      speed = 50;
      return -7;
    }

    else if (bot.ballDirection() == -6){
      return 8;
      speed = 55;
    }

    else if (bot.ballDirection() == -7){
      speed = 50;
      return 7;
    }

  }

  else if (bot.ballDirection() > 0) {
    speed = 50;
    if (bot.ballDirection() == 1)
      return 2;

    else if (bot.ballDirection() == 2)
      return 4;

    else if (bot.ballDirection() == 3)
      return 5;

    else if (bot.ballDirection() == 4)
      return 6;

    else if (bot.ballDirection() == 5){
      speed = 50;
      return 7;
    }

    else if (bot.ballDirection() == 6){
      return 8;
      speed = 55;
    }

    else if (bot.ballDirection() == 7) {
      speed = 50;
      return -7;
      }

    else if (bot.ballDirection() == 8)
      return -6;
  }
  
  if (bot.goalDirection() < 5 && bot.goalDirection() > -5) {
    speed = 70 ;
    return 0;
  }

  return 0;
  speed = 65;
}#