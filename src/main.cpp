#include "settings.h"

/*
	STATE 0: stand by (LCD -> Next state)
	STATE 1: check-in (Car info & Room Info -> Get Room  || Deposit Car (and come back)-> STATE 0)
	STATE 2: check-out (Key info -> Deposits Room || Get Car (and comes back) -> STATE 0)
	STATE 3: livraison

*/


//CHECK IN
	//FONCTION RFID (confirmation payment)
		//AVEC CLÉ DE VOITURE ( suite a avoir deposer clé dans plaquette)
			//SÉQUENCE ROBOT A PARCOURIR
					//Porter clés voiture qui ont été récupéré
					//Chercher clé de chambre
					//Porter clé de chambre au client a l'acceuil
					
		//SANS CLÉ DE VOITURE 
			//SÉQUENCE ROBOT A PARCOURIR
        