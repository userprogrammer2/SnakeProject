// Библиотека для ввода/вывода
#include <iostream> 
// Библиотека для работы с клавиатурой
#include <conio.h> 
// Библиотека для работы с системными функциями Windows
#include <windows.h>
// Библиотека для работы с векторами (динамическими массивами)
#include <vector> 
// Библиотека для работы с файлами
#include <fstream> 
using namespace std;

// Переменная, которая определяет, окончена ли игра
bool gameOver;

// Ширина игрового поля
const int width = 20;

// Высота игрового поля
const int height = 20;

// Позиция змеи, позиция фрукта и счет
int x, y, fruitX, fruitY, score;

// Массивы для хранения позиций хвоста змеи
int tailX[100], tailY[100];

// Длина хвоста змеи
int nTail;

/// \file
/// \brief Определение игры "Змейка"
/// \enum eDirecton
/// \brief Перечисление для направлений
enum eDirecton { STOP = 0, LEFT, RIGHT, UP, DOWN };

// Текущее направление змеи
eDirecton dir;

// Переменная для хранения уровня сложности
int difficulty;

// Флаг ускорения змейки
bool isSpeedUp = false;

// Позиция бонусного фрукта
int bonusFruitX, bonusFruitY;

// Таймер для появления бонусного фрукта
int bonusFruitTimer = 0;

// Счет за сбор бонусного фрукта
int bonusFruitScore = 0;

// Целочисленные константы для обозначения различных типов ячеек на сетке
const int emptyw = 0;
const int wall = 1;
const int snake = 2;
const int fruit = 3;
const int bonusFruit = 4;
const int ai_snake = 5;
const int bonus = 6;
const int obstacle = 7;

// Позиция бонуса
int bonusX, bonusY;

// Позиция препятствия
int obstacleX, obstacleY;

// Двумерный массив для хранения сетки игры
int grid[height][width];

/// \struct Record
/// \brief Структура для хранения записей игроков
struct Record {
	string name; ///< Имя игрока
	int score; ///< Счет игрока
};

// Позиция и счет AI-змеи
int ai_x, ai_y, ai_score;

// Позиции хвоста AI-змеи
int ai_tailX[100], ai_tailY[100];

// Длина хвоста AI-змеи
int ai_nTail;

// Направление AI-змеи
eDirecton ai_dir;

/// \fn void AIInput()
/// \brief Функция для определения направления движения ИИ-змеи
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
/// \brief Функция для сохранения рекорда игрока в файл
/// \param record Запись игрока, которую нужно сохранить
void saveRecord(const Record& record)
{
	// Открываем файловый поток на запись в текстовый файл
	ofstream file("records.txt", ios::app);

	// Открыть файл на запись с возможностью дополнения
	if (file.is_open()) 
	{
		// Записываем в конец файла имя игрока и результат игры
		file << record.name << " " << record.score << endl;
		// Закрываем файл
		file.close();
	}
	// Иначе (не удалось открыть файл) выводим сообщение об ошибке
	else 
	{
		cout << "Не удалось открыть файл для записи." << endl;
	}
}

/// \fn vector<Record> loadRecords()
/// \brief Функция для загрузки рекордов игроков из файла
/// \return Вектор записей игроков
vector<Record> loadRecords()
{
	// Создаем вектор для хранения результатов
	vector<Record> records;
	// Открываем файловый поток на чтение из текстового файла
	ifstream file("records.txt");
	// Если файл открыт, то
	if (file.is_open()) 
	{
		// Считать из файла имя игрока и его результат
		string name;
		int score;
		while (file >> name >> score)
			// Добавить результат в массив рекордов
			records.push_back({ name, score });
		// Закрыть файл
		file.close();
	}
	// Иначе (не удалось открыть файл) выводим сообщение об ошибке
	else 
	{
		cout << "Не удалось открыть файл для чтения." << endl;
	}
	return records;
}

