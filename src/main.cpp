#include "settings.h"

int screen_output;
int screen_display = 0;
int bouton_selection = 1;
int Numcarte = -1;

int i, incoming = 0;
char id_tag[20];
char crecu;
String id_carte = "";

int Identification(String IDCard)
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

	return numCarte + numChambre;
}

struct chambre
{
	bool isDispo = true;
	bool isKeyCar = false;
	int numChambre;
};

// gestion de la distribution des clees

chambre Chambre1;
chambre Chambre2;
chambre Chambre3;

int NumeroIdentification;

void setup()
{

	Wire.begin();
	BoardInit();

	Serial.begin(9600);
	Serial.print("RESET");

	// initialisation des structs
	Chambre1.numChambre = 1;
	Chambre2.numChambre = 2;
	Chambre3.numChambre = 3;
	initialiserPinceEtBras();
	SERVO_Disable(RIGHT);
	SERVO_Disable(LEFT);
}

// return la distribution de la chambre
int DistributionChambre()
{
	int distribution = 0;
	if (Chambre1.isDispo == true)
	{
		distribution = 1;
		Chambre1.isDispo = false;
	}
	else if (Chambre2.isDispo == true)
	{
		distribution = 2;
		Chambre2.isDispo = false;
	}
	else if (Chambre3.isDispo == true)
	{
		distribution = 3;
		Chambre3.isDispo = false;
	}

	return distribution;
}

void loop()
{
	Serial.print(screen_display);

	// event
	// move_state = 0;

	// MENU BIENVENUE
	if (screen_display == 0 || screen_display == 10)
	{
		NumeroIdentification = 0;
		if (ROBUS_IsBumper(3))
		{
			key_num = 0;
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
	// EXEMPLE UTILISATION RFID
	/*if (screen_display == 1)
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
	}*/
	// MENU DE SELECTION
	else if (screen_display == 1)
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
				screen_output = 2;
				if (Chambre1.isDispo == false && Chambre2.isDispo == false && Chambre3.isDispo == false)
			{
				beep(20);
				// AFFICHER PAS DE CHAMBRE DISPONIBLE
			}
			}


			if (bouton_selection == 2)
			{
				screen_output = 6;
			}
			if (bouton_selection == 3)
			{
				screen_output = 12;
			}


			bouton_selection = 1;
			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;
			delay(250);
		}
	}
	// SCANNE LA CARTE RFID --- > MERCI VEUILLEZ PATIENTER ---> DEPLACEMENT ---> VOICI VOTRE CLEE DE CHAMBRE
	else if (screen_display == 2)
	{
		Serial2.begin(9600);
		while (screen_display == 2 /* modifier pour variable de scanne*/)
		{
			key_num = 0;
			Serial.print("Screen : Paiment");
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
					key_num = Identification(id_carte);
					break;
				default:
					if (incoming)
						id_tag[i++] = crecu;
					break;
				}
			}

			if (key_num != 0)
			{
				screen_output = 21;
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
		// condition logique de deplacement

		// DEPLACEMENT
		if (key_num != 0)
		{
			NumeroIdentification = DistributionChambre();
			beep(NumeroIdentification);
			logiqueMouvement(NumeroIdentification, 1);
		}

		delay(500); // a enlever


		screen_output = 3;
		bouton_selection = 1;
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;
		delay(250);
	}
	// VOICI VOTRE CLEE DE CHAMBRE --> 4 DEPOSER LES CLEES DE CHAMBRES
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
	// RESERVER CHAMBRE -- DEPOSER LES CLEES DE VOITURES
	else if (screen_display == 4)
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
			 // OUI
			if (bouton_selection == 1)
			{
				if (NumeroIdentification == 1){
					Chambre1.isKeyCar = true;
				}
				if (NumeroIdentification == 2){
					Chambre2.isKeyCar = true;
				}
				if (NumeroIdentification == 3){
					Chambre3.isKeyCar = true;
				}
				screen_output = 5;
			}
			// NON
			else if (bouton_selection == 2)
			{
				screen_output = 20;
			}

			// remet bouton a 1 lorsque confimer
			bouton_selection = 1;

			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;
			delay(250);


			if (screen_output == 20){
			  //va reporter a sa place d origine la boite
			  logiqueMouvement(NumeroIdentification, 4);
			}
		}
	}
	// RESERVER CHAMBRE -- DEPOSER LES CLEES DE VOITURES -- OUI
	else if (screen_display == 5)
	{

		if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
		{

			// *************************** A MODIFIER LE NUMERO D IDENTIFICATION
			bouton_selection = 1;
			// merci bonne journee
			screen_output = 20;
			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;

			logiqueMouvement(NumeroIdentification, 2);

		}
	}
