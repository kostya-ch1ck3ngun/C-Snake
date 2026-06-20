#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <math.h>

#define MAP_SIZE 25

//coordinates
typedef struct Coordinates Coordinates;

//coordinates func
Coordinates* createCoordinates(int x, int y);

//queue
typedef struct Node Node;
typedef struct Queue Queue;

//queue func
Node* createNode(Coordinates* coords);
Queue* createQueue();
void push(Queue* queue, Coordinates* coords);
Coordinates* poll(Queue* queue);
void deleteQueue(Queue* queue);

//snake
typedef enum Direction Direction;
typedef enum Skin Skin;
typedef struct Snake Snake;

//snake func
Snake* createSnake();
void deleteSnake(Snake* snake);

//game
typedef enum Difficult Difficult;
typedef struct Game Game;

//game func
int** createMatrix();
void deleteMatrix(int** matrix);
Game* createGame();
void setup();
void createApple(int** matrix);
Coordinates* checkNext(Direction dir, Node* head);
bool move(Game* game);
void drawGame(Game* game);
void startGame();
void gameProcess(Game* game);
void endGame(Game* game);

//menu
void skinMenu();
void difficultyMenu();
void mainMenu();



enum Difficult {
    EASY,
    MIDDLE,
    HARD,
    EXTREME
};

enum Skin {
    CIRCLE,
    ALPHABET,
    SQUARE
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

static int bestScore = 0;
static int timeBetweenTicks = 1000;
static bool isHard = false;
static Direction prevDir = RIGHT;
static Difficult diff = EASY;
static Skin skin = CIRCLE;

struct Coordinates {
    int x;
    int y;
};

Coordinates* createCoordinates(int x, int y) {
    Coordinates* coords = (Coordinates*)malloc(sizeof(Coordinates));
    if (!coords) return NULL;

    coords->x = x;
    coords->y = y;

    return coords;
}

struct Node {
    Node* next;
    Node* prev;
    Coordinates* coords;
};

struct Queue {
    Node* head;
    Node* tail;
    int size;
};

Node* createNode(Coordinates* coords) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    node->next = NULL;
    node->prev = NULL;
    node->coords = coords;

    return node;
}

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if (!queue) return NULL;

    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return queue;
}

void push(Queue* queue, Coordinates* coords) {
    Node* node = createNode(coords);

    if (queue->head == NULL) {
        queue->tail = node;
    }
    else {
        queue->head->prev = node;
    }

    node->next = queue->head;
    queue->head = node;
    queue->size++;
}

Coordinates* poll(Queue* queue) {
    Node* node = queue->tail;

    if (node == NULL) {
        return NULL;
    }

    Coordinates* coords = node->coords;

    if (node == queue->head) {
        queue->head = NULL;
    }
    queue->tail = node->prev;
    if (queue->tail) {
        queue->tail->next = NULL;
    }
    queue->size--;

    free(node);

    return coords;
}

void deleteQueue(Queue* queue) {
    while (queue->size > 0) {
        Coordinates* coords = poll(queue);
        free(coords);
    }
    free(queue);
}

struct Snake {
    Queue* queue;
    Direction dir;
};

Snake* createSnake() {
    Snake* snake = (Snake*)malloc(sizeof(Snake));
    if (!snake) return NULL;

    snake->dir = RIGHT;
    snake->queue = createQueue();

    return snake;
}

void deleteSnake(Snake* snake) {
    deleteQueue(snake->queue);
    free(snake);
}

struct Game {
    Snake* snake;
    int** matrix;
    int score;
};

int** createMatrix() {
    int** matrix = (int**)malloc(sizeof(int*) * MAP_SIZE);
    if (!matrix) {
        printf("Ошибка выделения памяти!\n");
        return NULL;
    }

    for (int i = 0; i < MAP_SIZE; i++) {
        matrix[i] = (int*)calloc(MAP_SIZE, sizeof(int));
        if (!matrix[i]) {
            for (int i = 0; i < MAP_SIZE; i++) {
                free(matrix[i]);
            }
            free(matrix);
            printf("Ошибка выделения памяти!\n");
            return NULL;
        }
    }

    return matrix;
}

