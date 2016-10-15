#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <pthread.h>
#include  <signal.h>
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

/*
typedef struct hashType{
    char nombre[32];
    int pos;
    struct hashType *next;
}hashType;
*/

typedef struct hashType{
    char nombre[32];
    int pos;
    struct hashType *next;
}hashType;

typedef struct hashRecord{
    char nombre[32];
    int posicion;
}hashRecord;

/*Prototypes*/
void     INThandler(int);
int verHashTable(struct dogType *randomPerro, FILE *hashPtr);
void busquedaPorNombre(char nombrePerro[], FILE *cfPtr, FILE *hashPtr, int newsockfd);
void recorrerListaEnlazada(int numeroHash, struct dogType *ultimo, FILE *cfPtr, int newsockfd);
int buscarRegistroPorNumero(int numero, struct dogType *encontrado, FILE *cfPtr);
void actualizarRegistro(struct dogType *perro, FILE *cfPtr, int posicion);
void actualizarLinkedList(int numeroHash, struct dogType *ultimo, FILE *cfPtr, int nuevoNext, int regEliminar);
void actualizarHashTable(struct dogType *perro,FILE *hashPtr,int posicion);
void copiarLosRegistros(FILE *cfPtr,int num, int excepcion);
void actualizarHashTableBorrar(struct dogType *perro, FILE *hashPtr, int pos);
//the thread function
void *connection_handler(void *);
void fileopen( FILE *log);
void fileclose( FILE *log);

int numRegistros;

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

void busquedaPorNombre(char nombrePerro[], FILE *cfPtr, FILE *hashPtr, int newsockfd){
    //hacer lower to nombrePerro
    //printf("Busqueda por nombre");
    struct dogType perro={ "", 0, "", 0, 0.0, 0, 0, 0 };
    strcpy( perro.nombre, nombrePerro );
    int hash;
    hash = verHashTable(&perro, hashPtr );
    //si existe la cadena
    if(hash != 0){
        recorrerListaEnlazada(hash,&perro,cfPtr,newsockfd);
    }else {
        int x = 2;
        int sendFlag = send(newsockfd, &x, sizeof(x) ,0);
        if(sendFlag<0)puts("error enviando x");
    }
    //printf("\nhash: %i\n", hash);
    //printf("fin busqueda\n");
}

void recorrerListaEnlazada(int numeroHash, struct dogType *ultimo, FILE *cfPtr, int newsockfd){
    //printf("Before buscar por numero %i\n", numeroHash);
    int encontroRegistro=1;
    int numero=numeroHash;
    int x = 0, sendFlag;
    do{
        encontroRegistro = buscarRegistroPorNumero(numero, ultimo, cfPtr);
        numero = ultimo->next;
        //printRecord(ultimo);
        sendFlag = send(newsockfd, &x, sizeof(x) ,0);
        if(sendFlag<0)puts("error enviando x");
        sendFlag = send(newsockfd,(dogType *) ultimo, sizeof(*ultimo) ,0);
        if(sendFlag == -1)puts("ocurrio un error enviando.");
    }
    while(encontroRegistro!=-1 && ultimo->next!=0);
    x = 1;
    sendFlag = send(newsockfd, &x, sizeof(x) ,0);
    if(sendFlag<0)puts("error enviando x");
    //printf("Encontro registro:%i\n", encontroRegistro);
    //printf("%s %i\n",ultimo->nombre, ultimo->registro);
}


