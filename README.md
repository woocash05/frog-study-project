# frog-study-project
Prosta gra zręcznościowa typu Frogger uruchamiana w terminalu, napisana w ramach kursu Podstawy Programowania. Aplikacja demonstruje implementację pętli gry (game loop) oraz obsługę zdarzeń w czasie rzeczywistym w środowisku konsolowym Windows.

## Funkcje i mechanika
- Sterowanie: Poruszanie postacią (F) w czterech kierunkach przy omijaniu przeszkód (C) oraz blokad (X).
- Dynamiczna trudność: System losowego przyspieszania wybranych rzędów, co wymusza na graczu zmianę tempa rozgrywki.
- Zarządzanie kolorem: Wykorzystanie API systemu Windows do wizualnego rozróżnienia obiektów (gracz, cele, auta o różnej prędkości).
- Konfiguracja zewnętrzna: Gra odczytuje parametry początkowe z pliku tekstowego.

## Technologie i biblioteki
- Jezyk: C++
- Naglowki systemowe:
  - windows.h: zarzadzanie atrybutami konsoli i czasem (Sleep).
  - conio.h: nieblokujaca obsluga wejscia z klawiatury (_kbhit, _getch).
  - stdio.h: obsluga strumieni plikow przy wczytywaniu konfiguracji.
- Srodowisko: Windows Console (CMD / PowerShell).

## Instrukcja uruchomienia

### 1. Przygotowanie pliku konfiguracyjnego
W katalogu z programem musi znajdowac sie plik config.txt. Przykladowa zawartosc:
height=20
width=30
num_cars=12
speed_level=2
num_obstacles=4

### 2. Kompilacja i start
Przy uzyciu kompilatora GCC (MinGW):
g++ main.cpp -o FrogGame.exe
./FrogGame.exe

## Sterowanie
- W, A, S, D - Ruch postacia.
- Q - Wyjscie z programu.

---
Autor: Łukasz Steciuk
