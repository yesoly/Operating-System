#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

int gNumber;
int save = 0;

void *producer_func(void *arg){
	printf("producer thread is running...\n");
	int prod_sum = 0;
	int i = 0;
	srand(time(NULL));
	while(i < 100){
		if(save == 0){
			gNumber = rand() % 100;	// make random number
			printf("producer >> Number[%d]: %d\n", i+1, gNumber);
			fflush(stdout);
			prod_sum += gNumber;
			save = 1;
			i++;
		}
		else{
			while(save != 0){
				wait();			
			}		
		}
	}
	sleep(1);
	pthread_exit((void *)prod_sum);
}
void *consumer_func(void *arg){	
	printf("consumer thread is running...\n");
	int cons_sum = 0;
	int i = 0;
	while(i < 100){
		if(save == 1){
			printf("consumer >> Number[%d]: %d\n", i+1, gNumber);
			fflush(stdout);
			cons_sum += gNumber;
			save = 0;
			i++;
		}
		else{
			while(save != 1){
				wait();			
			}			
		}
	}
	sleep(1);
	pthread_exit((void *)cons_sum);
}

int main(){
	int prod_res;
	int cons_res;
	void *prod_result;
	void *cons_result;

	pthread_t producer;
	pthread_t consumer;

	prod_res = pthread_create(&producer, NULL, producer_func, NULL);
	cons_res = pthread_create(&consumer, NULL, consumer_func, NULL);
	
	if(prod_res != 0 || cons_res != 0){
		printf("Thread create return error code\n");
		exit(-1);
	}

	printf("Waiting for thread to finish...\n");
	prod_res = pthread_join(producer, &prod_result);
	cons_res = pthread_join(consumer, &cons_result);

	if(prod_res != 0 || cons_res != 0){
		printf("Thread join return error code\n");
		exit(-1);
	}

	if((int*)prod_result == (int*)cons_result){
		printf("========== Success ==========\n");
		printf("Producer Thread returned: %d\n", (int*)prod_result);
		printf("Consumer Thread returned: %d\n", (int*)cons_result);
	}
	else{
		printf("========== Fail ==========\n");
		printf("Producer Thread returned: %d\n", (int*)prod_result);
		printf("Consumer Thread returned: %d\n", (int*)cons_result);
	}

	exit(NULL);
}