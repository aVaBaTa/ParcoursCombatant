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







  Detecteur_IR_Objet();

  delay(500);
}




/*
                                                          // CODE GEDEON
 
//Pour le suiveur de igne
int CaptLeft = A10;
int CaptMid = A11;
int CaptRight = A12;
int ligneGauche;
int ligneMilieu;
int ligneDroite;

 
// Gestion des cerveaux moteurs 
//ajuster la tension VServo qui les alimentes à 7.2V max)
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
 




 
//Suiveur de ligne https://github.com/UdeS-GRO/LibRobus/wiki/Documentation-mat%C3%A9rielle#suiveur-de-ligne 



 // delete si utiliser avancer***
void avanceLent(){
  resetEncodeurs();
    MOTOR_SetSpeed(RIGHT,vitesse);
    MOTOR_SetSpeed(LEFT, vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
};
 


void ajusterdroite(){
 
  ligneGauche = analogRead(CaptLeft);
  ligneMilieu = analogRead(CaptMid);
  ligneDroite = analogRead(CaptRight);
 
  while ((ligneGauche > 40) && (ligneMilieu < 640))
  {
    resetEncodeurs();
    while (pulses_gauche < 600) // 1800 pulses = 950ms représente le nombre de pulses avant que la fonction tourneDroit arrête
    {
      MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
      MOTOR_SetSpeed(LEFT, vitesse);
      pulses_gauche = ENCODER_Read(LEFT);
    }
  }
}
 
void ajustergauche(){
 
  ligneGauche = analogRead(CaptLeft);
  ligneMilieu = analogRead(CaptMid);
  ligneDroite = analogRead(CaptRight);
 
  while ((ligneDroite > 40) && (ligneMilieu < 640) && (pulses_droit < 600))
  {
    resetEncodeurs();
    while (pulses_droit < 600) // 1800 pulses = 950ms représente le nombre de pulses avant que la fonction tourneDroit arrête
    {
      MOTOR_SetSpeed(RIGHT, vitesse);
      MOTOR_SetSpeed(LEFT, 0.5*vitesse);
      pulses_droit = ENCODER_Read(RIGHT);
    }
  }
}
 
void suivreLigne(){
 
  ligneGauche = analogRead(CaptLeft);
  ligneMilieu = analogRead(CaptMid);
  ligneDroite = analogRead(CaptRight);
  Serial.println(ligneDroite);
  Serial.println(ligneGauche);
  Serial.println(ligneMilieu);
 
  if ((ligneMilieu > 640) && (ligneDroite < 40) && (ligneGauche < 40)){
    avanceLent();
  }
 
  if ((ligneDroite > 650) &&  (ligneMilieu < 40) && (ligneGauche < 40)){
    ajusterdroite();
  }
 
  if ((ligneGauche > 610) && (ligneMilieu < 40) && (ligneDroite < 40)){
    ajustergauche();
  }
 
  if ((ligneMilieu < 40) && (ligneDroite < 40) && (ligneGauche < 40)){
    arret();
  }
 
  if ((ligneMilieu > 640) && (ligneDroite > 650) && (ligneGauche > 610)){
    arret();
  }
 
  if ((ligneMilieu > 640) && (ligneDroite > 650) && (ligneGauche < 40)){
    ajusterdroite();
  }
 
  if ((ligneMilieu > 640) && (ligneDroite < 40) && (ligneGauche > 610)){
    ajustergauche();
  }
  /*
    Lorsqu'on est sur le planche blanc
    -------------------------------
            |       Gauche        |
    Vide    |      24 - 30          
    Ligne   |  319 - 458 - 1023
    -------------------------------
 
    -------------------------------
            |       Milieu        |
    Vide    |      28 - 37          
    Ligne   |  649 - 759 - 1023
    -------------------------------
 
    -------------------------------
            |       Droit         |
    Vide    |      25 - 33          
    Ligne   |  558 - 655 - 1023
    -------------------------------
  */
//}
 
 
 
