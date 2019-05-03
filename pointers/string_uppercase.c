#include <stdio.h>
#include <ctype.h>

void convertToUpperCase(char *sPtr);
void printCharacters(const char *sPtr);

int main(){
  char string[]  = "characters and $32.98";
  printf("String before: %s\n", string);
  //non-constant pointer to non-constant data
  convertToUpperCase(string);
  printf("String after: %s\n", string);
  //non-constant pointer to constant data
  printCharacters(string);

  int x;
  int y = 23;

  int * const ptr = &x;
  *ptr = 1000;
  printf("Number: %d\n", *ptr);
  //The pointer cannot be made to point to y or another variable

  const int *const yPtr = &y;

  //*yPtr = 3; This way the pointer is able just to read the variable. Least privilege of all.
  printf("Number y: %d\n", *yPtr);

  return 0;
}

//non-constant pointer to non-constant data
void convertToUpperCase(char *sPtr){
  while(*sPtr != '\0'){
    if(islower(*sPtr)){
      *sPtr = toupper(*sPtr);
    }

    ++sPtr;
  }
}

//Function cannot change the value of data of the pointer
void printCharacters(const char *sPtr){

  for(; *sPtr != '\0'; sPtr++){
    printf("%c", *sPtr);
  }
  printf("\n");
}