void deleteMatrix(int** matrix) {
    for (int i = 0; i < MAP_SIZE; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

Game* createGame() {
    Game* game = (Game*)malloc(sizeof(Game));
    if (!game) return NULL;

    game->matrix = createMatrix();
    game->snake = createSnake();
    game->score = 0;

    return game;
}

void setup() {
    switch (diff) {
    case EASY:
        isHard = false;
        timeBetweenTicks = 500;
        break;
    case MIDDLE:
        isHard = false;
        timeBetweenTicks = 250;
        break;
    case HARD:
        isHard = true;
        timeBetweenTicks = 250;
        break;
    case EXTREME:
        isHard = true;
        timeBetweenTicks = 125;
        break;
    }
}

void createApple(int** matrix) {
    int row, col;
    do {
        row = rand() % MAP_SIZE;
        col = rand() % MAP_SIZE;
    } while (matrix[row][col] != 0);
    matrix[row][col] = 2;    
}

Coordinates* checkNext(Direction dir, Node* head) {
    int x = head->coords->x;
    int y = head->coords->y;

    if (dir == UP) {
        if (y - 1 >= 0 && y - 1 < MAP_SIZE) {
            return createCoordinates(x, y - 1);
        }
        
        if (isHard == true) {
            return NULL;
        }
        else {
            return createCoordinates(x, MAP_SIZE - 1);
        }
    }
    else if (dir == DOWN) {
        if (y + 1 >= 0 && y + 1 < MAP_SIZE) {
            return createCoordinates(x, y + 1);
        }
        
        if (isHard == true) {
            return NULL;
        }
        else {
            return createCoordinates(x, 0);
        }
    }
    else if (dir == RIGHT) {
        if (x + 1 >= 0 && x + 1 < MAP_SIZE) {
            return createCoordinates(x + 1, y);
        }
        
        if (isHard == true) {
            return NULL;
        }
        else {
            return createCoordinates(0, y);
        }
    }
    else if (dir == LEFT) {
        if (x - 1 >= 0 && x - 1 < MAP_SIZE) {
            return createCoordinates(x - 1, y);
        }

        if (isHard == true) {
            return NULL;
        }
        else {
            return createCoordinates(MAP_SIZE - 1, y);
        }
    }
    return NULL;
}

bool move(Game* game) {
    prevDir = game->snake->dir;
    Coordinates* newCoords = checkNext(prevDir, game->snake->queue->head);

    if (newCoords == NULL) {
        return false;
    }

    if (game->matrix[newCoords->x][newCoords->y] == 1) {
        return false;
    }
    else if (game->matrix[newCoords->x][newCoords->y] == 2) {
        createApple(game->matrix);
        game->score++;
    }
    else {
        Coordinates* coords = poll(game->snake->queue);
        game->matrix[coords->x][coords->y] = 0;
        free(coords);
    }

    game->matrix[newCoords->x][newCoords->y] = 1;

    push(game->snake->queue, newCoords);

    drawGame(game);
    return true;
}

void drawGame(Game* game) {
    int score = bestScore > game->score ? bestScore : game->score;
    char scoreStr[MAP_SIZE * MAP_SIZE];
    int scoreStrSize = sprintf(scoreStr, "Счёт: %d, Рекорд: %d\n", game->score, score);

    char str[MAP_SIZE * MAP_SIZE + (MAP_SIZE + 2) * (MAP_SIZE + 3)];

    int counter = 0;

    for (int i = 0; i < scoreStrSize; i++) {
        str[counter++] = scoreStr[counter];
    }

    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    int** matrix = game->matrix;
    for (int i = 0; i < MAP_SIZE + 2; i++) {
        for (int j = 0; j < MAP_SIZE + 2; j++) {
            if (i == 0 && j == 0
                || i == 0 && j == MAP_SIZE + 1
                || j == 0 && i == MAP_SIZE + 1
                || i == MAP_SIZE + 1 && j == MAP_SIZE + 1) {
                str[counter++] = '#';
            }
            else if (i == 0 || i == MAP_SIZE + 1) {
                str[counter++] = '-';
            }
            else if (j == 0 || j == MAP_SIZE + 1) {
                str[counter++] = '|';
            }
            else {
                if (matrix[j - 1][i - 1] == 0) {
                    str[counter++] = ' ';
                }
                else if (matrix[j - 1][i - 1] == 1) {
                    if (skin == CIRCLE) {
                        if (game->snake->queue->head->coords->x == j - 1
                            && game->snake->queue->head->coords->y == i - 1) {
                            str[counter++] = '@';
                        }
                        else {
                            str[counter++] = '*';
                        }
                    }
                    else if (skin == SQUARE) {
                        if (game->snake->queue->head->coords->x == j - 1
                            && game->snake->queue->head->coords->y == i - 1) {
                            str[counter++] = '$';
                        }
                        else {
                            str[counter++] = '#';
                        }
                    }
                    else if (skin == ALPHABET) {
                        Node* current = game->snake->queue->head;
                        for (int k = 0; k < game->snake->queue->size; k++) {
                            if (current->coords->x == j - 1
                                && current->coords->y == i - 1) {
                                str[counter++] = alphabet[k % 26];
                                break;
                            }
                            current = current->next;
                        }
                    }
                }
                else if (matrix[j - 1][i - 1] == 2) {
                    str[counter++] = '?';
                }
            }
        }
        str[counter++] = '\n';
    }
    str[counter++] = '\0';
    printf("%s", str);
}

void startGame() {
    Game* game = createGame();
    push(game->snake->queue, createCoordinates(MAP_SIZE / 2, MAP_SIZE / 2));
    createApple(game->matrix);
    setup();
    printf("Игра началась\n\n\n");

    gameProcess(game);
}

void gameProcess(Game* game) {
    time_t last = clock();
    while (true) {
        if (clock() - last >= timeBetweenTicks) {
            if (move(game) == false) {
                break;
            }
            last = clock();
        }
        if (_kbhit()) {
            char input = _getch();
            if (input == 27) {
                break;
            }
            else if (input == 119 || input == 72) {
                if (game->snake->queue->size > 1 && prevDir != DOWN || game->snake->queue->size == 1) {
                    game->snake->dir = UP;
                }
            }
            else if (input == 97 || input == 75) {
                if (game->snake->queue->size > 1 && prevDir != RIGHT || game->snake->queue->size == 1) {
                    game->snake->dir = LEFT;
                }
            }
            else if (input == 115 || input == 80) {
                if (game->snake->queue->size > 1 && prevDir != UP || game->snake->queue->size == 1) {
                    game->snake->dir = DOWN;
                }
            }
            else if (input == 100 || input == 77) {
                if (game->snake->queue->size > 1 && prevDir != LEFT || game->snake->queue->size == 1) {
                    game->snake->dir = RIGHT;
                }
            }
        }
    }
    endGame(game);
}

void endGame(Game* game) {
    bestScore = bestScore < game->score ? game->score : bestScore;

    deleteSnake(game->snake);
    deleteMatrix(game->matrix);

    free(game);

    printf("Игра завершилась\n\n\n");

    mainMenu();
}

void skinMenu() {
    char input = 0;

    printf("Скины:\n1) Круги\n2) Кубанойды\n3) Алфавит\nНажмите ESC для выхода\n\n\n\n");
    while (true) {
        if (_kbhit()) {
            input = _getch();
            if (input == '1' || input == '2' || input == '3' || input == 27) {
                break;
            }
        }
    }

    switch (input) {
    case '1': skin = CIRCLE; break;
    case '2': skin = SQUARE; break;
    case '3': skin = ALPHABET; break;
    default: break;
    }
    mainMenu();
}

void difficultyMenu() {
    char input = 0;

    printf("Сложность:\n1) Лёгкая\n2) Средняя\n3) Сложная\n4) Экстримальная\nНажмите ESC для выхода\n\n\n\n");
    while (true) {
        if (_kbhit()) {
            input = _getch();
            if (input == '1' || input == '2' || input == '3' || input == '4' || input == 27) {
                break;
            }
        }
    }

    switch (input) {
    case '1': diff = EASY; break;
    case '2': diff = MIDDLE; break;
    case '3': diff = HARD; break;
    case '4': diff = EXTREME; break;
    default: break;
    }
    mainMenu();
}

void mainMenu() {
    char input = 0;
    char diffStr[15];
    char skinStr[15];

    switch (diff) {
    case EASY:
        strcpy(diffStr, "Лёгкая");
        break;
    case MIDDLE:
        strcpy(diffStr, "Средняя");
        break;
    case HARD:
        strcpy(diffStr, "Сложная");
        break;
    case EXTREME:
        strcpy(diffStr, "Экстримальная");
        break;
    }

    switch (skin) {
    case CIRCLE:
        strcpy(skinStr, "Круги");
        break;
    case SQUARE:
        strcpy(skinStr, "Кубанойды");
        break;
    case ALPHABET:
        strcpy(skinStr, "Алфавит");
        break;
    }

    printf("Меню:\n1) Начать игру\n2) Выбор сложности (%s)\n3) Скины (%s)\n\nРекорд: %d\n\nНажмите ESC для выхода\n\n\n\n", diffStr, skinStr, bestScore);
    while (true) {
        if (_kbhit()) {
            input = _getch();
            if (input == '1' || input == '2' || input == '3' || input == 27) {
                break;
            }
        }
    }

    switch (input) {
    case '1': startGame(); break;
    case '2': difficultyMenu(); break;
    case '3': skinMenu(); break;
    default: break;
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    mainMenu();

    printf("\nПрограмма завершена.\n");
	return 0;
}