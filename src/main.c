/*
 * Author      : Prajwal Chapagain <prajjwal2058@gmail.com>
 * Date        : Tuesday Aug 24, 2021 18:09:15 NPT
 * License     : MIT
 */

#include <ncurses/curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_HEIGHT 7
#define BOARD_WIDTH 19

#define PLAYER_HUMAN 'X'
#define PLAYER_COMPUTER 'O'
#define NOONE 'N'

typedef struct CellPosition {
    int x;
    int y;
} CellPosition;

void close_curses(WINDOW *board)
{
    delwin(board); // delete the board
    endwin();
    printf("Curses Ended\n");
}

//--------------------

void reset_game(WINDOW *board, CellPosition *cells_position, char *cells_state, char *whose_turn)
{
    for (int idx = 0; idx < 9; idx++) {
        cells_state[idx] = '0';
        mvwaddch(board, cells_position[idx].y, cells_position[idx].x, ' ');
    }
    wrefresh(board);

    if (*whose_turn == NOONE)
        *whose_turn = PLAYER_HUMAN;
    
}

void declare_winner(char winner)
{
    if (winner == PLAYER_HUMAN)
        mvaddstr(4, (COLS - 7) / 2, "You win");
    else if (winner == PLAYER_COMPUTER)
        mvaddstr(4, (COLS - 11) / 2, "Computer win");
    refresh();
}

bool diagonal_crossed(char *cells_state)
{
    if (cells_state[0] != '0' &&
        cells_state[0] == cells_state[4] &&
        cells_state[4] == cells_state[8])
        return true;

    if (cells_state[2] != '0' &&
        cells_state[2] == cells_state[4] &&
        cells_state[4] == cells_state[6])
        return true;
    return false;
}

bool column_crossed(char *cells_state)
{
    if (cells_state[0] != '0' &&
        cells_state[0] == cells_state[3] &&
        cells_state[3] == cells_state[6])
        return true;

    if (cells_state[1] != '0' &&
        cells_state[1] == cells_state[4] &&
        cells_state[4] == cells_state[7])
        return true;

    if (cells_state[2] != '0' &&
        cells_state[2] == cells_state[5] &&
        cells_state[5] == cells_state[8])
        return true;

    return false;
}

bool row_crossed(char *cells_state)
{
    if (cells_state[0] != '0' &&
        cells_state[0] == cells_state[1] &&
        cells_state[1] == cells_state[2])
        return true;

    if (cells_state[3] != '0' &&
        cells_state[3] == cells_state[4] &&
        cells_state[4] == cells_state[5])
        return true;

    if (cells_state[6] != '0' &&
        cells_state[6] == cells_state[7] &&
        cells_state[7] == cells_state[8])
        return true;

    return false;
}

char check_winner(char *cells_state, char whose_turn)
{
    bool is_crossed = row_crossed(cells_state) || column_crossed(cells_state) || diagonal_crossed(cells_state);
    if (is_crossed)
        return whose_turn;
    return NOONE;
}

//--------------------

bool is_board_full(char *cells_state)
{
    for (int idx = 0; idx < 9; idx++) {
        if (cells_state[idx] == '0' || cells_state[idx] == ' ')
            return false;
    }
    return true;
}

void update_board(WINDOW *board, int x_pos, int y_pos, char ch)
{
    mvwaddch(board, y_pos, x_pos, ch);
    wrefresh(board);
}

void update_cells_state(char *cells_state, int idx, char ch)
{
    cells_state[idx] = ch;
}

bool is_legal_move(char *cells_state, int idx)
{
    if (cells_state[idx] == 'X' || cells_state[idx] == 'O')
        return false;
    return true;
}

int generate_computer_move(void)
{
    return rand() % 9;
}

void computer_turn(WINDOW *board, CellPosition *cells_position, char *cells_state)
{
    if (is_board_full(cells_state))
        return;

    int cell_idx_num = generate_computer_move();
    while (!is_legal_move(cells_state, cell_idx_num))
        cell_idx_num = generate_computer_move();

    update_cells_state(cells_state, cell_idx_num, PLAYER_COMPUTER);
    update_board(board, cells_position[cell_idx_num].x, cells_position[cell_idx_num].y, PLAYER_COMPUTER);
}

int find_cell_idx_num(int choosen_x_pos, int choosen_y_pos, CellPosition *cells_position)
{
    int idx = 0;
    for (; idx < 9; idx++) {
        if (cells_position[idx].x == choosen_x_pos && cells_position[idx].y == choosen_y_pos)
            return idx;
    }
    return 0;
}

bool human_turn(WINDOW *board, CellPosition *cells_position, char *cells_state)
{
    int choosen_x_pos = 0,
        choosen_y_pos = 0;
    getyx(board, choosen_y_pos, choosen_x_pos);

    int cell_idx_num = find_cell_idx_num(choosen_x_pos, choosen_y_pos, cells_position);
    if (!is_legal_move(cells_state, cell_idx_num)) {
        return false;
    }

    update_cells_state(cells_state, cell_idx_num, PLAYER_HUMAN);
    update_board(board, choosen_x_pos, choosen_y_pos, PLAYER_HUMAN);
    return true;
}

//--------------------

