#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#define N_SIZE 1280

struct Pairs{
	long i,j;
	double sum;
	int matrixA [N_SIZE][N_SIZE];
	int matrixB [N_SIZE][N_SIZE];
	int matrixResult[N_SIZE][N_SIZE];
};
double sum;
void *runner(void *param);

void print_matrix(int rowSize, int column_size, int matrix[][N_SIZE]){
	for(int j=0; j<N_SIZE; j++){
		for(int k=0;k<N_SIZE; k++){
			printf("%d",matrix[j][k]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	//Struct to calculate the time taken by the program to run
	struct timeval start, end;
	gettimeofday(&start, NULL);

	pthread_attr_t attr; /* set of thread attributes*/
	int n_threads=0;
	//unsigned long terms =4000000000;
	//printf("Number  of terms: %lu\n", terms);
        if(argc != 2){
                fprintf(stderr, "usage: a.out <integer value>\n");
                return -1;
        }

        if(atoi(argv[1]) < 0){
                fprintf(stderr, "%d must be >=0\n", atoi(argv[1]));
                return -1;
        }

	//Setup the number of threads
	n_threads= atoi(argv[1]);
	//int matrixA [N_SIZE][N_SIZE];
	//int matrixB [N_SIZE][N_SIZE];
	int matrixResult[N_SIZE][N_SIZE];
	//Initialize global matrix
	for(int j=0; j<N_SIZE; j++){
		for(int k=0;k<N_SIZE; k++){
			//matrixA[j][k]=1;
			//matrixB[j][k]=1;
			matrixResult[j][k]=0;
		}
	}
	pthread_t tid[n_threads]; /*Thread identifier*/
	printf("Number of threads: %d\n", n_threads);
        struct  Pairs  *pair[n_threads];


	unsigned long amount = N_SIZE / n_threads;
	printf("Amount per thread: %lu\n", amount);
	for(int i=0;i<n_threads;i++){
		pair[i] =  malloc(sizeof(struct Pairs));
		pair[i]->i = i*amount;
        	pair[i]->j = ((i+1)*amount)-1;
		pair[i]->sum = 0.0;
		//initialize the matrixA and B and the result

		for(int j=0; j<N_SIZE; j++){
			for(int k=0;k<N_SIZE; k++){
				pair[i]->matrixA[j][k]=1;
				pair[i]->matrixB[j][k]=1;
				pair[i]->matrixResult[j][k]=0;
			}
		}
	}

  pthread_attr_init(&attr);
	for(int i=0; i<n_threads;i++){
		pthread_create(&tid[i], NULL, runner, (void*) pair[i]);
	}
  /*wait for the thread to exit*/
	for(int i=0;i<n_threads;i++){
	pthread_join(tid[i], NULL);
	}

	//matrix result join of the several matrices
	for(int k =0; k<n_threads; k++){
		for(int i= pair[k]->i;i<=pair[k]->j; i++){
			for(int j=0; j<N_SIZE; j++){
				matrixResult[i][j]= pair[k]->matrixResult[i][j];
			}
		}
	}

	//print_matrix(N_SIZE,N_SIZE,matrixResult );

  //Calculates the time taken by the program to run
	gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

	printf("Time elpased is %d seconds and %d micros\n", seconds, micros);
}




void *runner(void *param)
{
  struct Pairs *myPair = (struct Pairs*) param;
  //printf("myPair->i: %li\n", myPair->i);
  //printf("myPair.j: %li\n", myPair->j);
	//print_matrix(N_SIZE,N_SIZE,myPair->matrixA);
	//Outer cycle to get the results
	for(int i=myPair->i;i<=myPair->j;i++){
		int suma=0;
		for(int j=0;j<N_SIZE;j++){
			myPair->matrixResult[i][j]=0;
			for(int k=0; k<N_SIZE; k++){
				myPair->matrixResult[i][j]+=myPair->matrixA[i][k] * myPair->matrixB[k][j];
			}
		}
	}


	pthread_exit(0);
}
