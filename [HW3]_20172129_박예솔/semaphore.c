#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

int total_money = 10000;
pthread_mutex_t mutex;

/*
void enter_region() {

    asm(
        ".data\n"
        "lock:\n"
        ".byte 0\n"
        ".text\n"
        "_enter_region:\n"
        "movb $1, %al\n" // move 1 to AL
        "xchgb lock,%al\n" // swap
        "cmp $0, %al\n" 
        "jne _enter_region\n"
    );
}

void leave_region() {
    asm("movb $0, lock");
}
*/

int critical_region(char *p, int borrow) {
    int d = rand()%1000000;
    int m = rand()%1000;
    int c = rand()%2;

    // error handling
    if (borrow == 0 && c == 1) c = 0;
    if (total_money < m && c == 0){
	printf("The bank went bankrupt....T_T");
	return 0;
    }

    // case 0: borrow money
    if (c == 0){
	printf("Before banking : %s borrowed [%d won].\n", p, borrow*1000);
	total_money = total_money - m;
	borrow = borrow + m;
	printf(">> %s borrow [%d won]. (Current Bank's balance = %d)\n", p, m*1000, total_money*1000);
	printf("After banking : %s borrowed [%d won].\n", p, borrow*1000);
	usleep(d);
	return borrow;
    }
    // case 1: repay money
    else if (c == 1){
	// error handling
	if (borrow < m) m = borrow;

	printf("Before banking : %s borrowed [%d won].\n", p, borrow*1000);
	total_money = total_money + m;
	borrow = borrow - m;
	printf(">> %s repay [%d won]. (Current Bank's balance = %d)\n", p, m*1000, total_money*1000);
	printf("After banking : %s borrowed [%d won].\n", p, borrow*1000);
	usleep(d);
	return borrow;
    }
}

void noncritical_region(char *p) {
    int d = rand()%1000000;
    printf("%s is doing economic activity...\n", p);
    usleep(d);
}

static void* f1(void* p) {
    int borrow = 0;
    for(int i=0; i<10; i++) {
        puts("f1 wait for f2");
	// borrow money, 
        //enter_region();
        pthread_mutex_lock(&mutex);
	puts("");
        printf("========== f1 start banking.(critical section) ==========\n");
        borrow = critical_region(p, borrow);
        printf("========== f1 end banking.(critical section) ==========\n");
	puts("");
        //leave_region();
        pthread_mutex_unlock(&mutex);
	// do economy activity
        noncritical_region(p);
    }
    pthread_exit((void*)borrow);
}

static void* f2(void* p) {
    int borrow = 0;
    for(int i=0; i<10; i++) {
        puts("f2 wait for f1");
        //enter_region();
        pthread_mutex_lock(&mutex);
	puts("");
        printf("========== f2 start banking.(critical section) ==========\n");
        borrow = critical_region(p, borrow);
        printf("========== f2 end banking.(critical section) ==========\n");
	puts("");
        //leave_region();
        pthread_mutex_unlock(&mutex);
	// do economy activity
        noncritical_region(p);
    }
    pthread_exit((void*)borrow);
}

int main() {
    printf("********* Initial Bank's Balance = %d won *********\n", total_money*1000);
    int rc;
    int f1_borrow;
    int f2_borrow;
    pthread_t t1, t2;
    pthread_mutex_init(&mutex, NULL);

    rc = pthread_create(&t1, NULL, f1, "f1");
    if(rc != 0) {
        fprintf(stderr, "pthread f1 failed\n");
        return EXIT_FAILURE;
    }

    rc = pthread_create(&t2, NULL, f2, "f2");
    if(rc != 0) {
        fprintf(stderr, "pthread f2 failed\n");
        return EXIT_FAILURE;
    }

    pthread_join(t1, (void**)&f1_borrow);
    pthread_join(t2, (void**)&f2_borrow);

    puts("All threads finished.");
    printf("f1 borrow [%d], f2 borrow [%d], Final Bank's balance [%d]\n", f1_borrow*1000, f2_borrow*1000, total_money*1000);

    pthread_mutex_destroy(&mutex);
    return 0;

} 