// ���������� ��� �����/������
#include <iostream> 
// ���������� ��� ������ � �����������
#include <conio.h> 
// ���������� ��� ������ � ���������� ��������� Windows
#include <windows.h>
// ���������� ��� ������ � ��������� (������������� ���������)
#include <vector> 
// ���������� ��� ������ � �������
#include <fstream> 
using namespace std;

// ����������, ������� ����������, �������� �� ����
bool gameOver;

// ������ �������� ����
const int width = 20;

// ������ �������� ����
const int height = 20;

// ������� ����, ������� ������ � ����
int x, y, fruitX, fruitY, score;

// ������� ��� �������� ������� ������ ����
int tailX[100], tailY[100];

// ����� ������ ����
int nTail;

/// \file
/// \brief ����������� ���� "������"
/// \enum eDirecton
/// \brief ������������ ��� �����������
enum eDirecton { STOP = 0, LEFT, RIGHT, UP, DOWN };

// ������� ����������� ����
eDirecton dir;

// ���������� ��� �������� ������ ���������
int difficulty;

// ���� ��������� ������
bool isSpeedUp = false;

// ������� ��������� ������
int bonusFruitX, bonusFruitY;

// ������ ��� ��������� ��������� ������
int bonusFruitTimer = 0;

// ���� �� ���� ��������� ������
int bonusFruitScore = 0;

// ������������� ��������� ��� ����������� ��������� ����� ����� �� �����
const int emptyw = 0;
const int wall = 1;
const int snake = 2;
const int fruit = 3;
const int bonusFruit = 4;
const int ai_snake = 5;
const int bonus = 6;
const int obstacle = 7;

// ������� ������
int bonusX, bonusY;

// ������� �����������
int obstacleX, obstacleY;

// ��������� ������ ��� �������� ����� ����
int grid[height][width];

/// \struct Record
/// \brief ��������� ��� �������� ������� �������
struct Record {
	string name; ///< ��� ������
	int score; ///< ���� ������
};

// ������� � ���� AI-����
int ai_x, ai_y, ai_score;

// ������� ������ AI-����
int ai_tailX[100], ai_tailY[100];

// ����� ������ AI-����
int ai_nTail;

// ����������� AI-����
eDirecton ai_dir;

/// \fn void AIInput()
/// \brief ������� ��� ����������� ����������� �������� ��-����
void AIInput()
{
	if (ai_x < fruitX)
		ai_dir = RIGHT;
	else if (ai_x > fruitX)
		ai_dir = LEFT;
	else if (ai_y < fruitY)
		ai_dir = DOWN;
	else if (ai_y > fruitY)
		ai_dir = UP;
}

/// \fn void saveRecord(const Record& record)
/// \brief ������� ��� ���������� ������� ������ � ����
/// \param record ������ ������, ������� ����� ���������
void saveRecord(const Record& record)
{
	// ��������� �������� ����� �� ������ � ��������� ����
	ofstream file("records.txt", ios::app);

	// ������� ���� �� ������ � ������������ ����������
	if (file.is_open()) 
	{
		// ���������� � ����� ����� ��� ������ � ��������� ����
		file << record.name << " " << record.score << endl;
		// ��������� ����
		file.close();
	}
	// ����� (�� ������� ������� ����) ������� ��������� �� ������
	else 
	{
		cout << "�� ������� ������� ���� ��� ������." << endl;
	}
}

/// \fn vector<Record> loadRecords()
/// \brief ������� ��� �������� �������� ������� �� �����
/// \return ������ ������� �������
vector<Record> loadRecords()
{
	// ������� ������ ��� �������� �����������
	vector<Record> records;
	// ��������� �������� ����� �� ������ �� ���������� �����
	ifstream file("records.txt");
	// ���� ���� ������, ��
	if (file.is_open()) 
	{
		// ������� �� ����� ��� ������ � ��� ���������
		string name;
		int score;
		while (file >> name >> score)
			// �������� ��������� � ������ ��������
			records.push_back({ name, score });
		// ������� ����
		file.close();
	}
	// ����� (�� ������� ������� ����) ������� ��������� �� ������
	else 
	{
		cout << "�� ������� ������� ���� ��� ������." << endl;
	}
	return records;
}

