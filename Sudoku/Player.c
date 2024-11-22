#include "allInlcudes.h"


// Управление вводом символов
void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO); // Отключаем буферизацию ввода и отображение символов
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

bool is_sudoku_solved(char **sudoku, short size) {
    short block_size = BLOCK_SIZE(size);

    for (short i = 0; i < size; i++) {
        bool row_check[size + 1];
        bool col_check[size + 1];
        for (short k = 0; k <= size; k++) *(row_check+k) = *(col_check+k) = false;

        for (short j = 0; j < size; j++) {
            // Проверка строки
            if (*(*(sudoku+i)+j) >= '1' && *(*(sudoku+i)+j) <= '9') {
                short num = *(*(sudoku+i)+j) - '0';
                if (*(row_check+num)) return false;
                *(row_check+num) = true;
            } else {
                return false; // Есть незаполненные ячейки
            }

            // Проверка столбца
            if (*(*(sudoku+j)+i) >= '1' && *(*(sudoku+j)+i) <= '9') {
                short num = *(*(sudoku+j)+i) - '0';
                if (*(col_check+num)) return false;
                *(col_check+num) = true;
            } else {
                return false; // Есть незаполненные ячейки
            }
        }
    }

    // Проверка блоков
    for (short block_row = 0; block_row < size; block_row += block_size) {
        for (short block_col = 0; block_col < size; block_col += block_size) {
            bool block_check[size + 1];
            for (short k = 0; k <= size; k++) *(block_check+k) = false;

            for (short i = 0; i < block_size; i++) {
                for (short j = 0; j < block_size; j++) {
                    char cell = *(*(sudoku+block_row+i)+block_col+j);
                    if (cell >= '1' && cell <= '9') {
                        short num = cell - '0';
                        if (block_check[num]) return false;
                        block_check[num] = true;
                    } else {
                        return false; // Есть незаполненные ячейки
                    }
                }
            }
        }
    }

    return true;
}


bool **initialize_editable_sudoku(char **sudoku, short size) {
    bool **is_editable = (bool **)malloc(size * sizeof(bool *));
    for (short i = 0; i < size; i++) {
        *(is_editable+i) = (bool *)malloc(size * sizeof(bool));
        for (short j = 0; j < size; j++) {
            *(*(is_editable+i)+j) = (*(*(sudoku+i)+j) == '.'); // Ячейка редактируемая, если она пустая
        }
    }
    return is_editable;
}

void free_editable_sudoku(bool **is_editable, short size) {
    for (short i = 0; i < size; i++) {
        free(*(is_editable+i));
    }
    free(is_editable);
}

void play_sudoku(char **sudoku, bool **is_editable, short size) {
    short cursor_row = 0, cursor_col = 0;
    enable_raw_mode();

    while (true) {
        print_sudoku_with_cursor(sudoku, size, cursor_row, cursor_col);
        printf("\nИспользуйте WASD для перемещения, 1-9 для ввода. Q для выхода.\n");
        printf("Текущая позиция: (%d, %d)\n", cursor_row + 1, cursor_col + 1);


        if (is_sudoku_solved(sudoku, size)) {
            printf("\nПоздравляем! Судоку решено!\n");
            break;
        }

        char input = getchar(); // Считываем один символ
        
        if (input == 'q') { // Выход из игры
            printf("\nВы вышли из игры.\n");
            break;
        }

        switch (input) {
            case 'w': // Вверх
                if (cursor_row > 0) cursor_row--;
                break;
            case 's': // Вниз
                if (cursor_row < size - 1) cursor_row++;
                break;
            case 'a': // Влево
                if (cursor_col > 0) cursor_col--;
                break;
            case 'd': // Вправо
                if (cursor_col < size - 1) cursor_col++;
                break;
            default:
                if (input >= '1' && input <= '9') { // Ввод числа
                    if (*(*(is_editable+cursor_row)+cursor_col)) { // Проверяем, можно ли редактировать ячейку
                        *(*(sudoku+cursor_row)+cursor_col) = input; // Записываем число
                    } else {
                        printf("\nЭту ячейку нельзя изменить!\n");
                        usleep(1000000); // Задержка 1 секунда
                    }
                }
                break;
        }
    }

    disable_raw_mode();
}