void recorrerListaEnlazadaIngresar(int numeroHash, struct dogType *ultimo, FILE *cfPtr, int pos){
    int in = numeroHash;
    int encontroRegistro=1;
    int numero=numeroHash;
    do{
        encontroRegistro = buscarRegistroPorNumero(numero, ultimo, cfPtr);
        numero = ultimo->next;
        if(numero != 0) in = numero;
    }
    while(encontroRegistro!=-1 && ultimo->next!=0);
    ultimo->next = pos;
    fseek( cfPtr, (in - 1) * sizeof( struct dogType ), SEEK_SET );
    fwrite( ultimo, sizeof( struct dogType ), 1, cfPtr );
}
/* retorna la posicion a la que apunta la hash para empezar busqueda del registro */
int verHashTable(struct dogType *randomPerro, FILE *hashPtr){
    //Verificar si existe en hashtable
    struct hashRecord hash ={"",0};
    int registroHash=0;
    int existHash=0;
    int r, w;
    //printf("Leer");
    //Obtener valores almacenados en el hash para ver si se tiene que crear la posicion
    r = fseek(hashPtr,0*sizeof( struct hashRecord ), SEEK_SET);//vuelve el archivo al comienzo para hacer comparaciones
    if(r==-1){
                puts("Error al hacer seek para actualizarHashTable");
            }
    while(fread(&hash, sizeof(struct hashRecord), 1, hashPtr )!=0 && randomPerro->registro!= 1){
        //printf("read hash\n");
        //printf("%s   %s\n",randomPerro->nombre, hash.nombre);
        if(strcmp(randomPerro->nombre,hash.nombre)==0 ){
            //printf("Comparacion");
            registroHash = hash.posicion;
            existHash=1;
        }
    }//End while
    
    if(existHash == 0){
        //printf("No se encontro la cadena buscada en el archivo\n");
        return 0;
    }
    return registroHash;
}

/*retorna 0 si no lo encuentra  
*/
int buscarRegistroPorNumero(int numero, struct dogType *encontrado, FILE *cfPtr){
    int r=0;
    int w=0;
    //printf("Numero %i\n", numero);
    int busqueda = numero-1;
    //int totalRegistros=contarRegistros(cfPtr);
    //printf("Busuqeda:%i\n", busqueda);
    if(numero>numRegistros ){
      busqueda = numRegistros-1;
    }
    r = fseek( cfPtr, busqueda*sizeof( struct dogType ), SEEK_SET );
    if(r==-1){
                puts("Error al hacer seek en buscarRegistroPorNumero");
            }
    //printf("Before read");
    w = fread( encontrado, sizeof( struct dogType ), 1, cfPtr );
    if(w!= 1){
                puts("Error al hacer lectura en buscar");
            }
    //printf("Before if");
    //printf("Encontrado registro:%i  Numero:%i\n",encontrado->registro,numero);
    
    if(encontrado->registro == numero){
        //printf("Encontrado");
        return busqueda;
    }else if(encontrado->registro > numero){/* en caso de que el encontrado sea mayor se busca hacia atras hasta que lo encuentre o se haga menor al buscado*/
        //printf("Else if\n");
        
        while(encontrado->registro > numero && busqueda>0){
            busqueda--;
            //printf("Busqueda: %i", busqueda);
            r = fseek( cfPtr, busqueda*sizeof( struct dogType ), SEEK_SET );
            if(r==-1){
                puts("Error al hacer seek en buscarRegistroPorNumero cuando registro es mayor que numero");
                return -1;
            }
            w = fread( encontrado, sizeof( struct dogType ), 1, cfPtr );
            if(w!= 1){
                puts("Error al hacer lectura en buscar");
            }
            //puts("busqueda:");
            //printRecord(encontrado);
            if(encontrado->registro == numero){
                return busqueda;
            }//end if
            
        }//end while
    }else{
        //printf("else");
        
        while(encontrado->registro < numero &&busqueda<numRegistros){
            busqueda++;
            fseek( cfPtr, busqueda*sizeof( struct dogType ), SEEK_SET );
            fread( encontrado, sizeof( struct dogType ), 1, cfPtr );
            if(encontrado->registro == numero){
                return busqueda;
            }
        }//end while
        
    }//end else
    //printf("end");
    return -1;
}//end buscar

