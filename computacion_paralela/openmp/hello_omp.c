#include "omp.h"
#include <stdio.h>
void main(){
  omp_set_num_threads(8);
  #pragma omp parallel
  {
    int ID = omp_get_thread_num();
    printf("Hello (%d)", ID);
    printf("World (%d) \n" , ID);
  }//End parallel region
}