/// \fn void Setup()
/// \brief Здесь начинается основная логика игры
void Setup()
{
	// Выводим приветствие и правила игры
	cout << "Добро пожаловать в игру Змейка!" << endl;
	cout << "Используйте 'w' для движения вверх, 's' для движения вниз, 'a' для движения влево и 'd' для движения вправо." << endl;
	cout << "Избегайте столкновений со стенами или собственным хвостом." << endl;
	cout << "Нажмите 'x' в любое время, чтобы выйти из игры." << endl;
	cout << "Выберите уровень сложности (1 - Легко, 2 - Средне, 3 - Трудно): ";
	cin >> difficulty;
	cin.ignore(); // Игнорируем символ новой строки в буфере ввода

	// Начальные настройки игры
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

	// Инициализация бонусного фрукта вне экрана
	bonusFruitX = -1;
	bonusFruitY = -1;

	// Инициализация сетки
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (i == 0 || i == height - 1 || j == 0 || j == width - 1)
				grid[i][j] = wall;
			else
				grid[i][j] = emptyw;

	// Устанавливаем змею, фрукт, препятствие и бонус на сетку
	grid[y][x] = snake;
	grid[fruitY][fruitX] = fruit;
	grid[obstacleY][obstacleX] = obstacle;
	grid[bonusY][bonusX] = bonus;

	// Начальные настройки AI змеи
	ai_dir = STOP; // AI змея не движется
	ai_x = width / 2 - 1;  // AI змея начинает немного левее от игрока
	ai_y = height / 2; // AI змея начинает в центре по вертикали
	ai_score = 0; // Обнуляем счет AI змеи

	// Случайные позиции для бонуса и препятствия
	bonusX = rand() % width;
	bonusY = rand() % height;
	obstacleX = rand() % width;
	obstacleY = rand() % height;
}

