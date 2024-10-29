#include <Arduino.h>
#include <LibRobUS.h>
#include "fonction_deplacement.h"

// Pour le sifflet
int Pin5KHZ = A10;
int PinAmbiant = A11;
int INTENSITE_5KHZ;
int SonAmbiant;
float sifflet;
float ambiant;
int start = 0;
bool detecteObjet = 0;

//lecture des valeurs analogiques pour le capteur IR
int capteurIRPin=A0;
int valeurIR;


void detecteurInfraRouge()
{
  valeurIR=analogRead(capteurIRPin);
  Serial.println(valeurIR);
  delay(500);
}

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


//Détecteurs de proximité
// a revoir
int ConversionDistancePulse = 0; // mettre une constante
int gaucheIRPin=A0;
int valeurGaucheIR;



int MainCapteurIRPin=A1;
int ValeurMainCapteurIR;

//exemple pour environ 20 cm ** distance minimale du detecteur IR
int seuilProximiteObjet = 200; 

int ValeurGlobalDistanceObjet;



// de Base peut etre modifier pour ajuster avec le capteur ***
void AllerVersObjet(int DistanceObjet)
{
  int LocalDistance = 0;
  
  LocalDistance =  ConversionDistancePulse * DistanceObjet; // Conversion de Distance en Pulse soit ici ou Dans la Fonction CapteurIR_Distance_Obj

 avance(LocalDistance);
}

// 
 int IsObjectDetected()// a modifie la sortie pour etre en cm pour calculer la distance
 {
  int result = 0;
  ValeurMainCapteurIR = analogRead(MainCapteurIRPin);
  if(ValeurMainCapteurIR >= seuilProximiteObjet)
  {
    result = 1;// a modifier
  }
  else
  {
    result = 0;
  }
  return result;
 }



// On pourrait Faire une Moyenne de Plusieurs Lectures
 int CapteurIR_Distance_Obj()
 {
  ValeurMainCapteurIR = analogRead(MainCapteurIRPin);

  // Transformer en pulse ici ou dans la fonction AllersVersObjets
  return ValeurMainCapteurIR;
 }


/*
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
 }*/


// Reviser le Code
/*
int Detecteur_IR_Objet(){
  int ObjectDetection;
  int Distance_Obj;

  if(IsObjectDetected() == 1){
    ObjectDetection = 1; 
    Distance_Obj = CapteurIR_Distance_Obj();
  }
  else {ObjectDetection = 0; Distance_Obj = -1;}
  return ObjectDetection, Distance_Obj;
}*/