int working() {
    short size;
    srand(time(NULL));

    printf("Введите размер судоку (4 или 9): ");
    scanf("%hd", &size);

    if (size != 4 && size != 9) {
        printf("Поддерживаются только размеры 4x4 и 9x9.\n");
        return 1;
    }

    char ** sudoku = initialiser(size);
    char ** solution = initialiser(size);

    if (place_holder(solution, size)) {
        // Копируем решение в игровое поле
        for (short i = 0; i < size; i++) {
            for (short j = 0; j < size; j++) {
                *(*(sudoku+i)+j) = *(*(solution+i)+j);
            }
        }

        // Удаляем ячейки для игры
        float percentage;
        printf("\nВведите процент удаления ячеек (например, 50 для 50%%): ");
        scanf("%f", &percentage);

        remove_cells(sudoku, size, percentage);

        // Создаём массив редактируемых ячеек
        bool **is_editable = initialize_editable_sudoku(sudoku, size);

        // Начинаем игровой процесс
        printf("\nИгра началась! Управляйте с помощью клавиш WASD, вводите числа 1-9. Для выхода нажмите 'q'.\n");
        play_sudoku(sudoku, is_editable, size);

        free_editable_sudoku(is_editable, size);
    } else {
        printf("Не удалось сгенерировать решённое судоку.\n");
    }

    free_sudoku(sudoku, size);
    free_sudoku(solution, size);
    return 0;
}
// void play_sudoku(char **sudoku, bool **is_editable, short size) {
//     short cursor_row = 0, cursor_col = 0;

//     struct termios old, new;
//     tcgetattr(STDIN_FILENO, &old);
//     new = old;
//     new.c_lflag &= ~(ICANON | ECHO);
//     tcsetattr(STDIN_FILENO, TCSANOW, &new);

//     while (true) {
//         system("clear");
//         print_sudoku_with_cursor(sudoku, size, cursor_row, cursor_col);
//         printf("\nИспользуйте WASD для перемещения, 1-9 для ввода. Q для выхода.\n");
//         printf("Текущая позиция: (%d, %d)\n", cursor_row + 1, cursor_col + 1);

//         if (place_holder(sudoku, size)) {
//             printf("\nПоздравляем! Судоку решено!\n");
//             break;
//         }

//         char input = getchar();

//         if (input == 'q') break;

//         switch (input) {
//             case 'w': if (cursor_row > 0) cursor_row--; break;
//             case 's': if (cursor_row < size - 1) cursor_row++; break;
//             case 'a': if (cursor_col > 0) cursor_col--; break;
//             case 'd': if (cursor_col < size - 1) cursor_col++; break;
//             default:
//                 if (input >= '1' && input <= '9') {
//                     if (*(*(is_editable+cursor_row)+cursor_col)) {
//                         *(*(sudoku+cursor_row)+cursor_col) = input;
//                     } else {
//                         printf("Эту ячейку нельзя изменить!\n");
//                         usleep(1000000);
//                     }
//                 }
//                 break;
//         }
//     }

//     tcsetattr(STDIN_FILENO, TCSANOW, &old);
// }


// int working() {
//     short size;
//     srand(time(NULL));

//     printf("Введите размер судоку (4 или 9): ");
//     scanf("%hd", &size);

//     if (size != 4 && size != 9) {
//         printf("Поддерживаются только размеры 4x4 и 9x9.\n");
//         return 1;
//     }

//     char **sudoku = initialiser(size);
//     char **solution = initialiser(size);

//     if (place_holder(solution, size)) {
//         for (short i = 0; i < size; i++) {
//             for (short j = 0; j < size; j++) {
//                 *(*(sudoku+i)+j) = *(*(solution+i)+j);
//             }
//         }

//         float percentage;
//         printf("Введите процент удаления ячеек (например, 50 для 50%%): ");
//         scanf("%f", &percentage);

//         remove_cells(sudoku, size, percentage);

//         bool **is_editable = initialize_editable_sudoku(sudoku, size);

//         play_sudoku(sudoku, is_editable , size);
//         free_editable_sudoku(is_editable, size);
//     } else {
//         printf("Не удалось сгенерировать решённое судоку.\n");
//     }

//     free_sudoku(sudoku, size);
//     free_sudoku(solution, size);
//     return 0;
// }