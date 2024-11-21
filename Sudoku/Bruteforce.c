//#include "allInlcudes.h"

bool solve_sudoku_with_visualization(char **sudoku, short size) {
    for (short row = 0; row < size; row++) {
        for (short col = 0; col < size; col++) {
            if (*(*(sudoku+row)+col) == '.') {
                char numbers[size];
                for (short i = 0; i < size; i++) {
                    *(numbers+i) = '1' + i;
                }
                shuffle(numbers, size);

                for (short i = 0; i < size; i++) {
                    if (checker(sudoku, size, row, col, *(numbers+i))) {
                        *(*(sudoku+row)+col) = *(numbers+i);
                        system("clear");
                        print_sudoku_with_cursor(sudoku, size, row, col);
                        usleep(200000);

                        if (solve_sudoku_with_visualization(sudoku, size)) return true;
                        *(*(sudoku+row)+col) = '.';
                    }
                }
                return false;
            }
        }
    }
    return true;
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
        //bool **is_editable = initialize_editable_sudoku(sudoku, size);

        // Начинаем игровой процесс
        printf("\nИгра началась!");
        solve_sudoku_with_visualization(sudoku, size);

        //free_editable_sudoku(is_editable, size);
    } else {
        printf("Не удалось сгенерировать решённое судоку.\n");
    }

    free_sudoku(sudoku, size);
    free_sudoku(solution, size);
    return 0;
}