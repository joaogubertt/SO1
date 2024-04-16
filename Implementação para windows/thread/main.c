#include <stdio.h>
#include <windows.h>

#define MAX_CONTADOR 250

// Variável compartilhada e mutex para proteger o acesso a ela
int soma_peso = 0;
int itens = 0;
HANDLE mutex;

LARGE_INTEGER frequency;
LARGE_INTEGER start, end;

// Semáforos para sincronizar a alternância entre as threads
HANDLE semThread1, semThread2, semThread3;

// Função que será executada pela thread 1
DWORD WINAPI thread1(LPVOID lpParam) {
    for (int i = 0; i < MAX_CONTADOR; i++) {
        WaitForSingleObject(semThread1, INFINITE); // Aguarda sinal para começar
        WaitForSingleObject(mutex, INFINITE); // Aguarda acesso exclusivo à variável compartilhada
        itens++;
        soma_peso += 5;
        printf("Esteira 1 passando mais um item...\n");
        ReleaseMutex(mutex); // Libera o acesso à variável compartilhada
        Sleep(2000);
        ReleaseSemaphore(semThread3, 1, NULL); 
    }
    return 0;
}

// Função que será executada pela thread 2
DWORD WINAPI thread2(LPVOID lpParam) {
    for (int i = 0; i < 2 * MAX_CONTADOR; i++) {
        WaitForSingleObject(semThread2, INFINITE); // Aguarda sinal para começar
        WaitForSingleObject(mutex, INFINITE); // Aguarda acesso exclusivo à variável compartilhada
        printf("Esteira 2 passando mais um item...\n");
        itens++;
        soma_peso += 2;
        ReleaseMutex(mutex); // Libera o acesso à variável compartilhada
        Sleep(1000);
        ReleaseSemaphore(semThread2, 1, NULL);
    }
    return 0;
}

// Função que será executada pela thread 3
DWORD WINAPI thread3(LPVOID lpParam) {
    int var_aux, peso_mostrar;
    for (int i = 0; i < MAX_CONTADOR; i++) {
        WaitForSingleObject(semThread3, INFINITE); // Aguarda sinal para começar
        WaitForSingleObject(mutex, INFINITE); // Aguarda acesso exclusivo à variável compartilhada
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
        var_aux = 500 - itens;
        if (itens % 500 == 0 && itens != 0)
            peso_mostrar = soma_peso;
        printf("Total de itens: %d. Peso Total =  %d\n", itens, peso_mostrar);
        QueryPerformanceCounter(&end);
        double elapsed = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
        printf("Tempo Display: %.6f milissegundos\n--------\n\n", elapsed);
        ReleaseMutex(mutex); // Libera o acesso à variável compartilhada
        // Sinaliza para a thread 1 e a thread 2 continuarem após a execução da thread 3
        ReleaseSemaphore(semThread1, 1, NULL); 
        ReleaseSemaphore(semThread2, 1, NULL);
    }
    return 0;
}

int main() {
    // Inicializa o mutex e os semáforos
    mutex = CreateMutex(NULL, FALSE, NULL);
    semThread1 = CreateSemaphore(NULL, 0, 1, NULL); // Inicialmente bloqueado
    semThread2 = CreateSemaphore(NULL, 0, 1, NULL); // Inicialmente bloqueado
    semThread3 = CreateSemaphore(NULL, 0, 1, NULL); // Inicialmente bloqueado

    // Cria as threads
    HANDLE hThread1 = CreateThread(NULL, 0, thread1, NULL, 0, NULL);
    HANDLE hThread2 = CreateThread(NULL, 0, thread2, NULL, 0, NULL);
    HANDLE hThread3 = CreateThread(NULL, 0, thread3, NULL, 0, NULL);

    if (hThread1 == NULL || hThread2 == NULL || hThread3 == NULL) {
        fprintf(stderr, "Erro ao criar threads.\n");
        return 1;
    }

    DWORD start = GetTickCount();
    // Libera semáforos para iniciar a thread 1
    ReleaseSemaphore(semThread1, 1, NULL);
    ReleaseSemaphore(semThread2, 1, NULL);

    // Aguarda o término das threads
    WaitForSingleObject(hThread1, INFINITE);
    WaitForSingleObject(hThread2, INFINITE);
    WaitForSingleObject(hThread3, INFINITE);
    DWORD end = GetTickCount();


    // Fecha as handles das threads
    CloseHandle(hThread1);
    CloseHandle(hThread2);
    CloseHandle(hThread3);

    // Fecha os semáforos
    CloseHandle(semThread1);
    CloseHandle(semThread2);
    CloseHandle(semThread3);

    // Fecha o mutex
    CloseHandle(mutex);

    DWORD elapsed = end - start;
    printf("Tempo de execucao: %ld ms\n", elapsed);

    return 0;
}