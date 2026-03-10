#include <iostream>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <stdio.h>

#define HEIGHT 40
#define WIDTH 40
#define NUM_CARS 100
#define NUM_OBSTACLES 10
#define MIN_HEIGHT 6
#define MIN_WIDTH 5
#define MIN_NUM_CARS 0
#define MIN_NUM_OBSTACLES 0
#define ACCELERATION_INTERVAL 5*CLOCKS_PER_SEC
#define DRAW_INTERVAL 10*CLOCKS_PER_SEC
#define LANE_FREQUENCY 4
#define COLLISION 1
#define WIN 2
#define CONTINUE 3
#define EXIT 4
#define LIGHTRED_RED 76
#define GREEN_BLACK 10
#define LIGHTGREEN_GREEN 42
#define WHITE_BLACK 15
#define RED_BLACK 12
#define WHITE_GOLD 110
#define RED_TURQOISE 188
#define NUM_ACCELERATION_ROWS 1

using namespace std;

struct Position
{
    int x, y;       //struktura do obsługi układu (x(wysokość-rzędy),y(szerokość-kolumny))
};

struct InputData
{
    int configHeight, configWidth, configNumCars, configLevel, configNumObstacles;      //struktura skupiająca dane wejściowe wczytane z pliku
};

int readConfig(const char* filename, InputData &config)     // wczytanie danych     
{
    FILE* file;

    if (fopen_s(&file, filename, "r") != 0)         // jeśli nie można otworzyć pliku
    {
        cout << "The configuration file could not be opened." << endl;
        return 1;
    }

    if (fscanf_s(file, "height=%d\n", &config.configHeight) != 1 ||    
        fscanf_s(file, "width=%d\n", &config.configWidth) != 1 ||
        fscanf_s(file, "num_cars=%d\n", &config.configNumCars) != 1 ||
        fscanf_s(file, "speed_level=%d\n", &config.configLevel) != 1 ||
        fscanf_s(file, "num_obstacles=%d\n", &config.configNumObstacles) != 1)
    {
        cout << "Error reading configuration file." << endl;
        fclose(file);
        return 1;
    }

    fclose(file);

    return 0;
}

void ConsoleColor(int color)                        //  kolory konsoli
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void initializeBoard(char board[HEIGHT][WIDTH], InputData config)       // inicjalizacja planszy gry
{
    for (int i = 0; i < config.configHeight; i++)
    {
        for (int j = 0; j < config.configWidth; j++)
        {
            if (i == 0 || i == config.configHeight - 1 || j == 0 || j == config.configWidth - 1)
            {
                board[i][j] = '#';
            }
            else if (i % LANE_FREQUENCY == 0)       // ścieżka dla wierszy podzielnych bez reszty przez pewną stałą(4)
            {
                board[i][j] = '='; 
            }
            else
            {
                board[i][j] = '.';
            }
        }
    }
}

void initializeDestination(char board[HEIGHT][WIDTH], Position &destination, InputData config)      // losowe ustawienie punktu docelowego na zerowym wierszu
{
    int randomColumn = 1 + rand() % (config.configWidth - 2);
    destination = { 0, randomColumn };
    board[destination.x][destination.y] = 'D';
}

void startPlaceFrog(char board[HEIGHT][WIDTH], Position &frog, InputData config)        
{
    frog = { config.configHeight - 1, config.configWidth / 2 };     
    board[frog.x][frog.y] = 'F';                                        // pozycja startowa żaby na "środku" dolnej granicy planszy
}

void initializeObstacles(char board[HEIGHT][WIDTH], Position obstacle[NUM_OBSTACLES], InputData config)     // losowe ustawienie przeszkód na ścieżkach
{
    for (int i = 0; i < config.configNumObstacles; i++)
    {
        int randRow = 1 + rand() % (config.configHeight - 2);
        int randCol = 1 + rand() % (config.configWidth - 2);

        while (board[randRow][randCol] == 'X' || randRow % LANE_FREQUENCY != 0)
        {
            randRow = 1 + rand() % (config.configHeight - 2);
            randCol = 1 + rand() % (config.configWidth - 2);
        }

        obstacle[i] = { randRow, randCol };
        board[obstacle[i].x][obstacle[i].y] = 'X';
    }
}