/// \fn void Algorithm()
/// \brief Функция, реализующая основной алгоритм игры
void Algorithm()
{
	// Сохраняем текущую позицию головы змеи
	int prevX = tailX[0];
	int prevY = tailY[0];
	int prev2X, prev2Y;

	// Записываем текущую позицию головы змеи в первый элемент массива хвоста
	tailX[0] = x; 

	// Записываем текущую позицию головы змеи в первый элемент массива хвоста
	tailY[0] = y; 

	// Обновляем позиции всех элементов хвоста змеи
	for (int i = 1; i < nTail; i++)
	{
		// Сохраняем текущую позицию i-го элемента хвоста
		prev2X = tailX[i]; 

		// Сохраняем текущую позицию i-го элемента хвоста
		prev2Y = tailY[i]; 
		
		// Записываем в i-й элемент хвоста предыдущую позицию
		tailX[i] = prevX;  
		
		// Записываем в i-й элемент хвоста предыдущую позицию
		tailY[i] = prevY;
		
		// Обновляем предыдущую позицию для следующего шага цикла
		prevX = prev2X;
		
		// Обновляем предыдущую позицию для следующего шага цикла
		prevY = prev2Y;
	}

	// Обновляем позицию головы змеи в зависимости от направления
	switch (dir)
	{
	case LEFT:
		// Если направление влево, уменьшаем координату x
		x--; 
		break;
	case RIGHT:
		// Если направление вправо, увеличиваем координату x
		x++;  
		break;
	case UP:
		// Если направление вверх, уменьшаем координату y
		y--; 
		break;
	case DOWN:
		// Если направление вниз, увеличиваем координату y
		y++; 
		break;
	default:
		break;
	}

	// Проверяем, не вышла ли змея за границы поля:
	// Если змея вышла за правую границу,
	if (x >= width) 
		// то перемещаем ее на левую границу
		x = 0;
	// Если змея вышла за левую границу,
	else if (x < 0) 
		// то перемещаем ее на правую границу
		x = width - 1;
	// Если змея вышла за верхнюю границу,
	if (y >= height) 
		// то перемещаем ее на нижнюю границу
		y = 0;      
	// Если змея вышла за нижнюю границу,
	else if (y < 0) 
		// то перемещаем ее на верхнюю границу
		y = height - 1; 

	// Проверяем, не столкнулась ли змея с собственным хвостом
	for (int i = 0; i < nTail; i++)
		// Если координаты головы совпадают с координатами какого-либо элемента хвоста,
		if (tailX[i] == x && tailY[i] == y) 
			// то игра заканчивается
			gameOver = true; 

	// Проверяем, съела ли змея фрукт
	// Если координаты головы совпадают с координатами фрукта,
	if (x == fruitX && y == fruitY) 
	{
		// то увеличиваем счет на 10,
		score += 10; 
		// генерируем новые координаты для фрукта
		fruitX = rand() % width; 
		fruitY = rand() % height; 
		// и увеличиваем длину хвоста
		nTail++; 
	}

	// Проверяем, съела ли змея бонус
	// Если координаты головы совпадают с координатами бонуса,
	if (x == bonusX && y == bonusY)
	{
		// то увеличиваем счет на 50,
		score += 50;
		// и генерируем новые координаты для бонуса
		bonusX = rand() % width;
		bonusY = rand() % height;
		// и увеличиваем длину хвоста
		nTail++;
	}

	// Проверяем, столкнулась ли змея с препятствием
	// Если координаты головы совпадают с координатами препятствия,
	if (x == obstacleX && y == obstacleY) 
	{
		// то игра заканчивается
		gameOver = true;
	}

	// Задержка в зависимости от сложности игры
	switch (difficulty)
	{
	case 1:
		// Если сложность 1, то задержка составляет 100 мс
		Sleep(isSpeedUp ? 50 : 100);
		break;
	case 2:
		// Если сложность 2, то задержка составляет 50 мс
		Sleep(isSpeedUp ? 25 : 50);
		break;
	case 3:
		// Если сложность 3, то задержка составляет 10 мс
		Sleep(isSpeedUp ? 5 : 10);
		break;
	default:
		break;
	}

	// Увеличиваем таймер бонусного фрукта
	bonusFruitTimer++;

	// Создаем бонусный фрукт каждые 30 ходов
	if (bonusFruitTimer % 30 == 0)
	{
		do {
			// Генерируем координату x для бонусного фрукта,
			bonusFruitX = rand() % (width - 6) + 3; 
			// генерируем координату y для бонусного фрукта,
			bonusFruitY = rand() % (height - 6) + 3; 
		} while (grid[bonusFruitY][bonusFruitX] != emptyw);  // пока не найдем пустую ячейку
		// Устанавливаем количество очков за бонусный фрукт
		bonusFruitScore = 50; 
	}

	// Удаляем бонусный фрукт после 10 ходов
	if (bonusFruitTimer % 40 == 0)
	{
		// Устанавливаем координату x бонусного фрукта вне игрового поля
		bonusFruitX = -1; 
		// Устанавливаем координату y бонусного фрукта вне игрового поля
		bonusFruitY = -1; 
	}

	// Проверяем, съела ли змея бонусный фрукт
	// Если координаты головы совпадают с координатами бонусного фрукта,
	if (x == bonusFruitX && y == bonusFruitY) 
	{
		// то увеличиваем счет на значение бонусного фрукта,
		score += bonusFruitScore;
		// и устанавливаем координату x бонусного фрукта вне игрового поля
		bonusFruitX = -1;
		// и устанавливаем координату y бонусного фрукта вне игрового поля
		bonusFruitY = -1; 
	}

	// Проверяем, столкнулась ли змея со стеной
	// Если змея столкнулась со стеной,
	if (grid[y][x] == wall)
	{
		// то уменьшаем счет на 1,
		score -= 1;
		// и убеждаемся, что счет не станет отрицательным
		if (score < 0) 
			score = 0;

		// Меняем направление на противоположное при столкновении со стеной
		switch (dir)
		{
		case LEFT:
			// Если змея шла влево, то теперь она будет идти вправо
			dir = RIGHT; 
			// Восстанавливаем стену на месте столкновения
			grid[y][x] = wall; 
			// Возвращаем змею на предыдущую ячейку
			x++; 
			break;
		case RIGHT:
			// Если змея шла вправо, то теперь она будет идти влево
			dir = LEFT; 
			// Восстанавливаем стену на месте столкновения
			grid[y][x] = wall; 
			// Возвращаем змею на предыдущую ячейку
			x--;
			break;
		case UP:
			// Если змея шла вверх, то теперь она будет идти вниз
			dir = DOWN; 
			// Восстанавливаем стену на месте столкновения
			grid[y][x] = wall;
			// Возвращаем змею на предыдущую ячейку
			y++;
			break;
		case DOWN:
			// Если змея шла вниз, то теперь она будет идти вверх
			dir = UP; 
			// Восстанавливаем стену на месте столкновения
			grid[y][x] = wall;
			// Возвращаем змею на предыдущую ячейку
			y--;
			break;
		default:
			break;
		}
	}

	// Обновляем сетку
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (i == y && j == x)
				// Если координаты совпадают с координатами головы змеи, то записываем в ячейку значение snake
				grid[i][j] = snake; 
			else if (i == fruitY && j == fruitX)
				// Если координаты совпадают с координатами фрукта, то записываем в ячейку значение fruit
				grid[i][j] = fruit;
			else if (i == bonusFruitY && j == bonusFruitX)
				// Если координаты совпадают с координатами бонусного фрукта, то записываем в ячейку значение bonusFruit
				grid[i][j] = bonusFruit;
			else if (grid[i][j] != wall)
				// Если в ячейке не стена, то записываем в нее значение emptyw (пусто)
				grid[i][j] = emptyw; 
			if (i == ai_y && j == ai_x)
				// Если координаты совпадают с координатами головы змеи ИИ, то записываем в ячейку значение ai_snake
				grid[i][j] = ai_snake; 
			if (i == obstacleY && j == obstacleX)
				// Если координаты совпадают с координатами препятствия, то записываем в ячейку значение obstacle
				grid[i][j] = obstacle; 
			if (i == bonusY && j == bonusX)
				// Если координаты совпадают с координатами бонуса, то записываем в ячейку значение bonus
				grid[i][j] = bonus; 
		}
	}

	// ИИ змея
	prevX = ai_tailX[0];  // Сохраняем текущую позицию головы змеи ИИ
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

	// Проверяем, столкнулась ли ИИ-змейка с хвостом змейки игрока
	for (int i = 0; i < nTail; i++)
		if (tailX[i] == ai_x && tailY[i] == ai_y)
			gameOver = true;

	// Проверяем, столкнулась ли змейка игрока с хвостом ИИ-змейки
	for (int i = 0; i < ai_nTail; i++)
		if (ai_tailX[i] == x && ai_tailY[i] == y)
			gameOver = true;
}

