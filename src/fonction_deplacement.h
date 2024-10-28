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
float vitesse = 0.3;
int x = 0; // x = 0 initialisation et représente la coordonnée x de la case de départ
int y = 0; // y = 0 initialisation et représente la coordonnée y de la case de départ
int regardeFace = 0; // Regarde à : AVANT = 0, GAUCHE = -1, DERRIÈRE = 2 ou -2, DROITE = 1
int last_move = 0; // est = 1 si x==0 et que le dernier movement est setorientation(1)
float KP1 = 0.0015;

//Pour le suiveur de ligne
int CaptLeft = A10;
int CaptMid = A11;
int CaptRight = A12;
int ligneGauche;
int ligneMilieu;
int ligneDroite;
int lastCheck = 0;





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
void avance(int nombre_pulses){
  resetEncodeurs();
  while (pulses_droit < 100)
  {
    MOTOR_SetSpeed(RIGHT,0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
    delay(50);
  }
  resetEncodeurs();
  while (pulses_droit < nombre_pulses) // 6300 représente le nombre de pulses avant que la fonction avancer arrête
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
void tourneDroit(int nombre_pulses){
  resetEncodeurs();
  while (pulses_gauche < nombre_pulses) // 1800 pulses = 950ms représente le nombre de pulses avant que la fonction tourneDroit arrête
  {
    MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
    MOTOR_SetSpeed(LEFT, 0.5*vitesse);
    pulses_gauche = ENCODER_Read(LEFT);
  }
  pause();
};
void tourneGauche(int nombre_pulses){
  resetEncodeurs();
  while (pulses_droit < nombre_pulses) // 1800 pulses = 900ms représente le nombre de pulses avant que la fonction tourneGauche arrête
  {
    MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
    MOTOR_SetSpeed(LEFT, -0.5*vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
  }
  pause();
};

void tourneDroitInfini()
{
  resetEncodeurs();
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
}

void tourneGaucheInfini()
{
  resetEncodeurs();
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);  
}


//FONCTIONS POUR LE SUIVEUR DE LIGNE
void avanceLent(){
  resetEncodeurs();
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
  pulses_droit = ENCODER_Read(RIGHT);
};

void ajusterdroite(){
 
  resetEncodeurs();
  ligneMilieu = 0;
  while (ligneMilieu < 640)
  {
    ligneGauche = analogRead(CaptLeft);
    ligneMilieu = analogRead(CaptMid);
    ligneDroite = analogRead(CaptRight);
 
 
    MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
    MOTOR_SetSpeed(LEFT, 0.5 * vitesse);
    pulses_gauche = ENCODER_Read(LEFT);  
  }
  
}

 
void ajustergauche(){
 
  resetEncodeurs();
  ligneMilieu = 0;
  while (ligneMilieu < 640)
  {
    ligneGauche = analogRead(CaptLeft);
    ligneMilieu = analogRead(CaptMid);
    ligneDroite = analogRead(CaptRight);
 
    MOTOR_SetSpeed(RIGHT, 0.5 * vitesse);
    MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
    pulses_droit = ENCODER_Read(RIGHT);
  }
   
}
 
void suivreLigne()
{
 
  ligneGauche = analogRead(CaptLeft);
  ligneMilieu = analogRead(CaptMid);
  ligneDroite = analogRead(CaptRight);
  Serial.print("Valeur du capteur de droite : ");
  Serial.println(ligneDroite);
  Serial.print("Valeur du capteur de gauche : ");
  Serial.println(ligneGauche);
  Serial.print("Valeur du capteur du milieu : ");
  Serial.println(ligneMilieu);
 
  if ((ligneMilieu > 640) && (ligneDroite < 200) && (ligneGauche < 200)){
    avanceLent();
  }
 
  if ((ligneDroite > 650) &&  (ligneMilieu < 200) && (ligneGauche < 200)){
    ajusterdroite();
  }
 
  if ((ligneGauche > 610) && (ligneMilieu < 200) && (ligneDroite < 200)){
    ajustergauche();
  }
 
  if ((ligneMilieu < 200) && (ligneDroite < 200) && (ligneGauche < 200)){
    avanceLent();
  }
 
  if ((ligneMilieu > 640) && (ligneDroite > 650) && (ligneGauche > 610)){
    arret();
  }
 
  if ((ligneMilieu > 640) && (ligneDroite > 650) && (ligneGauche < 200)){
    ajusterdroite();
  }
 
  if ((ligneMilieu > 640) && (ligneDroite < 200) && (ligneGauche > 610)){
    ajustergauche();
  }
}
















