/* Sieve of eratosthenes */
#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//#include "MyMPI.h"

#define OFFSET 3
#define MIN(a,b)  ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(2 * (((n)/2)/(p) + 1))) + OFFSET
#define BLOCK_HIGH(id,p,n)   (BLOCK_LOW(((id)+1),p,n)-2)
#define BLOCK_SIZE(id,p,n)   ((BLOCK_LOW(((id)+1),p,n))-(BLOCK_LOW(id,p,n)))/2
#define BLOCK_OWNER(index,p,n)   (((p)*(index)+1)-1)/(n))

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
	low_value = BLOCK_LOW(id,p,n-1);
	high_value = BLOCK_HIGH(id,p,n-1);
	size = BLOCK_SIZE(id,p,n-1);
	proc0_size = (n-1)/(2*p);

	// - main loop works only for prime * prime <= n
	// - this means it only runs as long as
	//	prime <= sqrt(n)
	// - In this setup, the program will exit if proc0 doesn't hold all starting primes (i.e.
	// it will exit if a starting prime will need to be chosen from another process - which this
	// program is not prepared to do.
	if ((OFFSET + proc0_size) < (int) sqrt((double) n))
	{	
		if (!id) printf ("Too many processes\n");
		MPI_Finalize();
		exit (1);
	}

	marked = (int *) malloc (size * sizeof(int));

	if (marked == NULL)
	{
		printf ("Cannot allocate enough memory\n");
		MPI_Finalize();
		exit (1);
	}

	for (i = 0; i < size; i++) marked[i] = 0;

	if (!id) index = 0;

	prime = OFFSET;
	
	printf("[%d-%s] ARRAY: low[0]=%d, high[%d]=%d\n", id, cpu_name, low_value, size-1, high_value);
	do {
		//printf("[%d] prime: %d\n", id, prime);
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

		// Now, mark all multiples of the prime. 
		// 'first' is a multiple of the prime, so += prime
		// is also a multiple.
		for (i = first; i < size; i += prime)
		{
			marked[i] = 1;
			//printf("[%d] marked: %d\n", id, (low_value + (i * 2)));
		}

		if (!id) {
			// Increase 'index' which pointed to the last
			// smallest prime, until it reaches the next
			// smallest prime.
			while (marked[++index]);

			// Remember, each 'index' in process 0 
			// represents the number = index + 2
			prime = (2 * index) + OFFSET;
		}

		// Now, broadcast the prime that will be used in the 
		// next iteration.
		MPI_Bcast (&prime,  1, MPI_INT, 0, MPI_COMM_WORLD);
		//sleep(1);
	} while (prime * prime <= n);

	count = 0;

	// Total up the amount of items that are not marked
	// i.e. the number of local primes

	// REMOVE LATER!!!!
	//sleep(1);
	MPI_Barrier(MPI_COMM_WORLD);
	//

	
	//printf("[%d] ", id);
	for (i = 0; i < size && (((2*i)+low_value) <= n); i++) 
	{
		if (!marked[i]) 
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