int buscarRegistroPorNumeroVer(int numero, struct dogType *encontrado, FILE *cfPtr){
    int r=0;
    int w=0;
    //printf("Numero %i\n", numero);
    int busqueda = numero-1;
    //printf("Busuqeda:%i\n", busqueda);
    r = fseek( cfPtr, busqueda*sizeof( struct dogType ), SEEK_SET );
    if(r==-1){
                puts("Error al hacer seek en buscarRegistroPorNumero");
            }
    //printf("Before read");
    w = fread( encontrado, sizeof( struct dogType ), 1, cfPtr );
    if(w!= 1){
                puts("Error al hacer lectura en buscar");
            }
    //printf("Before if");
    //printf("Encontrado registro:%i  Numero:%i\n",encontrado->registro,numero);
    if(encontrado->registro == numero){
        //printf("Encontrado");
        return busqueda;
    }else if(encontrado->registro > numero){
		/* en caso de que el encontrado sea mayor se busca hacia atras hasta que lo encuentre o se haga 		menor al buscado*/
        printf("Else if\n");
        
        while(encontrado->registro > numero){
            busqueda--;
            printf("Busqueda: %i", busqueda);
            r = fseek( cfPtr, busqueda*sizeof( struct dogType ), SEEK_SET );
            if(r==-1){
                puts("Error al hacer seek en buscarRegistroPorNumero cuando registro es mayor que numero");
                return -1;
            }
            w = fread( encontrado, sizeof( struct dogType ), 1, cfPtr );
            if(w!= 1){
                puts("Error al hacer lectura en buscar");
            }
            if(encontrado->registro == numero){
                return busqueda;
            }//end if
            
        }//end while
    }else{
        printf("else");
        
        while(encontrado->registro < numero){
            busqueda++;
            fseek( cfPtr, busqueda*sizeof( struct dogType ), SEEK_SET );
            fread( encontrado, sizeof( struct dogType ), 1, cfPtr );
            if(encontrado->registro == numero){
                return busqueda;
            }
        }//end while
        
    }//end else
    
    return -1;
}//end buscar

int contarRegistros(FILE *cfPtr){
    int r;
    r = fseek(cfPtr,0*sizeof( struct dogType ), SEEK_SET);
    if(r==-1){
                puts("Error al hacer seek para actualizarHashTable");
            }
    struct dogType perro={ "", 0, "", 0, 0.0, 0, 0, 0 };
    int count =0;
    while(fread(&perro, sizeof(struct dogType), 1, cfPtr )!=0){
        count++;
        //printf("Nombre:%s Reg:%i\n",hash2.nombre, hash2.posicion);
    //w = ;
        /*if(w!= 1){
                printf("W:%i\n",w);
                puts("Error al hacer lectura en actualizarHashTable");
            }*/
    }
    return count;
}


int borrarRegistro(FILE *cfPtr, int numero, FILE *hashPtr){
    int buscar, posicion, proximo, posicionHash;
    int posicionActualizar;
    struct dogType perro={ "", 0, "", 0, 0.0, 0, 0, 0 };
    //mostrar numero de registros
    //contarRegistros(cfPtr);
    //encontrar registro por id
     
    buscar = buscarRegistroPorNumero(numero, &perro, cfPtr);
    if(buscar== -1){
        //puts("No se encontro el registro a borrar\n");
        return 1;
    }else{
    //cambiar lista enlazada considerar si es el de la tabla hash
        //Si es de la tabla hash toca actualizarla
        posicion = perteneceHashTableBorrar(&perro, hashPtr);
		copiarLosRegistros(cfPtr,contarRegistros(cfPtr),numero);
        numRegistros--;
		//printf("%s %i", perro.nombre, posicion);
        if(posicion == -1){
			//Si  toca actualizar linked list
            proximo = perro.next;
            posicionHash=verHashTable(&perro, hashPtr);
            actualizarLinkedList(posicionHash, &perro,cfPtr,proximo, perro.registro);
            //printf("Actualizar linked list");
        }else{
			actualizarHashTableBorrar(&perro, hashPtr, posicion);
            //printf("pertenece hash");        
        }
    //sobreescribir archivo
        
		//eliminarAnterior
		//renombrarAnterior
		//reabrirArchivo
    }//End else
    return 0;
}//ENd borrarRegistro
/*REcibe numeroHash que es la posicion en la hashTable
 nuevoNext que es el valor a ser reemplazado
 regEliminar que es el numero de registro que se quiere desenlazar*/
