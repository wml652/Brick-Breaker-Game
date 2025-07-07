#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
using namespace std;             

const int width = 60;
const int height = 25;
const int maxEnemies = 20;

// Game variables
int playerX, playerY;
int playerHealth;
string playerName;
int paddleX, paddleY;
int paddleWidth = 10;
int ballX, ballY;
int ballDX, ballDY;
bool ballActive;
int score;
int totalScore;
bool gameOver;
bool gameWon;
int level;
int enemiesKilled;
int enemyX[maxEnemies], enemyY[maxEnemies];
bool enemyAlive[maxEnemies];
bool enemyDirection[maxEnemies];
char map[height][width];
bool scoreSaved = false;

void saveScore()

{
    if (!scoreSaved)
    {
        ofstream scoreFile("scores.txt", ios::app);
        if (scoreFile.is_open())
        {
            scoreFile << playerName << " " << totalScore << "\n";
            scoreFile.close();
            scoreSaved = true;
        }
    }
}

void gotoxy(int x, int y)
{
    COORD coord = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void getPlayerName()
{
    cout << "Enter your name: ";
    getline(cin, playerName);
    system("cls");
}

void setup()
{                
    playerX = width / 2;
    playerY = height - 4;
    playerHealth = 3;
    gameOver = false;            
    gameWon = false;          
    enemiesKilled = 0;
    ballActive = false;

    for (int i = 0; i < maxEnemies; i++)                
    {
        enemyX[i] = 2 + i * 2;
        enemyY[i] = 2 + (i % 4);
        enemyDirection[i] = (i % 2 == 0);
        enemyAlive[i] = true;
    }

    paddleX = width / 2 - 5;             
    paddleY = height - 3;
    ballX = width / 2;
    ballY = height - 4;
    ballDX = 1;
    ballDY = -1;
}

void drawMap()              //used to draw the map
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
                map[i][j] = '#';
            else
                map[i][j] = ' ';
        }
    }

    if (playerX >= 1 && playerX < width - 1 && playerY >= 1 && playerY < height - 1)
        map[playerY][playerX] = 'A';

    for (int i = 0; i < maxEnemies; i++)
    {
        if (enemyAlive[i] && enemyX[i] > 0 && enemyX[i] < width - 1 && enemyY[i] > 0 && enemyY[i] < height - 1)
            map[enemyY[i]][enemyX[i]] = 'E';
    }

    for (int i = 0; i < paddleWidth; i++)
    {
        int px = paddleX + i;
        if (px > 0 && px < width - 1 && paddleY > 0 && paddleY < height - 1)
            map[paddleY][px] = '=';
    }

    if (ballActive && ballX > 0 && ballX < width - 1 && ballY > 0 && ballY < height - 1)
        map[ballY][ballX] = 'O';
}

void render()         //helping in drawing of map and also telling the score health and level of the game 
{
    gotoxy(0, 0);
    drawMap();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            cout << map[i][j];
        }
        cout << "\n";
    }
    cout << "Player: " << playerName << "  Score: " << score << "  Total Score: " << totalScore
         << "  Health: " << playerHealth << "  Level: " << level << "  Kills: " << enemiesKilled << "/";
    if (level == 1)
        cout << "5";
    else if (level == 2)
        cout << "10";
    else
        cout << "15";
    cout << "\n";
    if (gameOver)
    {
        cout << "GAME OVER! Press Enter to exit...\n";
        saveScore();
    }
    if (gameWon && level == 3)
    {
        cout << "GAME COMPLETED! Final Score: " << totalScore << "\nPress Enter to exit...\n";
        saveScore();
    }
    else if (gameWon)
    {
        cout << "LEVEL CLEARED! Press Enter to Continue...\n";
    }
}

void moveBall()
{
    if (!ballActive || gameOver)
        return;

    map[ballY][ballX] = ' ';
    ballX += ballDX;
    ballY += ballDY;

    if (ballX <= 1 || ballX >= width - 2)
        ballDX = -ballDX;
    if (ballY <= 1)
        ballDY = -ballDY;

    if (ballY >= paddleY - 1 && ballY <= paddleY + 1 &&
        ballX >= paddleX && ballX < paddleX + paddleWidth)
    {
        ballDY = -ballDY;
        score += 5;
    }

    if (ballY == playerY && ballX == playerX)
    {
        ballDY = -ballDY;
        score += 2;
    }

    for (int i = 0; i < maxEnemies; i++)
    {
        if (enemyAlive[i] && abs(ballY - enemyY[i]) <= 1 && abs(ballX - enemyX[i]) <= 1)
        {
            enemyAlive[i] = false;
            ballDY = -ballDY;
            score += 10;
            enemiesKilled++;

            if ((level == 1 && enemiesKilled >= 5) ||
                (level == 2 && enemiesKilled >= 10) ||
                (level == 3 && enemiesKilled >= 15))
            {
                gameWon = true;
                totalScore += score;
            }
            break;
        }
    }

    if (ballY >= height - 2)
    {
        playerHealth--;
        if (playerHealth <= 0)
        {
            gameOver = true;
            ballActive = false;
        }
        else
        {
            ballActive = false;
        }
    }
}

void moveEnemies()              // initalizing movement of enemies 
{
    for (int i = 0; i < maxEnemies; i++)
    {
          if (!enemyAlive[i])

            continue;
        map[enemyY[i]][enemyX[i]] = ' ';
        if (enemyDirection[i])
            enemyX[i]++;
        else
            enemyX[i]--;

        if (enemyX[i] <= 1)
            enemyDirection[i] = true;
        if (enemyX[i] >= width - 2)
            enemyDirection[i] = false;
    }
}

void input()           // to get which key is preesed from keyboard
{
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 && paddleX > 3)
    {
        paddleX -= 3;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && paddleX + paddleWidth + 3 < width)
    {
        paddleX += 3;
    }
    if (GetAsyncKeyState(VK_UP) & 0x8000 && paddleY > 3)
    {
        paddleY -= 3;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000 && paddleY < height - 4)
    {
        paddleY += 3;        // 1 position paddle slow n=ball fast 
    }
    if (GetAsyncKeyState(VK_RETURN) & 0x8000 && (gameOver || gameWon))
    {
        if (gameWon)
        {
            level++;
            if (level > 3)
            {
                exit(0); // Exit after level 3
            }
            gameWon = false;
            score = 0;
            ballActive = true;
            setup();
        }
        else if (gameOver)
        {
            exit(0); // Exit on game over
        }
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !ballActive && playerHealth > 0 && !gameOver && !gameWon)
    {
        ballX = paddleX + paddleWidth / 2;
        if (ballX >= width - 1)
            ballX = width - 2;
        ballY = paddleY - 1;
        ballDX = 1;
        ballDY = -1;
        ballActive = true;
    }
}

int main()             // thank you very much for viewing this game presentation
{
    hideCursor();
    getPlayerName();
    level = 1;
    totalScore = 0;
    scoreSaved = false;
    setup();

    while (true)
    {
        input();
        if (!gameOver && !gameWon)
        {
            moveBall();
            moveEnemies();
        }
        render();
        int delay = 50 - (level * 3);
        if (delay < 10)
            delay = 10;
        Sleep(delay);
    }

    return 0;
}
