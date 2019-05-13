/*
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "omp.h"
#include <sys/time.h>

#define PNG_DEBUG 3
#include <png.h>


struct Pairs{
	long i,j,k;
	double sum;
};


void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}

int kernel_size;
int width, height, widthC, heightC;
png_byte color_type, color_typeC;
png_byte bit_depth, bit_depth2, bit_depthC;
//int matrix_result[10000][10000][4];

png_structp png_ptr, png_ptr2;
png_infop info_ptr, info_ptr2;
int number_of_passes,number_of_passes2;
png_bytep * row_pointers;
png_bytep * row_pointers2;

void read_png_file(char* file_name)
{
        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
                abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[read_png_file] png_create_read_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during init_io");


        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);
        //Duplicate
        //png_init_io(png_ptr2, fp);
        //png_set_sig_bytes(png_ptr2, 8);

        png_read_info(png_ptr, info_ptr);
        //png_read_info(png_ptr2, info_ptr2);

        width = png_get_image_width(png_ptr, info_ptr);
        height = png_get_image_height(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        //printf("bit_depth: %d\n",bit_depth );
        number_of_passes = png_set_interlace_handling(png_ptr);
        //Duplicate
        /*
        width = png_get_image_width(png_ptr2, info_ptr2);
        height = png_get_image_height(png_ptr2, info_ptr2);
        color_type = png_get_color_type(png_ptr2, info_ptr2);
        bit_depth2 = png_get_bit_depth(png_ptr2, info_ptr2);
        printf("bit_depth: %d\n",bit_depth );
        number_of_passes2 = png_set_interlace_handling(png_ptr2);
        */
        png_read_update_info(png_ptr, info_ptr);
        //png_read_update_info(png_ptr2, info_ptr2);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[read_png_file] Error during read_image");

        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        //row_pointers2 = (png_bytep*) malloc(sizeof(png_bytep) * height);
        for (int y=0; y<height; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
                //row_pointers2[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

        png_read_image(png_ptr, row_pointers);
        //png_read_image(png_ptr, row_pointers2);

        fclose(fp);
}

void read_png_file2(char* file_name)
{
        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */


        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
                abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        png_ptr2 = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr2)
                abort_("[read_png_file] png_create_read_struct failed");

        info_ptr2 = png_create_info_struct(png_ptr2);
        if (!info_ptr2)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr2)))
                abort_("[read_png_file] Error during init_io");


        png_init_io(png_ptr2, fp);
        png_set_sig_bytes(png_ptr2, 8);
        //Duplicate
        //png_init_io(png_ptr2, fp);
        //png_set_sig_bytes(png_ptr2, 8);

        png_read_info(png_ptr2, info_ptr2);
        //png_read_info(png_ptr2, info_ptr2);

        widthC = png_get_image_width(png_ptr2, info_ptr2);
        heightC = png_get_image_height(png_ptr2, info_ptr2);
        color_typeC = png_get_color_type(png_ptr2, info_ptr2);
        bit_depthC = png_get_bit_depth(png_ptr2, info_ptr2);
        //printf("bit_depth: %d\n",bit_depthC );
        number_of_passes2 = png_set_interlace_handling(png_ptr2);
        //Duplicate
        /*
        width = png_get_image_width(png_ptr2, info_ptr2);
        height = png_get_image_height(png_ptr2, info_ptr2);
        color_type = png_get_color_type(png_ptr2, info_ptr2);
        bit_depth2 = png_get_bit_depth(png_ptr2, info_ptr2);
        printf("bit_depth: %d\n",bit_depth );
        number_of_passes2 = png_set_interlace_handling(png_ptr2);
        */
        png_read_update_info(png_ptr2, info_ptr2);
        //png_read_update_info(png_ptr2, info_ptr2);


        /* read file */
        if (setjmp(png_jmpbuf(png_ptr2)))
                abort_("[read_png_file] Error during read_image");

        //row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        row_pointers2 = (png_bytep*) malloc(sizeof(png_bytep) * heightC);
        for (int y=0; y<heightC; y++)
                //row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
                row_pointers2[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr2,info_ptr2));

        png_read_image(png_ptr2, row_pointers2);
        //png_read_image(png_ptr, row_pointers2);

        fclose(fp);
}


