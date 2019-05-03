#include <stdio.h>

int cubeByValue(int n){
  return n*n*n;
}

void cubeByReference(int *nPtr){
  *nPtr = *nPtr * *nPtr * *nPtr;
}

int main(){
  int number= 5;

  printf("Number: %d\n", number);

  number = cubeByValue(number);
  printf("Cube: %d\n", number);
  int num =4;
  cubeByReference(&num);
  printf("Cube of before: %d\n", num );
}
