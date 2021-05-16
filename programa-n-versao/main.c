//
//  main.c
//  programa-n-versao
//
//  Created by Amanda Tavares on 16/05/21.
//  Copyright © 2021 Amanda Tavares. All rights reserved.
//

#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

char listachar[30];

int indice = 0;
sem_t sem_send[6];
sem_t sem_receive[6];
int canal[6] = {-1,-1,-1,-1,-1,-1};

void send_async(int *buf, int c) {
    canal[c] = *buf;
    return;
}

void receive(int *buf, int c) {
    while (canal[c]==1) { // atencao aqui
        *buf = canal[c];
        canal[c] = -1;
    }
    return;
}

void send_sync(int *buf, int c) {
    canal[c] = *buf;
    while (canal[c != 1]) {
        return;
    }
}

int delay_s(float tempo) {
    time_t start, end;
    long unsigned t;
    start = time(NULL);
    do {
        end = time(NULL);
    } while (difftime(end, start)<=tempo);
    return 0;
}

int comparacao(int vetor[], int *versao_errada) {
    if((vetor[0]==vetor[1]) && (vetor[1]==vetor[2])) {
        *versao_errada = 0;
        return vetor[0];
    }
    if((vetor[0]==vetor[1]) && (vetor[1]!=vetor[2])) {
       *versao_errada = 3;
       return vetor[0];
    }
    if((vetor[0]!=vetor[1]) && (vetor[1]==vetor[2])) {
       *versao_errada = 1;
       return vetor[1];
    }
    if((vetor[0]==vetor[2]) && (vetor[1]!=vetor[2])) {
       *versao_errada = 2;
       return vetor[0];
    }
}

void send_async(int *buf, int c) {
    canal[c] = *buf;
    sem_post(&sem_send[c]);
    return;
}

void receive_send_async(int *buf, int c) {
    sem_wait(&sem_send[c]);
    *buf = canal[c];
    return;
}

void send_sync(int *buf, int c) {
    canal[c] = *buf;
    sem_post(&sem_send[c]);
    sem_wait(&sem_receive[c]);
    return;
}

void *thread_code_a(void *threadno) {
    int voto = 1; int status;
    send_async(&voto, 0);
    receive_send_async(&status, 3);
    if (status == 0) {
        printf("\nTA prossegue");
        while (1);
    } else {
        printf("\nTA gerou erro");
        return NULL;
    }
}

void *thread_code_b(void *threadno) {
    int voto = 1; int status;
    send_async(&voto, 1);
    receive_send_async(&status, 4);
    if (status == 0) {
        printf("\nTB prossegue");
        while (1);
    } else {
        printf("\nTB gerou erro");
        return NULL;
    }
}

void *thread_code_c(void *threadno) {
    int voto = 5; int status;
    send_async(&voto, 2);
    receive_send_async(&status, 5);
    if (status == 0) {
        printf("\nTB prossegue");
        while (1);
    } else {
        printf("\nTB gerou erro");
        return NULL;
    }
}

void *driver(void *threadno) {
    int vetor_comp[3]; int statusok = 0; int statuserro = 1;
    int voto_maj; int versao_erro;
    
    receive_send_async(&vetor_comp[0], 0);
    receive_send_async(&vetor_comp[1], 1);
    receive_send_async(&vetor_comp[2], 2);
    
    voto_maj = comparacao(vetor_comp, &versao_erro);
    printf("\n Resposta correta: %d", voto_maj);
    
    if (versao_erro == 0) {
        send_async(&statusok, 3);
        send_async(&statusok, 4);
        send_async(&statusok, 5);
    }
    if (versao_erro == 1) {
        send_async(&statusok, 3);
        send_async(&statusok, 4);
        send_async(&statusok, 5);
    }
    if (versao_erro == 2) {
        send_async(&statusok, 3);
        send_async(&statusok, 4);
        send_async(&statusok, 5);
    }
    if (versao_erro == 3) {
        send_async(&statusok, 3);
        send_async(&statusok, 4);
        send_async(&statusok, 5);
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    pthread_t TA;
    pthread_t TB;
    pthread_t TC;
    pthread_t TD;
    
    for (int i = 0; i<=5; i++) {
        sem_init(&sem_send[i], 0, 0);
        sem_init(&sem_receive[i], 0, 0);
    }
    
    pthread_t_create(&TA, NULL, thread_code_a, NULL);
    pthread_t_create(&TB, NULL, thread_code_b, NULL);
    pthread_t_create(&TC, NULL, thread_code_c, NULL);
    pthread_t_create(&TD, NULL, driver, NULL);
    
    system("PAUSE");
    return 0;
}
