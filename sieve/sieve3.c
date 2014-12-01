/* Sieve of eratosthenes */
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//#include "MyMPI.h"

#define MIN(a,b)  		((a)<(b)?(a):(b))
#define OFFSET 3
#define SEED_SIZE(n) 		(int)ceil((((double)sqrt(n))/2))
#define SEED_OFFSET(n) 		((SEED_SIZE(n)*2) + OFFSET)
#define SEED_HIGH(n) 		(SEED_OFFSET(n)-2)
#define BLOCK_LOW(id,p,n)  	((((id) * ((((n)-1)/2) - SEED_SIZE(n))/(p)) * 2) + SEED_OFFSET(n))
#define BLOCK_HIGH(id,p,n)   	(BLOCK_LOW(((id)+1),p,n)-2)
#define BLOCK_SIZE(id,p,n)   	((BLOCK_LOW(((id)+1),p,n))-(BLOCK_LOW(id,p,n)))/2
#define BLOCK_SXN_SIZE(n)	((((n)-1)/2) - SEED_SIZE(n))
#define BLOCK_OWNER(index,p,n)  (((p)*(index)+1)-1)/(n))

// [SEED | | | ] [BLOCK_SXN | | | | ]

int main (int argc, char *argv[])
{
	int count;		/* local prime count				*/
	double elapsed_time; 	/* execution time				*/
	int first;		/* index of the first sieve			*/
	int global_count;	/* global count of prime numbers 		*/
	int high_value; 	/* highest value assigned to this process 	*/
	int i;			/* loop counter					*/
	int id;			/* this process id				*/
	int index;		/* index of the current sieve			*/
	int low_value;		/* lowest value assigned to this process 	*/
	int *marked;		/* array elements to be  marked			*/
	int n;			/* value of the largest number			*/
	int p; 			/* number of processes				*/
	int proc0_size;		/* number of elements assigned to process zero 	*/
				/* this is to find if process zero has all primes */
	int prime;		/* current prime or sieve			*/
	int size;		/* elements in marked array 			*/
	int seed_size;
	char cpu_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;

	MPI_Init (&argc, &argv);
	/* start timer */
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -MPI_Wtime();
	MPI_Comm_rank (MPI_COMM_WORLD, &id);
	MPI_Comm_size (MPI_COMM_WORLD, &p);
	MPI_Get_processor_name(cpu_name, &namelen);

	if (argc != 2)
	{
		if (!id) printf ("Command line: %s <m>\n", argv[0]);
		MPI_Finalize();
		exit (1);
	}

	n = atoi(argv[1]);

	/* find how many elements are assigned to this process */
	low_value = BLOCK_LOW(id,p,n);
	high_value = BLOCK_HIGH(id,p,n);
	size = BLOCK_SIZE(id,p,n);
	seed_size = SEED_SIZE(n);
	proc0_size = (n-1)/(2*p);

	// - main loop works only for prime * prime <= n
	// - this means it only runs as long as
	//	prime <= sqrt(n)
	// - In this setup, the program will exit if proc0 doesn't hold all starting primes (i.e.
	// it will exit if a starting prime will need to be chosen from another process - which this
	// program is not prepared to do.
	/*
	if ((OFFSET + proc0_size) < (int) sqrt((double) n))
	{	
		if (!id) printf ("Too many processes\n");
		MPI_Finalize();
		exit (1);
	}*/
	// There is too many processes when we cannot split
	// up what is left of the numbers after taking out
	// the SEED section (which encloses sqrt(n))
	if (BLOCK_SXN_SIZE(n) < p)
	{
		if (!id) printf ("Too many processes\n");
		MPI_Finalize();
		exit (1);	
	}

	marked = (int *) malloc ((seed_size + size) * sizeof(int));

	if (marked == NULL)
	{
		printf ("Cannot allocate enough memory\n");
		MPI_Finalize();
		exit (1);
	}

	for (i = 0; i < (seed_size + size); i++) marked[i] = 0;

	index = 0;

	prime = OFFSET;
	if(!id) printf("[%d-%s] SEED:  low[     0]=     3, high[%6d]=%6d (%d)\n", id, cpu_name, (SEED_SIZE(n)-1), SEED_HIGH(n), seed_size);
	MPI_Barrier(MPI_COMM_WORLD);	
	printf("[%d-%s] ARRAY: low[%6d]=%6d, high[%6d]=%6d (%d)\n", id, cpu_name, seed_size, low_value, seed_size + size-1, high_value, size);
	do {
		//printf("[%d] *prime: %d\n", id, prime);
		// SEED marking - mark the multiples of the seed - within
		// the seed block. Start marking from the value of 
		// the prime * prime, which is at the position
		// index + prime
		for (i = index + prime; i < seed_size; i += prime) 
		{
			//printf("[%d] marked: %d\n", id, (OFFSET + (i * 2)));
			marked[i] = 1;
		}

		// Now, we need to continue to the block section, and keep 
		// marking. But we first need to find out where to start.
		// For each process, we need to find the first element to mark.
		// The first number that we would have to mark is prime * prime

		// If the first number to mark (prime * prime) is at least 
		// above the low bound of this process ...
		if (prime * prime > low_value) 
		{
			// ... Then the first index is that number (prime * prime)
			// - the low_value. E.g:
			// prime = 7
			// low_value = 41
			// first = [(7*7) - 41]/2 = (49 - 41)/2 = 8/2 = 4
			// marked[4] will be marked first
			first = (prime * prime - low_value)/2;
			//printf("[%d] first(a): %d(%d)\n", id, first, low_value + (2 * first));
		

		}
		else 
		{
			// This section is for "run-on" arrays, e.g.
			// prime=3
			// p0 [ 3| 5] 3*3 > 3
			// p1 [ 7| 9] 3*3 !> 7 "run-on", need mark 9
			// p2 [11|13] 3*3 !> 11 "run-on"
		 	if (!(low_value % prime)) 
			{
				// If it is, then the first element of the
				// array will be the starting place			
				first = 0;
				//printf("[%d] first(b): %d(%d)\n", id, first, low_value + (2 * first));
			}		 	
			else 
			{
				// (3 - (11 % 3))/2
				// (3 - (2))/2s
				int tmp = prime - (low_value % prime);
				first = tmp % 2 == 0 ? tmp/2 : (tmp + prime)/2;
				//printf("[%d] first(c): %d(%d)\n", id, first, low_value + (2 * first));
			}
		}
		//  0         sqrt(n)
		// [SEED             ][     BLOCK_SXN ]
		// SEED_SIZE -------> ---> first
		// ----------------------> first + SEED_SIZE
		// Now, mark all multiples of the prime. 
		// 'first' is a multiple of the prime, so += prime
		// is also a multiple.
		for (i = first; i < size; i += prime)
		{
			//printf("[%d] marked: %d\n", id, (low_value + (i * 2)));
			marked[i + seed_size] = 1;
		}

		// Increase 'index' which pointed to the last
		// smallest prime, until it reaches the next
		// smallest prime.
		while (marked[++index]);

		// Remember, each 'index' in process 0 
		// represents the number = index + 2
		prime = (2 * index) + OFFSET;
	} while (prime * prime <= n);

	count = 0;

	// Total up the amount of items that are not marked
	// i.e. the number of local primes

	// REMOVE LATER!!!!
	//sleep(1);
	MPI_Barrier(MPI_COMM_WORLD);
	//

	
	//printf("[%d] ", id);
	if (!id)
	{
		for (i = 0; i < seed_size; i++) 
		{
			if (!marked[i]) 
			{
				//printf("%d,", ((2*i)+low_value));
				count++;
			}
		}
	}
	for (i = 0; i < size; i++) 
	{
		if (!marked[i + seed_size]) 
		{
			//printf("%d,", ((2*i)+low_value));
			count++;
		}
	}
	//printf("\n");

	
	// process 0 will receieve the sum of the number 
	// of local primes
	MPI_Reduce (&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	elapsed_time += MPI_Wtime();

	if (!id) {
		global_count++; 	// To account for 2

		printf ("%d primes are less than or equal to %d\n",
		global_count, n);
		printf ("Total elapsed time: %10.6f\n", elapsed_time);
	}

	MPI_Finalize ();

	return 0;
}

