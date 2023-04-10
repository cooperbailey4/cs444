#include <pthread.h>
#include <stdio.h>

void *zero_to_four(void *arg){
	char *threadnum = arg;

	for(int i = 0; i < 5; i++) {
		printf("thread %s: %d \n", threadnum, i);
	}
	return NULL;

}

pthread_t t1, t2;

int main(void) {
	printf("Launching threads\n");
	pthread_create(&t1, NULL, zero_to_four, "1");
	pthread_create(&t2, NULL, zero_to_four, "2");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	printf("Threads complete!\n");
}
