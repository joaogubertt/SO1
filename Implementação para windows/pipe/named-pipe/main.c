#include <stdio.h>
#include <windows.h>

#define PIPE_NAME1 "\\\\.\\pipe\\meu_pipe1"
#define PIPE_NAME2 "\\\\.\\pipe\\meu_pipe2"

int main() {
    HANDLE pipe, pipe2;
    DWORD bytesRead;
    int contador =0;
    char buffer1[1024];
    char buffer2[1024];
    LARGE_INTEGER frequency;
    LARGE_INTEGER start, end;

    // Abre os pipes para leitura
    pipe = CreateFile(PIPE_NAME1, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (pipe == INVALID_HANDLE_VALUE) {
        printf("Erro ao abrir o pipe. Código de erro: %d\n", GetLastError());
        return 1;
    }

    pipe2 = CreateFile(PIPE_NAME2, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (pipe2 == INVALID_HANDLE_VALUE) {
        printf("Erro ao abrir o pipe. Código de erro: %d\n", GetLastError());
        return 1;
    }

    while (1)
    {

        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
        // Lê dos pipes
        if (!ReadFile(pipe, buffer1, sizeof(buffer1), &bytesRead, NULL)) {
            printf("Erro ao ler do pipe. Código de erro: %d\n", GetLastError());
            CloseHandle(pipe);

            return 1;
        }

        if (!ReadFile(pipe2, buffer2, sizeof(buffer2), &bytesRead, NULL)) {
            printf("Erro ao ler do pipe. Código de erro: %d\n", GetLastError());
            CloseHandle(pipe);

            return 1;
        }

        // Adicione uma terminação de string ao buffer lido
        buffer1[bytesRead] = '\0';
        buffer2[bytesRead] = '\0';

        // Agora você pode analisar os dados no buffer usando strtok
        strcpy(buffer1, buffer1);
        int itens1 = atoi(strtok(buffer1, ", "));
        float peso1 = atof(strtok(NULL, ", "));
        
        strcpy(buffer2, buffer2);
        int itens2 = atoi(strtok(buffer2, ", "));
        float peso2 = atof(strtok(NULL, ", "));


        // Processar os dados lidos dos pipes
        printf("Esteira 1 -> Itens=%d, Peso=%.2f\n", itens1, peso1);
        printf("Esteira 2 -> Itens=%d, Peso=%.2f\n", itens2, peso2);

        QueryPerformanceCounter(&end);
        double elapsed = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
        printf("Tempo Display: %.6f milissegundos\n--------\n\n", elapsed);
        // Soma dos valores
        int total_itens = itens1 + itens2;
        float total_peso = peso1 + peso2;


        if (total_itens / 20 > contador) {
            contador = total_itens / 20;
            printf("Total de itens: %d, Peso total: %.2f Kg\n", total_itens, total_peso);
        }
            
        Sleep(2000);
    }

    // Fecha os handles
    CloseHandle(pipe);
    return 0;

}