void initializeCars(char board[HEIGHT][WIDTH], Position car[NUM_CARS], InputData config)        // samochody (tam gdzie to możliwe)
{
    for (int i = 0; i < config.configNumCars; i++)
    {
        int randRow = 1 + rand() % (config.configHeight - 2);
        int randCol = 1 + rand() % (config.configWidth - 2);

        while (board[randRow][randCol] == 'C' || randRow % LANE_FREQUENCY == 0)         // gdy pole zajęte przez C lub jest ścieżką
        {
            randRow = 1 + rand() % (config.configHeight - 2);
            randCol = 1 + rand() % (config.configWidth - 2);
        }

        car[i] = { randRow, randCol };
        board[car[i].x][car[i].y] = 'C';
    }
}

void displayBoard(char board[HEIGHT][WIDTH], int elapsedTime, InputData config, bool acceleratedRow[HEIGHT])       // wyświetlenie planszy
{
    for (int i = 0; i < config.configHeight; i++)
    {
        for (int j = 0; j < config.configWidth; j++)
        {
            if (board[i][j] == 'F')
            {
                ConsoleColor(LIGHTGREEN_GREEN);           // wykorzystanie funkcji do ustawiania kolorów
                cout << "F ";
            }
            else if (board[i][j] == 'C' && acceleratedRow[i] == true)
            {
                ConsoleColor(RED_TURQOISE);
                cout << "C ";
            }
            else if (board[i][j] == 'C')
            {
                ConsoleColor(LIGHTRED_RED);
                cout << "C ";
            }
            else if (board[i][j] == '=')
            {
                ConsoleColor(WHITE_BLACK);
                cout << "= ";
            }
            else if (board[i][j] == 'D')
            {
                ConsoleColor(WHITE_GOLD);
                cout << "D ";
            }
            else if (board[i][j] == '.' && acceleratedRow[i] == true)
            {
                ConsoleColor(GREEN_BLACK);
                cout << "> ";
            }
            else if (board[i][j] == 'X')
            {
                ConsoleColor(RED_BLACK);
                cout << "X ";
            }
            else
            {
                ConsoleColor(GREEN_BLACK);
                cout << board[i][j] << " ";
            }
        }
        cout << endl;
    }
    ConsoleColor(WHITE_BLACK);
    cout << endl;
    cout << "czas gry: " << elapsedTime << " sec" << endl;
    cout << "q - quit game" << endl; 
    cout << "Lukasz Steciuk " << endl;
}

void moveFrog(char board[HEIGHT][WIDTH], Position &frog, char direction, InputData config, bool& gameOver, int &gameResult)      // funkcja odpowiedzialna za możliwe ruchy żaby
{
    static char previousChar = '#';         // przechowanie znaku, domyślnie #

    board[frog.x][frog.y] = previousChar;       // poprzedni znak na miejscu żaby

    if (direction == 'a' && frog.y > 1 && board[frog.x][frog.y - 1] == 'C')         // wewnętrzne sprawdzenie kolizji
    {
        frog.y--;
        gameOver = true;
        gameResult = COLLISION;
    }
    else if (direction == 'a' && frog.y > 1 && board[frog.x][frog.y - 1] != 'X')
        frog.y--;

    else if (direction == 'd' && frog.y > 1 && board[frog.x][frog.y+1] == 'C')
    {
        frog.y++;
        gameOver = true;
        gameResult = COLLISION;
    }
    else if (direction == 'd' && frog.y < config.configWidth - 2 && board[frog.x][frog.y + 1] != 'X')
        frog.y++;

    else if (direction == 'w' && frog.y > 1 && board[frog.x - 1][frog.y] == 'C')
    {
        frog.x--;
        gameOver = true;
        gameResult = COLLISION;
    }
    else if (direction == 'w' && frog.x >= 1 && board[frog.x - 1][frog.y] != 'X')
    {
        if (frog.x - 1 > 0 || board[frog.x - 1][frog.y] == 'D')
        {
            frog.x--;
        }
    }

    else if (direction == 's' && frog.y > 1 && board[frog.x + 1][frog.y] == 'C')
    {
        frog.x++;
        gameOver = true;
        gameResult = COLLISION;
    }
    else if (direction == 's' && frog.x < config.configHeight - 2 && board[frog.x + 1][frog.y] != 'X')
        frog.x++;

    previousChar = board[frog.x][frog.y];       // znak pola na które żaba wchodzi

    board[frog.x][frog.y] = 'F';
}

