#include <stdio.h>
#include <windows.h>

#define PIPE_NAME "\\\\.\\pipe\\meu_pipe2"

int main() {
    HANDLE pipe;
    DWORD bytesWritten;
    int itens=0;
    float peso=0;
    char buffer[1024];

    // Cria o pipe para escrita
    pipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, 5000, NULL);
    if (pipe == INVALID_HANDLE_VALUE) {
        printf("Erro ao criar o pipe. Código de erro: %d\n", GetLastError());
        return 1;
    }

    printf("Named pipe criado com sucesso.\n");

    // Aguarda uma conexão de cliente
    if (!ConnectNamedPipe(pipe, NULL)) {
        printf("Erro ao aguardar a conexão do cliente. Código de erro: %d\n", GetLastError());
        CloseHandle(pipe);
        return 1;
    }else{
        printf("Cliente conectado!");
    }

    while(1){
        // Aguarda o sinal para iniciar
        itens++;
        peso+=2;

        // Escreve no pipe
        sprintf(buffer, "%d, %.2f\n", itens, peso);
        if (!WriteFile(pipe, buffer, strlen(buffer) + 1, &bytesWritten, NULL)) {
            printf("Erro ao escrever no pipe. Código de erro: %d\n", GetLastError());
            CloseHandle(pipe);
            return 1;
        }

        Sleep(1000);
    }

    // Fecha os handles
    CloseHandle(pipe);

    return 0;
}