/// \fn void Setup()
/// \brief ����� ���������� �������� ������ ����
void Setup()
{
	// ������� ����������� � ������� ����
	cout << "����� ���������� � ���� ������!" << endl;
	cout << "����������� 'w' ��� �������� �����, 's' ��� �������� ����, 'a' ��� �������� ����� � 'd' ��� �������� ������." << endl;
	cout << "��������� ������������ �� ������� ��� ����������� �������." << endl;
	cout << "������� 'x' � ����� �����, ����� ����� �� ����." << endl;
	cout << "�������� ������� ��������� (1 - �����, 2 - ������, 3 - ������): ";
	cin >> difficulty;
	cin.ignore(); // ���������� ������ ����� ������ � ������ �����

	// ��������� ��������� ����
	gameOver = false;
	dir = STOP;
	x = width / 2;
	y = height / 2;
	do 
	{
		fruitX = rand() % (width - 6) + 3;
		fruitY = rand() % (height - 6) + 3;
	} while (grid[fruitY][fruitX] != emptyw);
	score = 0;

	// ������������� ��������� ������ ��� ������
	bonusFruitX = -1;
	bonusFruitY = -1;

	// ������������� �����
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
				grid[i][j] = wall;
			else
				grid[i][j] = emptyw;

	// ������������� ����, �����, ����������� � ����� �� �����
	grid[y][x] = snake;
	grid[fruitY][fruitX] = fruit;
	grid[obstacleY][obstacleX] = obstacle;
	grid[bonusY][bonusX] = bonus;

	// ��������� ��������� AI ����
	ai_dir = STOP; // AI ���� �� ��������
	ai_x = width / 2 - 1;  // AI ���� �������� ������� ����� �� ������
	ai_y = height / 2; // AI ���� �������� � ������ �� ���������
	ai_score = 0; // �������� ���� AI ����

	// ��������� ������� ��� ������ � �����������
	bonusX = rand() % width;
	bonusY = rand() % height;
	obstacleX = rand() % width;
	obstacleY = rand() % height;
}

