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
int gaucheIRPin=A0;
int droiteIRPin=A1;
int valeurGaucheIR;
int valeurDroiteIR;
int seuilProximiteObjet = 200; //exemple pour environ 20 cm ** distance minimale du detecteur IR
int DistanceObjet;
String DirectionObjet;


void AllerVersObjet(String Direction, int Distance){// peut rajouter confirmation

  if(Direction == "Droite"){
    /*Tourne a 90
    Avance de la Distance*/
    Serial.print("Tourne a Droite\n");
    Serial.print("avance de ");
    Serial.print(Distance);
  }
  else if(Direction == "Gauche"){

    Serial.print("Tourne a Gauche");
    Serial.print("avance de ");
    Serial.print(Direction);
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
