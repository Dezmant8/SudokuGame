#include <stdio.h>
#include <time.h> // Возарващает время в секундах
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define BLOCK_SIZE(size) ((size == 4) ? 2 : 3)


//typedef  bool (check) (short **, short, short, short, short)

char **initialiser(short size){
    char ** sudoku=(char**)malloc(size * sizeof(char*));
    for(short i=0; i!=size;++i){
        sudoku[i]=(char*)malloc(size * sizeof(char));
        for(short j=0; j!=size; ++j){
            *(*(sudoku+i)+j)='0';
        }
    }
    return sudoku;
}

void free_sudoku(char **sudoku, short size) {
    for (short i = 0; i < size; i++) {
        free(*(sudoku + i));
    }
    free(sudoku);
    printf("mem cleared");
}

void shuffle(char * numbers, short size){
    for(short i=size-1; i!=0; --i){
        short j =rand()%(i+1);
        char tmp=*(numbers+i);
        *(numbers+i)=*(numbers+j);
        *(numbers+j)=tmp;
    }
}


bool row_check(char **sudoku, short size, short row, char number){
    for(short j=0; j!=size; ++j){
        if(*(*(sudoku+row)+j)==number){
            return false;
        }
    }
    return true;
}
bool column_check(char **sudoku, short size, short column, char number){
    for(short i=0; i!=size; ++i){
        if(*(*(sudoku+i)+column)==number){
            return false;
        }
    }
    return true;
}
bool square_check(char **sudoku, short size, short currRow, short currColumn, char number){
    short blockSize = BLOCK_SIZE(size);
    for(short i=0; i!=blockSize;++i){
        for(short j=0; j!=blockSize; ++j){
            if(*(*(sudoku+currRow+i)+currColumn+j)==number){
                return false;
            }
        }
    }
    return true;
}

bool checker(char ** sudoku, short size, short row, short column, char number){
    short blockSize=BLOCK_SIZE(size);
    return(square_check(sudoku, size, row-row%blockSize, column-column%blockSize, number)
    && column_check(sudoku, size, column, number)
    && row_check(sudoku, size, row, number));
}




bool place_holder(char **sudoku, short size){
    for (short i=0; i!=size; ++i){
        for(short j=0; j!=size; ++j){
            if(*(*(sudoku+i)+j)=='0'){
                char numbers[size];
                for(short k=0; k!=size; ++k){
                    *(numbers+k)=k+'1'; //чиселки для заполнения
                }
                shuffle(numbers, size); //создать рандомайзер по алгоритму ?Фишера-Йетса?

                for(short k=0; k!=size; ++k){
                    if(checker(sudoku, size, i, j, *(numbers+k))){ //написать провверку на корректность эллемента
                        *(*(sudoku+i)+j)=*(numbers+k);
                        if(place_holder(sudoku, size)){
                            return true; //если всё гуд, то тру для выхода из рекурсии
                        }
                        *(*(sudoku+i)+j)='0';
                    }
                }
                return false; //не смогли подобрать эллемент(вроде невозможно такое)
            }
        }
    }
    return true; //круто классно, получилось сгенерить
}
void print_sudoku(char **sudoku, short size) {
    for (short i = 0; i < size; i++) {
        for (short j = 0; j < size; j++) {
            if (*(*(sudoku+i)+j) == '0') {
                printf(". ");
            } else {
                printf("%c ", *(*(sudoku+i)+j));
            }
        }
        printf("\n");
    }
}
// Печать судоку с выделенной ячейкой
void print_sudoku_with_cursor(char **sudoku, short size, short cursor_row, short cursor_col) {
    system("clear");
    short block_size = BLOCK_SIZE(size);
    for (short i = 0; i < size; i++) {
        for (short j = 0; j < size; j++) {
            if (i == cursor_row && j == cursor_col) {
                printf("[%c] ", *(*(sudoku+i)+j)); // Выделяем текущую ячейку
            } else {
                printf(" %c  ", *(*(sudoku+i)+j));
            }
            if ((j + 1) % block_size == 0 && j + 1 < size) {
                printf("|"); // Вертикальная линия между блоками
            }
        }
        printf("\n");
        if ((i + 1) % block_size == 0 && i + 1 < size) {
            for (short k = 0; k < size * 4; k++) {
                printf("-"); // Горизонтальная линия между блоками
            }
            printf("\n");
        }
    }
}


// Удаление случайных ячеек для игры
void remove_cells(char **puzzle, short size, float percentage) {
    short total_cells = size * size;
    short cells_to_remove = (short)(total_cells * percentage / 100);

    while (cells_to_remove > 0) {
        short row = rand() % size;
        short col = rand() % size;

        if (*(*(puzzle+row)+col) != '.') { // Удаляем только заполненные ячейки
            *(*(puzzle+row)+col) = '.';    // Заменяем на точку
            cells_to_remove--;
        }
    }
}


// int main(){
//     short size;

//     // Инициализируем генератор случайных чисел
//     srand(time(NULL));

//     // Запрашиваем размер у пользователя
//     printf("Введите размер судоку (4 или 9): ");
//     scanf("%hd", &size);

//     if (size != 4 && size != 9) {
//         printf("Поддерживаются только размеры 4x4 и 9x9.\n");
//         return 1;
//     }

//     short **sudoku=initialiser(size);
//     generator(sudoku, size);

//     printer(sudoku, size);
//     free_sudoku(sudoku, size);
//     return 0;
// }