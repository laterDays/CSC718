/*

The program can be used to calculate harmonic progression sum.

The simplest harmonic progression is
	1/, 1/2, 1/3, ...
Let S(n) = 1/1 + 1/2 + 1/3 + ... + 1/i.

For example S(7) = 2.592857142857 to 12 digits of precison.

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#define MAX_PROC_NAME 15
int main(int argc, char ***argv)
{
	long unsigned int *digits;
	long unsigned int *all_digits;
	long unsigned int remainder, div, mod;
	int i, digit;
	double elapsed_time; 			/* execution time */
	int id; 				/* this process id */
	int num_procs; 				/* number of processes */
	int namelen;
	char cpu_name [MAX_PROC_NAME];

	MPI_Init (&argc, argv);
	MPI_Barrier(MPI_COMM_WORLD); 
	MPI_Comm_rank (MPI_COMM_WORLD, &id);
	MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
	MPI_Get_processor_name(cpu_name, &namelen);
	printf("[%d] starting on %s\n", id, cpu_name);
	elapsed_time = -MPI_Wtime();
	//printf("[%d] start...\n", id);
	
	//Sequential Method
	if (argc != 3)
	{
		printf("usage: hps n d\n");
		printf("    n: value of N (Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5)\n");
		printf("    d: numbers of Digits (Example: 5 => 0,xxxxx) \n");
		MPI_Finalize();
		exit(-1);
	}

	int n = atoi((char*)argv[1]);	// value of N
	int d = atoi((char*)argv[2]);	// numbers of digits

	digits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d+11));
	if(id == 0) all_digits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d+11));

	if (!digits)
	{
		printf("memory allocation failed\n");
		MPI_Finalize();
		exit(-1);
	}

	// [A] set digits to 0
	for (digit = 0; digit < d + 11; ++digit)
		digits[digit] = 0;



	// [B] - round-robin distribution
	// e.g. if 3 processes and n = 5
	// proc 0 - 1,4
	// proc 1 - 2,5
	// proc 2 - 3
	for (i = id + 1; i <= n; i += num_procs) 
	{
		remainder = 1;
		//printf("[B] i = %d\n", i);
		for (digit = 0; digit < d + 11 && remainder; ++digit) 	
		{
			div = remainder / i;
			mod = remainder % i;
			digits[digit] += div;
			remainder = mod * 10;
			//printf("div = %lu, mod = %lu, digits[%d] += %lu, remainder = %lu\n", div, mod, digit, div, remainder);
		}
	}
	MPI_Reduce(digits, all_digits, d + 11, MPI_UNSIGNED_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

	if (id == 0)
	{
		// [C] Roll back excess (cannot have decimal digits > 10)
		//	init regroupng, from d+11-1 to 1
		for (i = d + 11 - 1; i > 0; --i) 
		{
			all_digits[i - 1] += all_digits[i] / 10;
			all_digits[i] %= 10;
		}

		// [D] Round off the last digit: skip all the digits after d
		if (digits[d + 1] >= 5) 
		{
			++all_digits[d];
		}

		// [E] Roll back excess due to [D]: regrouping again because of D
		for (i = d; i > 0; --i) 
		{
			all_digits[i - 1] += all_digits[i] / 10;
			all_digits[i] %= 10;
		}

		// print sum
		printf("%ld.", all_digits[0]);

		for (i = 1; i <= d; ++i) 
		{
			printf("%ld",all_digits[i]);
		}
	
		elapsed_time += MPI_Wtime();
		if(id == 0)
		{
			printf(" in  %10.6f sec.\n", elapsed_time);
		}
		else
		{
			printf("\n");
		}
		free(all_digits);
	}

	MPI_Finalize();
	free(digits);

	return 0;
}