bool checkCollision(Position frog, Position car[NUM_CARS], char board[HEIGHT][WIDTH], InputData config)        // sprawdza kolizję żaby z samochodami
{
    for (int i = 0; i < config.configNumCars; i++)
    {
        if (car[i].x == frog.x && car[i].y == frog.y)
        {
            return true;
        }
    }
    return false;
}

bool checkWin(Position frog, Position destination)      // zwraca prawdę, gdy pozycja żaby pokrywa się z miejscem docelowym 
{
    if (frog.x == destination.x && frog.y == destination.y)
    {
        return true;
    }
}

int check(char board[HEIGHT][WIDTH], Position frog, Position car[NUM_CARS], Position destination, InputData config)        // stan gry 1-Collision, 2-win, 3-continue         
{
    if (checkCollision(frog, car, board, config))
    {
        return COLLISION;
    }
    else if (checkWin(frog, destination))
    {
        return WIN;
    }
    return CONTINUE;
}

void AcceleratedRow(bool acceleratedRow[HEIGHT], clock_t &lastAccelerationTime, InputData config)         // obsługa losowej zmiany prędkości 
{
    clock_t currentTime = clock();

    if (currentTime - lastAccelerationTime >= ACCELERATION_INTERVAL)
    {
        for (int i = 0; i < config.configHeight; i++)                   
        {
            acceleratedRow[i] = false;          // wyzerowanie stanu przyspieszenia dla wszystkich rzędów
        }
        
        for (int i = 0; i < NUM_ACCELERATION_ROWS; i++)
        {
            int randRow = 1 + rand() % (config.configHeight - 2);            // losowanie przyspieszonych rzędów
            while (randRow % LANE_FREQUENCY == 0 || acceleratedRow[randRow])
            {
                randRow = 1 + rand() % (config.configHeight - 2);
            }
            acceleratedRow[randRow] = true;
        }

        lastAccelerationTime = currentTime;         // resetuj licznik czasu dla kolejnego interwału
    }
}

void DrawnCar(char board[HEIGHT][WIDTH], InputData config, clock_t &lastDrawTime, bool drawnCar[NUM_CARS])        // obsługa losowania auta, które zniknie
{
    clock_t currentTime = clock();

    if (currentTime - lastDrawTime >= DRAW_INTERVAL)        // częstość losowania
    {
        
        for (int i = 0; i < config.configNumCars; i++)      
        {
            drawnCar[i] = false;                // "wyzerowanie" na fałsz
        }

        int randCarIndex = rand() % config.configNumCars;       // losowanie indeksu

        drawnCar[randCarIndex] = true;      // oznaczenie wylosowanego

        lastDrawTime = currentTime;         //reset timera
    } 
}

