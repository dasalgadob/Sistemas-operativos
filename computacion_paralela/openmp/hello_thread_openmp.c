#include <omp.h>
#include <stdio.h>
#define NUM_THREADS 4
int main() {
    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
}
