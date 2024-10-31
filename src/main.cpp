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
bool verification2 = true;

/* Gestion des cerveaux moteurs
 ajuster la tension VServo qui les alimentes à 7.2V max)*/

void initialiserServoMoteur()
{
	SERVO_Enable(LEFT);
	SERVO_Enable(RIGHT);
	SERVO_SetAngle(LEFT, 180);
	SERVO_SetAngle(RIGHT, 0);
}

void desactiverServoMoteur()
{
	SERVO_Disable(LEFT);
	SERVO_Disable(RIGHT);
}

void InitialiserCapteurCouleurs()
{
	MOTOR_SetSpeed(RIGHT, 0);
	MOTOR_SetSpeed(LEFT, 0);
	Serial.println("Color View Test!");

	if (tcs.begin())
	{
		Serial.println("Found sensor");
	}
	else
	{
		Serial.println("No TCS34725 found ... check your connections");
		while (1)
			; // halt!
	}

	// use these three pins to drive an LED
	pinMode(redpin, OUTPUT);
	pinMode(greenpin, OUTPUT);
	pinMode(bluepin, OUTPUT);

	// thanks PhilB for this gamma table!
	// it helps convert RGB colors to what humans see
	for (int i = 0; i < 256; i++)
	{
		float x = i;
		x /= 255;
		x = pow(x, 2.5);
		x *= 255;

		if (commonAnode)
		{
			gammatable[i] = 255 - x;
		}
		else
		{
			gammatable[i] = x;
		}
		// Serial.println(gammatable[i]);
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
	for (int i = 0; i < 3; i++)
	{
		MOTOR_SetSpeed(RIGHT, 0);
		MOTOR_SetSpeed(LEFT, 0);
		uint16_t clear, red, green, blue;

		tcs.setInterrupt(false); // turn on LED

		//delay(60); // takes 50ms to read

		tcs.getRawData(&red, &green, &blue, &clear);

		tcs.setInterrupt(true); // turn off LED

		/*Serial.print("C:\t"); Serial.print(clear);
		Serial.print("\tR:\t"); Serial.print(red);
		Serial.print("\tG:\t"); Serial.print(green);
		Serial.print("\tB:\t"); Serial.print(blue);*/

		// Figure out some basic hex code for visualization
		uint32_t sum = clear;
		float r, g, b;
		r = red;
		r /= sum;
		g = green;
		g /= sum;
		b = blue;
		b /= sum;
		r *= 256;
		g *= 256;
		b *= 256;

		sommeR += r;
		sommeG += g;
		sommeB += b;

		// analogWrite(redpin, gammatable[(int)r]);
		// analogWrite(greenpin, gammatable[(int)g]);
		// analogWrite(bluepin, gammatable[(int)b]);
	}
	moyenneR = sommeR / 3.0;
	moyenneG = sommeG / 3.0;
	moyenneB = sommeB / 3.0;
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

void ramasserObjet()
{
	int angle = 1;
	int anglegauche = 180; 

	while (angle<=110)
	{
		anglegauche -= 1;
		SERVO_SetAngle(LEFT, anglegauche);
		SERVO_SetAngle(RIGHT, angle);
		angle += 1; 
		delay(10);
	}
	

}

// a modifier
void lacherObjet()
{
	SERVO_SetAngle(LEFT, 180);
	SERVO_SetAngle(RIGHT, 0);
}

void setup()
{

	Serial.begin(9600);
	BoardInit();
	initialiserServoMoteur();
	InitialiserCapteurCouleurs();
	pinMode(vertpin, INPUT);
	pinMode(rougepin, INPUT);
	delay(100);
	beep(3);
}
// Peut etre pas besoin de cette Fonction || Ajouter  la fonction Sifflet
int StartSequence()
{
	int start = -1;
	if (DetecterCouleur() == 0) // rouge
	{
		start = 0;
	}
	else if (DetecterCouleur() == 1) // Jaune
	{
		start = 1;
	}
	else if (DetecterCouleur() == 2) // Vert
	{
		start = 2;
	}
	else if (DetecterCouleur() == 3) // Bleu
	{
		start = 3;
	}
	return start;
}

int CycleBoucle = -1; // changer le nom pls
// pour Etape 5 constante de detection des capteurs Milieu CycleBoucle 1
int SuiveurLigneCapteurMilieu = 0;
// pour Etape 5 constante de detection des capteurs Milieu CycleBoucle 1
int SuiveurLigneCapteurGauche = 0;
// pour Etape 5 constante de detection des capteurs Milieu CycleBoucle 1
int SuiveurLigneCapteurDroit = 0;

int CouleurDepart;
int CompteurSequence = 0;

void loop()
{
	
	
	
	// Faire la Fonction de depart
	if (etat == -1)
	{
		etat = StartSequence();
		CycleBoucle = 0;
	}
	else if (etat != -1)
	{
		while (CompteurSequence < 4)
		{
			CouleurDepart = DetecterCouleur();
			//avance(2000); // A determiner												// A ajuster pour atteindre la ligne du milieu
			while (verification == 0) // a modifier
			{
				Serial.println("Alright");
				suivreLigne();
			}
			arret();
			verification = 0;
			avance(1500);// Le robot avance un peu pour se recalibrer

			if (CouleurDepart == 0) // Rouge
			{
				beep(1);
			}
			else if (CouleurDepart == 1) // Jaune
			{
				tourneDroit(2058); // tourne 90 degre
			}
			else if (CouleurDepart == 2) // Vert
			{
				tourneGauche(2041); // tourne 90 degre
			}
			else if (CouleurDepart == 3) // Bleu
			{
				tourneDroit(4117); // tourne 180 degree
			}

			// Boucle principale
			tourneDroit(180); // 180 degree
			if (CompteurSequence == 0)// sequence Rouge
			{
				tourneDroit(4117);
			}
			else if (CompteurSequence == 1)// Sequence Jaune
			{
				tourneDroit(2058);
			}
			else if (CompteurSequence == 2)// Sequence Vert
			{
				tourneGauche(4083);
			}
			else if (CompteurSequence == 3)// Sequence Bleu
			{
				tourneGauche(2041);
			}
			
			// || une fonction devrait remplacer sa
			while (IsObjectDetected() == 0) // et pas fait de 90 degree
			{
				if (CompteurSequence == 0)
				{
					tourneGaucheInfini(0.5); // Peut-être ajouter coefficient de vitesse
				}
				else if (CompteurSequence == 1)
				{
					tourneGaucheInfini(0.5);
				}
				else if (CompteurSequence == 2)
				{
					tourneDroitInfini(0.5); // Peut-être ajouter coefficient de vitesse
				}
				else if (CompteurSequence == 3)
				{
					tourneDroitInfini(0.5);
				}

			}
			arret();
			beep(3);
			tourneGauche(340);
			avanceLentLent(2000);


			//AllerVersObjet(CapteurIR_Distance_Obj());


													 // if   (Fonction de Detection d objet proche)
			//ramasserObjet(); // Peut etre ajouter un delay dans la fonction parce que le robot va fermer ses pinces vrm vite

			
			ligneMilieu = 0;
			// si CompteurSequence == 2 ou 3 les Suiveur lignes de gauches Pas sur a verifier*********
			while(ligneMilieu < seuilSuiveurLigne) // les deux capteurs milieu et droits
      		{
        	ligneMilieu = analogRead(CaptMid);
        	avanceLent();
			}
			SuiveurLigneCapteurDroit = 0;
			SuiveurLigneCapteurGauche = 0;
			SuiveurLigneCapteurMilieu = 0;
			arret();
			delay(100);
			ramasserObjet();
			avance(200);
			ligneMilieu = 0;
			ligneDroite = 0;
			
			while (ligneDroite < seuilSuiveurLigne)
			{
				//ligneMilieu = analogRead(CaptMid);
				ligneDroite = analogRead(CaptRight);
				tourneDroitInfini(1);
			}

//			while (ligneMilieu < seuilSuiveurLigne)
//			{
//				ligneMilieu = analogRead(CaptMid);
//				tourneDroitInfini(1);
//			}
//
			int i = 0;
			ligneMilieu = 0;
			ligneDroite = 0;
			ligneGauche = 0;
			while (verification == 0)
			{
				suivreLigne();
			}
			verification = 0;

			arret();
			delay(200);
			// peut etre tourner a gauche ou a droite selon normalement c gauche dans tout les cas

			// TRES IMPORTANT ON ARRIVE TOUJOURS DU MEME SENS DONC MEME SEQUENCE POUR LA FIN
			// on avance un peu on tourne a gauche vers le but
			// on avance encore un peu
			// on depose l objet
			// on recule et 180 degree
			//avance(2000);
			//arret();
			//tourneGauche(1020);
			//avance(1000);

			lacherObjet();
			recule(4000);
			tourneDroit(4117); // fait un 180

			CompteurSequence += 1;
		}
	}
		
}