/// \fn void Algorithm()
/// \brief �������, ����������� �������� �������� ����
void Algorithm()
{
	// ��������� ������� ������� ������ ����
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;

	// ���������� ������� ������� ������ ���� � ������ ������� ������� ������
	tailX[0] = x; 

	// ���������� ������� ������� ������ ���� � ������ ������� ������� ������
	tailY[0] = y; 

	// ��������� ������� ���� ��������� ������ ����
	for (int i = 1; i < nTail; i++)
	{
		// ��������� ������� ������� i-�� �������� ������
		prev2X = tailX[i]; 

		// ��������� ������� ������� i-�� �������� ������
		prev2Y = tailY[i]; 
		
		// ���������� � i-� ������� ������ ���������� �������
		tailX[i] = prevX;  
		
		// ���������� � i-� ������� ������ ���������� �������
		tailY[i] = prevY;
		
		// ��������� ���������� ������� ��� ���������� ���� �����
		prevX = prev2X;
		
		// ��������� ���������� ������� ��� ���������� ���� �����
		prevY = prev2Y;
	}

	// ��������� ������� ������ ���� � ����������� �� �����������
	switch (dir)
	{
	case LEFT:
		// ���� ����������� �����, ��������� ���������� x
		x--; 
		break;
	case RIGHT:
		// ���� ����������� ������, ����������� ���������� x
		x++;  
		break;
	case UP:
		// ���� ����������� �����, ��������� ���������� y
		y--; 
		break;
	case DOWN:
		// ���� ����������� ����, ����������� ���������� y
		y++; 
		break;
	default:
		break;
	}

	// ���������, �� ����� �� ���� �� ������� ����:
	// ���� ���� ����� �� ������ �������,
	if (x >= width) 
		// �� ���������� �� �� ����� �������
		x = 0;
	// ���� ���� ����� �� ����� �������,
	else if (x < 0) 
		// �� ���������� �� �� ������ �������
		x = width - 1;
	// ���� ���� ����� �� ������� �������,
	if (y >= height) 
		// �� ���������� �� �� ������ �������
		y = 0;      
	// ���� ���� ����� �� ������ �������,
	else if (y < 0) 
		// �� ���������� �� �� ������� �������
		y = height - 1; 

	// ���������, �� ����������� �� ���� � ����������� �������
	for (int i = 0; i < nTail; i++)
		// ���� ���������� ������ ��������� � ������������ ������-���� �������� ������,
		if (tailX[i] == x && tailY[i] == y) 
			// �� ���� �������������
			gameOver = true; 

	// ���������, ����� �� ���� �����
	// ���� ���������� ������ ��������� � ������������ ������,
	if (x == fruitX && y == fruitY) 
	{
		// �� ����������� ���� �� 10,
		score += 10; 
		// ���������� ����� ���������� ��� ������
		fruitX = rand() % width; 
		fruitY = rand() % height; 
		// � ����������� ����� ������
		nTail++; 
	}

	// ���������, ����� �� ���� �����
	// ���� ���������� ������ ��������� � ������������ ������,
	if (x == bonusX && y == bonusY)
	{
		// �� ����������� ���� �� 50,
		score += 50;
		// � ���������� ����� ���������� ��� ������
		bonusX = rand() % width;
		bonusY = rand() % height;
		// � ����������� ����� ������
		nTail++;
	}

	// ���������, ����������� �� ���� � ������������
	// ���� ���������� ������ ��������� � ������������ �����������,
	if (x == obstacleX && y == obstacleY) 
	{
		// �� ���� �������������
		gameOver = true;
	}

	// �������� � ����������� �� ��������� ����
	switch (difficulty)
	{
	case 1:
		// ���� ��������� 1, �� �������� ���������� 100 ��
		Sleep(isSpeedUp ? 50 : 100);
		break;
	case 2:
		// ���� ��������� 2, �� �������� ���������� 50 ��
		Sleep(isSpeedUp ? 25 : 50);
		break;
	case 3:
		// ���� ��������� 3, �� �������� ���������� 10 ��
		Sleep(isSpeedUp ? 5 : 10);
		break;
	default:
		break;
	}

	// ����������� ������ ��������� ������
	bonusFruitTimer++;

	// ������� �������� ����� ������ 30 �����
	if (bonusFruitTimer % 30 == 0)
	{
		do {
			// ���������� ���������� x ��� ��������� ������,
			bonusFruitX = rand() % (width - 6) + 3; 
			// ���������� ���������� y ��� ��������� ������,
			bonusFruitY = rand() % (height - 6) + 3; 
		} while (grid[bonusFruitY][bonusFruitX] != emptyw);  // ���� �� ������ ������ ������
		// ������������� ���������� ����� �� �������� �����
		bonusFruitScore = 50; 
	}

	// ������� �������� ����� ����� 10 �����
	if (bonusFruitTimer % 40 == 0)
	{
		// ������������� ���������� x ��������� ������ ��� �������� ����
		bonusFruitX = -1; 
		// ������������� ���������� y ��������� ������ ��� �������� ����
		bonusFruitY = -1; 
	}

	// ���������, ����� �� ���� �������� �����
	// ���� ���������� ������ ��������� � ������������ ��������� ������,
	if (x == bonusFruitX && y == bonusFruitY) 
	{
		// �� ����������� ���� �� �������� ��������� ������,
		score += bonusFruitScore;
		// � ������������� ���������� x ��������� ������ ��� �������� ����
		bonusFruitX = -1;
		// � ������������� ���������� y ��������� ������ ��� �������� ����
		bonusFruitY = -1; 
	}

	// ���������, ����������� �� ���� �� ������
	// ���� ���� ����������� �� ������,
	if (grid[y][x] == wall)
	{
		// �� ��������� ���� �� 1,
		score -= 1;
		// � ����������, ��� ���� �� ������ �������������
		if (score < 0) 
			score = 0;

		// ������ ����������� �� ��������������� ��� ������������ �� ������
		switch (dir)
		{
		case LEFT:
			// ���� ���� ��� �����, �� ������ ��� ����� ���� ������
			dir = RIGHT; 
			// ��������������� ����� �� ����� ������������
			grid[y][x] = wall; 
			// ���������� ���� �� ���������� ������
			x++; 
			break;
		case RIGHT:
			// ���� ���� ��� ������, �� ������ ��� ����� ���� �����
			dir = LEFT; 
			// ��������������� ����� �� ����� ������������
			grid[y][x] = wall; 
			// ���������� ���� �� ���������� ������
			x--;
			break;
		case UP:
			// ���� ���� ��� �����, �� ������ ��� ����� ���� ����
			dir = DOWN; 
			// ��������������� ����� �� ����� ������������
			grid[y][x] = wall;
			// ���������� ���� �� ���������� ������
			y++;
			break;
		case DOWN:
			// ���� ���� ��� ����, �� ������ ��� ����� ���� �����
			dir = UP; 
			// ��������������� ����� �� ����� ������������
			grid[y][x] = wall;
			// ���������� ���� �� ���������� ������
			y--;
			break;
		default:
			break;
		}
	}

	// ��������� �����
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i == y && j == x)
				// ���� ���������� ��������� � ������������ ������ ����, �� ���������� � ������ �������� snake
				grid[i][j] = snake; 
			else if (i == fruitY && j == fruitX)
				// ���� ���������� ��������� � ������������ ������, �� ���������� � ������ �������� fruit
				grid[i][j] = fruit;
			else if (i == bonusFruitY && j == bonusFruitX)
				// ���� ���������� ��������� � ������������ ��������� ������, �� ���������� � ������ �������� bonusFruit
				grid[i][j] = bonusFruit;
			else if (grid[i][j] != wall)
				// ���� � ������ �� �����, �� ���������� � ��� �������� emptyw (�����)
				grid[i][j] = emptyw; 
			if (i == ai_y && j == ai_x)
				// ���� ���������� ��������� � ������������ ������ ���� ��, �� ���������� � ������ �������� ai_snake
				grid[i][j] = ai_snake; 
			if (i == obstacleY && j == obstacleX)
				// ���� ���������� ��������� � ������������ �����������, �� ���������� � ������ �������� obstacle
				grid[i][j] = obstacle; 
			if (i == bonusY && j == bonusX)
				// ���� ���������� ��������� � ������������ ������, �� ���������� � ������ �������� bonus
				grid[i][j] = bonus; 
		}
	}

	// �� ����
	prevX = ai_tailX[0];  // ��������� ������� ������� ������ ���� ��
	prevY = ai_tailY[0];

	ai_tailX[0] = ai_x;
	ai_tailY[0] = ai_y;

	for (int i = 1; i < ai_nTail; i++)
	{
		prev2X = ai_tailX[i];
		prev2Y = ai_tailY[i];
		ai_tailX[i] = prevX;
		ai_tailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}

	switch (ai_dir)
	{
	case LEFT:
		ai_x--;
		break;
	case RIGHT:
		ai_x++;
		break;
	case UP:
		ai_y--;
		break;
	case DOWN:
		ai_y++;
		break;
	default:
		break;
	}
	if (ai_x >= width)
		ai_x = 0; else if (ai_x < 0) ai_x = width - 1;
	if (ai_y >= height)
		ai_y = 0; else if (ai_y < 0) ai_y = height - 1;

	for (int i = 0; i < ai_nTail; i++)
		if (ai_tailX[i] == ai_x && ai_tailY[i] == ai_y)
			gameOver = true;

	if (ai_x == fruitX && ai_y == fruitY && grid[fruitY][fruitX] == fruit)
	{
		ai_score += 5;
		fruitX = rand() % width;
		fruitY = rand() % height;
		ai_nTail++;
	}

	// ���������, ����������� �� ��-������ � ������� ������ ������
	for (int i = 0; i < nTail; i++)
		if (tailX[i] == ai_x && tailY[i] == ai_y)
			gameOver = true;

	// ���������, ����������� �� ������ ������ � ������� ��-������
	for (int i = 0; i < ai_nTail; i++)
		if (ai_tailX[i] == x && ai_tailY[i] == y)
			gameOver = true;
}