void actualizarLinkedList(int numeroHash, struct dogType *ultimo, FILE *cfPtr, int nuevoNext, int regEliminar){
    struct dogType perro={ "", 0, "", 0, 0.0, 0, 0, 0 };
    //printf("Before buscar por numero %i\n", numeroHash);
    int encontroRegistro=1;
    int numero=numeroHash;
    int posicionActualizar;
    do{
        encontroRegistro = buscarRegistroPorNumero(numero, ultimo, cfPtr);
        //printf("ciclo");
        numero = ultimo->next;
        //puts("DO:");
            //printRecord(ultimo);
        if(ultimo->next==regEliminar){
            
            //actualizar el archivo
            
            posicionActualizar = buscarRegistroPorNumero(ultimo->registro,&perro,cfPtr);
            perro.next = nuevoNext;
            //puts("Registro a actualizar");
            //printRecord(&perro);
            actualizarRegistro(&perro, cfPtr, posicionActualizar);
            break;
        }
    }
    while(encontroRegistro!=0 && ultimo->next!=0);
    //printf("Encontro registro:%i\n", encontroRegistro);
    //printf("%s %i\n",ultimo->nombre, ultimo->registro);
}

void actualizarRegistro(struct dogType *perro, FILE *cfPtr, int posicion){
    int r,w;
    r = fseek(cfPtr,posicion*sizeof( struct dogType ), SEEK_SET);
    if(r==-1){
        puts("Error al hacer seek para actualizarHashTable");
    }
    w = fwrite( perro, sizeof( struct dogType ), 1, cfPtr );
            if(w!= 1){
                puts("Error al hacer write previous");
            }
}

/*Retorna la posicion si el registro hace parte de la hashTable 0 si no */
int perteneceHashTable(struct dogType *perro, FILE *hashPtr){
    int pertenece = -1;
    int posicion = 0;
    //Verificar si existe en hashtable
    struct hashRecord hash = {"",0};
    int registroHash = 0;
    //int existHash=0;
    int r, w;
    
    //Obtener valores almacenados en el hash para ver si se tiene que crear la posicion
    r = fseek(hashPtr, 0 * sizeof( struct hashRecord ), SEEK_SET);//vuelve el archivo al comienzo para hacer comparaciones
    if(r==-1) puts("Error al hacer seek para perteneceHashTable");
    while(fread(&hash, sizeof(struct hashRecord), 1, hashPtr )!=0){
        //printf("read hash\n");
        //
        if( strcmp(perro->nombre, hash.nombre) == 0 ){
            //printf("Comparacion");
            return posicion;
        }
        posicion++;
    }//End while
    return pertenece;
}//End perteneceHashTable

int perteneceHashTableBorrar(struct dogType *perro, FILE *hashPtr){
    int pertenece = -1;
    int posicion = 0;
    //Verificar si existe en hashtable
    struct hashRecord hash = {"",0};
    int registroHash = 0;
    //int existHash=0;
    int r, w;
    
    //Obtener valores almacenados en el hash para ver si se tiene que crear la posicion
    r = fseek(hashPtr, 0 * sizeof( struct hashRecord ), SEEK_SET);//vuelve el archivo al comienzo para hacer comparaciones
    if(r==-1) puts("Error al hacer seek para perteneceHashTable");
    while(fread(&hash, sizeof(struct hashRecord), 1, hashPtr )!=0){
        //printf("read hash\n");
        //
        if( strcmp(perro->nombre, hash.nombre) == 0 && perro->registro== hash.posicion){
            //printf("Comparacion");
            return posicion;
        }
        posicion++;
    }//End while
    return pertenece;
}//End perteneceHashTable

void actualizarHashTable(struct dogType *perro, FILE *hashPtr, int pos){
    struct hashRecord hash = {"",0};
    strcpy( hash.nombre, perro->nombre );
    hash.posicion = pos;
    int r, w;
    //Obtener valores almacenados en el hash para ver si se tiene que crear la posicion
    r = fseek(hashPtr, 0, SEEK_END);//vuelve el archivo al comienzo para hacer comparaciones
    if(r==-1) puts("Error actualizarHashTable seek");
    w = fwrite(&hash, sizeof(struct hashRecord), 1, hashPtr );
    if(w==-1) puts("Error actualizarHashTable write");
}//End actualizarHashTable