void write_png_file(char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (int y=0; y<height; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);
}

void write_png_file2(char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        png_ptr2 = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr2)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr2 = png_create_info_struct(png_ptr2);
        if (!info_ptr2)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr2)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr2, fp);


        /* write header */
        if (setjmp(png_jmpbuf(png_ptr2)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr2, info_ptr2, widthC, heightC,
                     bit_depthC, color_typeC, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr2, info_ptr2);


        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr2)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr2, row_pointers2);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr2)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr2, NULL);

        /* cleanup heap allocation */
        //for (y=0; y<height; y++)
        //        free(row_pointers[y]);
        //free(row_pointers);

        fclose(fp);
}


void process_file()
{
  if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
          abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                 "(lacks the alpha channel)");

  if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
          abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                 PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

  //Iterar sobre las filas del hilo
  #pragma omp parallel for
  for (int y=0; y<=height - kernel_size; y++) {
          //png_byte* row = row_pointers[y];
          for (int x=0; x<= width -kernel_size; x++) {
            double rsum=0, gsum=0, bsum=0, asum=0;
            //Hacer la convolucion
            //Primero iteral sobre las filas de la convolucion
            for(int i =y; i<y+kernel_size; i++){
              png_byte* row = row_pointers[i];
              //Iterar sobre las columnas de la convolution

              for(int j=x; j<x+kernel_size;j++){
                png_byte* ptr = &(row[j*4]);
                int r=ptr[0], g= ptr[1], b=ptr[2], a=ptr[3];
                rsum+= r/(kernel_size * kernel_size);
                gsum+= g/(kernel_size * kernel_size);
                bsum+= b/(kernel_size * kernel_size);
                asum+= a/(kernel_size * kernel_size);
              }

            }
            //Asignar valor a posicion de la convolucion
            png_byte* row_change = row_pointers2[y + (kernel_size/2)];
            png_byte* position = &(row_change[(x+ (kernel_size/2))*4]);
            position[0] = (int) rsum;
            position[1] = (int) gsum;
            position[2] = (int) bsum;
            position[3] = (int) asum;

            //matrix_result[y + (myPair->k/2)][(x+ (myPair->k/2))][0] = (int) rsum;
            //matrix_result[y + (myPair->k/2)][(x+ (myPair->k/2))][1] = (int) gsum;
            //matrix_result[y + (myPair->k/2)][(x+ (myPair->k/2))][2] = (int) bsum;
            //matrix_result[y + (myPair->k/2)][(x+ (myPair->k/2))][3] = (int) asum;
                  //printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
                  //       x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

                  /* set red value to 0 and green value to the blue one */
                  //ptr[0] = 0;
                  //ptr[1] = ptr[2];
          }
  }
}//Process file



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

    //printf("File copied successfully.\n");
    fclose(source);
    fclose(target);
  }


int main(int argc, char **argv)
{
  struct timeval start, end;
  gettimeofday(&start, NULL);

  pthread_attr_t attr; /* set of thread attributes*/
	int n_threads=0;

  //Validations of number of arguments and constraints on data
  if (argc != 5)
          abort_("Usage: program_name <file_in> <file_out> <kernel_size> <num_threads>");
  if(atoi(argv[3]) < 0){
          fprintf(stderr, "%d must be >=0\n", atoi(argv[3]));
          return -1;
  }
  if(atoi(argv[4]) < 0){
          fprintf(stderr, "%d must be >=0\n", atoi(argv[4]));
          return -1;
  }
  //preprocessing();
  copyFile(argv[1], "temporal.png");
  read_png_file(argv[1]);
  read_png_file2("temporal.png");
  //write_png_file("temp.png");
  //read_png_file2("temp.png");

  kernel_size = atoi(argv[3]);
  n_threads= atoi(argv[4]);
  omp_set_num_threads(n_threads);
	printf("Image: %s ", argv[1]);
  printf("Number of threads: %2d\t", n_threads);
	printf("Kernel Size: %2d\t", kernel_size);

  process_file();

  write_png_file2(argv[2]);

  gettimeofday(&end, NULL);
	long seconds = (end.tv_sec - start.tv_sec);
	long micros = ((seconds * 1000000) + end.tv_usec) - (start.tv_usec);

	printf("Time elpased is %d micros\n", micros);
  return 0;
}