/// \fn void Draw()
/// \brief Функция для отрисовки игрового поля
void Draw()
{
	// Очищаем консоль для нового отображения игрового поля
	system("cls");

	// Рисуем игровое поле
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			bool print = false;
			// Проверяем, есть ли часть хвоста змейки в текущей ячейке
			for (int k = 0; k < nTail; k++)
			{
				if (tailX[k] == j && tailY[k] == i)
				{
					cout << "*"; // Если да, то рисуем часть хвоста змейки
					print = true;
				}
			}

			// Проверяем, есть ли часть хвоста ИИ-змейки в текущей ячейке
			for (int k = 0; k < ai_nTail; k++)
			{
				if (ai_tailX[k] == j && ai_tailY[k] == i)
				{
					cout << "o"; // Если да, то рисуем часть хвоста ИИ-змейки
					print = true;
				}
			}

			// Если в ячейке нет части хвоста змейки, рисуем содержимое ячейки
			if (!print)
			{
				// В зависимости от содержимого ячейки рисуем соответствующий символ
				switch (grid[i][j])
				{
				case emptyw:
					cout << " "; // Пустое пространство
					break;
				case wall:
					cout << "#"; // Стена
					break;
				case snake:
					cout << "*"; // Голова змейки
					break;
				case ai_snake:
					cout << "o"; // Часть хвоста змеи ИИ
					break;
				case fruit:
					cout << "%"; // Фрукт
					break;
				case bonusFruit:
					cout << "@"; // Бонусный фрукт
					break;
				case bonus:
					cout << "$"; // Бонус
					break;
				case obstacle:
					cout << "X"; // Препятствие
					break;
				}
			}
		}
		cout << endl; // Переходим на новую строку после отрисовки каждой строки игрового поля
	}

	// Выводим счет игрока и ИИ
	cout << "Счет игрока:" << score << endl;
	cout << "Счет ИИ:" << ai_score << endl;
}

/// \fn void Input()
/// \brief Функция для обработки ввода игрока
void Input()
{
	// Если была нажата клавиша
	if (_kbhit())
	{
		// В зависимости от нажатой клавиши меняем направление движения змеи
		switch (_getch())
		{
		case 'a':
			dir = LEFT; // Движение влево
			break;
		case 'd':
			dir = RIGHT; // Движение вправо
			break;
		case 'w':
			dir = UP; // Движение вверх
			break;
		case 's':
			dir = DOWN; // Движение вниз
			break;
		case ' ':
			isSpeedUp = true;
			break;
		case 'x':
			gameOver = true; // Завершаем игру
			break;
		}
	}
	else
	{
		// Если клавиша не нажата, то змейка движется со стандартной скоростью
		isSpeedUp = false;
	}
}

/// \fn int main()
/// \brief Главная функция программы
int main()
{
	system("chcp 1251"); // Устанавливаем кодировку консоли на Windows-1251 для корректного отображения кириллицы
	system("cls"); // Очищаем консоль

	Setup(); // Инициализируем игру
	while (!gameOver) { // Пока игра не завершена
		Draw(); // Рисуем игровое поле
		Input(); // Обрабатываем ввод игрока
		AIInput(); // Обрабатываем ввод ИИ
		Algorithm(); // Обновляем состояние игры
	}
	cout << "Игра окончена! Ваш счет: " << score << endl; // Выводим сообщение о завершении игры и счет игрока

	if (score > ai_score)
		cout << "Игрок выиграл!" << endl; // Если счет игрока больше счета ИИ, то игрок выиграл
	else if (score < ai_score)
		cout << "ИИ выиграл!" << endl; // Если счет ИИ больше счета игрока, то ИИ выиграл
	else
		cout << "Ничья!" << endl; // Если счеты равны, то ничья

	cout << "Введите имя: ";
	string name;
	cin >> name; // Запрашиваем имя игрока для сохранения результата
	saveRecord({ name, score }); // Сохраняем результат
	cout << "Результат сохранен!" << endl;

	cout << "Результаты:" << endl;
	vector<Record> records = loadRecords(); // Загружаем сохраненные результаты
	for (const Record& record : records) { // Выводим все сохраненные результаты
		cout << record.name << ": " << record.score << endl;
	}

	system("pause"); // Задерживаем выполнение программы до нажатия любой клавиши

	return 0; // Завершаем программу с кодом 0 (успешное завершение)
}