void actualizarHashTableBorrar(struct dogType *perro, FILE *hashPtr, int pos){
    
    FILE *tempPtr;

  
    struct hashRecord hash = {"",0};
	struct hashRecord hash1 = {"",0};
	int contador=0;
    strcpy( hash.nombre, perro->nombre );
    hash.posicion = perro->next;
    int r, w;
    //Obtener valores almacenados en el hash para ver si se tiene que crear la posicion
	if(perro->next == 0){//cuando no hay mas structs por delante se elimina
		//sobreescribir el archivo
       if ( ( tempPtr = fopen( "tempHash.dat", "wb+" ) ) == NULL ) {
            //function that add the newline to the stdout
            puts( "File to save temp could not be opened." );
    		//return 1;
        } 
    	r = fseek( hashPtr, 0, SEEK_SET );
    	if(r==-1){
            	puts("Error al hacer seek para insertar nuevo registro");
            }
   
   
		while(fread(&hash1, sizeof( struct hashRecord ), 1, hashPtr)){
		
		if(pos != contador){
			w= fwrite(&hash1, sizeof( struct hashRecord ), 1, tempPtr);
			if(w!=1){
				puts("Error al hacer escritura en nuevo archivo");
			}
		}//end inf
		contador++;
   }//end while
       fclose(tempPtr);
      fclose(hashPtr);
      if(remove("hashTable.dat")==0)
         {
            //printf("\n El registro hash fue eliminado satisfactoriamente\n");
         }
        else
         {
            printf("No se pudo eliminar el archivo hash\n");
         }
         
       int err = rename("tempHash.dat","hashTable.dat");
       if(err == -1){
         puts("Error al renombrar el archivo hash");
       }  
       if ( ( hashPtr = fopen( "hashTable.dat", "rb+" ) ) == NULL ) {
            //function that add the newline to the stdout
            puts( "File to save  hash could not be opened." );
    		//return 1;
        } 
		//printf("fin borrar");
	

	}else{//se actualiza el registro de la hash
		r = fseek(hashPtr, pos, SEEK_SET);
		if(r==-1) puts("Error actualizarHashTable seek");
		w = fwrite(&hash, sizeof(struct hashRecord), 1, hashPtr );
		if(w==-1) puts("Error actualizarHashTable write");
	}
    
}//End actualizarHashTable

void pause(void){
    printf("\nPresione \'Enter\' para continuar...");
    while(getchar() != '\n');
    while(getchar() != '\n');
    //return;
}

/*
void hash( FILE* cfPtr, int *numRegistros, struct hashType * head ){
    dogType client;
    hashType * current = NULL;
    int j = 0, k;
    fseek( cfPtr, j * sizeof( struct dogType ), SEEK_SET );
    fread( &client, sizeof( struct dogType ), 1, cfPtr );
    strcpy( head->nombre, client.nombre );
    head->pos = j;
    head->next = NULL;
    j++;
    while( fread( &client, sizeof( struct dogType ), 1, cfPtr ) == 1 ){
        //printf("\n%i", j);
        current = head;
        k = 0;
        while ( current->next != NULL ) {
            if( strcmp (current->nombre, client.nombre) == 0 ){
                k = 1;
                //printf("Break\n");
                break;
            }
            current = current->next;
        }
        if(k == 0){
            current->next = malloc(sizeof(hashType));
            strcpy( current->next->nombre, client.nombre );
            current->next->pos = j;
            current->next->next = NULL;
            //printf("%s", client.nombre);
        }
        j++;
    }
    *numRegistros = j;
}

void print(struct hashType * head){
    while ( head->next != NULL ) {
        printf("%s\n", head->nombre);
        printf("%i\n", head->pos);
        head = head->next;
    }
}
*/

