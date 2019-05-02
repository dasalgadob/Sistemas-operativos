#include <stdio.h>
#include <omp.h>
#define NUM_THREADS 4
#include <sys/time.h>

int main(){
	struct timeval start, end;
        gettimeofday(&start, NULL);

	double pi =0.0;
	int i;
	omp_set_num_threads(NUM_THREADS);
	#pragma omp parallel for reduction(+:pi)
	for(i=1; i< 1000000000; i+=4){
	 pi +=  1.0/(double)i;
	pi -= 1.0/(double)(i+2);
	}//End for
	printf("Pi: %f\n", pi*4);

	gettimeofday(&end, NULL);
        long seconds = (end.tv_sec - start.tv_sec);
        long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);
        printf("Time elpased is %d seconds and %d micros\n", seconds, micros);
return 0;
}
