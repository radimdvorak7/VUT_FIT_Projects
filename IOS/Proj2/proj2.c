/* proj2.c
 * Reseni IOS-proj2, 28.04.2024
 * Autor: Radim Dvorak, xdvorar00
 */

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

FILE *file;

//Semaphores
sem_t *bus;
sem_t *write_mutex;
sem_t *board;
sem_t *arrival;
sem_t *mutex;
sem_t *leave;
sem_t *final;

//Shared memory variables
int *out_log;
int *waiting_passangers;
int *bus_stop;
int *all_passangers;
int *actual_passangers;

void cleanup();

//Error when creating shared memory
void memory_error(){
    fprintf(stderr, "Error: Can't create shared memory\n");
    cleanup();
    exit(1);
}

//Error when creating semaphore
void semaphore_error(){
    fprintf(stderr, "Error: Can't create semaphore\n");
    cleanup();
    exit(1);
}
//Error when forking process
void fork_error(){
    fprintf(stderr, "Error: Can't fork process\n");
    cleanup();
    exit(1);
}

//Generates random integer between lower and upper bound
int random_int(int lower,int upper){
    int num = (rand() % (upper - lower + 1)) + lower; 
    return num;
}

//Function for writing into output file
void write_log(const char *fmt, ... ){
    sem_wait(write_mutex);

    va_list args;
    va_start(args, fmt);
    (*out_log)++;
    fprintf(file,"%d: ",*out_log);
    vfprintf(file, fmt, args);
    fflush(NULL);
    va_end(args);
    

    sem_post(write_mutex);
}

//Parsing arguments
int parseArguments(int *params,char **argv){
    char *stopstring;

    for(int i = 0; i< 5;i++){    
        errno = 0; 
        params[i] = strtol(argv[i+1],&stopstring,10);
        if(errno != 0 || !argv[i+1] || *stopstring){
            errno = 0; 
            return -1;
        }
    }
    //Checking range of arguments
    if(params[0] >= 20000){
        return 1;
    }
    if(params[1] <= 0 || params[1] > 10){
        return 2;
    }
    if(params[2] < 10 || params[2] > 100){
        return 3;
    }
    if(params[3] < 0 || params[3] > 10000){
        return 4;
    }
    if(params[4] < 0 || params[4] > 1000){
        return 5;
    }
    return 0;
}

//Initilaze semaphores
void semaphor_init(){
    if((write_mutex = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(write_mutex,1,1)) == -1){
        semaphore_error();
    }

    if((board = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(board,1,0)) == -1){
        semaphore_error();
    }

    if((bus = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(bus,1,0)) == -1){
        semaphore_error();
    }

    if((arrival = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(arrival,1,0)) == -1){
        semaphore_error();
    }

    if((mutex = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(mutex,1,1)) == -1){
        semaphore_error();
    }

    if((leave = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(leave,1,0)) == -1){
        semaphore_error();
    }

    if((final = mmap(NULL,sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,-1,0)) == MAP_FAILED){
        memory_error();
    }
    if((sem_init(final,1,0)) == -1){
        semaphore_error();
    }
}

void create_shared_memory(){
    if((out_log = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,0,0)) == MAP_FAILED){
        memory_error();
    }

    if((waiting_passangers = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,0,0)) == MAP_FAILED){
        memory_error();
    }

    if((bus_stop = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,0,0)) == MAP_FAILED){
        memory_error();
    }

    if((actual_passangers = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,0,0)) == MAP_FAILED){
        memory_error();
    }

    if((all_passangers = mmap(NULL,sizeof(int),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,0,0)) == MAP_FAILED){
        memory_error();
    }
}

//Closes file and
void cleanup(){
    fclose(file);

    sem_destroy(write_mutex);
    sem_destroy(board);
    sem_destroy(bus);
    sem_destroy(arrival);
    sem_destroy(mutex);

    munmap(write_mutex,sizeof(sem_t));
    munmap(board,sizeof(sem_t));
    munmap(bus,sizeof(sem_t));
    munmap(arrival,sizeof(sem_t));
    munmap(mutex,sizeof(sem_t));

    munmap(out_log,sizeof(int));
    munmap(waiting_passangers,sizeof(int));
    munmap(bus_stop,sizeof(int));
    munmap(actual_passangers,sizeof(int));
    munmap(all_passangers,sizeof(int));
}

