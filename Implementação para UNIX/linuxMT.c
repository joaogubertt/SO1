#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

// Declaração das variáveis globais
int total_itens_esteira1 = 0;
float total_peso_esteira1 = 0;
int total_itens_esteira2 = 0;
float total_peso_esteira2 = 0;
int contador = 0;

pthread_mutex_t mutex_itens1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_peso1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_itens2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_peso2 = PTHREAD_MUTEX_INITIALIZER;

void *esteira1(void *args);
void *esteira2(void *args);
void *atualizar_display(void *args);

// Função principal do programa
int main() {

        pthread_t display_thread;
        pthread_create(&display_thread, NULL, atualizar_display, NULL);

        pthread_t esteira1_thread, esteira2_thread;
        pthread_create(&esteira1_thread, NULL, esteira1, NULL);
        pthread_create(&esteira2_thread, NULL, esteira2, NULL);

        pthread_join(display_thread, NULL);
        pthread_join(esteira1_thread, NULL);
        pthread_join(esteira2_thread, NULL);

        return 0;
}

// Função para atualizar o display
void *atualizar_display(void *args){
        struct timeval start_time, end_time;
        double tempoFinal, tempoMedio;
        int iteracao;

        while (1) {
        int total_itens;
        float total_peso;

        iteracao ++;

        pthread_mutex_lock(&mutex_itens1);
        pthread_mutex_lock(&mutex_itens2);
        pthread_mutex_lock(&mutex_peso1);
        pthread_mutex_lock(&mutex_peso2);

        gettimeofday(&start_time, NULL);


        total_itens = total_itens_esteira1 + total_itens_esteira2;
        total_peso = total_peso_esteira1 + total_peso_esteira2;
         
        printf("Esteira 1 -> Itens: %d, Peso: %.2f Kg\n", total_itens_esteira1, total_peso_esteira1);
        printf("Esteira 2 -> Itens: %d, Peso: %.2f Kg\n", total_itens_esteira2, total_peso_esteira2);
            
            if (total_itens / 500 > contador) {
            contador = total_itens / 500;
            printf("Total de itens: %d, Peso total: %.2f Kg\n", total_itens, total_peso);
        }
         
        pthread_mutex_unlock(&mutex_itens1);
        pthread_mutex_unlock(&mutex_itens2);
        pthread_mutex_unlock(&mutex_peso1);
        pthread_mutex_unlock(&mutex_peso2);

        gettimeofday(&end_time, NULL);

        tempoFinal = (end_time.tv_sec - start_time.tv_sec) * 1000.0; // Segundos para milissegundos
        tempoFinal += (end_time.tv_usec - start_time.tv_usec) / 1000.0; // Microssegundos para milissegundos
        tempoMedio += tempoFinal;
        printf("Tempo Display: %f\n--------------\n\n", tempoMedio/iteracao);

        sleep(2);
        
        }
}

// Função para simular a esteira 1
void *esteira1(void *args) {
        while (1) {
        pthread_mutex_lock(&mutex_itens1);
        pthread_mutex_lock(&mutex_peso1);

        total_itens_esteira1++;
        total_peso_esteira1 += 5; 
         
        pthread_mutex_unlock(&mutex_itens1);
        pthread_mutex_unlock(&mutex_peso1);
         
        sleep(2); 
        }
}

// Função para simular a esteira 2
void *esteira2(void *args) {
        while (1) {
        pthread_mutex_lock(&mutex_itens2);
        pthread_mutex_lock(&mutex_peso2);

        total_itens_esteira2++;
        total_peso_esteira2 += 2; 
         
        pthread_mutex_unlock(&mutex_itens2);
        pthread_mutex_unlock(&mutex_peso2);

         
        sleep(1); 
        }
}