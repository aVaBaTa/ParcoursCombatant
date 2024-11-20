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
int DisplayState = 1;
int ChangeState = 0;












void setup()
{
    
    Serial.begin(9600);
    // Setup the LCD
    myGLCD.InitLCD();
    // myGLCD.setFont(SmallFont);

  pinMode (LED, OUTPUT);
  // Start the I2C Bus as Slave on address 9
  Wire.begin(9); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
    
}

void loop()
{
    int buf[478];
    int x, x2;
    int y, y2;
    int r;

    // Clear the screen and draw the frame
    // myGLCD.clrScr();
  
    if (Input > 0){
      ChangeState = 1;
      DisplayState = Input;
      Input = 0;
    }

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
        myGLCD.print("Option 1 -> Reserver une chambre", LEFT, 100);
        myGLCD.print("Option 2 -> Recuperer ses clees", LEFT, 120);
        myGLCD.print("Option 3 -> Livraison de colis", LEFT, 140);
        DisplayState = -1;
    }


    
    // Ecran Pour Reserver une chambre
    /*if (DisplayState == 2)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Reserver une chambre *", CENTER, 25);
        myGLCD.print("* Voulez Deposer vos clees de voitures? *", CENTER, 45);
        myGLCD.setTextSize(2);
        myGLCD.print("Option 1 -> Oui", LEFT, 100);
        myGLCD.print("Option 2 -> Non", LEFT, 120);
        DisplayState = -1;
    }




    /// Verrifier l' se qui est ecrit si c est pertinent
        if (DisplayState == 3)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Laisser nous le soins d'entreposer vos clees *", CENTER, 25);
        //myGLCD.print("* Voulez Deposer vos clees de voitures? *", CENTER, 25);
        myGLCD.setTextSize(2);
        myGLCD.print("Option 1 -> Oui", LEFT, 100);
        myGLCD.print("Option 2 -> Non", LEFT, 120);
        DisplayState = -1;
    }*/




    // les clees ne sont pas laisser
    if (DisplayState == 4)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Laisser nous le soins d'entreposer vos clees *", CENTER, 25);
        //myGLCD.print("* Voulez Deposer vos clees de voitures? *", CENTER, 25);
        myGLCD.setTextSize(2);
        myGLCD.print("Option 1 -> Oui", LEFT, 100);
        myGLCD.print("Option 2 -> Non", LEFT, 120);
        DisplayState = -1;
    }



    
    if (DisplayState == 3)
    {
        myGLCD.fillScreen(TFT_WHITE);
        myGLCD.setColor(0, 0, 255);
        myGLCD.setBackColor(255, 255, 255);
        myGLCD.setTextSize(3);
        myGLCD.print("* Laisser nous le soins d'entreposer vos clees *", CENTER, 25);
        //myGLCD.print("* Voulez Deposer vos clees de voitures? *", CENTER, 25);
        myGLCD.setTextSize(2);
        myGLCD.print("Option 1 -> Oui", LEFT, 100);
        myGLCD.print("Option 2 -> Non", LEFT, 120);
        DisplayState = -1;
    }








     // Ecran Pour les colis
    if (DisplayState == -2)
    {
        myGLCD.setColor(255, 0, 0);
        myGLCD.setBackColor(150, 150, 150);
        myGLCD.setTextSize(5);
        myGLCD.print("* Bienvenue *", CENTER, 100);
        myGLCD.setTextSize(1);
        myGLCD.print("* ----- *", CENTER, 170);
        DisplayState = -1;
    }


  
}