/// \fn void Draw()
/// \brief ������� ��� ��������� �������� ����
void Draw()
{
	// ������� ������� ��� ������ ����������� �������� ����
	system("cls");

	// ������ ������� ����
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			bool print = false;
			// ���������, ���� �� ����� ������ ������ � ������� ������
			for (int k = 0; k < nTail; k++)
			{
				if (tailX[k] == j && tailY[k] == i)
				{
					cout << "*"; // ���� ��, �� ������ ����� ������ ������
					print = true;
				}
			}

			// ���������, ���� �� ����� ������ ��-������ � ������� ������
			for (int k = 0; k < ai_nTail; k++)
			{
				if (ai_tailX[k] == j && ai_tailY[k] == i)
				{
					cout << "o"; // ���� ��, �� ������ ����� ������ ��-������
					print = true;
				}
			}

			// ���� � ������ ��� ����� ������ ������, ������ ���������� ������
			if (!print)
			{
				// � ����������� �� ����������� ������ ������ ��������������� ������
				switch (grid[i][j])
				{
				case emptyw:
					cout << " "; // ������ ������������
					break;
				case wall:
					cout << "#"; // �����
					break;
				case snake:
					cout << "*"; // ������ ������
					break;
				case ai_snake:
					cout << "o"; // ����� ������ ���� ��
					break;
				case fruit:
					cout << "%"; // �����
					break;
				case bonusFruit:
					cout << "@"; // �������� �����
					break;
				case bonus:
					cout << "$"; // �����
					break;
				case obstacle:
					cout << "X"; // �����������
					break;
				}
			}
		}
		cout << endl; // ��������� �� ����� ������ ����� ��������� ������ ������ �������� ����
	}

	// ������� ���� ������ � ��
	cout << "���� ������:" << score << endl;
	cout << "���� ��:" << ai_score << endl;
}