//Function for process of skibus
void skibus(int bus_sleep,int num_stops, int capacity){
    srand(time(NULL) * getpid());
    write_log("BUS: started\n");
    usleep(random_int(0,bus_sleep));

    do {
        (*bus_stop) = 0;
        while((*bus_stop) < num_stops){
            //Arrival
            sem_wait(mutex);
            write_log("BUS: arrived to %d\n",++(*bus_stop));
            sem_post(mutex);
            for(int i = (*waiting_passangers); i > 0;i--){
                sem_post(arrival);
            }
            //Boarding of all posible passangers
            for(int i = (*waiting_passangers); i > 0 && (*actual_passangers) < capacity; i--){
                sem_post(bus);
                sem_wait(board);
            }
            //Departure
            write_log("BUS: leaving %d\n",(*bus_stop));
            usleep(random_int(0,bus_sleep));
        
        }
        write_log("BUS: arrived to final\n");
        //Getting of passengers at final stop
        while((*actual_passangers) != 0){
            sem_post(leave);
            sem_wait(final);
        }
        write_log("BUS: leaving final\n");
    }while((*all_passangers) != 0);
    //Bus finishes
    write_log("BUS: finish\n");
    exit(0);
}

//Function for process of skier
void ski(int index, int max_stops, int ski_sleep){
    //Randomly setting starting stop
    srand(time(NULL) * getpid());
    int start_stop = random_int(1,max_stops);

    sem_wait(mutex);
    write_log("L %d: started\n",index+1);
    sem_post(mutex);
    //Breakfest
    usleep(random_int(0,ski_sleep));

    //Skier arrives to stop
    sem_wait(mutex);
    write_log("L %d: arrived to %d\n",index+1,start_stop);
    (*waiting_passangers)++;
    sem_post(mutex);

    sem_wait(arrival);
    //Gets on bus if it is in the same stop
    while(start_stop != (*bus_stop)){
        sem_wait(bus);
        sem_post(arrival);   
    }
    write_log("L %d: boarding\n",index+1);
    (*waiting_passangers)--;
    (*actual_passangers)++;
    sem_post(board);

    //Getting of the bus
    sem_wait(leave);
    (*actual_passangers)--;
    (*all_passangers)--;
    write_log("L %d: going to ski\n",index+1);

    sem_post(final);
    exit(0);
}

int main(int argc, char **argv){

    if(argc < 6){
        fprintf(stderr,"ERROR: Missing input parameter\n");
        return 1;
    }

    int params[5];
    int arg_correct = parseArguments(params,argv);

    if(arg_correct == 1){
        fprintf(stderr,"ERROR: Invalid value of parameter L\n");
        return 1;
    }
    else if(arg_correct == 2){
        fprintf(stderr,"ERROR: Invalid value of parameter Z\n");
        return 1;
    }
    else if(arg_correct == 3){
        fprintf(stderr,"ERROR: Invalid value of parameter K\n");
        return 1;
    }
    else if(arg_correct == 4){
        fprintf(stderr,"ERROR: Invalid value of parameter TL\n");
        return 1;
    }
    else if(arg_correct == 5){
        fprintf(stderr,"ERROR: Invalid value of parameter TB\n");
        return 1;
    }
    else if(arg_correct != 0){
        fprintf(stderr,"ERROR: Invalid format of input parameters\n");
        return 1;
    }

    if((file = fopen("proj2.out","w")) == NULL){
        fprintf(stderr,"ERROR: Can't open file\n");
        return 1;
    }
    //Initiate semaphores and shared memory
    semaphor_init();
    create_shared_memory();
    (*waiting_passangers) = 0;
    (*all_passangers) = params[0];
    //Fork skibus
    pid_t bus = fork();
    if(bus == 0){
        skibus(params[4],params[1],params[2]);
    }
    else if (bus == -1){
        fork_error();
    }
    //Fork skiers
    pid_t skier;
    for(int i = 0; i < params[0];i++){
        skier = fork();
        if(skier == 0){
            ski(i,params[1],params[3]);
        }
        else if (skier == -1)
        {
            fork_error();
        }
        
    }
    //Wait for child processes
    while(wait(NULL) > 0); 

    //Clean semaphores and memory, closes file
    cleanup();
    return 0;
}