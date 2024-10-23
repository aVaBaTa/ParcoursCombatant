#include <Arduino.h>
#include <LibRobUS.h>
/*
Projet: main.cpp
Equipe: 09
Auteurs: Simon Gaudet, Xavier Tremblay, Vincent Dupéré
Description: Ce programme permet de faire circuler un robot dans un labyrinthe et trouver le bon chemin
Date: 10-08-2024
*/
#include <math.h>
/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/
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
// Pour le sifflet
int Pin5KHZ = A10;
int PinAmbiant = A11;
int INTENSITE_5KHZ;
int SonAmbiant;
float sifflet;
float ambiant;
float gammecouleurs[4][2][2] = { //estimation gamme de couleurs dans chaque case sous forme de liste (array)
    {{0.59910, 0.33132}, {0.62175, 0.33246}},  // Rouge (plus foncee, plus pale)
    {{0.26523, 0.42739}, {0.29899, 0.44315}},  // Vert
    {{0.41618, 0.47968}, {0.44801, 0.50511}},  // Jaune
    {{0.19647, 0.16201}, {0.20428, 0.16455}}  // Bleu
    //format: {{/*col1_x*/, /col1_y*/}, {/*col2_x*/, /*col2_y*/}} et prendre note qu<on a pris Yxy pour valeurs
};

/*
Vos propres fonctions sont creees ici
*/

 //Fonction qui converti RGB en chromaticity coordinates (x, y)

void rgbtoxy (int R,int G,int B){ 

  //valeur des chromaticity coordinates (x, y)
    float X = (-0.14282)*R + (1.54924)*G +(-0.95641)*B;
    float Y = (-0.32466)*R + (1.57837)*G +(-0.73191)*B;
    float Z = (-0.68202)*R + (0.77073)*G +(0.56332)*B;

    float colx =X/(X+Y+Z);
    float coly =Y/(X+Y+Z);

 // loop : essay de trouve un match entre la couleur detecte et celle dans la liste de couleurs
    for (int i = 0; i < 4; i++) {
        // Vérifier si colx est compris entre les valeurs x des teintes les plus foncées et les plus claires
        if (colx >= gammecouleurs[i][0][0] && colx <= gammecouleurs[i][1][0] &&
            coly >= gammecouleurs[i][0][1] && coly <= gammecouleurs[i][1][1]) {
            return i;  // Retourne la couleur qui match
        }
    }

    // Trouve pas de match
    return -1;
}
 
// Réajuster la vitesse des moteurs
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
// Ce qui fait démarrer le robot avec le son du sifflet
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
    beep(2);
    etat = 1;
  }
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
/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les variables globales
*/
void setup(){
  BoardInit();
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(3);
}
/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {
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