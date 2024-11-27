#include "settings.h"

int screen_output;
int screen_display = 0;
int bouton_selection = 1;
int Numcarte = -1;

int i, incoming = 0;
char id_tag[20];
char crecu;
String id_carte = "";

void identificationCarte(String IDCard)
{

	int numCarte = 0, numChambre = 0;
	String texteCarte = "Paiement avec la carte ";
	String texteChambre = "Clé de la chambre ";

	// Carte de paiement
	if (IDCard == "1600CA59E8")
	{
		numCarte = 1;
	}
	if (IDCard == "08003C1409")
	{
		numCarte = 2;
	}
	if (IDCard == "16000AE6C7")
	{
		numCarte = 3;
	}
	if (IDCard == "0F027D6050")
	{
		numCarte = 4;
	}

	// Clé de chambre
	if (IDCard == "48007593EF")
	{
		numChambre = 1;
	}
	if (IDCard == "48007378FF")
	{
		numChambre = 2;
	}
	if (IDCard == "48007479D2")
	{
		numChambre = 3;
	}
	if (IDCard == "00008926A7")
	{
		numChambre = 4;
	}

	// Texte de sortie qu'on pourra envoyer au LCD
	if (numCarte > 0)
	{
		texteCarte = texteCarte + numCarte;
		Serial.println(texteCarte);
	}

	if (numChambre > 0)
	{
		texteChambre = texteChambre + numChambre;
		Serial.println(texteChambre);
	}

	// Son au contact

	AX_BuzzerON();
	delay(500);
	AX_BuzzerOFF();
}

void setup()
{

	Wire.begin();
	BoardInit();

	Serial.begin(9600);
	Serial.print("RESET");
}