/// \fn void Input()
/// \brief ������� ��� ��������� ����� ������
void Input()
{
	// ���� ���� ������ �������
	if (_kbhit())
	{
		// � ����������� �� ������� ������� ������ ����������� �������� ����
		switch (_getch())
		{
		case 'a':
			dir = LEFT; // �������� �����
			break;
		case 'd':
			dir = RIGHT; // �������� ������
			break;
		case 'w':
			dir = UP; // �������� �����
			break;
		case 's':
			dir = DOWN; // �������� ����
			break;
		case ' ':
			isSpeedUp = true;
			break;
		case 'x':
			gameOver = true; // ��������� ����
			break;
		}
	}
	else
	{
		// ���� ������� �� ������, �� ������ �������� �� ����������� ���������
		isSpeedUp = false;
	}
}

/// \fn int main()
/// \brief ������� ������� ���������
int main()
{
	system("chcp 1251"); // ������������� ��������� ������� �� Windows-1251 ��� ����������� ����������� ���������
	system("cls"); // ������� �������

	Setup(); // �������������� ����
	while (!gameOver) { // ���� ���� �� ���������
		Draw(); // ������ ������� ����
		Input(); // ������������ ���� ������
		AIInput(); // ������������ ���� ��
		Algorithm(); // ��������� ��������� ����
	}
	cout << "���� ��������! ��� ����: " << score << endl; // ������� ��������� � ���������� ���� � ���� ������

	if (score > ai_score)
		cout << "����� �������!" << endl; // ���� ���� ������ ������ ����� ��, �� ����� �������
	else if (score < ai_score)
		cout << "�� �������!" << endl; // ���� ���� �� ������ ����� ������, �� �� �������
	else
		cout << "�����!" << endl; // ���� ����� �����, �� �����

	cout << "������� ���: ";
	string name;
	cin >> name; // ����������� ��� ������ ��� ���������� ����������
	saveRecord({ name, score }); // ��������� ���������
	cout << "��������� ��������!" << endl;

	cout << "����������:" << endl;
	vector<Record> records = loadRecords(); // ��������� ����������� ����������
	for (const Record& record : records) { // ������� ��� ����������� ����������
		cout << record.name << ": " << record.score << endl;
	}

	system("pause"); // ����������� ���������� ��������� �� ������� ����� �������

	return 0; // ��������� ��������� � ����� 0 (�������� ����������)
}