#include "settings.h"
//#include <Wire.h>

void setup()
{
	BoardInit();
	Serial.begin(9600);
}

void loop()
{
		move_state = 1;
		car_key = 1;
		key_num = 1;
		suivreLigneIntersect(2);
		Virage("droite", "avance", 180);
		suivreLigneIntersect(1);
		Virage("gauche", "avance", 90);
		suivreLigneIntersect(1);
		Virage("droite", "avance", 180);
		suivreLigneIntersect(1);
		Virage("droite", "avance", 90);
		suivreLigneIntersect(1);
		Virage("gauche", "avance", 180);
		suivreLigneIntersect(2);

		arret();

		delay(10000);
		//logiqueMouvement();
		
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