void loop()
{

	// event
	// move_state = 0;

	// MENU BIENVENUE
	if (screen_display == 0 || screen_display == 10)
	{
		if (ROBUS_IsBumper(3))
		{
			beep(3);
			screen_output = 1;
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;
			bouton_selection = 1;
			delay(250);
		}
	}

	if (screen_display == 1)
	{
		Serial2.begin(9600);
		while (screen_display == 1)
		{
			Serial.print("Screen : 1");
			if (Serial2.available() > 0)
			{
				Serial.print(Serial2.available());
				crecu = Serial2.read(); // lit le ID-12

				switch (crecu)
				{
				case 0x02:
					// START OF TRANSMIT
					i = 0;
					incoming = 1;
					break;
				case 0x03:
					// END OF TRANSMIT
					incoming = 0;
					id_carte = "";
					for (i = 0; i < 10; i++)
					{
						id_carte = id_carte + String(id_tag[i]);
					}
					identificationCarte(id_carte);
					break;
				default:
					if (incoming)
						id_tag[i++] = crecu;
					break;
				}
			}
			if (ROBUS_IsBumper(2)) // CHANGER SÉLECTION
			{
				if (bouton_selection < 3)
				{
					bouton_selection += 1;
				}
				else
				{
					bouton_selection = 1;
				}
				delay(250);
			}
			if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
			{

				if (bouton_selection == 1)
				{
					screen_output = 2;
				}
				if (bouton_selection == 2)
				{
					screen_output = 6;
				}
				if (bouton_selection == 3)
				{
					screen_output = 7;
				}

				// remet bouton a 1 lorsque confimer
				bouton_selection = 1;

				// ENVOYER L'ACTION AU LCD
				Wire.beginTransmission(9);
				Wire.write(screen_output);
				Wire.endTransmission();
				screen_display = screen_output;
				delay(250);
			}
		}
		Serial2.end();
	}

	if (screen_display == 2)
	{
		while (screen_display == 2)
		{
			Serial.print("Screen : 2");
			if (Serial2.available() > 0)
			{
				Serial.print(Serial2.available());
				crecu = Serial2.read(); // lit le ID-12

				switch (crecu)
				{
				case 0x02:
					// START OF TRANSMIT
					i = 0;
					incoming = 1;
					break;
				case 0x03:
					// END OF TRANSMIT
					incoming = 0;
					id_carte = "";
					for (i = 0; i < 10; i++)
					{
						id_carte = id_carte + String(id_tag[i]);
					}
					identificationCarte(id_carte);
					break;
				default:
					if (incoming)
						id_tag[i++] = crecu;
					break;
				}
			}
			if (ROBUS_IsBumper(2)) // CHANGER SÉLECTION
			{
				if (bouton_selection < 2)
				{
					bouton_selection += 1;
				}
				else
				{
					bouton_selection = 1;
				}
				delay(250);
			}
			if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
			{

				// Si Oui || == 1 Mets en memoire deposer les clees
				if (bouton_selection == 1)
				{
					screen_output = 3;
				}
				if (bouton_selection == 2)
				{
					screen_output = 4;
				}

				// remet bouton a 1 lorsque confimer
				bouton_selection = 1;

				// ENVOYER L'ACTION AU LCD
				Wire.beginTransmission(9);
				Wire.write(screen_output);
				Wire.endTransmission();
				screen_display = screen_output;
				delay(250);
			}
		}
	}


}
// MENU DE SELECTION
/*else if (screen_display == 1)
{
	//Serial.print("Salut");
	if (ROBUS_IsBumper(2)) // CHANGER SÉLECTION
	{
		if (bouton_selection < 3)
		{
			bouton_selection += 1;
		}
		else
		{
			bouton_selection = 1;
		}
		delay(250);
	}
	if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
	{

		if (bouton_selection == 1)
		{
			screen_output = 2;
		}
		if (bouton_selection == 2)
		{
			screen_output = 6;
		}
		if (bouton_selection == 3)
		{
			screen_output = 7;
		}

		// remet bouton a 1 lorsque confimer
		bouton_selection = 1;

		// ENVOYER L'ACTION AU LCD
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;
		delay(250);
	}
}
// DEMANDE SI DEPOSER LES CLEES ----> Inverser avec Les chambres
else if (screen_display == 2)
{
	if (ROBUS_IsBumper(2)) // CHANGER SÉLECTION
	{
		if (bouton_selection < 2)
		{
			bouton_selection += 1;
		}
		else
		{
			bouton_selection = 1;
		}
		delay(250);
	}
	if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
	{

		// Si Oui || == 1 Mets en memoire deposer les clees
		if (bouton_selection == 1)
		{
			// Mettre la Variable des clées a oui
			screen_output = 3;
		}
		if (bouton_selection == 2)
		{
			screen_output = 4;
		}

		// remet bouton a 1 lorsque confimer
		bouton_selection = 1;

		// ENVOYER L'ACTION AU LCD
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;
		delay(250);
	}
}
// Screen Deposer les clees Pour Reserver la Chambre
else if (screen_display == 3)
{

	if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
	{

		// remet bouton a 1 lorsque confimer
		bouton_selection = 1;

		// Renvoie a reserver la chambre
		screen_output = 4;
		// ENVOYER L'ACTION AU LCD
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;

		delay(250);
	}
}
// RESERVER LA CHAMBRE
else if (screen_display == 4)
{
	if (ROBUS_IsBumper(2)) // CHANGER SÉLECTION
	{
		if (bouton_selection < 3)
		{
			bouton_selection += 1;
		}
		else
		{
			bouton_selection = 1;
		}
		delay(250);
	}
	if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
	{
		if (bouton_selection == 1)
		{
			// METTRE LE CODE POUR ALLER A LA CHAMBRE 1
		}
		if (bouton_selection == 2)
		{
			// METTRE LE CODE POUR ALLER A LA CHAMBRE 2
		}
		if (bouton_selection == 3)
		{
			// METTRE LE CODE POUR ALLER A LA CHAMBRE 3
		}
		if (bouton_selection == 4)
		{
				// a enlever
		}

		// remet bouton a 1 lorsque confimer
		bouton_selection = 1;



				// 			A CHANGER
		// Merci
		screen_output = -2;

		// ENVOYER L'ACTION AU LCD
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;

		delay(250);
	}
}
// Remise de clees de chambres
else if (screen_display == 5)
{
	if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
	{

		// remet bouton a 1 lorsque confimer
		bouton_selection = 1;

		//****************************************
			// a changer pour les clees de chambres
		screen_output = 8;
		//****************************************

		// ENVOYER L'ACTION AU LCD
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;
		delay(5000);

		// remettre a 0 le programme
		screen_display = 0;
		screen_output = 0;
	}
}
// RECUPERER SES CLEES
else if (screen_display == 6)
{

	// LE CODE POUR RECUPERER SES CLEES
	// RFID
	// GARDER EN MEMOIRE LA PERSONNE DU RFID
	// AFFICHER MERCI BONNE JOURNEE
	// DEPLACEMENT SELON RFID

	screen_output = 5;

	Wire.beginTransmission(9);
	Wire.write(screen_output);
	Wire.endTransmission();
	screen_display = screen_output;
	delay(250);
}
// LIVRAISON DE COLIS
else if (screen_display == 7)
{

	// LE CODE POUR RECUPERER SES CLEES

	// LES RFID?

	if (ROBUS_IsBumper(2)) // CHANGER SÉLECTION
	{
		if (bouton_selection < 4)
		{
			bouton_selection += 1;
		}
		else
		{
			bouton_selection = 1;
		}
		delay(250);
	}

	// GARDER EN MEMOIRE LA PERSONNE DU RFID
	// AFFICHER MERCI BONNE JOURNEE
	// DEPLACEMENT SELON RFID

	if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
	{
		// AFFICHER MERCI

		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;
		delay(250);

		if (bouton_selection == 1)
		{
			// LE CODE DE LIVRAISON DE COLIS POUR LA CHAMBRE 1
		}
		if (bouton_selection == 2)
		{
			// LE CODE DE LIVRAISON DE COLIS POUR LA CHAMBRE 2
		}
		if (bouton_selection == 3)
		{
			// LE CODE DE LIVRAISON DE COLIS POUR LA CHAMBRE 3
		}
		if (bouton_selection == 4)
		{
			// LE CODE DE LIVRAISON DE COLIS POUR LA CHAMBRE 4
		}

		// MERCI BONNE JOURNEE

		// remet bouton a 1 lorsque confimer
		bouton_selection = 1;

		// LE CODE DE REMISE A 0 DE L'ECRAN
		// remet bouton a 1 lorsque confimer
		/* bouton_selection = 1;

			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;
			delay(250);

		*/

