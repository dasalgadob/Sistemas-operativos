#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

typedef struct hashRecord{
    char nombre[32];
    int posicion;
}hashRecord;

/*Prototypes*/
int verHashTable(struct dogType *randomPerro, FILE *hashPtr);
void busquedaPorNombre(char nombrePerro[], FILE *cfPtr, FILE *hashPtr);
void recorrerListaEnlazada(int numeroHash, struct dogType *ultimo, FILE *cfPtr);
int buscarRegistroPorNumero(int numero, struct dogType *encontrado, FILE *cfPtr);
void actualizarRegistro(struct dogType *perro, FILE *cfPtr, int posicion);
void actualizarLinkedList(int numeroHash, struct dogType *ultimo, FILE *cfPtr, int nuevoNext, int regEliminar);
void actualizarHashTable(struct dogType *perro,FILE *hashPtr,int posicion);
void copiarLosRegistros(FILE *cfPtr,int num, int excepcion);
void actualizarHashTableBorrar(struct dogType *perro, FILE *hashPtr, int pos);

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

void busquedaPorNombre(char nombrePerro[], FILE *cfPtr, FILE *hashPtr){
    //hacer lower to nombrePerro
    //printf("Busqueda por nombre");
    struct dogType perro={ "", 0, "", 0, 0.0, 0, 0, 0 };
    strcpy( perro.nombre, nombrePerro );
    int hash ;
    hash = verHashTable(&perro, hashPtr );
    //si existe la cadena
    if(hash != 0) recorrerListaEnlazada(hash,&perro,cfPtr);
    //printf("\nhash: %i\n", hash);
}

void recorrerListaEnlazada(int numeroHash, struct dogType *ultimo, FILE *cfPtr){
    //printf("Before buscar por numero %i\n", numeroHash);
    int encontroRegistro=1;
    int numero=numeroHash;
    do{
        encontroRegistro = buscarRegistroPorNumero(numero, ultimo, cfPtr);
        numero = ultimo->next;
        printRecord(ultimo);
    }
    while(encontroRegistro!=-1 && ultimo->next!=0);
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
        printf("No se encontro la cadena buscada en el archivo\n");
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


void borrarRegistro(FILE *cfPtr, int numero, FILE *hashPtr){
    int buscar, posicion, proximo, posicionHash;
    int posicionActualizar;
    struct dogType perro={ "", 0, "", 0, 0.0, 0, 0, 0 };
    //mostrar numero de registros
    //contarRegistros(cfPtr);
    //encontrar registro por id
     
    buscar = buscarRegistroPorNumero(numero, &perro, cfPtr);
    if(buscar== -1){
        puts("No se encontro el registro a borrar\n");
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
    return;
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
        printf("\n El registro fue eliminado satisfactoriamente\n");
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

int main( void ){
    FILE *cfPtr; // dataDogs.dat file pointer
    FILE *hashPtr;
	
    if ( ( cfPtr = fopen( "dataDogs.dat", "rb+" ) ) == NULL ) {
        //function that add the newline to the stdout
        puts( "File to save structs could not be opened." );
        return 1;
    } 

    if ( ( hashPtr = fopen( "hashTable.dat", "rb+" ) ) == NULL ) {
        //function that add the newline to the stdout
        puts( "File to save hashTable could not be opened." );
        return 1;
    } 
	//copiarLosRegistros(cfPtr,1000001, 1000001);

    int salida = 1;
    int c, x, i, num;
    char nom[] = "", cadena[] = "";
    numRegistros = contarRegistros(cfPtr);
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
                fseek( cfPtr, (numRegistros - 1) * sizeof( struct dogType ), SEEK_SET );
                fread( &new1, sizeof( struct dogType ), 1, cfPtr );
                new.registro = new1.registro + 1;
                new.next = 0;
                //printRecord(&new);
                //printf("\nIngresando...");
                numRegistros = ingresarRegistro( &new, numRegistros, cfPtr, hashPtr );
                //printf("\nIngresado con éxito.");
                pause();
            break;
            case 2:
                do{
                    system("clear");
                    printf("En este momento hay ");
                    printf("%i", numRegistros);
                    printf(" registros en la tabla.\n\n");
                    printf("Ingrese el número de registro a observar o -1 para salir: ");
                    scanf("%i", &num);
                    if(num > 0){
                        int err = buscarRegistroPorNumero( num, &new,cfPtr);
                        if(err==-1){
                          puts("No se encontro el registro a buscar.");
                        }
                        else{
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
                do{
                    system("clear");
                    printf("En este momento hay ");
                    printf("%i", numRegistros);
                    printf(" registros en la tabla.\n\n");
                    printf("Ingrese el número de registro a borrar o -1 para salir: ");
                    scanf("%i", &num);
                    if(num > 0 ){
						          borrarRegistro(cfPtr, num, hashPtr);
                      numRegistros = contarRegistros(cfPtr);                 
						//copiarLosRegistros(cfPtr, 1000000, 1000000);
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
                busquedaPorNombre (nom, cfPtr, hashPtr);
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
        }
    }
}
