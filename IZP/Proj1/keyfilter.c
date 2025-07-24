/*
IZP - 1.Projekt: Práce s textem 
Jméno: Radim Dvořák
Login: xdvorar00

29.10.2023
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define ADDRESS_LENGTH 102     //100 characters + space for '\n' and '\0'
#define MAX_NUM_ADRESS 50   

struct adress_data
{
    char result_adress[ADDRESS_LENGTH];
    char next_char;
};

/*
Function, searches given array of chars, and saves them into output array in order of their ASCII value, and prints the result
*/
void print_sorted_array(char init_char[],int size_string){      
    char out_char[size_string];                                 // Declaration of output string of the same length as input string
    int k = 0;                                                  // Index of output array
    for (int ch = ' '; ch <= '~'; ch++){                        // Cycle for searching of printable chars in input string
        for(int i = 0; i < size_string;i++){
            if(init_char[i] == ch){                             // When char is found, it is saved into out-string
                out_char[k] = ch;
                out_char[k+1] = '\0';
                k++;
                break;
            }
            else{
                continue;
            }
        }
    }
    printf("%s\n",out_char);
}


int main(int argc, char** argv){
    int arg_length;

    int count_adress = 0;

    struct adress_data adress_data[MAX_NUM_ADRESS];                 // Array of structs of type adress_data
    
    char arg[ADDRESS_LENGTH];                                   

    if(argc == 1){                                                  // When the argument isn't typed, set arg to '\0'
        arg[0] = '\0';
        arg_length = 0;
    }
    else if(argc > 2){                                              //When are typed more arguments than 1, print error message
        fprintf(stderr,"Too many arguments, consider putting them into quotation marks\n");
        return 1;
    }
    else{                                                           //When there is typed only 1 argument, copy it into arg
        arg_length = strlen(argv[1]);
        strncpy(arg, argv[1], ADDRESS_LENGTH);      
    }

    if (arg_length >= (ADDRESS_LENGTH - 1))                         //Control of length of argument, when it is more than 100, print error message
    {
        fprintf(stderr,"The argument exceeds lenght of 100 characters\n");
        return 1;
    }
    
    while(fgets(adress_data[count_adress].result_adress,ADDRESS_LENGTH,stdin) != NULL){     //Gets adress from stdin and saves it into string
        if(strlen(adress_data[count_adress].result_adress) > (ADDRESS_LENGTH - 1)){
            fprintf(stderr,"The adress exceeds lenght of 100 characters\n");        //Control of length of adress, when it is more than 100, print error message
            return 1;
        }
        for(int i = 0; i < ADDRESS_LENGTH; i++){                                   //Get uppercase variant of all lowercase letters
            adress_data[count_adress].result_adress[i] = toupper(adress_data[count_adress].result_adress[i]);  
            if(adress_data[count_adress].result_adress[i] == '\0'){
                break;
            }
            else{
                continue;
            }
        }

        /*
        When the both chars in arg and adress at the same index are same, continue the cycle
        If the char is at the end of arg, save next char and increment count_adress
        When first different chars are read, cycle through next adress
        */
        for (int i = 0; i < ADDRESS_LENGTH; i++){
            arg[i] = toupper(arg[i]);                                      //Get uppercase variant of all lowercase letters
            if(adress_data[count_adress].result_adress[i] == arg[i]){                                   
                continue;    
            }
            else if (i == arg_length){
                adress_data[count_adress].next_char = adress_data[count_adress].result_adress[i];
                count_adress++;  
                break;
            }
            else {
                break;
            }     
        }
    }

    if(count_adress == 0){                                  //When count_adress is zero, so the arg wasn't prefix to any adress, print "Not found"
        printf("Not found\n");
    }
    else if(count_adress == 1){                             //When count_adress is 1, so the arg was prefix to only one adress, print "Found:" and the adress
        printf("Found: %s",adress_data[0].result_adress);
    }

    /*
    If there are multiple adresses with the prefix, that they share with arg, print "Enable:"
    All chars write into another array, which will then be sorted and printed
    */
    else{                                             
        char next_chars[count_adress + 1];
        for (int i = 0; i < count_adress; i++){
            next_chars[i] = adress_data[i].next_char;
            next_chars[i+1] = '\0';
        }
        printf("Enable: ");
        print_sorted_array(next_chars,count_adress);
    }
    return 0;
}

