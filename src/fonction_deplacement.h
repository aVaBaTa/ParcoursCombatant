#include <LibRobus.h>
#include <Arduino.h>





bool bumperArr;
int32_t difference_moteurs = 0;
uint32_t pulses_droit = 0;
uint32_t pulses_gauche = 0;
int vertpin = 48;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.45;
int x = 0; // x = 0 initialisation et représente la coordonnée x de la case de départ
int y = 0; // y = 0 initialisation et représente la coordonnée y de la case de départ
int regardeFace = 0; // Regarde à : AVANT = 0, GAUCHE = -1, DERRIÈRE = 2 ou -2, DROITE = 1
int last_move = 0; // est = 1 si x==0 et que le dernier movement est setorientation(1)
float KP1 = 0.0015;





void AjusterVitesse(uint32_t difference, uint32_t droit, uint32_t gauche)
{
    if (droit >= gauche)
    {
      MOTOR_SetSpeed(RIGHT, vitesse - (KP1 * difference));
      MOTOR_SetSpeed(LEFT, vitesse + (KP1 * difference));
    }
    else
    {
      MOTOR_SetSpeed(RIGHT, vitesse + (KP1 * difference));
      MOTOR_SetSpeed(LEFT, vitesse - (KP1 * difference));    
    }
}
// Fonction qui calcule le PID de la différence de pulses des moteurs pour corriger leur vitesse
void PID(uint32_t droit, uint32_t gauche)
{
  uint32_t diff;
  diff = 0;
  Serial.println(droit);
  //Serial.println(gauche);
  if (gauche > droit)
  {
    diff = gauche - droit;
    diff = diff / 2;
  }
  else
  {
    diff = droit - gauche;
    diff = diff / 2;
  }
  //Serial.println(diff);
  AjusterVitesse(diff, droit, gauche);
}
void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}
// Pour RESET les encodeurs et les variables qui stockent les valeurs lues des encodeurs
void resetEncodeurs()
{
  ENCODER_Reset(RIGHT);
  ENCODER_Reset(LEFT);
  pulses_droit = 0;
  pulses_gauche = 0;
}
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};
void pause()
{
  arret();
  delay(100);
}
void avance(){
  resetEncodeurs();
  while (pulses_droit < 100)
  {
    MOTOR_SetSpeed(RIGHT,0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
    delay(50);
  }
  resetEncodeurs();
  while (pulses_droit < 5600) // 6300 représente le nombre de pulses avant que la fonction avancer arrête
  {
    MOTOR_SetSpeed(RIGHT,vitesse);
    MOTOR_SetSpeed(LEFT, vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
    pulses_gauche = ENCODER_Read(LEFT);
    PID(pulses_droit, pulses_gauche);
    delay(50);
  }
  resetEncodeurs();
  while (pulses_droit < 300)
  {
    MOTOR_SetSpeed(RIGHT,0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
  }
  pause();
};
void avance_retour () {
  resetEncodeurs();
  while (pulses_droit < 100)
  {
    MOTOR_SetSpeed(RIGHT,0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
    delay(50);
  }
  resetEncodeurs();
  while (pulses_droit < 66000) // 6300 représente le nombre de pulses avant que la fonction avancer arrête
  {
    MOTOR_SetSpeed(RIGHT,vitesse);
    MOTOR_SetSpeed(LEFT, vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
    pulses_gauche = ENCODER_Read(LEFT);
    PID(pulses_droit, pulses_gauche);
    delay(50);
  }
  resetEncodeurs();
  while (pulses_droit < 300)
  {
    MOTOR_SetSpeed(RIGHT,0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
  }
  pause();
};
void recule(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -vitesse);
};
void tourneDroit(){
  resetEncodeurs();
  while (pulses_gauche < 1930) // 1800 pulses = 950ms représente le nombre de pulses avant que la fonction tourneDroit arrête
  {
    MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_gauche = ENCODER_Read(LEFT);
  }
  pause();
};
void tourneGauche(){
  resetEncodeurs();
  while (pulses_droit < 1850) // 1800 pulses = 900ms représente le nombre de pulses avant que la fonction tourneGauche arrête
  {
    MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
    MOTOR_SetSpeed(LEFT, -0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
  }
  pause();
};
// Permet de réajuster la direction de face du robot en le tournant (physiquement)
void setOrientation(int direction)
{
  if (direction == 0)
  {
    // Scénarios possibles :
    if (regardeFace == -1)
    {
      tourneDroit();
    }
    else if (regardeFace == 1)
    {
      tourneGauche();
    }
    else if ((regardeFace == 2) || (regardeFace == -2))
    {
      tourneDroit();
      tourneDroit();
    }
    // À la fin il faut remettre la variable regardeFace à la direction souhaitée
    regardeFace = 0;
  }
  else if (direction == -1)
  {
    if (regardeFace == 0)
    {
      tourneGauche();
    }
    else if (regardeFace == 1)
    {
      tourneGauche();
      tourneGauche();
    }
    else if ((regardeFace == 2) || (regardeFace == -2))
    {
      tourneDroit();
    }
    regardeFace = -1;
  }
  else if (direction == 1)
  {
    if (regardeFace == 0)
    {
      tourneDroit();
    }
    else if (regardeFace == -1)
    {
      tourneDroit();
      tourneDroit();
    }
    else if ((regardeFace == 2) || (regardeFace == -2))
    {
      tourneGauche();
    }
    regardeFace = 1;
  }
  // À MODIFIER SI JAMAIS ON VEUT QUE LE ROBOT FAIT FACE EN ARRIÈRE
  //else if (direction == 2)
}