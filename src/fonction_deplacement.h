#include <LibRobus.h>
#include <Arduino.h>

// ---------------------------- Contrôle Général du Mouvement ----------------------------
//POUR PID 
int32_t difference_moteurs = 0;
uint32_t pulses_droit = 0; // Compteur de pulses pour moteur droit
uint32_t pulses_gauche = 0;
float KP1 = 0.0015; // Constante proportionnelle pour le contrôle PID ()

int etat = -1; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.25;
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
int seuilSuiveurLigne = 100;
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
	delay(400);
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
	while (pulses_gauche < nmbr_pulses) // 1800 pulses = 950ms représente le nombre de pulses avant que la fonction tourneDroit arrête
	{
		MOTOR_SetSpeed(RIGHT, -0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
		pulses_gauche = - (ENCODER_Read(LEFT));
	}
	pause();
};

void tourneDroit(int nombre_pulses)
{
	resetEncodeurs();
	while (pulses_gauche < nombre_pulses) // 1800 pulses = 950ms représente le nombre de pulses avant que la fonction tourneDroit arrête
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
	while (pulses_droit < nombre_pulses) // 1800 pulses = 900ms représente le nombre de pulses avant que la fonction tourneGauche arrête
	{
		MOTOR_SetSpeed(RIGHT, 0.5 * vitesse);
		MOTOR_SetSpeed(LEFT, -0.5 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
	}
	pause();
};

void tourneDroitInfini(float coefficient)
{
	resetEncodeurs();
	MOTOR_SetSpeed(RIGHT, coefficient * -0.5 * vitesse);
	MOTOR_SetSpeed(LEFT, coefficient * 0.5 * vitesse);
}
void tourneGaucheInfini(float coefficient)
{
	resetEncodeurs();
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
		Serial.print("Valeur du capteur de droite : ");
		Serial.println(ligneDroite);
		Serial.print("Valeur du capteur de gauche : ");
		Serial.println(ligneGauche);
		Serial.print("Valeur du capteur du milieu : ");
		Serial.println(ligneMilieu);

		MOTOR_SetSpeed(RIGHT, 0.75 * vitesse);
		MOTOR_SetSpeed(LEFT, vitesse);
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
		Serial.print("Valeur du capteur de droite : ");
		Serial.println(ligneDroite);
		Serial.print("Valeur du capteur de gauche : ");
		Serial.println(ligneGauche);
		Serial.print("Valeur du capteur du milieu : ");
		Serial.println(ligneMilieu);

		MOTOR_SetSpeed(RIGHT, vitesse);
		MOTOR_SetSpeed(LEFT, 0.75 * vitesse);
		pulses_droit = ENCODER_Read(RIGHT);
	}
}

void suivreLigne()
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
	

	//if ligne milieu is on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite <seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		avanceLent(); //0
	}

	//if ligne droit is on
	if ((ligneDroite > seuilSuiveurLigne) && (ligneMilieu < seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		ajusterdroite(); //1
	}

	//if ligne gauche is on
	if ((ligneGauche > seuilSuiveurLigne) && (ligneMilieu < seuilSuiveurLigne) && (ligneDroite < seuilSuiveurLigne))
	{
		ajustergauche(); //2
	}

	//if all is off
	if ((ligneMilieu < seuilSuiveurLigne) && (ligneDroite < seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		avanceLent(); //0
	}

	//if all is on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite > seuilSuiveurLigne) && (ligneGauche > seuilSuiveurLigne))
	{
		verification = 1;
		avanceLent(); //3
	}

	//if ligne milieu and droite on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite > seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		ajusterdroite(); //1
	}

	//if ligne milieu and gauche on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite < seuilSuiveurLigne) && (ligneGauche > seuilSuiveurLigne))
	{
		ajustergauche(); //2
	}
}


//This function takes in a number of intersections it wants to pass ( through the parameter)
//and makes the robot follow the line until the numbers of intersections have been reached
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

		int etat = (2 * (int)(ligneGauche > seuilSuiveurLigne)) + (int)(ligneDroite > seuilSuiveurLigne);// conversion en nombre binaire, donc on fait *2
		if (etat == 0)
		{
			avanceLent();
		}
		if (etat == 1)
		{
			ajusterdroite();
		}
		if (etat == 2)
		{
			ajustergauche();
		}
		if (etat == 3)
		{
			if (ligneMilieu > seuilSuiveurLigne)
			{
				 // compteur
				avanceLent();
				i++;
				delay(500);

			}
		}	
			
	}
	
	verification_ligne = false;
}


