#include <Arduino.h>
#include <math.h>
#include <LibRobUS.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>


bool bumperArr;
int32_t difference_moteurs = 0;
uint32_t pulses_droit = 0;
uint32_t pulses_gauche = 0;
int vertpin = 48;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = -1; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.25;
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
  MOTOR_SetSpeed(RIGHT, 0.75 * vitesse);
  MOTOR_SetSpeed(LEFT, 0.75 * vitesse);
  pulses_droit = ENCODER_Read(RIGHT);
};

void ajusterdroite(){
 
  resetEncodeurs();
  ligneMilieu = 0;
  while (ligneMilieu < 150)
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
  while (ligneMilieu < 150)
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
 
  if ((ligneMilieu > 150) && (ligneDroite < 150) && (ligneGauche < 150)){
    avanceLent();
  }
 
  if ((ligneDroite > 150) &&  (ligneMilieu < 150) && (ligneGauche < 150)){
    ajusterdroite();
  }
 
  if ((ligneGauche > 150) && (ligneMilieu < 150) && (ligneDroite < 150)){
    ajustergauche();
  }
 
  if ((ligneMilieu < 150) && (ligneDroite < 150) && (ligneGauche < 150)){
    avanceLent();
  }
 
  if ((ligneMilieu > 150) && (ligneDroite > 150) && (ligneGauche > 150)){
    arret();
  }
 
  if ((ligneMilieu > 150) && (ligneDroite > 150) && (ligneGauche < 150)){
    ajusterdroite();
  }
 
  if ((ligneMilieu > 150) && (ligneDroite < 150) && (ligneGauche > 150)){
    ajustergauche();
  }
}

















// Pour le sifflet
int Pin5KHZ = A10;
int PinAmbiant = A11;
int INTENSITE_5KHZ;
int SonAmbiant;
float sifflet;
float ambiant;
int start = 0;
bool detecteObjet = 0;




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

//lecture des valeurs analogiques pour le capteur IR
int capteurIRPin=A0;
int valeurIR;
void detecteurInfraRouge()
{
  valeurIR=analogRead(capteurIRPin);
  Serial.println(valeurIR);
  delay(500);
}

//Détecteurs de proximité
int gaucheIRPin=A0;
int droiteIRPin=A1;
int valeurGaucheIR;
int valeurDroiteIR;
int seuilProximiteObjet = 200; //exemple pour environ 20 cm ** distance minimale du detecteur IR
int DistanceObjet;
String DirectionObjet;


void AllerVersObjet(String Direction, int Distance){

  if(Direction == "Droite"){
    /*Tourne a 90
    Avance de la Distance*/
    tourneDroit(90); // alerte
    //Serial.print("Tourne a Droite\n");
    avance(DistanceObjet);
    //Serial.print("avance de ");
    //Serial.print(Distance);
  }
  else if(Direction == "Gauche"){

    tourneGauche(90); // alerte
    //Serial.print("Tourne a Gauche");
    avance(DistanceObjet);
    //Serial.print("avance de ");
    //Serial.print(Direction);
  }
  else if(Direction != "Droite" || Direction != "Gauche")
  {
    Serial.print("\n AllerVersObjet : error ==> Direction est mal ecrit ||   Droite ou Gauche");
  }

}
 int Detecteur_IR_Distance()// a modifie la sortie pour etre en cm pour calculer la distance
 {
  int result;
  valeurDroiteIR = analogRead(droiteIRPin);
  valeurGaucheIR = analogRead(gaucheIRPin);

  if(valeurDroiteIR >= seuilProximiteObjet and valeurGaucheIR >= seuilProximiteObjet)
  {
    Serial.print("Les deux detecteurs sont active");
    result = -1;// a modifier
  }
  else if(valeurGaucheIR >= seuilProximiteObjet)
  {
    result = valeurGaucheIR;
  }
  else if(valeurDroiteIR >= seuilProximiteObjet)
  {
    result = valeurDroiteIR;
  }
  else if(seuilProximiteObjet > valeurDroiteIR and seuilProximiteObjet > valeurGaucheIR)
  {
    Serial.print("Aucun detecteur ne depasse le seuil");
    result = -1;
  }

  return result;
 }
 int isObjectOutOfReach()
 {
  /*int pos
  if(DistanceObjet >= pos){

  }
*/

  return 0;
 }
