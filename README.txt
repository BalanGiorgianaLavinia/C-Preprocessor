	Balan Giorgiana-Lavinia
		331CB

						Tema 1

Organizare:
	Pentru aceasta tema am organizat codul in 4 fisiere:
		-doua fisiere folosite pentru a declara headerele functiilor necesare 
			pentru hashmap si inca o structura utila pentru retinerea 
			tuturor cailor pentru include (caile date prin parametrul -I 
			la rularea programului)(utils.h) si pentru a implementa functiile 
			declarate in header (utils.c)
		-doua fisiere: tema1.h in care am declarat headerele pentru functiile 
			implementate in tema1.c; tema1.c este fisierul principal.

	Consider tema utila pentru ca am reusit prin intermediul ei sa inteleg mai 
		multe lucruri despre sistemele de operare (windows si linux) si am invatat 
		sa folosesc functiile din ANSI C, dar si sa scriu cod aranjat. Era totusi 
		de preferat ca enuntul sa fie mai detaliat si sa nu fiu nevoita sa sterg si
		sa rescriu bucati de cod din nou si din nou cu alte restrictii.
	
Implementare:
	Am implementat toate cerintele temei, desi ultimul test imi pica intrucat 
		nu am luat in considerare de la inceput faptul ca la #define o variabila 
		poate fi definita ca fiind o functie pentru care era nevoie de alte 
		restrictii. Nemaiavand destul timp pentru a reimplementa aceasta 
		functionalitate, am lasat codul asa.
	Am intampinat dificultati mai mult la coding style decat la implementarea 
		propriu-zisa. Dificultatile intampinate la implementare au fost datorate
		faptului ca, asa cum am spus, am fost nevoita sa rescriu bucati de cod 
		pentru ca foloseam functii nepotrivite care nu mergeau pe ambele sisteme.
	Pe parcursul acestei teme am descoperit functiile care sunt cross-platform.
	La implementarea Hashmapului NU m-am folosit de lab-urile de SD sau orice alte 
		resurse. Am realizat functiile de la zero.
		
	Am implementat HashMapul ca fiind un vector de pointeri catre liste inlantuite 
		ale caror celule cuprind cheie si valoare. Functia de hashcode ia de fapt 
		prima litera din cheie (ASCII) si o mapeaza in una din cele 16 intrari in map.
		
	Pentru retinerea directoarelor am folosit o structura care contine un vector alocat 
		dinamic care contine pointeri catre path-uri si un camp primaryDir care reprezinta 
		directorul in care se afla fisierul de input.
		
Git:
	https://gitlab.cs.pub.ro/giorgiana.balan/l3-so-assignments.git