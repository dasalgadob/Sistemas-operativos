#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>

#define PORT 3535

typedef struct dogType{
    char nombre[32];
    int edad;
    char raza[16];
    int estatura;
    float peso;
    char sexo;
    int registro;
    int next;
}dogType;

void printRecord(struct dogType *record){
    printf("\nName: %s ", record->nombre);
    printf("Edad: %i ", record->edad);
    printf("Raza: %s ", record->raza);
    printf("Estatura: %i ", record->estatura);
    printf("Peso: %.01f ", record->peso);
    printf("Sexo: %c ", record->sexo);
    printf("Registro: %i ", record->registro);
    printf("Next: %i \n", record->next);
    //printf("After print");
}

void pause(void){
    printf("\nPresione \'Enter\' para continuar...");
    while(getchar() != '\n');
    while(getchar() != '\n');
    return;
}

int main( void ){
     ////////################### SOCKET #######################///////////////
    int sockfd, n, rec;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    ssize_t sendFlag;
    char buffer[256];
    
    sockfd= socket(AF_INET, SOCK_STREAM,0);
    if(sockfd<0){
      puts("Error en funcion socket");
      return -1;
    }
    char *remoto = "127.0.0.1";
    server = gethostbyname(remoto);
    if(server == NULL){
      puts("error al buscar host");
      return -1;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(PORT);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
      puts("ERROR connecting");
      return -1;
    }
  
    int salida = 1;
    int c, x, i, num, numRegistros;
    char nom[] = "", cadena[] = "";
    //numRegistros = contarRegistros(cfPtr);
    //hashType * head = NULL;
    //head = malloc(sizeof(hashType));
    //hash( cfPtr, &numRegistros, head );
    //print(head);
    struct dogType new = { "", 0, "", 0, 0.0, 0, 0, 0 };
    struct dogType new1 = { "", 0, "", 0, 0.0, 0, 0, 0 };
    while(salida == 1){
        system("clear");
        printf("Ingrese la opción que desea seleccionar mediante el número:\n\n");
        printf("\t1. Ingresar Registro\n");
        printf("\t2. Ver Registro\n");
        printf("\t3. Borrar Registro\n");
        printf("\t4. Buscar Registro\n");
        printf("\t5. Salir\n\n");
        scanf("%i", &x);
        sendFlag = send(sockfd, &x, sizeof(x) ,0);
        if(sendFlag<0)puts("error enviando struct");
        switch(x){
            case 1:
                system("clear");
                printf("Ingrese el nombre: ");
                scanf(" %s", cadena);
                for(i = 0; i < strlen(cadena); i++){
                    if(cadena[i] > 0 && cadena[i] != 32) cadena[i] = tolower(cadena[i]);
                }
                strcpy( new.nombre, cadena );
                printf("\nIngrese la edad en años: ");
                scanf(" %i", &new.edad);
                printf("\nIngrese la raza: ");
                scanf(" %s", new.raza);
                printf("\nIngrese la estatura en cm: ");
                scanf(" %i", &new.estatura);
                printf("\nIngrese el peso en kg: ");
                scanf(" %f", &new.peso);
                printf("\nIngrese el sexo (m/f): ");
                scanf(" %c", &new.sexo);
                sendFlag = send(sockfd,(dogType *) &new, sizeof(new) ,0);
                if(sendFlag<0)puts("error enviando struct");
                //fseek( cfPtr, (numRegistros - 1) * sizeof( struct dogType ), SEEK_SET );
                //fread( &new1, sizeof( struct dogType ), 1, cfPtr );
                //new.registro = new1.registro + 1;
                //new.next = 0;
                //printRecord(&new);
                //printf("\nIngresando...");
                //numRegistros = ingresarRegistro( &new, numRegistros, cfPtr, hashPtr );
                //printf("\nIngresado con Ã©xito.");
                pause();
            break;
            case 2:
                rec = recv(sockfd,&numRegistros,sizeof(numRegistros),0);
                if(rec== -1) puts("ocurrio un error recibiendo");
                //sendFlag = send(sockfd,(dogType *) &new, sizeof(new) ,0);
                //if(sendFlag<0)puts("error enviando struct");
                do{
                    system("clear");
                    printf("En este momento hay ");
                    printf("%i", numRegistros);
                    printf(" registros en la tabla.\n\n");
                    printf("Ingrese el número de registro a observar o -1 para salir: ");
                    scanf("%i", &num);
                    sendFlag = send(sockfd, &num, sizeof(num) ,0);
                    if(sendFlag<0)puts("error enviando struct");
                    if(num > 0){
                        //int err = buscarRegistroPorNumero( num, &new,cfPtr);
                        int err;
                        rec = recv(sockfd,&err,sizeof(err),0);
                        if(rec== -1) puts("ocurrio un error recibiendo");
                        if(err==-1){
                            puts("\nNo se encontro el registro a buscar.");
                        }
                        else{
                            rec = recv(sockfd,(dogType *) &new, sizeof(new) ,0);
                            if(rec<0)puts("error enviando struct");
                            printRecord(&new);
                        }
                        pause();
                    }else{
                        if(num != -1){
                            system("clear");
                            printf("Por favor, ingrese un registro válido o -1 para salir.\n");
                            pause();
                        }
                    }
                }while(num != -1);
            break;
            case 3:
                rec = recv(sockfd,&numRegistros,sizeof(numRegistros),0);
                if(rec== -1) puts("ocurrio un error recibiendo");
                do{
                    system("clear");
                    printf("En este momento hay ");
                    printf("%i", numRegistros);
                    printf(" registros en la tabla.\n\n");
                    printf("Ingrese el número de registro a borrar o -1 para salir: ");
                    scanf("%i", &num);
                    sendFlag = send(sockfd, &num, sizeof(num) ,0);
                    if(sendFlag<0)puts("error enviando struct");
                    if(num > 0){
			            //borrarRegistro(cfPtr, num, hashPtr);
                        //numRegistros = contarRegistros(cfPtr);                 
						//copiarLosRegistros(cfPtr, 1000000, 1000000);
                        int errb;
                        rec = recv(sockfd,&errb,sizeof(errb),0);
                        if(rec== -1) puts("ocurrio un error recibiendo");
                        rec = recv(sockfd,&numRegistros,sizeof(numRegistros),0);
                        if(rec== -1) puts("ocurrio un error recibiendo");
                        if (errb == 0) {
                            printf("\n El registro fue eliminado satisfactoriamente.\n");
                        }else{
                            puts("\n No se encontro el registro a borrar.\n");
                        }
                        pause();
                    }else{
                        if(num != -1){
                            system("clear");
                            printf("Por favor, ingrese un registro válido o -1 para salir.\n");
                            pause();
                        }
                    }
                }while(num != -1);
            break;
            case 4:
                system("clear");
                printf("Ingrese el nombre de los registros que desea ver: ");
                scanf("%s", nom);
                sendFlag = send(sockfd, nom, 100, 0);
                if(sendFlag<0)puts("error enviando struct");
                //busquedaPorNombre (nom, cfPtr, hashPtr);
                x = 0;
                while(x == 0){
                    rec = recv(sockfd,&x,sizeof(x),0);
                    if(rec== -1) puts("ocurrio un error recibiendo");
                    if (x == 0){
                        rec = recv(sockfd,(dogType *) &new, sizeof(new) ,0);
                        if(rec<0)puts("error enviando struct");
                        printRecord(&new);
                    }
                }
                if (x == 2)puts("\nEl nombre no está registrado.");
                pause();
            break;
            case 5:
                system("clear");
                printf("El programa a finalizado con éxito.\n");
                pause();
                salida = 0;
            break;
            default:
                system("clear");
                printf("Por Favor, ingrese un número válido.\n");
                pause();
            break;
        }//end switch
    }//end while
}//end main