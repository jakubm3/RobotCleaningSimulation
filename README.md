# 209.1 Robot sprzatajacy
Autorzy: Gabryel Jundzi³³, Jakub Adamczyk, Jakub Mierzejewski

Za³o¿enia projektu:
Symulacja zawiera pokój bêd¹cy prostok¹tem. Sk³ada siê on z przeszkód, pod³ogi i ³adowarki.
Robot sprz¹taj¹cy przemieszcza siê po pod³odze i czyœci j¹. Raz na jakiœ czas musi wróciæ do ³adowarki, aby uzupe³niæ bateriê.

Klasy:
Room:
	pola: int height, int width, (kolekcja) tiles, int robot_position
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
		operator<< - wyœwietla jako symbol 'P'

Floor(Tile):
	pola: int cleanliness (0 znaczy czysty, max 9)
	metody: getCleaned(), getDirty(int howDirty)
		operator<< - wyœwietla jako cyfrê odpowiadaj¹c¹ poziomu czystoœci

Charger:
	pola:
	metody:
		operator<< - wyœwietla jako symbol 'B'



Podzia³ obowi¹zków w zespole:



Zadania:
	- Stworzyæ klasê Tile i jego dzieci: Obstacle, Floor, Charger
	- Stworzyæ klasê Room: pobiera uk³ad pomieszczenia z pliku i tworzy kolekcjê
	- Plik g³ówny, który w wywo³aniu przyjmuje œcie¿kê pliku z planem pokoju