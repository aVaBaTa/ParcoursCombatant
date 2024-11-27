/*
  ID-12.ino
*/
#include "settings.h"


void WriteInputScreen(){
  int PinOutputScreen = A7;
  analogWrite(PinOutputScreen, 255);
}

int i, incoming=0;
char id_tag[20];
String id_carte = "";

void identificationCarte(String id_carte){
          
          int numCarte = 0, numChambre = 0;
          String texteCarte = "Paiement avec la carte ";
          String texteChambre = "Clé de la chambre ";

          //Carte de paiement
          if (id_carte == "1600CA59E8"){ numCarte = 1; }
          if (id_carte == "08003C1409"){ numCarte = 2; }
          if (id_carte == "16000AE6C7"){ numCarte = 3; }
          if (id_carte == "0F027D6050"){ numCarte = 4; } 
         
          //Clé de chambre
          if (id_carte == "48007593EF"){ numChambre = 1; } 
          if (id_carte == "48007378FF"){ numChambre = 2; } 
          if (id_carte == "48007479D2"){ numChambre = 3; }
          if (id_carte == "00008926A7"){ numChambre = 4; }  
          
          //Texte de sortie qu'on pourra envoyer au LCD
          if(numCarte > 0){
            texteCarte = texteCarte + numCarte;
            Serial.println(texteCarte);
          }

          if(numChambre > 0){
            texteChambre = texteChambre + numChambre;
            Serial.println(texteChambre);
          }

          // Son au contact
          AX_BuzzerON();
          delay(500);
          AX_BuzzerOFF();
}


int s = 0;

void setup()
{
  BoardInit();
  Serial2.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  beep(3);
}

void loop()
{
  if(ROBUS_IsBumper(2)){ //Rouge
    beep(1);
  }
  if(ROBUS_IsBumper(3)){ //vert
    beep(1);
    avance(3000);
    tourneDroit(3000);
    tourneGauche(3000);
  }


  while (1)
  {  
    if( Serial2.available() > 0)
    {
      int crecu = Serial2.read();     // lit le ID-12
      switch(crecu)
      {
        case 0x02:
          // START OF TRANSMIT
          i=0;
          incoming=1;
          break;
        case 0x03:
          // END OF TRANSMIT
          incoming=0;
          id_carte = "";
          for( i=0 ; i<10 ; i++ ){
            id_carte = id_carte + String(id_tag[i]);
          }
          identificationCarte(id_carte);
        break;
        default:
          if( incoming )
            id_tag[i++] = crecu;
          break;
      }
    }
  }

}

/* fin du fichier */