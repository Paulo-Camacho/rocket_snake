#include <iostream>
#include <cstdlib> // for rand()
#include <string>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h> // for non-blocking input

using namespace std;

const int width = 50;
const int height = 25;

int head_x;
int head_y;
int fruit_x;
int fruit_y;
int player_score;
int snake_tail_x[100], snake_tail_y[100];
int snake_tail_length;

enum snake_direction {
    STOP = 0, LEFT, RIGHT, UP, DOWN
};

snake_direction dir;

bool is_game_over;

void game_over() 
{ 
    is_game_over = false; 
    dir = STOP; 
    head_x = width / 2; 
    head_y = height / 2; 
    fruit_x = rand() % width; 
    fruit_y = rand() % height; 
    player_score = 0; 
    snake_tail_length = 0;
}

void game_render(string player_name) {
    system("clear"); // Use "clear" instead of "cls" for Linux
    for (int i = 0; i < width + 2; i++) { // top border
        cout << '*';
    }
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width + 2; j++) {
            if (j == 0 || j == width + 1) {
                cout << "*"; // creates horizontal walls
            } else if (i == head_y && j == head_x) {
                cout << "@"; // creates snake head
            } else if (i == fruit_y && j == fruit_x) {
                cout << "*"; // fruit
            } else {
                bool pr_tail = false;
                for (int k = 0; k < snake_tail_length; k++) {
                    if (snake_tail_x[k] == j && snake_tail_y[k] == i) {
                        cout << "#";
                        pr_tail = true;
                        break;
                    }
                }
                if (!pr_tail) {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++) {
        cout << '*'; // bottom border
    }
    cout << endl;
    cout << player_name << "'s score: " << player_score << endl;
}

void game_update() {
    int pre_x = snake_tail_x[0];
    int pre_y = snake_tail_y[0];
    int pre2x, pre2y;
    snake_tail_x[0] = head_x;
    snake_tail_y[0] = head_y;

    for (int i = 1; i < snake_tail_length; i++) {
        pre2x = snake_tail_x[i];
        pre2y = snake_tail_y[i];
        snake_tail_x[i] = pre_x;
        snake_tail_y[i] = pre_y;
        pre_x = pre2x;
        pre_y = pre2y;
    }
    
    switch (dir) {
        case LEFT:
            head_x--;
            break;
        case RIGHT:
            head_x++;
            break;
        case UP:
            head_y--;
            break;
        case DOWN:
            head_y++;
            break;
        case STOP:
            break;
    }

    // checking snake's collision with wall
    if (head_x >= width || head_x < 0 || head_y >= height || head_y < 0) {
        is_game_over = true;
    }

    // checking for collision with tail
    for (int i = 0; i < snake_tail_length; i++) {
        if (snake_tail_x[i] == head_x && snake_tail_y[i] == head_y) {
            is_game_over = true;
        }
    }

    // adding fruit 
    if (head_x == fruit_x && head_y == fruit_y) {
        player_score += 10;
        fruit_x = rand() % width;
        fruit_y = rand() % height;
        snake_tail_length++;
    }
}

void enable_raw_mode() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~ICANON;
    term.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &term);

    // Set non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void disable_raw_mode() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON;
    term.c_lflag |= ECHO;
    tcsetattr(0, TCSANOW, &term);

    // Reset non-blocking mode
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
}

void user_input() {
    char input;
    if (read(STDIN_FILENO, &input, 1) == -1) {
        return;
    }
    switch (input) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'x':
            is_game_over = true;
            break;
    }
}

int main() {
    string player_name;
    cout << "Enter your name(text might not show), then hit 'Enter' twice: ";
    cin >> player_name;

    game_over();

    enable_raw_mode();

    while (!is_game_over) {
        user_input();
        game_update();
        game_render(player_name);
        usleep(75000); // sleep for 75ms 
    }

    disable_raw_mode();

    cout << "Game Over! Final score: " << player_score << endl;

    return 0;
}