void move_cursor(WINDOW *board, int pressed_key, int *x_pos, int *y_pos)
{
    switch (pressed_key) {
        case KEY_UP:
        case 'w':
            if ((*y_pos) > 1) (*y_pos) -= 2;
            break;
        case KEY_LEFT:
        case 'a':
            if ((*x_pos) > 3) (*x_pos) -= 6;
            break;
        case KEY_DOWN:
        case 's':
            if ((*y_pos) < 5) (*y_pos) += 2;
            break;
        case KEY_RIGHT:
        case 'd':
            if ((*x_pos) < 15) (*x_pos) += 6;
            break;
        default:
            break;
    }
    wmove(board, *y_pos, *x_pos);
    wrefresh(board);
}

void draw_board_grid(WINDOW *board, int x_pos, int y_pos)
{
    for (int i = 0; i < BOARD_WIDTH - 2; i++) {
        for (int j = 0; j < BOARD_HEIGHT - 2; j++)
            mvwaddch(board, y_pos + j, x_pos + 5, ACS_VLINE);

        for (int j = 0; j < BOARD_HEIGHT - 2; j++)
            mvwaddch(board, y_pos + j, (BOARD_WIDTH - 2) - 5, ACS_VLINE);

        for (int j = 0; j < BOARD_WIDTH - 2; j++)
            mvwaddch(board, y_pos + 1, x_pos + j, ACS_HLINE);

        for (int j = 0; j < BOARD_WIDTH - 2; j++)
            mvwaddch(board, y_pos + 1 + 2, x_pos + j, ACS_HLINE);
    }
}

WINDOW *create_board(void)
{
    int start_x = (COLS - BOARD_WIDTH) / 2;
    int start_y = (LINES - BOARD_HEIGHT) / 2;

    WINDOW *board = newwin(BOARD_HEIGHT, BOARD_WIDTH, start_y, start_x);
    box(board, 0, 0); // enable border
    curs_set(TRUE); // enable cursor
    keypad(board, TRUE); // enable extra keys (F1, F2, ARROWS KEYS, ..)

    draw_board_grid(board, 1, 1);
    wrefresh(board); // refresh board to show border
    return board;
}

void display_game_tittle(void)
{
    attron(A_BOLD);
    mvwprintw(stdscr, 2, (COLS - 11) / 2, "Tic Tac Toe");
    attroff(A_BOLD);
    refresh();
}

void init_curses(void)
{
    initscr(); // initilize the screen
    raw(); // disable line buffering

    curs_set(FALSE); // don't display the cursor
    keypad(stdscr, FALSE); // enable the keypad (F1, F2, ARROE KEYS, ....)
    noecho(); // don't echo any key when press
    refresh();
}

//--------------------

int main(int argc, char **argv)
{
    srand(time(NULL));
    init_curses();
    display_game_tittle();

    WINDOW *board = create_board();
    CellPosition cells_position[9] = {
        {.x = 3, .y = 1},  // row1 col1
        {.x = 9, .y = 1},  // row1 col2
        {.x = 15, .y = 1}, // row1 col3

        {.x = 3, .y = 3},  // row2 col1
        {.x = 9, .y = 3},  // row2 col2
        {.x = 15, .y = 3}, // row2 col3

        {.x = 3, .y = 5},  // row3 col1
        {.x = 9, .y = 5},  // row3 col2
        {.x = 15, .y = 5}  // row3 col3
    };

    char cells_state[9] = {
        '0', '0', '0',
        '0', '0', '0',
        '0', '0', '0'
    };
    int pressed_key = 0;
    int cursor_x_pos = cells_position[0].x,
        cursor_y_pos = cells_position[0].y;
    char whose_turn = PLAYER_COMPUTER;
    char winner = NOONE;

    mvaddstr(7, 10, "Press ENTER to start the game");
    refresh();
    wmove(board, cursor_y_pos, cursor_x_pos);
    while ((pressed_key = wgetch(board)) != 'Q') {
        if (pressed_key == 'S' && whose_turn == NOONE) {
            /*
             * TODO: clear winner announce message after user restart the game
             *
             * This doesn't work
             * move(4, 0);
             * clrtoeol();
             * refresh();
             */

            reset_game(board, cells_position, cells_state, &whose_turn); /
        }

        if (pressed_key == '\n') {
            // clear "Press ENTER to **" message
            move(7, 10);
            clrtoeol();
            refresh();

            if (whose_turn == PLAYER_HUMAN) {
                bool is_valid_move = human_turn(board, cells_position, cells_state);
                if (is_valid_move) {
                    winner = check_winner(cells_state, whose_turn);
                    whose_turn = (winner == NOONE) ? PLAYER_COMPUTER : NOONE;
                }
            }

            if (whose_turn == PLAYER_COMPUTER) {
                computer_turn(board, cells_position, cells_state);
                winner = check_winner(cells_state, whose_turn);
                whose_turn = (winner == NOONE) ? PLAYER_HUMAN : NOONE;
            }
        }

        if (is_board_full(cells_state) && winner == NOONE) {
            mvaddstr(4, (COLS - 11) / 2, "It's a draw");
            refresh();
        } else {
            declare_winner(winner);
        }

        move_cursor(board, pressed_key, &cursor_x_pos, &cursor_y_pos);
    }
    close_curses(board);
    return 0;
}

