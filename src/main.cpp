#include "settings.h"

/*
	STATE 0: stand by (LCD -> Next state)
	STATE 1: check-in (Car info & Room Info -> Get Room  || Deposit Car (and come back)-> STATE 0)
	STATE 2: check-out (Key info -> Deposits Room || Get Car (and comes back) -> STATE 0)
	STATE 3: livraison ( info on which key -> Deposit room door )

*/
/* 
*/

//CHECK IN
	//FONCTION RFID (confirmation payment)
		//AVEC CLÉ DE VOITURE ( suite a avoir deposer clé dans plaquette)
			//FONCTION DISPONIBILITÉ CLÉ ( check si clé et dispo et associe le clé voiture à chambre)
				//SÉQUENCE ROBOT A PARCOURIR
						//Porter clés voiture qui ont été récupéré
							//Tourner 180 degrées vers la gauche (faces back of plane)
							//2 x Avancer vers la ligne jusqu'au carré 
							//Tourner 90 deg vers la gauche
							//Porter clé voiture 
								//FONCTION INTERKEYDOOR
						//Chercher clé de chambre
							//recule jusqu'au carré + tourner 90 deg vers la droite 
							//avancer (selon la chambre)
								//FONCTION INTERKEYDOOR
						//Porter clé de chambre au client a l'accueil
							//Tourner 180 degrées vers la gauche (faces back of plane)
							//2 x Avancer vers la ligne jusqu'au carré 

					
		//SANS CLÉ DE VOITURE 
			//SÉQUENCE ROBOT A PARCOURIR
				//Chercher clé de chambre
							//recule jusqu'au carré + tourner 90 deg vers la droite 
							//avancer (selon la chambre)
								//FONCTION INTERKEYDOOR
						//Porter clé de chambre au client a l'accueil
							//Tourner 180 degrées vers la gauche (faces back of plane)
							//2 x Avancer vers la ligne jusqu'au carré 

        