float Detecteur_IR_Distance_TestMoyenne()// a modifie la sortie pour etre en cm pour calculer la distance

 {
  float result;
  int TestValeurDroiteIR[10];
  int TestValeurGaucheIR[10];

  for(int i = 0; i < 10; i++){
    TestValeurDroiteIR[i] = analogRead(droiteIRPin);
    TestValeurGaucheIR[i] = analogRead(gaucheIRPin);
  }
  float moyenneDroiteIR = 0;
  float moyenneGaucheIR = 0;
  for(int i = 0; i < 10; i++)
  {
    moyenneDroiteIR += TestValeurDroiteIR[i];
    moyenneGaucheIR += TestValeurGaucheIR[i];
  }
  moyenneDroiteIR = moyenneDroiteIR / 10;
  moyenneGaucheIR = moyenneGaucheIR / 10;




  if(moyenneDroiteIR >= seuilProximiteObjet && moyenneGaucheIR >= seuilProximiteObjet)
  {
    Serial.print("Les deux detecteurs sont active");
    result = -1;// a modifier
  }
  else if(moyenneGaucheIR >= seuilProximiteObjet)
  {
    result = moyenneGaucheIR;
  }
  else if(moyenneDroiteIR >= seuilProximiteObjet)
  {
    result = moyenneDroiteIR;
  }
  else if(seuilProximiteObjet > moyenneDroiteIR and seuilProximiteObjet > moyenneGaucheIR)
  {
    Serial.print("Aucun detecteur ne depasse le seuil");
    result = -1;
  }

  return result;
 }
// Reviser le Code
int Detecteur_IR_Objet(){
  int result;

  if(Detecteur_IR_Distance() != -1){
    DistanceObjet = Detecteur_IR_Distance();
    if(valeurDroiteIR >= valeurGaucheIR){
      DirectionObjet = "Droite";
      result = 1;
    }
    else if(valeurDroiteIR < valeurGaucheIR){
      DirectionObjet = "Gauche";
      result = 2;
    }
    //if(DistanceObjet >= 0){} isObjectOutOfReach
    //AllerVersObjet(DirectionObjet, DistanceObjet);   en dehors ou en dedans de la fonction
  }
  else if(Detecteur_IR_Distance() == -1)
  {
    Serial.print("No object detected");
    result = -1;
  }

  return result;

}




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

    sommeR += r;
    sommeG += g;
    sommeB += b;
 
    //analogWrite(redpin, gammatable[(int)r]);
    //analogWrite(greenpin, gammatable[(int)g]);
    //analogWrite(bluepin, gammatable[(int)b]);    
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
//a modifier
void lacherObjet(){
  SERVO_SetAngle(LEFT, 180);
  SERVO_SetAngle(RIGHT, 0);
}
 




 
// Peut etre pas besoin de cette Fonction || Ajouter  la fonction Sifflet
int StartSequence()
{
int start = -1;
if(DetecterCouleur() == 0)// rouge
{
  start = 0;
}
else if(DetecterCouleur() == 1)// Jaune
{
  start = 1;
}
else if(DetecterCouleur() == 2)// Vert
{
  start = 2;
}
else if(DetecterCouleur() == 3)// Bleu
{
  start = 3;
}
return start;
}





// pour Etape 5 constante de detection des capteurs Milieu CycleBoucle 1
int SuiveurLigneCapteurMilieu = 0;
// pour Etape 5 constante de detection des capteurs Milieu CycleBoucle 1
int SuiveurLigneCapteurGauche = 0;
// pour Etape 5 constante de detection des capteurs Milieu CycleBoucle 1
int SuiveurLigneCapteurDroit = 0;

int CouleurDepart;
int CompteurSequence = 0;


