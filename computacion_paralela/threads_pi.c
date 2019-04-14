#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Pairs{
	long i,j;
	double sum;
}; 
double sum;
void *runner(void *param);

int main(int argc, char *argv[])
{
	clock_t start, end;
     	double cpu_time_used;
	start = clock();        

	pthread_attr_t attr; /* set of thread attributes*/
	int n_threads=0;
	unsigned long terms =4000000000;
	printf("Number  of terms: %lu\n", terms);
        if(argc != 2){
                fprintf(stderr, "usage: a.out <integer value>\n");
                return -1;
        }

        if(atoi(argv[1]) < 0){
                fprintf(stderr, "%d must be >=0\n", atoi(argv[1]));
                return -1;
        }
	n_threads= atoi(argv[1]);
	pthread_t tid[n_threads]; /*Thread identifier*/
	printf("Number of threads: %d\n", n_threads);
        struct  Pairs  *pair[n_threads];
	unsigned long amount = terms / n_threads;
	printf("Amount per thread: %lu\n", amount);
	for(int i=0;i<n_threads;i++){
		pair[i] =  malloc(sizeof(struct Pairs));
		pair[i]->i = i*amount;
        	pair[i]->j = ((i+1)*amount)-1;
		pair[i]->sum = 0.0;	
	}
        //s1.a = 1;
        //s1.b = 2;
        /*Get default attributes*/
	//pair[0] = malloc(sizeof(struct Pairs));
	//pair[1] = malloc(sizeof(struct Pairs));
	//pair[0]->i = 21;
	//pair[0]->j = 2;
	//pair[1]->i = 28;
        //pair[1]->j = 1;

        pthread_attr_init(&attr);
	for(int i=0; i<n_threads;i++){
		pthread_create(&tid[i], NULL, runner, (void*) pair[i]);
	}
        //pthread_create(&tid[0], NULL, runner, (void*) pair[0]);
	//pthread_create(&tid[1], NULL, runner, (void*) pair[1]);
        /*wait for the thread to exit*/
	for(int i=0;i<n_threads;i++){
	pthread_join(tid[i], NULL);
	}

	//Se realiza suma final
	for(int i=0; i<n_threads;i++){
		sum+= pair[i]->sum;
	}
        printf("suma total = %f\n", sum*4);

	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("program took %f seconds to execute \n", cpu_time_used); 
}



void *runner(void *param)
{
        struct Pairs *myPair = (struct Pairs*) param;
        //mySt = (struct myStruct*) *param;
        //printf("myPair->i: %li\n", myPair->i);
        //printf("myPair.j: %li\n", myPair->j);
        //printf("mypair->sum %f\n",myPair->sum);
	double tempSum=0.0;
	unsigned long start = myPair->i;
	if(myPair->i % 2 ==0){
	start = myPair->i+1;
}
	for(unsigned long i=start; i<=myPair->j;i+=2){
		//printf("i: %lu\n", i);
		if(i==1 || (i-1)%4 ==0 ){
			//printf("Suma: %f\n", 1/(double) i);
			tempSum+= 1/(double)i;
		}else{
			//printf("Resta: %f\n", 1/(double) i);
			tempSum-=1/(double)i;
		}		
	}
	myPair->sum = tempSum;
	pthread_exit(0);
}
