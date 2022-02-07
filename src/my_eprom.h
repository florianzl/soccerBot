#include "EEPROM.h"

void readEEPROM()
{
 for (int i = 0; i < 4; i++)
  {
    eprombyte[i]=(byte(EEPROM.read(i)));
  }
  
}

void writeEEPROM()
{
  eprombyte[0] = modus;
  eprombyte[1] = head/256;  // highbyte
  eprombyte[2] = head%256;  // lowbyte
  eprombyte[3] = start;
  
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i, eprombyte[i]);
  }
  EEPROM.commit();
}

void eprom_init()
{
  readEEPROM();
  head = eprombyte[1]*265+eprombyte[2];//  hightbyte,lowbyte
  if (eprombyte[0] ==2)
  {
    modus = 2; // Stürmer soll sofort loslegen
    // eprombyte[3] wird noch nicht benutzt
  }
}
