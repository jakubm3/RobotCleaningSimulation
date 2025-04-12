# 209.1 Robot sprzatajacy
Autorzy: Gabryel Jundzill, Jakub Adamczyk, Jakub Mierzejewski

Zalozenia projektu:
Symulacja zawiera pokoj bedacy prostokatem. Sklada sie on z przeszkod, podlogi i ladowarki.
Robot sprzatajacy przemieszcza sie po podlodze i czysci ja. Raz na jakis czas musi wrocic do ladowarki, aby uzupe³nic baterie.

Klasy:
Room:
	pola: int height, int width, (kolekcja) tiles, int robot_position
	metody:
		addRubbish(int howDirty, int id) - brudzi tile o podanym id (lub losowym jesli nie podanym) o poziom howDirty
		operator<< - wyswietla aktualna mape

Robot:
	pola: int id (jego pozycja), batteryLevel 
	metody:

Tile:
	pola: int id (odpowiada pozycji w kolekcji klasy Room)
	metody:

Obstacle(Tile):
	pola:
	metody:
		operator<< - wyswietla jako symbol 'P'

Floor(Tile):
	pola: int cleanliness (0 znaczy czysty, max 9)
	metody: getCleaned(), getDirty(int howDirty)
		operator<< - wyswietla jako cyfre odpowiadajsca poziomu czystosci

Charger:
	pola:
	metody:
		operator<< - wyswietla jako symbol 'B'



Podzial obowiazkow w zespole:



Zadania:
	- Stworzyc klase Tile i jego dzieci: Obstacle, Floor, Charger
	- Stworzyc klase Room: pobiera uk³ad pomieszczenia z pliku i tworzy kolekcje
	- Plik glowny, ktory w wywolaniu przyjmuje sciezke pliku z planem pokoju