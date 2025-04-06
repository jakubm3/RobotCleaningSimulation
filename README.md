# 209.1 Robot sprzatajacy
Autorzy: Gabryel Jundzi³³, Jakub Adamczyk, Jakub Mierzejewski

Za³o¿enia projektu:
Symulacja zawiera pokój bêd¹cy prostok¹tem. Sk³ada siê on z przeszkód, pod³ogi i ³adowarki.
Robot sprz¹taj¹cy przemieszcza siê po pod³odze i czyœci j¹. Raz na jakiœ czas musi wróciæ do ³adowarki, aby uzupe³niæ bateriê.

Klasy:
Room:
	pola: int height, int width, (kolekcja) tiles
	metody:
		addRubbish(int howDirty, int id) - brudzi tile o podanym id (lub losowym jeœli nie podanym) o poziom howDirty
		operator<< - wyœwietla aktualn¹ mapê

Robot:
	pola: int id (jego pozycja), batteryLevel 
	metody:

Tile:
	pola: int id (odpowiada pozycji w kolekcji klasy Room)
	metody:

Obstacle(Tile):
	pola:
	metody:

Floor(Tile):
	pola: int cleanliness (0 znaczy czysty, max 9)
	metody: getCleaned(), getDirty(int howDirty)

Charger:
	pola:
	metody: