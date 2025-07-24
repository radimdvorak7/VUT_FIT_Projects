/**
 * IZP - 2.Projekt: Práce s datovými strukturami 
 * Name: Radim Dvořák
 * Login: xdvorar00
 * Last change: 1.12.2023
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define SOLVE_MAZE Arg == lpath || Arg == rpath
#define TEST_MAZE Arg == test

typedef struct{
    int rows;
    int cols;
    unsigned char *cells;
} Map;

typedef struct{
    int row;
    int col;
}Cell;

enum Argument {error = -1, rpath, lpath, help, test};
enum Border {left_border, right_border, top_border, bottom_border = 2};

/**
 * @brief Prints help
 */ 
void printHelp(){
    printf("Usage:          maze --help\n"
           "                maze --test FILE\n"      
           "                maze --option R C FILE\n"
           "\n"
           "Options:\n"
           "--help          Prints help\n"
           "--test          Checks if the maze is valid\n"
           "--rpath         Finds path following the right-hand rule\n"
           "--lpath         Finds path following the left-hand rule\n"
           "\n"
           "Arguments:\n"
           "R               Row of starting cell\n"
           "C               Column of starting cell\n"
           "FILE            File to load from\n");
}

/**
 * @brief Based on command line options returns value
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Value of option
 */ 
enum Argument parseArguments(int argc, char *argv[]){
    if(argc < 2){
        return error;
    }

    if(strcmp(argv[1],"--help") == 0){
        if(argc > 2){
            return error;
        }
        return help;
    }
    else if (strcmp(argv[1],"--test") == 0){
        if(argc > 3){
            return error;
        }
        return test;
    }
    else if (strcmp(argv[1],"--lpath") == 0){
        if(argc > 5){
            return error;
        }
        return lpath;
    }
    else if (strcmp(argv[1],"--rpath") == 0){
        if(argc > 5){
            return error;
        }
        return rpath;
    }
    else{
        return error;
    }
}
/**
 * @brief Checks if the row and column of starting cell is correct
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments 
 * @param row Row of starting cell
 * @param col Column of starting cell
 * @return 0 if it's correct, -1 if file is missing, -2 if the coordinate is missing, -3 if the values of coordinates are incorrect
 */
int startingCellValidity(int argc, char *argv[], int *row, int *col){
    if (argc == 3 || argc == 2){
        return -1;
    }
    //Saving coordinates of entry cell from command line
    *row = atoi(argv[2]);
    *col = atoi(argv[3]);

    if(*row < 1 || *col <1){
        return -2;
    }
    return 0;
}


/**
 * @brief Constructor of map 
 * @return Pointer to the structure of Map
 */
Map *ctor_Map(void){
    Map *map = malloc(sizeof(Map));
    if(map == NULL){
        return NULL;
    }
    return map;
}

/**
 * @brief Loads map from file into created memory
 * @param map Pointer to the allocated structure of Map
 * @param file Pointer to the the file from where is maze loaded
 * @return 0 on success, 1 on incorect allocation, 2 on wrong dimensions of maze, 3 on missing dimension
 */ 
int loadMap(Map *map, FILE *file){
    //Reading dimensions of maze
    if(fscanf(file," %d %d",&map->rows,&map->cols) < 2){
        return 2;
    }

    if(map->rows < 1 || map->cols < 1){
        return 3;
    }

    map->cells = malloc(sizeof(unsigned char) * (map->rows * map->cols));
    if(map->cells == NULL){
        return 1;
    }

    //Loads values from file into array cells
    for (int row = 0; row < map->rows; row++) {
        for (int col = 0; col < map->cols; col++) {
            char ch = getc(file);      
            if(isspace(ch) != 0){
                col--;
                continue;
            }
            map->cells[map->cols*row + col] = ch;
        }
    }
    return 0;
}

/**
 * @brief Destructor of map
 * @param map Pointer to the allocated structure of Map
 */
void dtor_Map(Map *map){
    free(map->cells);
    free(map);
    map = NULL;
}

/** 
 * @brief Loads maze from file into structure Map
 * @param filename Name of file from which the maze will be loaded
 * @param map Pointer to the allocated structure of Map
 * @return 0 on success, -1 when opening file fails, 1 on incorect allocation, 2 on wrong dimensions of maze, 3 on missing dimension
 */ 
int readFile(char *filename, Map *map){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        return -1;
    }
    int check_load = loadMap(map,file);

    //Allocation failure
    if(check_load == 1){
        dtor_Map(map);
        fclose(file);
        return 1;
    }
    //Wrong dimension
    else if (check_load == 2)
    {   
        dtor_Map(map);
        fclose(file);
        return 2;
    }
    //Missing dimension
    else if(check_load == 3){
        dtor_Map(map);
        fclose(file);
        return 3;
    }

    fclose(file);
    return 0;
}

/**
 * @brief Checks if the border is wall or not
 * @param map Pointer to the allocated structure of Map
 * @param r Current row
 * @param c Current column
 * @param border Current border; 0 for left, 1 for right, 2 for up/down
 * @return TRUE if the border is wall, otherwise FALSE
 */
bool isborder(Map *map, int r, int c, int border){
    if(map->cells[map->cols*r + c] & (1 << (1 * border))){
        return true;
    }
    return false;
}
/** 
 * @brief Checks validity of maze
 * @param map Pointer to the allocated structure of Map
 * @return TRUE when the maze is valid, FALSE when invalid
 */
bool isMazeValid(Map *map){
    for (int row = 0; row < map->rows; row++) {
        for (int col = 0; col < map->cols; col++) {
            //Checking range of values
            if(map->cells[map->cols * row + col] <'0' || map->cells[map->cols * row + col] >'7'){
                return false;
            }
            //Checking left border
            if(col != 0 && isborder(map,row,col,left_border) != isborder(map,row,col-1,right_border)){
                return false;
            }
            //Checking right border
            if(col != map->cols - 1 && isborder(map,row,col,right_border) != isborder(map,row,col + 1,left_border)){
                return false;
            }
            //Checking top/bottom border
            if(((row + col) % 2 == 1) && row != (map->rows - 1)){
                if(isborder(map,row,col,bottom_border) != isborder(map,row + 1,col,top_border)){
                    return false;
                }
            }
            else if(((row + col) % 2 == 0) && row != 0){
                if((isborder(map,row,col,bottom_border) != isborder(map,row - 1,col,top_border))){
                    return false;
                }
            }
        }
    }
    return true;
}
/**
 * @brief Sets starting border when entering maze, that will be followed by hand rule
 * @param map Pointer to the allocated structure of Map
 * @param r Current row
 * @param c Current column
 * @param leftright Identifies which hand rule is followed: 0 for right hand, 1 for left hand
 * @return Starting border, that will be followed by hand rule; -1 if failure
 */
int start_border(Map *map, int r, int c, int leftright){
    //Entering maze from left
    if(c == 0 && isborder(map,r,c,left_border) == false){
        if(r % 2 == leftright){
            return 1;
            }  
        else{
            return 2;
        }
    }
    //Entering maze from right
    else if(c == map->cols - 1 && isborder(map,r,c,right_border) == false){
        if((r + c) % 2 == leftright){
            return 2;
        }
        else{
            return 0;
        }
     }
    //Entering maze from top
    if(r == 0 && (r + c) % 2 == 0 && isborder(map,r,c,top_border) == false){
        return leftright;
    }
    //Entering maze from bottom
    else if(r == map->rows - 1 && (r + c) % 2 == 1 && isborder(map,r,c,bottom_border) == false){
        return (leftright + 1) % 2;
    }
    else{
        return -1;
    }
    return -1;
}
/**
 * @brief Prints the route though maze with hand rule
 * @param map Pointer to the allocated structure of map
 * @param entry_row Row of entry cell
 * @param entry_col Column of entry cell
 * @param Arg Which hand will be followed: 0 for right, 1 for left
 */
void handSolveMaze(Map *map, int entry_row,int entry_col,int Arg){
    Cell current_cell = {.col = entry_col, .row = entry_row};
    //Starting border
    int border = start_border(map,current_cell.row,current_cell.col,Arg);
    //Solves until it reaches out of the maze
    while(current_cell.col < map->cols && current_cell.row < map->rows && current_cell.col > -1 && current_cell.row > -1){
        //Change of position and border 
        if(isborder(map,current_cell.row,current_cell.col,border) == false){
           fprintf(stdout,"%d,%d\n",current_cell.row + 1,current_cell.col + 1);
            //Based on modulo of sum of row and col and argument
            if(border == left_border){
                if((current_cell.col + current_cell.row) % 2 == Arg){
                    border = left_border;
                }
                else{
                    border = top_border;
                }
                current_cell.col -= 1;
            }
            else if(border == right_border){
                if((current_cell.col + current_cell.row) % 2 == Arg){
                    border = bottom_border;
                }
                else{
                     border = right_border;
                }
                current_cell.col += 1;
            }
            else if(border == top_border){
                if((current_cell.col + current_cell.row) % 2 == 0){
                    border = (Arg + 1) % 2;
                    current_cell.row -= 1;
                }
                else{
                    border = Arg;
                    current_cell.row += 1;
                }
            }
        }
        //Change of actual border in current cell
        else{
            if((current_cell.col + current_cell.row) % 2 == Arg){
                    border += 1;
                    if(border == 3){
                        border = left_border;
                    }     
            }
            else{
                border -= 1;
                if(border == -1){
                    border = bottom_border;
                }            
            }
        } 
    }
}

/**
 * @brief Main function that handles command line arguments and executes functions based on them
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, error code on failure
 */
int main(int argc, char **argv){
    
    enum Argument Arg;

    Arg = parseArguments(argc,argv);
    //Wrong arguments
    if(Arg == error){
        fprintf(stderr,"Invalid argument. Use --help for more information\n");
        return 1;
    }
    else if (Arg == help)
    {
        printHelp();
        return 0;
    }
    //Allocate structure Map
    Map *map = ctor_Map();
    //Allocation failure of map
    if(map == NULL){
        fprintf(stderr,"The map couldn't be allocated\n");
        return 1;
    }

    Cell entry_cell;

    if(TEST_MAZE){
        //Missing file
        if(argc < 3){
            fprintf(stderr,"Missing file\n");
            dtor_Map(map);
            return 1;
        }
        //Loading maze from file into structure Map
        int file_valid = readFile(argv[2],map);
        //Opening the file failure
        if(file_valid == -1){
            fprintf(stderr,"File couldn't be opened\n");
            dtor_Map(map);
            return 1;
        }
        //Allocation failure of cells
        if(file_valid == 1){
            fprintf(stderr,"Cells couldn't be allocated\n");
            dtor_Map(map);
            return 1;
        }
        //Missing dimension of maze
        else if(file_valid == 2){
            fprintf(stderr,"Invalid\n");
            dtor_Map(map);
            return 1;
        }
        // Incorrect dimension of maze
        else if(file_valid == 3){
            fprintf(stdout,"Invalid\n");
            dtor_Map(map);
            return 0;
        }

        //Validity of maze
        if(isMazeValid(map) == true){
            fprintf(stdout,"Valid\n");
        }
        else{
            fprintf(stdout,"Invalid\n");
        }
        dtor_Map(map);
        return 0;
    }
    else if(SOLVE_MAZE){
        //Missing file
        if(argc < 5){
            fprintf(stderr,"Missing file\n");
            dtor_Map(map);
            return 1;
        }
        //Loading maze from file into structure Map
        int file_valid = readFile(argv[4],map);
        //Opening the file failure
        if(file_valid == -1){
            fprintf(stderr,"File couldn't be opened\n");
            dtor_Map(map);
            return 1;
        }
        //Allocation failure of cells
        if(file_valid == 1){
            fprintf(stderr,"Cells couldn't be allocated\n");
            dtor_Map(map);
            return 1;
        }
        //Incorrect dimension of maze
        else if(file_valid == 2){
            fprintf(stderr,"Incorect dimension of maze\n");
            dtor_Map(map);
            return 1;
        }
        //Missing dimension of maze
        else if(file_valid == 3){
            fprintf(stderr,"Missing dimension of maze\n");
            dtor_Map(map);
            return 1;
        }

        int cell_valid = startingCellValidity(argc,argv,&entry_cell.row,&entry_cell.col);
        //Missing coordinate of start position
        if (cell_valid == -1)
        {
            fprintf(stderr,"Missing coordinate of starting position\n");
            dtor_Map(map);
            return 1;
        }
        //Start position outside of the maze 
        else if (cell_valid == -2)
        {
            fprintf(stderr,"Wrong coordinate of starting position\n");
            dtor_Map(map);
            return 1;
        }
        //Validity of maze
        if(isMazeValid(map) == false){
            fprintf(stderr,"Maze is invalid\n");
            dtor_Map(map);
            return 1;
        }
    }
    //Start position outside of the maze
    if(entry_cell.col > map->cols || entry_cell.row > map->rows){
        fprintf(stderr,"Incorrect start position\n");
        dtor_Map(map);
        return 1;
    }
    else{
        entry_cell.row -= 1;
        entry_cell.col -= 1;
    }

    //Failure at entering the maze
    if(start_border(map,entry_cell.row,entry_cell.col,Arg) == -1){
        fprintf(stderr,"Can't enter the maze\n");
        dtor_Map(map);
        return 1;
    }
    //Finding path through maze
    handSolveMaze(map,entry_cell.row,entry_cell.col,Arg);
    
    dtor_Map(map);
    return 0;
}