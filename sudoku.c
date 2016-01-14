#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "mt19937ar.c"

#define ROWS 9
#define COLS 9
#define SQ 3  // SQUARE rows and cols

/* FUNCTIONS PROTOTYPES (declaring functions that we will use in the code) */
void init_random_seed();
int getRandomDigit(int initial, int final);;
void read_sudoku();
void fill_rand_cells();
int existsInSquare(int row, int col, int digit);
void write_sudoku();
int evaluate_cost();
void choosing_neighbour();
int number_nonfixed_cells();
void swap_cells(int row1,int col1,int row2,int col2);


/* GLOBAL VARIABLES (are visible everywhere in the code) */
int cell[ROWS][COLS];    // Cells content (0 means undefined)
int fixed[ROWS][COLS];   // Cells fixed (1) or not fixed (0)

int cost;
double T;
double alpha = 0.8; //0.8<=alpha<=0.95

int N = 25; //number of Markov chains
int MCL;    // Markov chain length


/********* Main function ************/
int main()
{
	/* Initialise random number generator */
	init_random_seed();
	
	/* read sudoku from a file */
	read_sudoku();			
	/* Fill cells with random digits so each 3 × 3 square contains each of the integers 1 through 9 exactly once */
	fill_rand_cells();


	cost = evaluate_cost();
	printf("COST: %d\n",cost);
	int number_nonfixed=number_nonfixed_cells();
	MCL=pow(number_nonfixed,2);

printf("MCL: %d\n",MCL);


	int m,n;
	int cycle = 0;

while(cost != 0) {
printf("CYCLE: %d\t COST: %d\n",++cycle,cost);

	T=2.5;

	for (n=1;n<=N;n++) 
	{
		for (m=1;m<=MCL;m++)
		{
			choosing_neighbour();

			if (cost==0)
			{
				break;
			}		
		}
		if (cost==0)
		{
			break;
		}
            printf("COST: %d\n",cost);

	T=alpha*T;	
	}
}
	

   printf("FINAL Cost: %d\t CYCLES: %d\n",cost, cycle);
	/* write sudoku */
	write_sudoku();	
}

/********** Initialise random number generator ************/

void init_random_seed()				
{
	unsigned long seed = time(NULL);
	init_genrand(seed);
	return;
}


int getRandomDigit(int initial, int final)
{
   return (int)floor((double)(final-initial+1)*genrand()) + initial;
}


void read_sudoku()
{
	char filename[500];
	FILE *input;
	int i,j;
	char c;
	
	sprintf(filename,"sudoku.txt");
	// open file in read mode
	input=fopen(filename,"r");
	
	// check if I opened the file correctly
	if(input==NULL) {fprintf(stderr,"ERROR in opening file\n"); exit(1);}
	
	// read cells
	for(i=0;i<ROWS;i++)
	{
		for(j=0;j<COLS;j++)
		{

			fscanf(input,"%c",&c);
			if(isdigit(c))
			{
				fixed[i][j] = 1;
				cell[i][j] = c - '0';
			}
			else
			{
				fixed[i][j] = 0;
			    cell[i][j] = 0;
			}
		}
	}
	
	// close the file
	fclose(input);
	
	return;
}

void fill_rand_cells()
{
	int i,j,digit;

	for(i=0;i<ROWS;i++)
	{
	  for(j=0;j<COLS;j++)
	  {
	    while (!fixed[i][j] && !cell[i][j])
	    {
		  digit = getRandomDigit(1,9);
		  if (!existsInSquare(i,j,digit))
		  {
			cell[i][j] = digit;
		  }
	    }	
	  }
	}
}

int existsInSquare(int row, int col, int digit)
{
	int i,j;
	int exists = 0;
	int irow = (row/SQ)*SQ;   // Initial row del cuadrado 3x3 
	int icol = (col/SQ)*SQ;   // Initial column del cuadrado 3x3
	int frow = irow + SQ - 1;  // Final row (square)
	int fcol = icol + SQ - 1;  // Final column (square)

	for (i=irow;i<=frow;i++)  // rows
	{
	    for (j=icol;j<=fcol;j++)  // columns

		{
				if (cell[i][j] == digit)
				{
					return 1;
				}
		}
	}
	return 0;
}

void write_sudoku()
{
	int i,j;
	FILE* output;
	
	output = fopen ("sudoku_solved.txt", "w");

	for(i=0;i<ROWS;i++)
	{
		for(j=0;j<COLS;j++)
		{
			fprintf (output, "%d  ", cell[i][j]);
		}
		fprintf (output, "\n");
	}
	
	fclose(output);
	return;
}

// We compute the cost of a board configuration by summing the number of unused values between 1 and 9 in each row and column
int evaluate_cost()
{
	int i,j,k;
	int count[10];
	int cost = 0;
	
	// ROWS
	for(i=0;i<ROWS;i++)
	{
		for(k=1;k<=ROWS;k++)
		{
			count[k] = 0;
		}
		for(j=0;j<COLS;j++)
		{
			count[cell[i][j]]++;
		}
		for(k=1;k<=ROWS;k++)
		{
			if (!count[k])
			{
				cost++;
			}
		}
	}
	
	// COLUMNS
	for(j=0;j<COLS;j++)
	{
		for(k=1;k<=COLS;k++)
		{
			count[k] = 0;
		}
		for(i=0;i<ROWS;i++)
		{
			count[cell[i][j]]++;
		}
		for(k=1;k<=COLS;k++)
		{
			if (!count[k])
			{
				cost++;
			}
		}
	}

	return cost;	
}

void choosing_neighbour() //intercambia los valores de 2 celdas aleatorias dentro del mismo cuadrado 3x3 y calcula el nuevo coste y lo acepta o lo rechaza
{
	int row1,col1;
	int row2,col2;
	do
	{
	  row1=getRandomDigit(0,8);
	  col1=getRandomDigit(0,8);
	}
 	while (fixed[row1][col1]);
		//	printf("(Row1,Col1): (%d,%d)",row1,col1);


    //Para meternos en el cuadrado 3x3 de la celda aleatoria que acabamos de coger:
	int irow = (row1/SQ)*SQ;   // Initial row del cuadrado 3x3 
	int icol = (col1/SQ)*SQ;   // Initial column del cuadrado 3x3
	int frow = irow + SQ - 1;  // Final row (square)
	int fcol = icol + SQ - 1;  // Final column (square)
	int c_new;
	do
	{
	  row2=getRandomDigit(irow,frow);
	  col2=getRandomDigit(icol,fcol);
	}
	while ((row1==row2 && col1==col2) || fixed[row2][col2]);
		//		printf("(Row2,Col2): (%d,%d)",row2,col2);


	swap_cells(row1,col1,row2,col2);

	c_new=evaluate_cost(); // C'

	if (c_new<cost || genrand()<exp((cost-c_new)/T))
	{
		cost=c_new;
	}
	else
	{
		swap_cells(row1,col1,row2,col2);
	}
}

void swap_cells(row1,col1,row2,col2)
{
   int aux;
   aux = cell[row1][col1];
   cell[row1][col1] = cell[row2][col2];
   cell[row2][col2] = aux;
}

int number_nonfixed_cells()
{
	int i,j;
	int number_nonfixed = 0;
	for (i=0;i<ROWS;i++)
	{
		for (j=0;j<COLS;j++)
		{
			if (fixed[i][j]==0)
			{
				number_nonfixed++;
			}
		}
	}
	return number_nonfixed;
}