int drawRow( char board[HEIGHT][WIDTH], InputData config)       // obsługa losowania rzędu spawnu znikniętego auta
{
    int newRow;
    bool foundEmpty = false;

    while (!foundEmpty)             // szukamy pustego miejsca w kolumnie 1
    {
        newRow = 1 + rand() % (config.configHeight - 2);        // losowanie wiersza

        if (board[newRow][1] == '.')         // sprawdzamy, czy miejsce jest wolne
        { 
            foundEmpty = true;
        }
    }

    return newRow;
}

void processCarMovement(char board[HEIGHT][WIDTH], Position &frog, Position car[NUM_CARS], clock_t lastMoveTime[HEIGHT], InputData config, bool acceleratedRow[HEIGHT], clock_t &lastAccelerationTime, clock_t &lastDrawTime, bool drawnCar[NUM_CARS])           // porusza samochody w odpowiednim czasie
{
    static int stopRow = 1 + rand() % (config.configHeight - 2);            // losowanie wiersza "stop"
    while (stopRow % LANE_FREQUENCY == 0)                           // nie może to być ścieżka, bo nie ma tam czego stopować
    {
        stopRow = 1 + rand() % (config.configHeight - 2);
    }

    AcceleratedRow(acceleratedRow, lastAccelerationTime, config);         // wywołanie funkcji przyspieszającej 
    DrawnCar(board, config, lastDrawTime, drawnCar);              // wywołanie funkcji losującej znikające auto

    for (int i = 0; i < config.configHeight; i++)       // obsługa poruszania się samochodów, opierająca się na "poruszaniu rzędami (parzyste-nieparzyste)"
    {
        int moveInterval = (i % 2 == 0) ? CLOCKS_PER_SEC / (2 * config.configLevel) : CLOCKS_PER_SEC / config.configLevel;

        if (acceleratedRow[i])
        {
            moveInterval /= 2;          // stosowne skrócenie interwału ruchu dla przyspieszonego rzędu - szybciej się porusza
        }

        if (clock() - lastMoveTime[i] >= moveInterval)      // timer mówiący o tym kiedy poruszał się poszczególny rząd 
        {
            for (int j = 0; j < config.configNumCars; j++)
            {
                if (i == stopRow && (frog.x == stopRow + 1 || frog.x == stopRow))
                {
                    continue;                                    
                }
                else if (car[j].x == i && drawnCar[j])      //  przypadek znikającego auta
                {
                    
                    board[car[j].x][car[j].y] = '.';

                    car[j].y++;                                 //przesunięcie

                    if (car[j].y >= config.configWidth - 1)
                    {
                        car[j].x = drawRow(board, config);      //nowy rząd
                        car[j].y = 1;                       //wrapping
                    }

                }
                else if (car[j].x == i)     // normalny ruch
                {

                    board[car[j].x][car[j].y] = '.';

                    car[j].y++;
                    
                    if (car[j].y >= config.configWidth - 1)
                    {
                        car[j].y = 1;  
                    }

                }
            }

            for (int j = 0; j < config.configNumCars; j++)
            {
                board[car[j].x][car[j].y] = 'C';
            }

            lastMoveTime[i] = clock();      // reset timera
        }
    }
}

void status(char board[HEIGHT][WIDTH], bool &gameOver, int &gameResult, Position frog, Position car[NUM_CARS], Position destination, InputData config  )
{
    gameResult = check(board, frog, car, destination, config);
    if (gameResult == COLLISION || gameResult == WIN)           // gdy gra ma być zakończona
    {
        gameOver = true;
    }
}

void processFrogInput(char board[HEIGHT][WIDTH], Position& frog, bool& gameOver, int& gameResult, InputData config)       //// Wczytuje wartości z klawiatury dla żaby i wykonuje jej ruch
{
    if (_kbhit() && !gameOver)      //gdy jest bufor z klawiatury i gra się toczy
    {
        char direction = _getch();

        if (direction == 'q')
        {
            gameOver = true;
            gameResult = EXIT;
            return;
        }

        moveFrog(board, frog, direction, config, gameOver, gameResult);         //obsługa ruchu żaby
    }
}