// doit rajouter une memoire lorsqu on scan pour savoir ou le robot doit aller porter la carte
	// RECUPERER SES CLEES
	else if (screen_display == 6)
	{
		int action = 0;
		Serial2.begin(9600);
		while (screen_display == 6 /* modifier pour variable de scanne*/)
		{
			Serial.print("Screen : Scan Cle de chambre");
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
					key_num = Identification(id_carte);
					break;
				default:
					if (incoming)
						id_tag[i++] = crecu;
					break;
				}
			}

			if (key_num != 0)
			{
				screen_output = 21;
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
		// condition logique de deplacement

		// DEPLACEMENT
		if (key_num != 0)
		{
			// mettre en memoire
			// verifier les dispo des boites*****


				if (key_num == 1 && Chambre1.isKeyCar == true){
					action = 1;
					Chambre1.isKeyCar == false;
				}
				else if (key_num == 3 && Chambre2.isKeyCar == true){
					action = 1;
					Chambre2.isKeyCar == false;
				}
				else if (key_num == 2 && Chambre3.isKeyCar == true){
					action = 1;
					Chambre3.isKeyCar == false;
				}

				if (action == 0){
					beep(key_num);
					screen_output = 20;
					bouton_selection = 1;
					screen_display = 8;
					delay(250);

				}
				else if ( action == 1){
					int NumeroIdentification = key_num;
					beep(NumeroIdentification);
					if (NumeroIdentification == 1){
						logiqueMouvement( 1, 3);
					}
					if (NumeroIdentification == 1){
						logiqueMouvement( 2, 3);
					}
					if (NumeroIdentification == 1){
						logiqueMouvement( 3, 3);
					}

				}


		}
		if (action == 1){


		delay(500); // a enlever

		// a la fin de deplacement
		screen_output = 7;
		bouton_selection = 1;
		Wire.beginTransmission(9);
		Wire.write(screen_output);
		Wire.endTransmission();
		screen_display = screen_output;
		delay(250);
		}

	}
	// Prendre les clees
	else if (screen_display == 7)
	{
		if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
		{

			// remet bouton a 1 lorsque confimer
			bouton_selection = 1;

			// Renvoie a reserver la chambre
			screen_output = 8;
			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;

			delay(250);
		}

	}
	// DEPOSER CLEE DE CHAMBRE
	else if (screen_display == 8)
	{
		if (ROBUS_IsBumper(3)) // CONFIRMER SÉLECTION
		{
			delay(250);
			// deplacement

			// modifier***
			beep(NumeroIdentification);
			if (NumeroIdentification == 1){
				logiqueMouvement(1, 4);
			}
			if (NumeroIdentification == 2){
				logiqueMouvement(2, 4);
			}
			if (NumeroIdentification == 3){
				logiqueMouvement(3, 4);
			}
			 // peut etre erreur



			// remet bouton a 1 lorsque confimer
			bouton_selection = 1;

			// Renvoie a Merci bonne journe
			screen_output = 20;
			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;

			delay(250);

		}

	}
	// LIVRAISON DE COLIS
	else if (screen_display == 12)
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
			// remet bouton a 1 lorsque confimer


			// Renvoie a reserver la chambre
			//pas encore implementer
			screen_output = 20;
			// ENVOYER L'ACTION AU LCD
			Wire.beginTransmission(9);
			Wire.write(screen_output);
			Wire.endTransmission();
			screen_display = screen_output;
			beep(bouton_selection);
			if (bouton_selection == 1){
				logiqueMouvement(1, 5);
			}
			if (bouton_selection == 2){
				logiqueMouvement(2, 5);
			}
			if (bouton_selection == 3){
				logiqueMouvement(3, 5);
			}
			//logiqueMouvement(bouton_selection, 5);
			bouton_selection = 1;
			delay(250);
		}

	}
	// MERCI BONNE JOURNEE
	else if (screen_display == 20)
	{
		NumeroIdentification = 0;
		// CONFIRMER SÉLECTION ou Delay a decider
		delay(5000);
		screen_output = 10;
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
}

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
