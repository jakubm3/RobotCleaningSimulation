# 209.1 Robot sprzatajacy
Autorzy: Gabryel Jundzi��, Jakub Adamczyk, Jakub Mierzejewski

Za�o�enia projektu:
Symulacja zawiera pok�j b�d�cy prostok�tem. Sk�ada si� on z przeszk�d, pod�ogi i �adowarki.
Robot sprz�taj�cy przemieszcza si� po pod�odze i czy�ci j�. Raz na jaki� czas musi wr�ci� do �adowarki, aby uzupe�ni� bateri�.

Klasy:
Room:
	pola: int height, int width, (kolekcja) tiles
	metody:
		addRubbish(int howDirty, int id) - brudzi tile o podanym id (lub losowym je�li nie podanym) o poziom howDirty
		operator<< - wy�wietla aktualn� map�

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