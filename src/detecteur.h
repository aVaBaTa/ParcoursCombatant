#include <Arduino.h>
#include <LibRobUS.h>


// Pour le sifflet
int Pin5KHZ = A10;
int PinAmbiant = A11;
int INTENSITE_5KHZ;
int SonAmbiant;
float sifflet;
float ambiant;
int start = 0;


void Sifflet()
{
  INTENSITE_5KHZ = analogRead(Pin5KHZ);
  SonAmbiant = analogRead(PinAmbiant);
  sifflet = INTENSITE_5KHZ * (5.0/1023.0);
  ambiant = SonAmbiant * (5.0/1023.0);
  Serial.println(INTENSITE_5KHZ);
  Serial.println(SonAmbiant);
  if(sifflet >= ambiant)                                                                            // robot a 3
  {
    start = 1;
  }
}