void logiqueMouvement(){
	
	if (move_state == 0)//stand by
	{
		arret();
	}
	
	
	if (move_state == 1) //CHECK IN
	{
	
		//make function to turn gauche (180 deg)
		//tourneGauche(3600);
		//follow the line until it sees two intersections
		while (verification_ligne)
		{
			suivreLigneIntersect(2);
		}
		verification_ligne = true;
		arret();
		avance(500);
		tourneGauche(1800);
		avance(500);
		while (verification_ligne)
		{
			suivreLigneIntersect(1);
		}


		/*
		if ( car_key ) { // If it has the car key
			//make function to turn (90 deg left)
			avance(500);
			tourneGauche(1800);

			suivreLigneIntersect(key_num); //note: key number is the same as the # of intersections
			//turn 90 deg right
			avance(500);
			tourneDroit(1800);
			// va sur point orange
			suivreLigneIntersect(1);
			//deposit car key (FONCTION POUR PINCE)
			// tourne 180 deg

			tourneGauche(3600);
			suivreLigneIntersect(1);
			//turn 90 deg left
			avance(500);
			tourneGauche(1800);
			suivreLigneIntersect(2 * key_num);
			
		}
		else {
			//90 deg droite
			tourneDroit(1800);
			suivreLigneIntersect(key_num);
		
		}
		//turn 90 left
		avance(500);
		tourneGauche(1800);
		//va sur point orange
		suivreLigneIntersect(1);
		//Prend clé voiture (FONCTION POUR PINCE)
		// tourne 180 deg
		tourneGauche(3600);
		suivreLigneIntersect(1);
		//turn 90 righ
		avance(500);
		tourneDroit(1800);
		suivreLigneIntersect(key_num);
		//turn 90 left
		avance(500);
		tourneGauche(1800);
		suivreLigneIntersect(2);
		//AFFICHAGE LCD POUR MENU
		//pour reset
		move_state = 0;
		car_key = false;
		break;
	
	*/
	} 

		
		
	if (move_state == 2)
	{
	
		//make function to turn gauche (180 deg)
		tourneGauche(3600);
		//follow the line until it sees two intersections
		suivreLigneIntersect(2);
		//90 deg droite
		tourneDroit(1800);
		suivreLigneIntersect(key_num);
		//turn 90 left
		tourneGauche(1800);
		//va sur point orange
		suivreLigneIntersect(1);
		//retour clé de chambre (FONCTION POUR PINCE)
		// tourne 180 deg
		tourneGauche(3600);
		suivreLigneIntersect(1);
		//turn 90 right
		tourneDroit(1800);	
		
		if ( car_key ) { // If it has the car key
			suivreLigneIntersect(2 * key_num);
			tourneDroit(1800);
			// va sur point orange
			suivreLigneIntersect(1);
			//deposit car key (FONCTION POUR PINCE)
			// tourne 180 deg
			tourneGauche(3600);
			suivreLigneIntersect(1);
			//turn 90 deg left
			tourneGauche(1800);
			suivreLigneIntersect(key_num);
			tourneDroit(1800);
		}
		else {
			suivreLigneIntersect(key_num);
			tourneGauche(1800);
		}
		suivreLigneIntersect(2);
		//AFFICHAGE LCD POUR MENU
		//pour reset ( revenir en stand_by i.e. start)
		move_state = 0;
		car_key = false;
	

	}

	if (move_state == 3)
	{
		//service clés de voiture seulement retour et recuperation
		
		//make function to turn gauche (180 deg)
		tourneGauche(3600);
		//follow the line until it sees two intersections
		suivreLigneIntersect(2);
		//make function to turn (90 deg left)
		tourneGauche(1800);
		suivreLigneIntersect(key_num); //note: key number is the same as the # of intersections
		//turn 90 deg right
		tourneDroit(1800);
		// va sur point orange
		suivreLigneIntersect(1);
		//deposit car key (FONCTION POUR PINCE)
		// tourne 180 deg
		tourneGauche(3600);
		suivreLigneIntersect(1);
		//turn 90 left
		tourneGauche(1800);
		//va sur point orange
		suivreLigneIntersect(key_num);
		tourneDroit(1800);
		suivreLigneIntersect(2);
		//AFFICHAGE LCD POUR MENU
		//pour reset
		move_state = 0;
		car_key = false;
	
	}
	

	if (move_state == 4)
	{
		//make function to turn gauche (180 deg)
		tourneGauche(3600);
		//follow the line until it sees two intersections
		suivreLigneIntersect(1);
		tourneGauche(1800);
		suivreLigneIntersect(1);
		tourneGauche(1800);
		suivreLigneIntersect(1);
		tourneGauche(3600);
		suivreLigneIntersect(1);
		tourneDroit(1800);
		suivreLigneIntersect(2);
			if ( key_num == 1){
			suivreLigneIntersect(1);
			//drop colis
			recule(1000); //À REVOIR AVEC TEST (besoin de reculer pour dposer colis)
			//make function to turn gauche (180 deg)
			tourneGauche(3600);
			suivreLigneIntersect(2);
			}
			else{
			tourneDroit(1800);
			suivreLigneIntersect(key_num -1);
			tourneGauche(1800);
			suivreLigneIntersect(1);
			recule(1000); //À REVOIR AVEC TEST (besoin de reculer pour dposer colis)
			tourneGauche(3600);
			suivreLigneIntersect(1);
			tourneDroit(1800);
			suivreLigneIntersect(key_num -1);
			tourneGauche(1800);
			suivreLigneIntersect(1);
			}
		tourneGauche(1800);
		suivreLigneIntersect(1);
		//pour reset
		move_state = 0;
		car_key = false;
	}
	

	


}

