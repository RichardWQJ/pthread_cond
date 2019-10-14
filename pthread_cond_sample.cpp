/*	
	1. g++ -c pthread_cond_sample.cpp -o pthread_cond_sample.o
	2. g++ -o pthread_cond_sample pthread_cond_sample.o -lpthread
	或者直接：
	g++ pthread_cond_sample.cpp -lpthread
*/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t count_lock;
pthread_cond_t count_nonzero;
unsigned count = 0;

static int workValueA = 0;
static int workValueB = 0;

//customerA
void * decrement_countA(void *arg) {
	while (1) {
		pthread_mutex_lock(&count_lock);
		printf("decrement_countA get count_lock\n");
		while (count == 0) {
			printf("decrement_countA count == 0 \n");
			printf("decrement_countA before cond_wait \n");
			//pthread_cond_wait内部会解锁，然后等待条件变量被其它线程激活, pthread_cond_wait被激活后会再自动加锁
			pthread_cond_wait(&count_nonzero, &count_lock);
			printf("decrement_countA after cond_wait \n");
		}
		count = count - 1;
		workValueA++;
		pthread_mutex_unlock(&count_lock);
		printf("--------------------------------------decrement_countA\n");
	}
}

//customerB
void * decrement_countB(void *arg) {
	while (1) {
		pthread_mutex_lock(&count_lock);
		printf("decrement_countB get count_lock\n");
		while (count == 0) {
			printf("decrement_countB count == 0 \n");
			printf("decrement_countB before cond_wait \n");
			//pthread_cond_wait内部会解锁，然后等待条件变量被其它线程激活, pthread_cond_wait被激活后会再自动加锁
			pthread_cond_wait(&count_nonzero, &count_lock);
			printf("decrement_countB after cond_wait \n");
		}
		count = count - 1;
		workValueB++;
		pthread_mutex_unlock(&count_lock);
		printf("--------------------------------------decrement_countB\n");
	}
}

//producer
void * increment_count(void *arg) {
	while (1) {
		pthread_mutex_lock(&count_lock);
		printf("increment_count get count_lock\n");
		if (count == 0) {
			printf("increment_count before cond_signal\n");
			pthread_cond_signal(&count_nonzero);
			//pthread_cond_broadcast(&count_nonzero);
			printf("increment_count after cond_signal\n");
		}
		count = count + 1;
		pthread_mutex_unlock(&count_lock);
		sleep(1);
		printf("--------------------------------------increment_count\n");
		printf("workValueA:[%d], workValueB:[%d]\n", workValueA, workValueB);
	}
}

int main(void)
{
	pthread_t tid1, tid2, tid3;

	pthread_mutex_init(&count_lock, NULL);
	pthread_cond_init(&count_nonzero, NULL);

	pthread_create(&tid1, NULL, decrement_countA, NULL);
	pthread_create(&tid2, NULL, decrement_countB, NULL);
	sleep(2);
	pthread_create(&tid3, NULL, increment_count, NULL);

	sleep(10);
	pthread_exit(0);
}

