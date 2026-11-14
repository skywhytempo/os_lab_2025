#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg){
    pthread_mutex_lock(&mutex1);
    printf("Поток 1: мьютекс 1 заблокирован\n");

    sleep(1);

    printf("Поток 1: мьютекс 2 ожидает\n");
    pthread_mutex_lock(&mutex2);
    printf("Поток 1: мьютекс 2 заблокирован\n");
    

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);

    return NULL;

}

void *thread2(void *arg){
    pthread_mutex_lock(&mutex2);
    printf("Поток 2: мьютекс 2 заблокирован\n");

    sleep(1);

    printf("Поток 2: мьютекс 1 ожидает\n");
    pthread_mutex_lock(&mutex2);
    printf("Поток 2: мьютекс 1 заблокирован\n");

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);

    return NULL;



}

int main(){
    pthread_t thr1, thr2;

    pthread_create(&thr1, NULL, thread1, NULL);
    pthread_create(&thr2, NULL, thread2, NULL);

    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

    printf("Конец");
    return 0;
}