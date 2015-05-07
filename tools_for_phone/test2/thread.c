#include <stdio.h>
#include <pthread.h>

void thread1(void)
{
	int i;
	for (i = 0; i < 5000; i++) {
		printf("The thread1's id is %lu.\n", pthread_self());
		sleep(1);
	}

}

void thread2(void)
{
	int i;
	for (i = 0; i < 5000; i++) {
		printf("The thread2's id is %lu.\n", pthread_self());
		sleep(1);
	}
}

int main(void)
{
	pthread_t id1,id2;
	int i, ret;
	ret = pthread_create(&id1, NULL, (void *)thread1, NULL);
	ret = pthread_create(&id2, NULL, (void *)thread2, NULL);
	if (ret != 0) {
		printf("creat failed\n");
		exit(1);
	}

//	for (i = 0; i < 100; i++)
//		printf("This is the main process\n");
	pthread_join(id1, NULL);
	pthread_join(id2, NULL);
	
	return 0;
}


