#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>

#define PIPE_NAME1 "/tmp/esteira1"
#define PIPE_NAME2 "/tmp/esteira2"

void atualizar_display() {
    int contador = 0;
    struct timeval start_time, end_time;
    double tempoFinal, tempoMedio;
    int iteracaoDisplay;

    while(1) {
        iteracaoDisplay++;

        int pipe_fd1 = open(PIPE_NAME1, O_RDONLY);
        int pipe_fd2 = open(PIPE_NAME2, O_RDONLY);

        gettimeofday(&start_time, NULL);
            
        if (pipe_fd1 < 0 || pipe_fd2 < 0) {
            perror("Erro ao abrir o pipe");
            exit(EXIT_FAILURE);
        }

        char data1[1024];
        char data2[1024];
        
        read(pipe_fd1, data1, sizeof(data1));
        read(pipe_fd2, data2, sizeof(data2));

        /// Ler dados do primeiro pipe
        char temp_data1[1024];
        strcpy(temp_data1, data1);
        int item1 = atoi(strtok(temp_data1, ", "));
        float peso1 = atof(strtok(NULL, ", "));
        char* nome1 = strtok(NULL, ", ");
        printf("Esteira %s -> Itens: %d, Peso total: %.2f Kg\n", nome1, item1, peso1);

        // Ler dados do segundo pipe
        char temp_data2[1024];
        strcpy(temp_data2, data2);
        int item2 = atoi(strtok(temp_data2, ", "));
        float peso2 = atof(strtok(NULL, ", "));
        char* nome2 = strtok(NULL, ", ");
        printf("Esteira %s -> Itens: %d, Peso total: %.2f Kg\n", nome2, item2, peso2);


        // Soma dos valores
        int total_itens = item1 + item2;
        float total_peso = peso1 + peso2;
 
        if (total_itens / 500 > contador) {
            contador = total_itens / 500;
            printf("Total de itens: %d, Peso total: %.2f Kg\n", total_itens, total_peso);
        }
        
        close(pipe_fd1);
        close(pipe_fd2);

        gettimeofday(&end_time, NULL);

        tempoFinal = (end_time.tv_sec - start_time.tv_sec) * 1000.0; // Segundos para milissegundos
        tempoFinal += (end_time.tv_usec - start_time.tv_usec) / 1000.0; // Microssegundos para milissegundos
        tempoMedio += tempoFinal;
        printf("Tempo Display: %f\n--------------\n\n", tempoMedio/iteracaoDisplay);

        sleep(2);
    }
}

void esteira(const char* pipe_name, float peso, char* nome, int intervalo) {
    int itens = 0;
    float pesoTotal = 0;

    while (1) {
        char buffer[1024];
        itens++;
        pesoTotal += peso; 
        
        int pipe_fd = open(pipe_name, O_WRONLY);

        if (pipe_fd < 0) {
            exit(EXIT_FAILURE);
        }

        int ret = sprintf(buffer, "%d, %.2f, %s\n", itens, pesoTotal, nome);
        if (ret < 0) {
            close(pipe_fd);
            exit(EXIT_FAILURE);
            continue;
        }else{
            write(pipe_fd, buffer, strlen(buffer));
        }

        close(pipe_fd);

        sleep(intervalo); // Intervalo personalizado de acordo com a esteira
    }
}

int main() {

    mkfifo(PIPE_NAME1, 0666);
    mkfifo(PIPE_NAME2, 0666);

    pid_t display_pid, esteira1_pid, esteira2_pid;

    esteira1_pid = fork();
    if (esteira1_pid == 0) {
        // Processo filho para a esteira 1
        esteira(PIPE_NAME1, 5.0, "1", 2); // Intervalo de 2 segundos
        exit(EXIT_SUCCESS);
    }

    esteira2_pid = fork();
    if (esteira2_pid == 0) {
        // Processo filho para a esteira 2
        esteira(PIPE_NAME2, 2.0, "2", 1); // Intervalo de 1 segundo
        exit(EXIT_SUCCESS);
    }

    display_pid = fork();
    if (display_pid == 0) {
        // Processo filho para o display
        atualizar_display();
        exit(EXIT_SUCCESS);
    }

    // Esperar pelo término do processo filho do display
    waitpid(display_pid, NULL, 0);

    // Esperar pelo término de todos os processos filhos de esteira
    wait(NULL);
    wait(NULL);

    return 0;
}