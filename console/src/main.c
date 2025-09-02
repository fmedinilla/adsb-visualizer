#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib") // Enlaza automáticamente la biblioteca Winsock

int main() {
    WSADATA wsa;
    SOCKET servidor, cliente;
    struct sockaddr_in server, client;
    int c;
    char buffer;

    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Error en WSAStartup: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock inicializado.\n");

    // 1. Crear socket
    if ((servidor = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error creando socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 2. Configurar estructura del servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // 3. Asociar socket a puerto
    if (bind(servidor, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Error en bind: %d\n", WSAGetLastError());
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    // 4. Escuchar conexiones
    listen(servidor, 3);
    printf("Servidor escuchando en puerto 8080...\n");

    // 5. Aceptar conexión
    c = sizeof(struct sockaddr_in);
    cliente = accept(servidor, (struct sockaddr*)&client, &c);
    if (cliente == INVALID_SOCKET) {
        printf("Error en accept: %d\n", WSAGetLastError());
        closesocket(servidor);
        WSACleanup();
        return 1;
    }
    printf("Conexión aceptada.\n");

    // 6. Recibir y responder
    int leido = recv(cliente, buffer, sizeof(buffer), 0);
    printf("Se recibieron %d bytes.\n", leido);

    // 7. Cerrar sockets y limpiar Winsock
    closesocket(cliente);
    closesocket(servidor);
    WSACleanup();
    return 0;
}
