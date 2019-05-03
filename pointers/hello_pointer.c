#include <stdio.h>

int main(){
  int *countPtr= NULL, count;

  int y=5;
  int *yPtr = &y;//The & unary operator returns the address of it operand; also known as address operator
  printf("y *: %d\n", *yPtr); //Indirection operator
  //printf("y: %d\n", yPtr);
  int a;
  int *aPtr;

  a = 7;
  aPtr = &a;/* aPtr set to address of a */

  printf("The address of a is: %p"
         "\nThe value of aPtr is: %p\n", &a, aPtr);

  printf("The value of a is: %d"
         "\nThe value of *aPtr is: %d\n", a, *aPtr);

  printf("Showing that * and & are complements to each other.\n &*aPtr: %p"
         "\n*&aPtr: %p\n", &*aPtr, *&aPtr);
}
