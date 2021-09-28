#include "directionStack.h"
#include "quadTree.h"
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int test_stack = 0;
int test_quadTree = 0;

float RandomInt(int min, int max){
	int total = max-min;
	return (int)(rand() % total - max);
}

int writeImage(char* filename, int width, int height,  char* title, quadTree *t)
{
   int code = 0;
   FILE *fp = NULL;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   png_bytep row = NULL;

   fp = fopen(filename, "wb");


   if (fp == NULL) {
      fprintf(stderr, "Could not open file %s for writing\n", filename);
      code = 1;
      goto finalise;
   }

   // Initialize write structure
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      fprintf(stderr, "Could not allocate write struct\n");
      code = 1;
      goto finalise;
   }

   // Initialize info structure
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      fprintf(stderr, "Could not allocate info struct\n");
      code = 1;
      goto finalise;
   }

   // Setup Exception handling
   if (setjmp(png_jmpbuf(png_ptr))) {
      fprintf(stderr, "Error during png creation\n");
      code = 1;
      goto finalise;
   }

   png_init_io(png_ptr, fp);


   // Write header (8 bit colour depth)
   png_set_IHDR(png_ptr, info_ptr, width, height,
         8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
         PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   // Set title
   if (title != NULL) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      title_text.key = "Title";
      title_text.text = title;
      png_set_text(png_ptr, info_ptr, &title_text, 1);
   }

   png_write_info(png_ptr, info_ptr);

   // Allocate memory for one row (3 bytes per pixel - RGB)
   row = (png_bytep) malloc(width * sizeof(png_byte));

   // Write image data
   int x, y;
   for (y=-512 ; y<height/2 ; y++) {
//	  printf("printing row %i", y);
      for (x=-512 ; x<width/2 ; x++) {
    	  treeNode *n = getNode(x, y, t->treeRoot, 512);
    	  row[x+512] = (n->occupancy +50);
      }
      png_write_row(png_ptr, row);
   }

   // End write
   png_write_end(png_ptr, NULL);

   finalise:
   if (fp != NULL) fclose(fp);
   if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
   if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   if (row != NULL) free(row);

   return code;
}

void dataToImage(const char *filename, quadTree *t)
{

    char line[MAX_LINE_LENGTH] = {0};
    unsigned int line_count = 0;
	FILE *file = fopen(filename, "r");
    while (fgets(line, MAX_LINE_LENGTH, file))
    {
        /* Print each line */
        printf("line[%06d]: %s", ++line_count, line);
        printf("%c\n", line[0]);
        if(line[0] == '-'){
        	char line2[MAX_LINE_LENGTH];
        	strncpy(line2, line+2, 10);
        	printf("line2: %s\n", line2);

        	char * token = strtok(line2, ", ");
        	int x = atoi(token);
        	token = strtok(NULL, " ");
        	int y = atoi(token);
        	printf("inserting (%i,%i)\n", x, y);
        	updateTree(t, 0,0, x, y);
        }
        /* Add a trailing newline to lines that don't already have one */
        if (line[strlen(line) - 1] != '\n')
            printf("\n");
    }

    printf("tree Made \n");

	int close = fclose(file);
	if(close)
	{
		printf("closed file\n");
	}
}
int main()
{
	pathStack s;
	s.top = 0;
	if(test_stack)
	{
		printf("%lu\n", sizeof(unsigned int));
		printf("Testing directionStack::\n");
		printf("Making Stack...\n");


		printf("Peeking stack\n");
		enum direction d = peek(&s);
		printf("top direction is %i\n", d);

		printf("Made Stack...inserting some directions\n");
		printf("North x 14 \n");
		for(int i = 0; i <14; i++)
		{
			push(north, &s);
		}


		printf("east\n");
		push(east, &s);


		printf("south\n");
		push(south, &s);


		printf("Peeking stack\n");
		d = peek(&s);
		printf("top direction is %i\n", d);


		printf("west\n");
		push(west, &s);
	//	printf("south\n");
	//	push(south, &s);

		printf("Peeking stack\n");
		d = peek(&s);
		printf("top direction is %i\n", d);


		printf("popping");
		d = pop(&s);
		printf("popped off: %i\n", d);
		d = peek(&s);
		printf("top direction is %i\n", d);


		printf("popping");
		d = pop(&s);
		printf("popped off: %i\n", d);
		d = peek(&s);
		printf("top direction is %i\n", d);
	}



	if(test_quadTree)
	{

		treeNode root = {0,0,0,0,0};
		quadTree t = {&root,0,0};

		int points[100][2];
		for(int i = 0; i<100; i++)
		{
			points[i][0] = RandomInt(-512, 512);
			points[i][1] = RandomInt(-512, 512);
			printf("inserting point at %i,%i\n", points[i][0],points[i][1]);

			updateTree(&t, 0,0, points[i][0], points[i][1]);

		}
		printf("printing tree\n" );
		int w = writeImage("testimg", 1024, 1024, "map", &t);
		printf("w: %i\n", w);
		printf("size of tree : %i \n", t.count);

	}
	treeNode root = {0,0,0,0,0};
	quadTree t = {&root,0,0};
	dataToImage("console_output.txt", &t);
	int w = writeImage("testimg", 1024, 1024, "map", &t);
	printf("w = %i\n", w);
	return 0;
}
//gcc -Wall test.c quadTree.c directionStack.c -o test -lpng

