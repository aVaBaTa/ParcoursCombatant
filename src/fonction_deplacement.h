#include <LibRobus.h>
#include <Arduino.h>
#include <string.h>

// ---------------------------- Contrôle Général du Mouvement ----------------------------
//POUR PID 
int32_t difference_moteurs = 0;
uint32_t pulses_droit = 0; // Compteur de pulses pour moteur droit
uint32_t pulses_gauche = 0;
float KP1 = 0.0015; // Constante proportionnelle pour le contrôle PID ()

int etat = -1; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.35;
int x = 0;			 // x = 0 initialisation et représente la coordonnée x de la case de départ
int y = 0;			 // y = 0 initialisation et représente la coordonnée y de la case de départ
int regardeFace = 0; // Regarde à : AVANT = 0, GAUCHE = -1, DERRIÈRE = 2 ou -2, DROITE = 1
int last_move = 0;	 // est = 1 si x==0 et que le dernier movement est setorientation(1)
int key_num;
bool verification_ligne = true;


enum MoveState {
	stand_by,
	check_in,
	check_out,
	car_service,
	livraison
};

int move_state = 0; // état initial du mouvement du robot
bool car_key = false; // booléen pour vérifier si le robot a une clé de voiture

// Pour le suiveur de ligne
int CaptLeft = A10; // A changer si PIN inverse
int CaptMid = A11;
int CaptRight = A12; // A changer si PIN inverse
int ligneGauche = 0;
int ligneMilieu = 0;
int ligneDroite = 0;
int lastCheck = 0;
int seuilSuiveurLigne = 270;
int verification = 0;

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
	// Serial.println(gauche);
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
	// Serial.println(diff);
	AjusterVitesse(diff, droit, gauche);
}
void beep(int count)
{
	for (int i = 0; i < count; i++)
	{
		AX_BuzzerON();
		delay(100);
		AX_BuzzerOFF();
		delay(100);
	}
}
// Pour RESET les encodeurs et les variables qui stockent les valeurs lues des encodeurs
void resetEncodeurs()
{
	ENCODER_Reset(RIGHT);
	ENCODER_Reset(LEFT);
	pulses_droit = 0;
	pulses_gauche = 0;
}
void arret()
{
	MOTOR_SetSpeed(RIGHT, 0);
	MOTOR_SetSpeed(LEFT, 0);
};
void pause()
{
	arret();
	delay(100);
}
void avance(int nombre_pulses)
{
	resetEncodeurs();
	while (pulses_droit < 100)
	{
		MOTOR_SetSpeed(RIGHT, 0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, 0.5 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
		delay(50);
	}
	resetEncodeurs();
	while (pulses_droit < nombre_pulses) // 6300 représente le nombre de pulses avant que la fonction avancer arrête
	{
		MOTOR_SetSpeed(RIGHT, vitesse);
		MOTOR_SetSpeed(LEFT, vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
		pulses_gauche = ENCODER_Read(LEFT);
		PID(pulses_droit, pulses_gauche);
		delay(50);
	}
	resetEncodeurs();
	while (pulses_droit < 300)
	{
		MOTOR_SetSpeed(RIGHT, 0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, 0.5 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
	}
	pause();
};
void recule(int nmbr_pulses)
{
	resetEncodeurs();
	while (pulses_droit < 100)
	{
		MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
		pulses_droit = abs(ENCODER_Read(RIGHT));
		delay(50);
	}
	resetEncodeurs();
	while (pulses_droit < nmbr_pulses) 
	{
		MOTOR_SetSpeed(RIGHT, -vitesse);
		MOTOR_SetSpeed(LEFT, -vitesse);
		pulses_droit = abs(ENCODER_Read(RIGHT));
		pulses_gauche = abs(ENCODER_Read(LEFT));
	}
	resetEncodeurs();
	while (pulses_droit < 200)
	{
		MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
		pulses_droit = abs(ENCODER_Read(RIGHT));
	}
	pause();
};

void tourneDroit(int nombre_pulses)
{
	resetEncodeurs();
	while (pulses_gauche < nombre_pulses) 
	{
		MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, 0.5 * vitesse);
		pulses_gauche = ENCODER_Read(LEFT);
	}
	pause();
};
void tourneGauche(int nombre_pulses)
{
	resetEncodeurs();
	while (pulses_droit < nombre_pulses)
	{
		MOTOR_SetSpeed(RIGHT, 0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
	}
	pause();
};

void tourneDroitInfini(float coefficient)
{
	MOTOR_SetSpeed(RIGHT, coefficient * -0.5 * vitesse);
	MOTOR_SetSpeed(LEFT, coefficient * 0.5 * vitesse);
}
void tourneGaucheInfini(float coefficient)
{
	MOTOR_SetSpeed(RIGHT, coefficient * 0.5 * vitesse);
	MOTOR_SetSpeed(LEFT, coefficient * -0.5 * vitesse);
}

// FONCTIONS POUR LE SUIVEUR DE LIGNE
void avanceLent()
{
	resetEncodeurs();
	MOTOR_SetSpeed(RIGHT, 0.75 * vitesse);
	MOTOR_SetSpeed(LEFT, 0.75 * vitesse);
	pulses_droit = ENCODER_Read(RIGHT);
};
void avanceLentLent(int nombre_pulses)
{
	resetEncodeurs();
	while (pulses_droit < nombre_pulses) // 1800 pulses = 900ms représente le nombre de pulses avant que la fonction tourneGauche arrête
	{
		MOTOR_SetSpeed(RIGHT, 0.4 * vitesse);
		MOTOR_SetSpeed(LEFT, 0.4 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
	}
};

void ajusterdroite()
{

	resetEncodeurs();
	ligneMilieu = 0;
	while (ligneMilieu < seuilSuiveurLigne)
	{
		ligneGauche = analogRead(CaptLeft);
		ligneMilieu = analogRead(CaptMid);
		ligneDroite = analogRead(CaptRight);
		/*Serial.print("Valeur du capteur de droite : ");
		Serial.println(ligneDroite);
		Serial.print("Valeur du capteur de gauche : ");
		Serial.println(ligneGauche);
		Serial.print("Valeur du capteur du milieu : ");
		Serial.println(ligneMilieu);*/

		MOTOR_SetSpeed(RIGHT, 0.68 * vitesse);
		MOTOR_SetSpeed(LEFT, 0.75*vitesse);
		pulses_gauche = ENCODER_Read(LEFT);
	}
}
void ajustergauche()
{

	resetEncodeurs();
	ligneMilieu = 0;
	while (ligneMilieu < seuilSuiveurLigne)
	{
		ligneGauche = analogRead(CaptLeft);
		ligneMilieu = analogRead(CaptMid);
		ligneDroite = analogRead(CaptRight);
		/*Serial.print("Valeur du capteur de droite : ");
		Serial.println(ligneDroite);
		Serial.print("Valeur du capteur de gauche : ");
		Serial.println(ligneGauche);
		Serial.print("Valeur du capteur du milieu : ");
		Serial.println(ligneMilieu);*/

		MOTOR_SetSpeed(RIGHT, 0.75 * vitesse);
		MOTOR_SetSpeed(LEFT, 0.67 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
	}
}



// Pour que la fonction fonctionne, il faut que chaque intersection du parcour puisse déclencher les trois détecteurs de lignes (G, M et D)
void suivreLigneIntersect(int num_intersect){

	int i = 0;
	while (i < num_intersect) 
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

		int etat = (2 * (int)(ligneGauche > seuilSuiveurLigne)) + (int)(ligneDroite > seuilSuiveurLigne);
		if (etat == 0)
		{
			avanceLent();
			//Serial.println("avance lent") ;
		}
		if (etat == 1)
		{
			ajusterdroite();
			//Serial.println("ajuste droite") ;
		}
		if (etat == 2)
		{
			ajustergauche();
			//Serial.println("ajuste gauche") ;
		}
		if (etat == 3)
		{
			if (ligneMilieu > seuilSuiveurLigne)
			{
				if (i != (num_intersect - 1))
				{
					avance(500);
				}
				i++;
				
				//Serial.println("les 3 détectent") ;

			}
		}
	}
	
}

// PREMIER PARAMÈTRE: 0: DROITE 1: GAUCHE
// DEUXIÈME PARAMÈTRE: "avance": AVANCE AVANT DE TOURNER 	"recule": RECULE AVANT DE TOURNER 	OU LAISSER VIDE POUR QU'IL NE FASSE AUCUN DES DEUX
// TROISIÈME PARAMÈTRE: 90: Fait un angle de 90 		180: Fait un angle de 180
// Le robot tourne sur lui-même jusqu'à temps que le capteur du milieu du suiveur de ligne rencontre une ligne
void Virage(String direction, String avance_recule, int angle_desire)
{
	resetEncodeurs();
	// À DÉTERMINER NMBR DE PULSES : AVANCE UN PEU RENDU À L'INTERSECTION POUR BIEN SE RÉAJUSTER
	if (avance_recule == "avance")
	{
		avance(200);
	} 
	else if (avance_recule == "recule")
	{
		recule(300);
	}

	if (angle_desire == 90)
	{
		// Pour éviter que le robot tourne et arrête à la mauvaise intersection (angle 90 ou 180)
		while (pulses_gauche < 1500)
		{
			pulses_gauche = abs(ENCODER_Read(LEFT));
			//Serial.println(pulses_gauche);
			if (direction == "droite")
			{
				tourneDroitInfini(1);
			}
			else if (direction == "gauche")
			{
				tourneGaucheInfini(1);
			}
		}
	}
	else if (angle_desire == 180)
	{
		while (pulses_gauche < 3000)
		{
			pulses_gauche = abs(ENCODER_Read(LEFT));
			//Serial.println(pulses_gauche);
			if (direction == "droite")
			{
				tourneDroitInfini(1);
			}
			else if (direction == "gauche")
			{
				tourneGaucheInfini(1);
			}
		}
	}
	ligneMilieu = 0;
	while (ligneMilieu < seuilSuiveurLigne)
	{
		ligneMilieu = analogRead(CaptMid);
		if (direction == "droite")
		{
			tourneDroitInfini(1);
		}
		else if (direction == "gauche")
		{
			tourneGaucheInfini(1);
		}
	}
	// À DÉTERMINER : SI LE ROBOT EST DROIT À 100% SUR LA LIGNE AVANT DE TOURNER ET QU'IL N'Y A PAS DE DELAY IL VA ARRETER
	// AVEC UN PETIT ANGLE
	delay(60);
	// AVANCE POUR ETRE SUR QUE LE SUIVEUR DE LIGNE NE DÉTECTE PAS LE CARRÉ QUAND IL RÉEXÉCUTE UNE AUTRE FONCTION
	avance(200);
	pause();
}



void logiqueMouvement(){
	
	if (move_state == 0)//stand by
	{
		arret();
	}
	
	
	if (move_state == 1) //CHECK IN
	{

		Virage("gauche", "", 180);
		suivreLigneIntersect(2);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(key_num);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(1);


		//prend la cle sur etagere


		Virage("gauche", "recule", 180);
		suivreLigneIntersect(1);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(key_num);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(2);
		//arrete pour que le client prenne sa cle de chambre 
		arret();

		//besoin de input du LCD pour confirmer que le client a pris sa clé
		// Simon : Faire la fonction du LCD ici

		if ( car_key ) { // Si le client possède une clé
			//demande au client de deposer ses cles de voiture 
			//le robot va porter la cle de voiture sur l etagere
			Virage("gauche", "", 180);
			suivreLigneIntersect(2);
			Virage("gauche", "avance", 90);
			suivreLigneIntersect(key_num); 
			//turn 90 deg right
			Virage("droite", "avance", 90);
			// va sur point orange
			suivreLigneIntersect(1);
			//deposit car key (FONCTION POUR PINCE)
			Virage("gauche", "recule", 180);
			suivreLigneIntersect(1);
			//turn 90 deg left
			Virage("gauche", "avance", 90);
			suivreLigneIntersect(key_num);
			Virage("droite", "avance", 90);
			suivreLigneIntersect(2);
			arret();
		}
		//AFFICHAGE LCD POUR MENU (bienvenue)
		//pour reset
		move_state = 0;
		car_key = false;
	} 

		
		
	if (move_state == 2) //Checkout
	{
		if ( car_key ) { //check out avec recuperation de clés
			//le robot va porter la cle de voiture sur l etagere
			Virage("gauche", "", 180);
			suivreLigneIntersect(2);
			Virage("gauche", "avance", 90);
			suivreLigneIntersect(key_num); //note: key number is the same as the # of intersections
			//turn 90 deg right
			Virage("droite", "avance", 90);
			// va sur point orange
			suivreLigneIntersect(1);

			//deposit car key (FONCTION POUR PINCE)

			Virage("gauche", "recule", 180);
			suivreLigneIntersect(1);
			//turn 90 deg left
			Virage("gauche", "avance", 90);
			suivreLigneIntersect(key_num);
			Virage("droite", "avance", 90);
			suivreLigneIntersect(2);
			arret();
		}

		// À AJOUTER (Simon)
		//demande au client de deposer sa cle de chambre 
		//va porter la cle de chambre sur l etagere
		

		Virage("gauche", "", 180);
		//follow the line until it sees two intersections
		suivreLigneIntersect(2);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(key_num);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(1);

		// dépose sa clé sur l'étagère (pince)

		Virage("gauche", "recule", 180);
		suivreLigneIntersect(1);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(key_num);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(2);
		arret();
		//AFFICHAGE LCD POUR MENU
		//pour reset ( revenir en stand_by i.e. start)
		move_state = 0;
		car_key = false;
	}

	if (move_state == 3) //service clés de voiture seulement retour et recuperation
	{
		
		
		//make function to turn gauche (180 deg)
		Virage("gauche", "", 180);
		//follow the line until it sees two intersections
		suivreLigneIntersect(2);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(key_num); //note: key number is the same as the # of intersections
		Virage("droite", "avance", 90);
		// va sur point orange
		suivreLigneIntersect(1);

		//deposit car key (FONCTION POUR PINCE)

		Virage("gauche", "recule", 180);
		suivreLigneIntersect(1);
		Virage("gauche", "avance", 90);
		//va sur point orange
		suivreLigneIntersect(key_num);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(2);
		//AFFICHAGE LCD POUR MENU
		//pour reset
		move_state = 0;
		car_key = false;
	
	}
	

	if (move_state == 4) //service livraison (colis)
	{
		//AJOUTER FONCTION RFID POUR VALIDER QUE CEST BIEN UN EMPLOYER AVANT 
		//DAVOIR ACCÈS AU SERVICE LIVRAISON
		//make function to turn gauche (180 deg)
		Virage("gauche", "", 180);
		//follow the line until it sees two intersections
		suivreLigneIntersect(1);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(1);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(1);
		Virage("gauche", "avance", 180);
		suivreLigneIntersect(1);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(2);
			if ( key_num == 1){
			suivreLigneIntersect(1);
			//drop colis
			recule(1000); //À REVOIR AVEC TEST (besoin de reculer pour dposer colis) (peut-être utiliser même fonction de la pince pour déposer boites de clés)
			//make function to turn gauche (180 deg)
			Virage("gauche", "recule", 180);
			suivreLigneIntersect(2);
			}
			else{
			Virage("droite", "avance", 90);
			suivreLigneIntersect(key_num -1);
			Virage("gauche", "avance", 90);
			suivreLigneIntersect(1);
			recule(1000); //À REVOIR AVEC TEST (besoin de reculer pour dposer colis)
			Virage("gauche", "avance", 180);
			suivreLigneIntersect(1);
			Virage("droite", "avance", 90);
			suivreLigneIntersect(key_num -1);
			Virage("gauche", "avance", 90);
			suivreLigneIntersect(1);
			}
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(1);
		//Affichage LCD pour menu
		//pour reset
		move_state = 0;
		car_key = false;
	}
	

	


}