/*				delay(250);
			}
		}



		// MERCI BONNE JOURNEE
		else if (screen_display == 8)
		{
			if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION ou Delay a decider
			{

				// LE CODE DE REMISE A 0 DE L'ECRAN
				// remet bouton a 1 lorsque confimer
				bouton_selection = 1;

				// ENVOYER L'ACTION AU LCD
				Wire.beginTransmission(9);
				Wire.write(screen_output);
				Wire.endTransmission();
				screen_display = 10;
				delay(250);
			}
		}*/
//}

/*
	STATE 0: stand by (LCD -> Next state)
	STATE 1: check-in (Car info & Room Info -> Get Room  || Deposit Car (and come back)-> STATE 0)
	STATE 2: check-out (Key info -> Deposits Room || Get Car (and comes back) -> STATE 0)
	STATE 3: livraison ( info on which key -> Deposit room door )

*/
/*
 */

// CHECK IN
// FONCTION RFID (confirmation payment)
// AVEC CLÉ DE VOITURE ( suite a avoir deposer clé dans plaquette)
// FONCTION DISPONIBILITÉ CLÉ ( check si clé et dispo et associe le clé voiture à chambre)
// SÉQUENCE ROBOT A PARCOURIR
// Porter clés voiture qui ont été récupéré
// Tourner 180 degrées vers la gauche (faces back of plane)
// 2 x Avancer vers la ligne jusqu'au carré
// Tourner 90 deg vers la gauche
// Porter clé voiture
// FONCTION INTERKEYDOOR
// Chercher clé de chambre
// recule jusqu'au carré + tourner 90 deg vers la droite
// avancer (selon la chambre)
// FONCTION INTERKEYDOOR
// Porter clé de chambre au client a l'accueil
// Tourner 180 degrées vers la gauche (faces back of plane)
// 2 x Avancer vers la ligne jusqu'au carré

// SANS CLÉ DE VOITURE
// SÉQUENCE ROBOT A PARCOURIR
// Chercher clé de chambre
// recule jusqu'au carré + tourner 90 deg vers la droite
// avancer (selon la chambre)
// FONCTION INTERKEYDOOR
// Porter clé de chambre au client a l'accueil
// Tourner 180 degrées vers la gauche (faces back of plane)
// 2 x Avancer vers la ligne jusqu'au carré

// FAIRE UN LOOP AVEC UN COMPTEUR A QUELQUE PART POUR SAVOIR ON EST RENDU OÙ POUR LES CHAMBRES
// FONCTION GOINTERKEYDOOR ( même pour les clés voiture et chambre, tout ce qu'il aura a faire c ce positionner de maniere a faire face a droite ou a gauche avant)
// MAYBE ON DEVRAIT ÇA DANS UNE FONCTION APPART qui s'applique pour clé chambre et voiture -note de gaby
// si chambre 1= Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé
// si chambre 3 = 2 x Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé
// si chambre 3 = 3 x Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé
// si chambre 3 = 4 x Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé

// FONCTION RETURNINTERKEYDOOR
// recule jusqu'au carré + tourner 90 deg vers la droite
// Sense inverse de GOINTERKEYDOOR ( nous retourne au interkeydoor )