/*
void hash( FILE* cfPtr, int *numRegistros, struct hashType * head ){
    dogType client;
    hashType * current = NULL;
    int j = 0, k;
    fseek( cfPtr, j * sizeof( struct dogType ), SEEK_SET );
    fread( &client, sizeof( struct dogType ), 1, cfPtr );
    strcpy( head->nombre, client.nombre );
    head->pos = j;
    head->next = NULL;
    j++;
    while( fread( &client, sizeof( struct dogType ), 1, cfPtr ) == 1 ){
        //printf("\n%i", j);
        current = head;
        k = 0;
        while ( current->next != NULL ) {
            if( strcmp (current->nombre, client.nombre) == 0 ){
                k = 1;
                //printf("Break\n");
                break;
            }
            current = current->next;
        }
        if(k == 0){
            current->next = malloc(sizeof(hashType));
            strcpy( current->next->nombre, client.nombre );
            current->next->pos = j;
            current->next->next = NULL;
            //printf("%s", client.nombre);
        }
        j++;
    }
    *numRegistros = j;
}//end hash
*/

int ingresarRegistro( struct dogType *new, int numRegistros, FILE *cfPtr, FILE *hashPtr ){
    int posicion, posicionHash;
    fseek( cfPtr, numRegistros * sizeof( struct dogType ), SEEK_SET );
    fwrite( new, sizeof( struct dogType ), 1, cfPtr );
    posicion = perteneceHashTable(new, hashPtr);
    //printf("%i\n", posicion);
    if(posicion == -1){
        actualizarHashTable( new, hashPtr, new->registro); 
        //printf("Pertenece hash");
    }else{
    //Si no toca actualizar linked list
        posicionHash = verHashTable ( new, hashPtr);
        recorrerListaEnlazadaIngresar (posicionHash, new, cfPtr, new->registro);
        //fseek( cfPtr, 999877 * sizeof( struct dogType ), SEEK_SET );
        //fwrite( &abel, sizeof( struct dogType ), 1, cfPtr );
        //printf("Actualizar linked list");
    }
    numRegistros++;
	fclose(cfPtr);
	if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
        //function that add the newline to the stdout
        puts( "File to save temp could not be opened." );
		//return 1;
    }
    return numRegistros;
}

void copiarLosRegistros(FILE *cfPtr,int num, int excepcion){
	int r,w,i,counter;
	FILE *tempPtr;
	struct dogType perro = { "", 0, "", 0, 0.0, 0, 0, 0 };

	if ( ( tempPtr = fopen( "temp2.dat", "wb+" ) ) == NULL ) {
        //function that add the newline to the stdout
        puts( "File to save temp could not be opened." );
		//return 1;
    } 
	r = fseek( cfPtr, 0, SEEK_SET );
	if(r==-1){
        	puts("Error al hacer seek para insertar nuevo registro");
        }
    for(counter=0; counter<num; counter++){
		w= fread(&perro, sizeof( struct dogType ), 1, cfPtr);
		if(excepcion != perro.registro){
			w= fwrite(&perro, sizeof( struct dogType ), 1, tempPtr);
			if(w!=1){
				puts("Error al hacer escritura en nuevo archivo");
			}
		}
		
	
	}
	fclose(tempPtr);
  fclose(cfPtr);
  if(remove("dataDogs.dat")==0)
     {
        //printf("\n El registro fue eliminado satisfactoriamente\n");
     }
    else
     {
        printf("No se pudo eliminar el archivo\n");
     }
     
   int err = rename("temp2.dat","dataDogs.dat");
   if(err == -1){
     puts("Error al renombrar el archivo");
   }  
   if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
        //function that add the newline to the stdout
        puts( "File to save temp could not be opened." );
		//return 1;
    } 
	//printf("COunter%i",counter);
}/*End copiar todos los registros*/
int socket_desc;
int main( void ){ 
    int r;
    ////////////SOCKET ////////////////////
    int  client_sock, c, *new_sock;
    struct sockaddr_in server, client;
    
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc== -1){
      puts("could not create socket");
      return -1;
    }   
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT); 
    
    if( bind(socket_desc, (struct sockaddr *) &server, sizeof(server))<0){
      puts("bind error");
      return -1;
    }
    
    r = listen(socket_desc, 32);
    if(r== -1){
      puts("error en listen");
    }
    
    c = sizeof(struct sockaddr_in);
     while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        //puts("Connection accepted");
        //printf("%i", client.sin_addr.s_addr);
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client.sin_addr), str, INET_ADDRSTRLEN);
        //printf("%s", str);
        //puts("After print");
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //puts("Handler assigned");
    }
    if (client_sock < 0)
    {
        puts("accept failed");
        return -1;
    }
     
    return 0;

}

