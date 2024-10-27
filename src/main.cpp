#include "settings.h"

// Pick analog outputs, for the UNO these three work well
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
#define redpin 3
#define greenpin 5
#define bluepin 6
// for a common anode LED, connect the common pin to +5V
// for common cathode, connect the common to ground
 
// set to false if using a common cathode LED
#define commonAnode true
 
// our RGB -> eye-recognized gamma color
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int couleurBut = 0;

/* Gestion des cerveaux moteurs
 ajuster la tension VServo qui les alimentes à 7.2V max)*/
 
void initialiserServoMoteur(){
  SERVO_Enable(LEFT);
  SERVO_Enable(RIGHT);
  SERVO_SetAngle(LEFT,180);
  SERVO_SetAngle(RIGHT,0);
}
 
void desactiverServoMoteur(){
  SERVO_Disable(LEFT);
  SERVO_Disable(RIGHT);
}

void InitialiserCapteurCouleurs()
{
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
  Serial.println("Color View Test!");
 
  if (tcs.begin()) {
      Serial.println("Found sensor");
  } else {
      Serial.println("No TCS34725 found ... check your connections");
      while (1); // halt!
  }
 
  // use these three pins to drive an LED
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);
 
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i = 0; i < 256; i++) {
      float x = i;
      x /= 255;
      x = pow(x, 2.5);
      x *= 255;
 
      if (commonAnode) {
          gammatable[i] = 255 - x;
      } else {
          gammatable[i] = x;
      }
      //Serial.println(gammatable[i]);
  }
}

// Détecte la couleur du but et retourne sa valeur
// 0 = Rouge, 1 = Jaune, Vert = 2, Bleu = 3
int DetecterCouleur()
{
  float sommeR = 0;
  float sommeG = 0;
  float sommeB = 0;
  float moyenneR, moyenneG, moyenneB;
  int couleurDetectee = 0;
  for (int i = 0; i < 5; i++)
  {
    MOTOR_SetSpeed(RIGHT, 0);
    MOTOR_SetSpeed(LEFT, 0);
    uint16_t clear, red, green, blue;
 
    tcs.setInterrupt(false);      // turn on LED
 
    delay(60);  // takes 50ms to read
 
    tcs.getRawData(&red, &green, &blue, &clear);
 
    tcs.setInterrupt(true);  // turn off LED
 
    /*Serial.print("C:\t"); Serial.print(clear);
    Serial.print("\tR:\t"); Serial.print(red);
    Serial.print("\tG:\t"); Serial.print(green);
    Serial.print("\tB:\t"); Serial.print(blue);*/
 
    // Figure out some basic hex code for visualization
    uint32_t sum = clear;
    float r, g, b;
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 256; g *= 256; b *= 256;
    /*Serial.print("C:\t"); Serial.print(sum);
    Serial.print("\tR:\t"); Serial.print(r);
    Serial.print("\tG:\t"); Serial.print(g);
    Serial.print("\tB:\t"); Serial.print(b);
    Serial.print("\t");
    Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
    Serial.println(); */
    sommeR += r;
    sommeG += g;
    sommeB += b;
    //Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
 
    analogWrite(redpin, gammatable[(int)r]);
    analogWrite(greenpin, gammatable[(int)g]);
    analogWrite(bluepin, gammatable[(int)b]);    
  }
  moyenneR = sommeR / 5.0;
  moyenneG = sommeG / 5.0;
  moyenneB = sommeB / 5.0;
  Serial.print("Valeur de R : ");
  Serial.println(moyenneR);
  Serial.print("Valeur de G : ");
  Serial.println(moyenneG);
  Serial.print("Valeur de B : ");
  Serial.println(moyenneB);

  if (moyenneR > 160 && moyenneG < 80 && moyenneB < 80)
  {
    couleurDetectee = 0;
  }
  else if (moyenneR > 70 && moyenneG > 70 && moyenneB < 50)
  {
    couleurDetectee = 1;
  }
  else if (moyenneR < 80 && moyenneG > 100 && moyenneB < 65)
  {
    couleurDetectee = 2;
  }
  else if (moyenneR < 77 && moyenneG > 75 && moyenneB > 83)
  {
    couleurDetectee = 3;
  }
  else
  {
    couleurDetectee = 4;
  }
  return couleurDetectee;
}


void ramasserObjet(){
  SERVO_SetAngle(LEFT,90);
  SERVO_SetAngle(RIGHT,90);
}
 

void setup(){
  Serial.begin(9600);
  BoardInit();
  initialiserServoMoteur();
  InitialiserCapteurCouleurs();
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(3);
}

void loop()
{
  bumperArr = ROBUS_IsBumper(3);
  if (bumperArr){
    if (etat == 0){
      //suivreLigne();
      etat = 1;
      beep(2);
    }
    else {
      beep(1);
      //desactiverServoMoteur();
      //initialiserServoMoteur();
      etat = 0;
    }
  }
  if (etat == 1)
  {
    arret();
    couleurBut = DetecterCouleur();
    arret();
    // Si la couleur du but initial détectée est ROUGE
    if (couleurBut == 0)
    {
      //Le robot avance de _ pulses pour atteindre la prochaine ligne noire.
      avance(1500);
      while (detecteObjet == false)
      {
        suivreLigne();
      }
      //Le robot doit détecter la ligne noire
      delay(10000);
    }   
  }
  arret();
  //Detecteur_IR_Objet();
  delay(1000);
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
 
 
 
