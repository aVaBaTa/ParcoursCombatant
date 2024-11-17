#include <LibRobus.h>
#include <Arduino.h>

bool bumperArr;
int32_t difference_moteurs = 0;
uint32_t pulses_droit = 0;
uint32_t pulses_gauche = 0;
int vertpin = 48;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = -1; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.35;
int x = 0;			 // x = 0 initialisation et représente la coordonnée x de la case de départ
int y = 0;			 // y = 0 initialisation et représente la coordonnée y de la case de départ
int regardeFace = 0; // Regarde à : AVANT = 0, GAUCHE = -1, DERRIÈRE = 2 ou -2, DROITE = 1
int last_move = 0;	 // est = 1 si x==0 et que le dernier movement est setorientation(1)
float KP1 = 0.0015;

enum MoveState {stand_by, check_in, check_out, livraison};
MoveState move_state = MoveState::stand_by;
bool car_key = false;

// Pour le suiveur de ligne
int CaptLeft = A10; // A changer si PIN inverse
int CaptMid = A11;
int CaptRight = A12; // A chanmger si PIN inverse
int ligneGauche = 0;
int ligneMilieu = 0;
int ligneDroite = 0;
int lastCheck = 0;
int seuilSuiveurLigne = 175;
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
		MOTOR_SetSpeed(RIGHT, -1 * vitesse);
		MOTOR_SetSpeed(LEFT, -1 * vitesse);
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
		avanceLent();
	}

	//if ligne droit is on
	if ((ligneDroite > seuilSuiveurLigne) && (ligneMilieu < seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		ajusterdroite();
	}

	//if ligne gauche is on
	if ((ligneGauche > seuilSuiveurLigne) && (ligneMilieu < seuilSuiveurLigne) && (ligneDroite < seuilSuiveurLigne))
	{
		ajustergauche();
	}

	//if all is off
	if ((ligneMilieu < seuilSuiveurLigne) && (ligneDroite < seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		avanceLent();
	}

	//if all is on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite > seuilSuiveurLigne) && (ligneGauche > seuilSuiveurLigne))
	{
		verification = 1;
		avanceLent();
	}

	//if ligne milieu and droite on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite > seuilSuiveurLigne) && (ligneGauche < seuilSuiveurLigne))
	{
		ajusterdroite();
	}

	//if ligne milieu and gauche on
	if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite < seuilSuiveurLigne) && (ligneGauche > seuilSuiveurLigne))
	{
		ajustergauche();
	}
}



void suivreLigneIntersect(int: num_intersect){
	ligneGauche = analogRead(CaptLeft);
	ligneMilieu = analogRead(CaptMid);
	ligneDroite = analogRead(CaptRight);

	i = num_intersect
	while i > 0 {
		avanceLent();
		//if all is on
		if ((ligneMilieu > seuilSuiveurLigne) && (ligneDroite > seuilSuiveurLigne) && (ligneGauche > seuilSuiveurLigne))
		{
			i--;
		}
	}
	stop()
}

void logiqueMouvement(){
	switch (move_state)
	{
	case MoveState::stand_by : break;
	
	case MoveState::check_in :
		if car_key {
			//make function to turn (180 deg)
			suivreLigneIntersect(2);
			//make function to turn (90 deg left)
			suivreLigneIntersect(/*Key Num*/);
			//turn 90 deg right
			//deposit key
			//turn 90 deg right
			suivreLigneIntersect(/* 2*Key Num */);
			//turn 90 left
			//deposit keys
			//turn 90 left
			suivreLigneIntersect(/* Key Num*/);
			//turn 90 left
			suivreLigneIntersect(2)
		}
		else {
			//make function to turn (180 deg)
			suivreLigneIntersect(2);
			//turn 90 right
			suivreLigneIntersect(/*Key Num*/);
			//turn 90 left
			//deposit keys
			//turn 90 left
			suivreLigneIntersect(/* Key Num*/);
			//turn 90 left
			suivreLigneIntersect(2)
		}
			
	case MoveState::check_out :

	case MoveState::livraison :

	default:
		break;
	}
}

//FAIRE UN LOOP AVEC UN COMPTEUR A QUELQUE PART POUR SAVOIR ON EST RENDU OÙ POUR LES CHAMBRES
// FONCTION GOINTERKEYDOOR ( même pour les clés voiture et chambre, tout ce qu'il aura a faire c ce positionner de maniere a faire face a droite ou a gauche avant)
//MAYBE ON DEVRAIT ÇA DANS UNE FONCTION APPART qui s'applique pour clé chambre et voiture -note de gaby
	//si chambre 1= Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé
	//si chambre 3 = 2 x Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé
	//si chambre 3 = 3 x Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé
	//si chambre 3 = 4 x Avancer vers la ligne jusqu'au carré + tourner 90 deg vers la droite + avancer jusqu'à clé

// FONCTION RETURNINTERKEYDOOR 
	//recule jusqu'au carré + tourner 90 deg vers la droite 
	// Sense inverse de GOINTERKEYDOOR ( nous retourne au interkeydoor )
	