void announcement(int gameResult)       // wypisuje rezultat
{
    switch (gameResult) 
    {
    case COLLISION:
        cout << "Game over!" << endl;
        break;
    case WIN:
        cout << "You won!" << endl;
        break;
    case EXIT:
        cout << "User has left the game." << endl;
        break;
    }
}

void updateBoard(char board[HEIGHT][WIDTH], int elapsedTime, InputData config, bool acceleratedRow[HEIGHT], bool drawnCar[NUM_CARS])       // Wyświetla stan końcowy
{
    system("cls");
    displayBoard(board, elapsedTime, config, acceleratedRow);
}

void game(char board[HEIGHT][WIDTH], Position &frog, Position car[NUM_CARS], time_t startTime, Position& destination, InputData config, bool acceleratedRow[HEIGHT], clock_t lastAccelerationTime, clock_t &lastDrawTime, bool drawnCar[NUM_CARS])          // główna pętla
{
    clock_t lastMoveTime[HEIGHT] = { 0 };
    bool gameOver = false;
    int gameResult = 0;

    while (!gameOver)
    {
        time_t currentTime = time(0);
        int elapsedTime = currentTime - startTime;          // licznik czasu trawania gry

        updateBoard(board, elapsedTime, config, acceleratedRow, drawnCar);

        processCarMovement(board, frog, car, lastMoveTime, config, acceleratedRow, lastAccelerationTime, lastDrawTime, drawnCar);

        status(board, gameOver, gameResult, frog, car, destination, config);
        
        updateBoard(board, elapsedTime, config, acceleratedRow, drawnCar);

        processFrogInput(board, frog, gameOver, gameResult, config);

        if (gameOver)
        {
            updateBoard(board, elapsedTime, config, acceleratedRow, drawnCar);
            break;
        }
        Sleep(50);
    }
    announcement(gameResult);
}

int checkConfig(InputData config)            // weryfikacja poprawności wartości z pliku
{
    if (config.configHeight > HEIGHT || config.configHeight < MIN_HEIGHT)
        return 1;
    if (config.configWidth > WIDTH || config.configWidth < MIN_WIDTH)
        return 1;
    if (config.configNumCars > NUM_CARS || config.configNumCars < MIN_NUM_CARS)
        return 1;
    if (config.configNumObstacles > NUM_OBSTACLES || config.configNumObstacles < MIN_NUM_OBSTACLES)
        return 1;
    if (config.configNumCars > ((config.configHeight - 2) / 2) * ((config.configWidth - 2) / 2) || config.configNumCars>100)
        return 1;
    if (config.configNumObstacles > ((config.configWidth-2)*(config.configHeight/4)/4) || config.configNumObstacles>10)
        return 1;

    return 0;
}

int main()
{
    InputData config;

    if (readConfig("config.txt", config) != 0)      // odczyt pliku 
    {
        return 1;           // błąd i zakończenie programu 
    }

    if (checkConfig(config) != 0)
    {
        cout << "Values in the configuration file are incorrect." << endl;
        return 1;                                                               // błąd i zakończenie programu 
    }

    srand(time(0));

    char board[HEIGHT][WIDTH];

    Position frog, car[NUM_CARS], destination, obstacle[NUM_OBSTACLES];

    clock_t lastAccelerationTime = clock(), lastDrawTime = clock();

    bool acceleratedRow[HEIGHT] = { false }, drawnCar[NUM_CARS] = { false };

    // funkcje inicjalizacyjne
    initializeBoard(board, config);
    initializeDestination(board, destination, config);
    startPlaceFrog(board, frog, config);
    initializeObstacles(board, obstacle, config);
    initializeCars(board, car, config);

    time_t startTime = time(0);

    // funkcja-serce (kluczowa, spina działanie)
    game(board, frog, car, startTime, destination, config, acceleratedRow, lastAccelerationTime, lastDrawTime, drawnCar);
}