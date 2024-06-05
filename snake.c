#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define WINDOW_WIDTH 1500
#define WINDOW_HEIGHT 1000

#define ROWS WINDOW_HEIGHT / TILE_SIZE
#define COLS WINDOW_WIDTH / TILE_SIZE

#define TILE_SIZE 50

typedef struct {
    Rectangle rec;
    int row;
    int col;
} Tile;

typedef struct {
    Tile tile;
    Color fill_color;
    Color outline_color;
} Grid;

typedef struct {
    Tile head;
    Tile *body;
    size_t length;
} Snake;

typedef struct {
    Tile tile;
} Fruit;

typedef struct {
    int x;
    int y;
} Direction;

// Grid grid[WINDOW_WIDTH / TILE_SIZE][WINDOW_WIDTH / TILE_SIZE];
Grid **create_grid()
{
    Grid **grid = (Grid **)malloc(ROWS * sizeof(Grid *));
    for (int i = 0; i < ROWS; i++) {
        grid[i] = (Grid *)malloc(COLS * sizeof(Grid));
    }

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            Rectangle rec = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            Tile tile = {rec, row, col};
            grid[row][col].tile = tile;
            grid[row][col].fill_color = GRAY;
            grid[row][col].outline_color = WHITE;
        }
    }

    return grid;
}

void delete_grid(Grid **grid)
{
    for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++) {
        free(grid[i]);
    }
    free(grid);
}

void draw_grid_background(Grid **grid)
{
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            DrawRectangleRec(grid[row][col].tile.rec, grid[row][col].fill_color);
            DrawRectangleLinesEx(grid[row][col].tile.rec, 2, WHITE);
        }
    }
}

void draw_snake(Snake *snake)
{
    DrawRectangleRec(snake->head.rec, GREEN);

    // TODO draw body
    for(size_t i = 0; i < snake->length; i++) {
        DrawRectangleRec(snake->body[i].rec, GREEN);
    }
}

Snake *create_snake(Grid **grid)
{
    Snake *snake = (Snake *)malloc(1 * sizeof(Snake));

    snake->head = grid[ROWS/2][COLS/2].tile;
    snake->length = 0;
    snake->body = (Tile *)malloc(snake->length * sizeof(Tile));

    return snake;
}

void delete_snake(Snake *snake)
{
    free(snake->body);
    free(snake);
}

void increase_snake(Snake *snake)
{
    snake->length++;
    snake->body = (Tile *)realloc(snake->body, snake->length*sizeof(Tile));
}

void move_snake(Snake *snake, Direction *direction, Grid **grid)
{
    if (
        (snake->head.row + direction->y < 0 ||
         snake->head.row + direction->y > ROWS - 1 ||
         snake->head.col + direction->x < 0 ||
         snake->head.col + direction->x > COLS - 1)) {
        direction->x = 0;
        direction->y = 0;
        return;
    }

    Tile previous = snake->head;
    Tile tmp;

    snake->head =
        grid[snake->head.row + direction->y][snake->head.col + direction->x].tile;

    // TODO move body

    tmp = snake->body[0];
    snake->body[0] = previous;
    previous = tmp;

    for (size_t i = 1; i < snake->length; i++) {
        tmp = snake->body[i];
        snake->body[i] = previous;
        previous = tmp;
    }
}

Fruit *create_fruit()
{
    int row = rand() % ROWS;
    int col = rand() % COLS;

    Rectangle rec = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};

    Fruit *fruit = (Fruit *)malloc(1 * sizeof(Fruit));
    Tile tile = {rec, row, col};
    fruit->tile = tile;

    return fruit;
}

void delete_fruit(Fruit *fruit)
{
    free(fruit);
}

void draw_fruit(Fruit *fruit)
{
    DrawRectangleRec(fruit->tile.rec, BLUE);
}

void update_fruit(Fruit *fruit, Grid **grid)
{
    int row = rand() % ROWS;
    int col = rand() % COLS;

    fruit->tile = grid[row][col].tile;
}

void fruit_collected(Fruit *fruit, Snake *player, Grid **grid, int *score)
{
    if(fruit->tile.col - player->head.col > 0 && fruit->tile.row - player->head.row > 0)
        return;

    if(fruit->tile.col == player->head.col && fruit->tile.row == player->head.row) {
        increase_snake(player);
        update_fruit(fruit, grid);
        (*score)++;
    }
}

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "first window");
    srand(time(NULL));

    Direction *direction = calloc(1, sizeof(Direction));

    Grid **grid = create_grid();
    Snake *player = create_snake(grid);

    Fruit *fruit = create_fruit();

    int score = 0;

    int frame_counter = 0;
    int frames_to_wait = 60 / 5;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        frame_counter++;

        if (IsKeyDown(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)) {
            direction->x = 1;
            direction->y = 0;
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyPressed(KEY_LEFT)) {
            direction->x = -1;
            direction->y = 0;
        }
        if (IsKeyDown(KEY_UP) || IsKeyPressed(KEY_UP)) {
            direction->y = -1;
            direction->x = 0;
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyPressed(KEY_DOWN)) {
            direction->y = 1;
            direction->x = 0;
        }

        if (frame_counter >= frames_to_wait) {
            move_snake(player, direction, grid);

            frame_counter = 0;
        }

        BeginDrawing();
        draw_grid_background(grid);
        DrawText(TextFormat("Score: %i", score), WINDOW_WIDTH / 2 - 6*25, 6, 50, BLACK);
        draw_snake(player);
        draw_fruit(fruit);
        fruit_collected(fruit, player, grid, &score);
        EndDrawing();
    }

    delete_grid(grid);
    delete_snake(player);
    delete_fruit(fruit);
    CloseWindow();

    return 0;
}