void *connection_handler(void *socket_desc)
{

   
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(sock, (struct sockaddr *)&addr, &addr_size);
    
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), str, INET_ADDRSTRLEN);
    //char clientip[20];
    //strcpy(clientip, inet_ntoa(addr.sin_addr));
    
    //printf("%s hilo\n", str);
    int read_size;
    //puts("hilo cliente");   
    char buf[200];  
    
     ssize_t rec;
    int sendFlag;
    FILE *cfPtr; // dataDogs.dat file pointer
    FILE *hashPtr;
    FILE *log; 

    int salida = 1;
    int c, x, i, num;
    char nom[] = "", cadena[] = "";
    if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
        //function that add the newline to the stdout
        puts( "File to save structs could not be opened." );
        //return 1;
    }     
    numRegistros = contarRegistros(cfPtr);
    fclose(cfPtr);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    /*hashType *head = NULL;
    head = malloc(sizeof(hashType));
    hash( cfPtr, &numRegistros, head );*/
    //print(head);
    struct dogType new = { "", 0, "", 0, 0.0, 0, 0, 0 };
    struct dogType new1 = { "", 0, "", 0, 0.0, 0, 0, 0 };
    while(salida == 1){
        signal(SIGINT, INThandler);
        signal(SIGTSTP, INThandler);
        rec = recv(sock, &x, sizeof(x), 0);
        if(rec== -1)puts("ocurrio un error recibiendo");
        switch(x){
            case 1:
                rec = recv(sock,(dogType *) &new,sizeof(new),0);
                if(rec== -1) puts("ocurrio un error recibiendo");
                //open files
                if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
                    //function that add the newline to the stdout
                    puts( "File to save structs could not be opened." );
                    //return 1;
                } 
            
                if ( ( hashPtr = fopen( "hashTable.dat", "rb+" ) ) == NULL ) {
                    //function that add the newline to the stdout
                    puts( "File to save hashTable could not be opened." );
                    //return 1;
                }
                fseek( cfPtr, (numRegistros - 1) * sizeof( struct dogType ), SEEK_SET );
                fread( &new1, sizeof( struct dogType ), 1, cfPtr );
                new.registro = new1.registro + 1;
                new.next = 0;
                //printRecord(&new);
                //printf("\nIngresando...");
                numRegistros = ingresarRegistro( &new, numRegistros, cfPtr, hashPtr );
                //printf("\nIngresado con Éxito.");
                fclose(cfPtr);
                fclose(hashPtr);
                t = time(NULL);
                tm = *localtime(&t);
                if((log = fopen("serverDogs.log","a")) == NULL){
                  puts("FIle log could not be opened.");
                }
                fprintf(log, "%d-%d-%d %d:%d:%d Cliente %s inserción %d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, str, new.registro);
                fclose(log);
            break;
            case 2:
                sendFlag = send(sock, &numRegistros, sizeof(numRegistros) ,0);
                if(sendFlag<0)puts("error enviando struct");
                //rec = recv(newsockfd,(dogType *) &new, sizeof(new),0);
                //if(rec== -1) puts("ocurrio un error recibiendo");
                do{
                    /*
                    system("clear");
                    printf("En este momento hay ");
                    printf("%i", numRegistros);
                    printf(" registros en la tabla.\n\n");
                    printf("Ingrese el nÃºmero de registro a observar o -1 para salir: ");
                    scanf("%i", &num);
                    */
                    rec = recv(sock, &num, sizeof(num), 0);
                    if(rec== -1)puts("ocurrio un error recibiendo");
                    if(num > 0){
                        if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
                            //function that add the newline to the stdout
                            puts( "File to save structs could not be opened." );
                            //return 1;
                        }
                        int err = buscarRegistroPorNumero( num, &new, cfPtr);
                        fclose(cfPtr);
                        sendFlag = send(sock, &err, sizeof(err) ,0);
                        if(sendFlag<0)puts("error enviando struct");
                        if(err!=-1){
                            sendFlag = send(sock,(dogType *) &new, sizeof(new),0);
                            if(sendFlag== -1) puts("ocurrio un error recibiendo");
                        }
                    }
                    t = time(NULL);
                    tm = *localtime(&t);
                if(num!= -1){
                if((log = fopen("serverDogs.log","a")) == NULL){
                  puts("FIle log could not be opened.");
                }
                fprintf(log, "%d-%d-%d %d:%d:%d Cliente %s lectura %d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, str,num);
                fclose(log);
                }//end if
                }while(num != -1);
            break;
            case 3:
                sendFlag = send(sock, &numRegistros, sizeof(numRegistros) ,0);
                if(sendFlag<0)puts("error enviando struct");
                do{
                    /*
                    system("clear");
                    printf("En este momento hay ");
                    printf("%i", numRegistros);
                    printf(" registros en la tabla.\n\n");
                    printf("Ingrese el nÃºmero de registro a borrar o -1 para salir: ");
                    scanf("%i", &num);
                    */
                    rec = recv(sock, &num, sizeof(num), 0);
                    if(rec== -1)puts("ocurrio un error recibiendo");
                    if(num > 0 ){
                        if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
                            //function that add the newline to the stdout
                            puts( "File to save structs could not be opened." );
                            //return 1;
                        } 
                    
                        if ( ( hashPtr = fopen( "hashTable.dat", "rb+" ) ) == NULL ) {
                            //function that add the newline to the stdout
                            puts( "File to save hashTable could not be opened." );
                            //return 1;
                        }                        
                        int errb = borrarRegistro(cfPtr, num, hashPtr);
                        sendFlag = send(sock, &errb, sizeof(errb) ,0);
                        if(sendFlag<0)puts("error enviando struct");
                        numRegistros = contarRegistros(cfPtr);
                        fclose(cfPtr);
                        fclose(hashPtr);
                        sendFlag = send(sock, &numRegistros, sizeof(numRegistros) ,0);
                        if(sendFlag<0)puts("error enviando struct");            
                        //copiarLosRegistros(cfPtr, 1000000, 1000000);
                    t = time(NULL);
                    tm = *localtime(&t);
                    if(num!= -1){
                    if((log = fopen("serverDogs.log","a")) == NULL){
                      puts("FIle log could not be opened.");
                    }
                    fprintf(log, "%d-%d-%d %d:%d:%d Cliente %s borrado %d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, str,num);
                    fclose(log);
                    }//end if
                    }//end if
                }while(num != -1);
            break;
            case 4:
                /*
                system("clear");
                printf("Ingrese el nombre de los registros que desea ver: ");
                scanf("%s", nom);
                */
                
                rec = recv(sock, buf, 100, 0);
                //printf("%s\n", buf);
                if(rec== -1)puts("ocurrio un error recibiendo");
                if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
                    //function that add the newline to the stdout
                    puts( "File to save structs could not be opened." );
                    //return 1;
                } 
            
                if ( ( hashPtr = fopen( "hashTable.dat", "rb+" ) ) == NULL ) {
                    //function that add the newline to the stdout
                    puts( "File to save hashTable could not be opened." );
                    //return 1;
                }                
                busquedaPorNombre(buf, cfPtr, hashPtr, sock);
                fclose(cfPtr);
                fclose(hashPtr);
                t = time(NULL);
                tm = *localtime(&t);
                if((log = fopen("serverDogs.log","a")) == NULL){
                  puts("FIle log could not be opened.");
                }
                fprintf(log, "%d-%d-%d %d:%d:%d Cliente %s busqueda %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, str,buf);            
                fclose(log);
                //puts("fin 4");
            break;
            case 5:
                system("clear");
                //printf("El programa a finalizado con éxito.\n");
                salida = 0;
            break;
            default:
                system("clear");
                printf("Por Favor, ingrese un nÃºmero válido.\n");
            break;
        }//end switch
    }//end while
    //fclose(log);
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}//end_connection_handler


void  INThandler(int sig)
{
     char  c;
     close(socket_desc);
     signal(sig, SIG_IGN);
     exit(0);
     
}

void fileopen( FILE *log){
   if((log = fopen("serverDogs.log","a")) == NULL){
      puts("FIle log could not be opened.");
    }
}

void fileclose( FILE *log){
  fclose(log);
}
