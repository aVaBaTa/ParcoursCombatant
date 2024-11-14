// UTFT_Demo_480x320
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution
// of 480x320 pixels.
//
// This program requires the UTFT library.
//

// ################################################
//  GLUE class that implements the UTFT API
//  replace UTFT include and constructor statements
//  remove UTFT font declaration e.g. SmallFont
// ################################################

#include <SPFD5408_TouchScreen.h>

#include <UTFTGLUE.h>                   //use GLUE class and constructor
UTFTGLUE myGLCD(0, A2, A1, A3, A4, A0); // all dummy args


TouchScreen ts  = TouchScreen( 9, 8, A3, A2);
// Declare which fonts we will be using
// extern uint8_t SmallFont[];      //GLUE defines as GFXFont ref

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Arduino Uno / 2009:
// -------------------
// Standard Arduino Uno/2009 shield            : <display model>,A5,A4,A3,A2
// DisplayModule Arduino Uno TFT shield        : <display model>,A5,A4,A3,A2
//
// Arduino Mega:
// -------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Mega      : <display model>,38,39,40,41
//
// Remember to change the model parameter to suit your display module!
// UTFT myGLCD(CTE32HR,38,39,40,41);

void parameter()
{
}
int DisplaySizeX = myGLCD.getDisplayXSize();
int FirstDisplay = 0;

void setup()
{
    randomSeed(analogRead(0));

    // Setup the LCD
    myGLCD.InitLCD();
    //myGLCD.setFont(SmallFont);
    parameter();
}

void loop()
{
    int buf[478];
    int x, x2;
    int y, y2;
    int r;

    Serial.println(ts.isTouching());

    // Clear the screen and draw the frame
     //myGLCD.clrScr();
    if (FirstDisplay == 0){
        myGLCD.clrScr();
        myGLCD.setColor(255, 0, 0);
        myGLCD.setBackColor(150,150,150);
        //myGLCD.setFont(BigFont);
        myGLCD.setTextSize(5);
        myGLCD.print("* Bienvenue *", CENTER, 100);
        
        
       // myGLCD.setFont(SmallFont);
       myGLCD.setTextSize(1);
        myGLCD.print("* Appuyer sur l ecran pour commencer *", CENTER, 170);
        FirstDisplay = 1;
    }

    
    delay(5000);



    if (FirstDisplay == 1){
        myGLCD.clrScr();
        myGLCD.setColor(255, 0, 0);
        myGLCD.setBackColor(150,150,150);
        //myGLCD.setFont(BigFont);
        myGLCD.setTextSize(5);
        myGLCD.print("* Bienvenue *", CENTER, 100);
        
        
       // myGLCD.setFont(SmallFont);
       myGLCD.setTextSize(1);
        myGLCD.print("* ----- *", CENTER, 170);
        FirstDisplay = 2;
    }

    /*myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(0, 0, 479, 13);

    myGLCD.setColor(64, 64, 64);
    myGLCD.fillRect(0, 306, 479, 319);

    myGLCD.setColor(255, 255, 255);
    myGLCD.setBackColor(255, 0, 0);

    myGLCD.print("* Universal Color TFT Display Library *", CENTER, 1);
    myGLCD.setBackColor(64, 64, 64);*/

    // myGLCD.setColor(255,255,0);
    // myGLCD.print("<http://www.RinkyDinkElectronics.com/>", CENTER, 307);
}