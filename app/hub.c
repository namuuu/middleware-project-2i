#include <stdio.h>

#include "../lib/data.h"
#include "hub.h"

#define CHECK(status, msg)                                                   \
    if (-1 == (status)) {                                                    \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }

// DECLARIATION DE FONCTIONS HUB
void serveur(char *ip, short port);
void waitForInput(socket_t sock, generic msg);
char *generateLobbyCode();
void genererCode(FILE* fichier, char code[6]);
void suppressionCode(const char *code);
void serveurLobby();
void deserial(generic quoi, char *msg);


typedef struct {
    char *ip;
    short port;
    char *code;
    int tidLobby;
} lobbyData_t;
lobbyData_t* tabLobby;

int main(int argc, char *argv[]){
    system("clear");
   
    if(argc != 3) { 
        printf(RED);
        printf("Usage: %s <ip> <port>\n", argv[0]);
        printf(RESET);
        return -1;
    }
    serveur(argv[1], atoi(argv[2]));
   
    return 0; 

    //creation d'un lobby
    // if (1) {
    //     CHECK(pthread_create(&tidLobby, NULL, serveurLobby, 0),"pthread_create(lobby)");
    //     shutdown(sock,2);
        
    // }


    int fd; // file descriptor
    fd = shm_open("tabLobby", O_CREAT | O_RDWR, S_IRWXU);  
    // Set the size of the shared memory object
    int pageSize = sysconf(_SC_PAGE_SIZE);
    CHECK(ftruncate(fd, pageSize), "__ftruncate__");
    // Map the tabEtats object into the virtual address space of the calling process
    tabLobby = mmap(0, pageSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   

}

/**
 * @fn void serveur(char *ip, short port);
 * 
 * @brief Lance un serveur HUB
 * @param ip Fournit l'ip du serveur HUB
 * @param port Fournit le port du serveur HUB
*/
void serveur(char *ip, short port) {
    printc(BOLDYELLOW, "Lancement du HUB\n");
    printc(YELLOW, "| IP: ");
    printf("%s\n", ip);
    printc(YELLOW, "| Port: ");
    printf("%d\n", port);

    received_t msg;

    socket_t sock = prepareForClient(ip, port, SOCK_STREAM);
    waitForInput(sock, &msg);

    close(sock.fd);
}

/**
 * @fn void serveurLobby();
 * 
 * @brief Lance un lobby
 * @param ip Fournit l'ip du lubby
 * @param port Fournit le port du lobby
*/
void serveurLobby() {
    printc(BOLDGREEN, "Lancement du Lobby\n");
    char* ip = "0.0.0.0";
    printc(GREEN, "| IP: ");
    printf("%s\n", ip);
    short port = 0;
    printc(GREEN, "| Port: ");
    printf("%d\n", port);
   
    // char *msg = NULL;

    // socket_t sock = prepareForClient(ip, port, SOCK_STREAM);

    // waitForInput(sock, msg);
    // pthread_exit(NULL);
}

void waitForInput(socket_t sock, generic msg){
    recevoir(sock, msg, NULL);
}

/**
 * \fn char *generateLobbyCode();
 * 
 * @brief Génération d'un code de session
*/
char *generateLobbyCode() {
    FILE* fichier;
    char *code = malloc(6 * sizeof(char));
    char codeLu[6];

    srand(time(NULL));
    fichier = fopen("code.txt", "a+");

    if(fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        exit(1);
    }

    genererCode(fichier, code);

    while(fgets(codeLu, 6, fichier) != NULL) {
        if(strcmp(codeLu, code) == 0) {
            genererCode(fichier, code);
        }
    }

    printf("Le code de votre session est : %s\n", code);
    fprintf(fichier, "%s\n", code);

    fclose(fichier);

    return code;
}

/**
 * \fn void genererCode(FILE* fichier, char code[6]);
 * 
 * @brief Génération d'un code aléatoire
 * @param fichier Nom du fichier à modifier
 * @param code Code à ajouter au fichier
*/
void genererCode(FILE *fichier, char code[6]) {
    int i;
    for (i = 0; i < 5; i++) {
        int random = rand() % 36;
        if(random < 10) {
            code[i] = random + 48;
        } else {
            code[i] = random + 55;
        }
    }
    code[5] = '\0'; 
}

/**
 * \fn void suppressionCode(FILE* fichier, const char *code);
 * 
 * @brief Suppression d'un code dans le fichier
 * @param fichier Nom du fichier à modifier
 * @param code Code à supprimer
 */
void suppressionCode(const char *code) {
    FILE* fichier;
    FILE* fichierTemp;
    char buffer[7];  // Un espace supplémentaire pour le caractère de fin de ligne

    fichier = fopen("code.txt", "r");
    fichierTemp = fopen("temp.txt", "w");

    if(fichier == NULL || fichierTemp == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        exit(1);
    }

    while(fgets(buffer, 7, fichier) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';  // Enlever le saut de ligne
        if(strcmp(buffer, code) != 0) {
            fprintf(fichierTemp, "%s\n", buffer);
        }
    }

    fclose(fichier);
    fclose(fichierTemp);

    remove("code.txt");
    rename("temp.txt", "code.txt");

    printf("En attente de connexion");
    recevoir(sock, msg, deserial);
}

void deserial(generic quoi, char *msg){
    // Séparer les données selon le séparateur "-" et les ranger dans une array de strings
    char *token = strtok(msg, "-");
    ((received_t*)quoi)->code = atoi(token);
    ((received_t*)quoi)->nbArgs = 0;
    token = strtok(NULL, "-");
    int i = 0;
    int switchToken = atoi(token);
    switch (switchToken)
    {
    default:
        while(token != NULL){
            ((received_t*)quoi)->args[i] = token;
            ((received_t*)quoi)->nbArgs++;
            token = strtok(NULL, "-");
            i++;
        }
        break;
    }
}