#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*Global variables */
int num_threads; 
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
    phil_data * phil = (phil_data *) args;
    phil->course += 1;
    sleep(1);
    return NULL;
    //pthread_exit(NULL);
    
    // pthread_exit(NULL);
/* 3 course meal: Each need to acquire both forks 3 times.
 *  First try for fork in front.
 * Then for the one on the right, if not fetched, put the first one back.
 * If both acquired, eat one course.
 */
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
	pthread_t threads[num_threads]; // Might need to make dynamic arrays.
	phil_data philosophers[num_philosophers]; //Struct for each philosopher
	mutexes = malloc(sizeof(pthread_mutex_t)*num_philosophers); //Each mutex element represent a fork
    pthread_cond_t cond_var[num_threads];

    // Initialize condition variables, is_blocked, and mutexes
    for (int i = 0; i < num_threads; i++) {
        pthread_cond_init(&cond_var[i], NULL);
        pthread_mutex_init(&mutexes[i], NULL);
    }
	/* Initialize structs */
	for( int i = 0; i < num_philosophers; i++ ){
		philosophers[i].phil_num = i;
		philosophers[i].course   = 0;
		philosophers[i].forks    = none;
	}

   
    int total_courses = num_philosophers * 3;
    int eaten_courses = 0;
    while (eaten_courses < total_courses) {
        for (int i = 0; i < num_philosophers; i++) {
            
            pthread_mutex_lock(&mutexes[i]);
            if (philosophers[i].course < 3 && philosophers[i].forks == none) {
                philosophers[i].forks = one;
                
                if (i == num_philosophers - 1) {
                    pthread_mutex_lock(&mutexes[0]);
                    if (philosophers[0].forks == none) {
                        philosophers[i].forks = two;
                        pthread_create(&threads[i], NULL, &eat_meal, &philosophers[i]);
                    }
                    pthread_mutex_unlock(&mutexes[0]);
                }
                else {
                    pthread_mutex_lock(&mutexes[i+1]);
                    if (philosophers[i+1].forks == none) {
                        philosophers[i].forks = two;
                        pthread_create(&threads[i], NULL, &eat_meal, &philosophers[i]);
                    }
                    pthread_mutex_unlock(&mutexes[i+1]);
                }

                if (philosophers[i].course == 3) {
                    eaten_courses += 3;
                }
                philosophers[i].forks = none;
            }
            pthread_mutex_unlock(&mutexes[i]);
        }

    }

    // cleanup
    for (int i = 0; i < num_philosophers; i++) {
        pthread_join(threads[i], NULL);
        pthread_cond_destroy(&cond_var[i]);
        pthread_mutex_destroy(&mutexes[i]);
    }

    run(philosophers, num_philosophers);

    
/* Each thread will represent a philosopher */

/* Initialize Mutex, Create threads, Join threads and Destroy mutex */

	return 0;
}
