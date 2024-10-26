/*
Projet: main.cpp
Equipe: 09
Auteurs: Simon Gaudet, Xavier Tremblay, Vincent Dupéré
Description: Ce programme permet de faire circuler un robot dans un labyrinthe et trouver le bon chemin
Date: 10-08-2024
*/

#include "settings.h"

/* Gestion des cerveaux moteurs
 ajuster la tension VServo qui les alimentes à 7.2V max)*/
 
void initialiserServoMoteur(){
  SERVO_Enable(LEFT);
  SERVO_Enable(RIGHT);
  SERVO_SetAngle(LEFT,0);
  SERVO_SetAngle(RIGHT,180);
}
 
void desactiverServoMoteur(){
  SERVO_Disable(LEFT);
  SERVO_Disable(RIGHT);
}

void ramasserObjet(){
  SERVO_SetAngle(LEFT,90);
  SERVO_SetAngle(RIGHT,90);
}
 

void setup(){
  BoardInit();
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(3);


}







void loop()
{



}














































/*void loop() {
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);
  if(etat == 0){Sifflet();}                                                                             // a ajouter
  Serial.println(etat);
  if (etat >= 1) // ou > 0
  {
    if (vert && y != 4) // Pas d'obstacle
    {
      if (x == 0)
      {
        if (y % 2 == 0)
        {
          if (regardeFace == 0)
          {
            setOrientation(1);
          }
          else if (regardeFace == -1)
          {
           
            avance();
            x -= 1;
          }
          else if (regardeFace == 1)
          {
           
            avance();
            x += 1;
          }
        }
        else if (y % 2 != 0) // Vu qu'il est entre 2 lignes c'est obligé qu'il regarde en avant
        {
         
          avance();
          y += 1;
        }
      }
      else if (x == 1)
      {
        if (regardeFace == 0)
        {
         
          avance();
          y += 1;
        }
        else if (regardeFace == 1)
        {
         
          setOrientation(0);
        }
        else if (regardeFace == -1)
        {
         
          avance();
          x -= 1;
        }
      }
        else if (x == -1)
      {
        if (regardeFace == 0)
        {
         
          avance();
          y += 1;
        }
        else if (regardeFace == -1)
        {
          Serial.println("setorientation");
          setOrientation(0);
        }
        else if (regardeFace == 1)
        {
          Serial.println("avance");
          avance();
          x += 1;
        }
        last_move = 0;
      }
    }
 
 
 
 
    else if (vert && y == 4) // Pas d'obstacle et pas de mur au milieu.
    {
      if (x == 0)
      {
        if (y % 2 == 0)
        {
          if (regardeFace == 0)
          {
            avance();
            y += 1;
          }
          else if (regardeFace == -1)
          {
           
            avance();
            x -= 1;
          }
          else if (regardeFace == 1)
          {
           
            avance();
            x += 1;
          }
        }
        else if (y % 2 != 0) // Vu qu'il est entre 2 lignes c'est obligé qu'il regarde en avant
        {
         
          avance();
          y += 1;
        }
      }
      else if (x == 1)
      {
        if (regardeFace == 0)
        {
         
          avance();
          y += 1;
        }
        else if (regardeFace == 1)
        {
         
          setOrientation(0);
        }
        else if (regardeFace == -1)
        {
         
          avance();
          x -= 1;
        }
      }
        else if (x == -1)
      {
        if (regardeFace == 0)
        {
         
          avance();
          y += 1;
        }
        else if (regardeFace == -1)
        {
          Serial.println("setorientation");
          setOrientation(0);
        }
        else if (regardeFace == 1)
        {
          Serial.println("avance");
          avance();
          x += 1;
        }
        last_move = 0;
      }
    }
    if (!vert) // Détection d'un obstacle
    {
      if (x == 0)
      {
        if (y % 2 == 0)
        {
          if (regardeFace == 0)
          {
            Serial.println("setOrientation");
            setOrientation(1);
            last_move = 1;
          }
          else if (regardeFace == 1 && last_move == 1)
          {
            Serial.println("setOrientation");
            setOrientation(-1);
            last_move = 0;
          }
          else if (regardeFace == 1 && last_move == 0)
          {
            Serial.println("setOrientation");
            setOrientation(0);
          }
          else if (regardeFace == -1)
          {
            Serial.println("setOrientation");
            setOrientation(0);
          }
        }
      }
      if (x == 1)
      {
        if(y % 2 == 0)
        {
          Serial.println("setOrientation");
          setOrientation(-1);
        }
      }
      if (x == -1)
      {
        if(y % 2 == 0)
        {
          Serial.println("setOrientation");
          setOrientation(1);
        }
      }
    }
  }
  if(y == 10)
  {
    if(x == 1)
    {
      tourneGauche();
      avance();
      tourneGauche();
      x = 0;
      avance_retour();
      etat = 0;
    }
    else if(x == -1)
    {
      tourneDroit();
      avance();
      tourneDroit();
      x = 0;
      avance_retour();
      etat = 0;
    }
  }
  delay(100);
}
*/