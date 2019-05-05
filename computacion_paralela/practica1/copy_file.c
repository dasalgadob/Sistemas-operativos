
	#include<stdio.h>

  void copyFile(char* sourcePath, char*  destPath){
    FILE *source, *target;
    int i;
    source = fopen(sourcePath, "rb");

    if( source == NULL ) { printf("Press any key to exit...\n");} //exit(EXIT_FAILURE);

    fseek(source, 0, SEEK_END);
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);
    target = fopen(destPath, "wb");

    if( target == NULL ) { fclose(source); } //exit(EXIT_FAILURE);

    for(i = 0; i < length; i++){
        fputc(fgetc(source), target);
    }

    printf("File copied successfully.\n");
    fclose(source);
    fclose(target);
  }

	int  main(int argc, char **argv)
	{
    copyFile(argv[1], argv[2]);
    return 0;
	}
