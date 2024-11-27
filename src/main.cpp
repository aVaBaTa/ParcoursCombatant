#include <Arduino.h>
#include <LibRobus.h>
#include <UTFTGLUE.h>                   //use GLUE class and constructor
UTFTGLUE myGLCD(0, A2, A1, A3, A4, A0); // all dummy args





// Include the required Wire library for I2C<br>#include <Wire.h>
int LED = 13;
int Input = 0;
void receiveEvent(int bytes) {
  Input = Wire.read();    // read one character from the I2C
}






int DisplaySizeX = myGLCD.getDisplayXSize();
int DisplayState = 0;
int ChangeState = 0;





int test;
int isTesting = true;






void setup()
{

    Serial.begin(9600);
    // Setup the LCD
    myGLCD.InitLCD();

  pinMode (LED, OUTPUT);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);

}

void loop()
{

	// a effacer si sa marche sans
    int buf[478];
    int x, x2;
    int y, y2;
    int r;


	if (isTesting == true){
		test++;
		if( test > 21) {
			test = 0;
		}
		DisplayState = test;

		delay(5000);
	}




    if (Input > 0){
      ChangeState = 1;
      DisplayState = Input;


      Input = 0;
    }
	/*if (Input == 10){
		DisplayState = 0;
	  }
      Input = 0;*/

    if (ChangeState == 1){
        myGLCD.clrScr();
        ChangeState = 0;
    }



    if (DisplayState == 0)
    {
        //myGLCD.clrScr();
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(5);
        myGLCD.print("* Bienvenue *", CENTER, 100);
        myGLCD.setTextSize(2);
        myGLCD.print("* Appuyer sur le bouton pour commencer *", CENTER, 170);
		// Va a DisplayState == 1
        DisplayState = -1;
    }


    // Ecran d option
    if (DisplayState == 1)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Choisir une option *", CENTER, 25);
        myGLCD.setTextSize(2);
		// Va a DisplayState == 2
        myGLCD.print("		Option 1 -> Reserver une chambre", LEFT, 100);
		// Va a DisplayState ==
        myGLCD.print("		Option 2 -> Recuperer ses clees", LEFT, 120);
		// Va a DisplayState ==
        myGLCD.print("		Option 3 -> Livraison de colis", LEFT, 140);
        DisplayState = -1;
    }

	// Réserver la chambre --------- Detection RFID
    if (DisplayState == 2)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Reserver une chambre *", CENTER, 25);
		 myGLCD.setTextSize(2);
        myGLCD.print("* Veuillez scanner votre *", CENTER, 50);
		myGLCD.print(" carte de paiement ", CENTER, 75);
        DisplayState = -1;
    }

	// VOICI VOTRE CLEE DE CHAMBRE
    if (DisplayState == 3)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Voici votre clee *", CENTER, 25);
		myGLCD.print(" de chambre ", CENTER, 50);
		myGLCD.setTextSize(2);
        myGLCD.print("* Appuyer sur le bouton vert *", CENTER, 75);
		myGLCD.print(" apres avoir recuperer votre clee ", CENTER, 90);
		// garde la boite
        DisplayState = -1;
    }

	// RESERVER CHAMBRE -- DEPOSER LES CLEES DE VOITURES
    if (DisplayState == 4)
    {
     myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Reserver une chambre *", CENTER, 25);
		myGLCD.setTextSize(2);
        myGLCD.print("*Voulez Deposer vos clees de voitures?*", CENTER, 45);
        myGLCD.setTextSize(2);
		// VA A 5
        myGLCD.print("		Option 1 -> Oui", LEFT, 100);
		// VA A MERCI BONNE JOURNEE
        myGLCD.print("		Option 2 -> Non", LEFT, 120);
		// va reporter la boite a sa place d 'origine
        DisplayState = -1;
    }

	// RESERVER CHAMBRE -- DEPOSER LES CLEES DE VOITURES -- OUI
    if (DisplayState == 5)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Deposer vos clees *", CENTER, 25);
		myGLCD.print(" de voitures ", CENTER, 50);
		myGLCD.setTextSize(2);
		myGLCD.print("* Appuyer sur le bouton vert *", CENTER, 75);
		// va porter la boite a l emplacement pour les clees
        DisplayState = -1;
    }








	// si la clé n a pas été selectionné ca va chercher la clé de chambre
	// si la clé a été selectionné ca va porter la clé et va chercher la clé de chambre
	// voici votre clé de chambre
	if (DisplayState == 5)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Voici votre cle *", CENTER, 100);
		myGLCD.print("  de chambre  ", CENTER, 120);

        DisplayState = -1;
    }










	// Récupérer ses clés
	if (DisplayState == 6)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Recuperer ses cles *", CENTER, 25);
        //myGLCD.print("* Voulez Deposer vos clees de voitures? *", CENTER, 25);
        myGLCD.setTextSize(3);
        myGLCD.print("Scanner votre carte", CENTER, 100);
        DisplayState = -1;

		// retourne l etat 0 dans le main arduino
    }




	// livraison de colis
	if (DisplayState == 7)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Livraison de colis *", CENTER, 25);
        myGLCD.print("* Deposer le colis *", CENTER, 50);
        myGLCD.setTextSize(2);
        myGLCD.print("		Option 1 -> Chambre 1", LEFT, 100);
        myGLCD.print("		Option 2 -> Chambre 2", LEFT, 120);
		myGLCD.print("		Option 3 -> Chambre 3", LEFT, 140);
		myGLCD.print("		Option 4 -> Chambre 4", LEFT, 160);
        DisplayState = -1;
    }















	// Merci
	if (DisplayState == 20)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Merci Bonne Journee *", CENTER, 100);

        DisplayState = -1;
    }
	// merci veuiller patienter
	if (DisplayState == 21)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Merci veuillez patienter *", CENTER, 100);

        DisplayState = -1;
    }






}
