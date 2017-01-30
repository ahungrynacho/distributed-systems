#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/*Global variables */
int num_threads, total_courses, num_courses;
pthread_mutex_t * mutexes;

/* For representing the status of each philosopher */
typedef enum{
	none,   // No forks
	one,    // One fork
	two     // Both forks to consume
} utensil;

/* Representation of a philosopher */
typedef struct phil_data{
	int phil_num;
	int course;
	utensil forks; 
}phil_data;

/* ****************Change function below ***************** */
void * eat_meal(void * args){
    // Have the thread acquire two locks in here. 
    
    phil_data * phil = (phil_data *) args;
    int i = phil->phil_num;
    if (!pthread_mutex_trylock(&mutexes[i]) && phil->course < 3) {
        phil->forks = one;

        if (i == num_threads - 1) {
            switch(pthread_mutex_trylock(&mutexes[0])) {
                case 0: // no error
                    phil->forks = two;
                    phil->course += 1;
                    sleep(1);
                    pthread_mutex_unlock(&mutexes[0]);
                    break;
                case EBUSY:
                    pthread_mutex_unlock(&mutexes[i]);
                    return NULL;
                default:
                    printf("other error occurred\n");

            }

        }
        else {
             switch(pthread_mutex_trylock(&mutexes[i+1])) {
                case 0: // no error
                    phil->forks = two;
                    phil->course += 1;
                    sleep(1);
                    pthread_mutex_unlock(&mutexes[i+1]);
                    break;
                case EBUSY:
                    pthread_mutex_unlock(&mutexes[i]);
                    return NULL;
                default:
                    printf("other error occurred\n");

            }  

        }
        if (phil->course == 3) {
            num_courses += 3;
        }
        phil->forks = none;
        pthread_mutex_unlock(&mutexes[i]);

    }
    else
        pthread_mutex_unlock(&mutexes[i]);

    return NULL;
}

void run(phil_data * philos, int length) {
    char buf[512 * length];
    sprintf(buf, "%-20s %-s\n", "Philosopher", "Courses Eaten");
    printf(buf);
    for (int i = 0; i < length; i++) {
        /*
        char * int_to_str;
        itoa(philos[i].phil_num, int_to_str, 10);
        strcat(buf, int_to_str);
        */
        printf("%-20d %-d\n", philos[i].phil_num, philos[i].course);
    }
    return;
}

    
/* ****************Add the support for pthreads in function below ***************** */
int main( int argc, char ** argv ){
	int num_philosophers;

	if (argc < 2) {
          fprintf(stderr, "Format: %s <Number of philosophers>\n", argv[0]);
          return 0;
     }
    
    // use opencv to open images
    num_philosophers = num_threads = atoi(argv[1]);
    total_courses = num_threads * 3;
    num_courses = 0;

	pthread_t threads[num_threads]; // Might need to make dynamic arrays.
	phil_data philosophers[num_philosophers]; //Struct for each philosopher
	mutexes = malloc(sizeof(pthread_mutex_t)*num_philosophers); //Each mutex element represent a fork

    // Initialize condition variables, is_blocked, and mutexes
    for (int i = 0; i < num_threads; i++) {
        pthread_mutex_init(&mutexes[i], NULL);
    }
	/* Initialize structs */
	for( int i = 0; i < num_philosophers; i++ ){
		philosophers[i].phil_num = i;
		philosophers[i].course   = 0;
		philosophers[i].forks    = none;
	}

   
    while (num_courses < total_courses) {
        for (int i = 0; i < num_philosophers; i++) {
            pthread_create(&threads[i], NULL, &eat_meal, &philosophers[i]);
        }
        //printf("%s %d\n", "num_courses", num_courses);
    }

    // cleanup
    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_mutex_destroy(&mutexes[i]);
    }

    // run(philosophers, num_philosophers);

    
/* Each thread will represent a philosopher */

/* Initialize Mutex, Create threads, Join threads and Destroy mutex */

	return 0;
}
