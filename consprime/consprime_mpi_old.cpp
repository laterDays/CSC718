#include <cmath>
#include <iostream>
#include <mpi.h>
#include <stdlib.h>

typedef unsigned char byte; 
#define BYTE_SIZE 8
#define A(i, b)  ((i) * 16) + (2 * (b)) + 1

void print (byte* A, int num_elements)
{
	byte mask = 128;
	for (int i = 0; i < num_elements; i++)
	{
		std::cout << "[";
		for (int bit = 0; bit < 8; bit++)
		{
			std::cout << ((A[i] & (mask>>bit))>>(7 - bit));
		}
		std::cout << "]";
	}
}
int can_divide (int rank, byte* A, int low, int high, int n)
{
	// Each processor will check only a region of the array
	// of odd integers. 
	// low = low array index bound
	// high = high array index bound	
	// low_n = lowest number represented in bit form
	// high_n = highest number represented in a bit
	int low_n = A(low,0);
	int high_n = A(high,7);

	std::cout << "[" << rank << "] checking for divisibility of " << n << " from " << low_n << " to " << high_n << std::endl;
	// I will deal with the outermost loop later, for now
	// This loop will find if n is divisible by any of 
	// the numbers that A[] represents.

	// 128 = 10000000 
	int mask = 128;

	// We will traverse the local array A_local
	// A_local [i]
	for (int i = low; i <= high; i++)
	{
		// We will traverse the bits within each
		// array element. 
		// A_local[i] 
		// [ | | | | | | | ]
		//  0 1 2 3 4 5 6 7
		for (int bit = 1; bit < 8 ; bit++)
		{
			int x = A(i,bit);			// Get the number respresented by the bit
			if (x > n) break;			// If the number is greater than n, break

			// If the bit is 1, then then x is a 
			// prime number. We will test n for divisibility
			// only by prime numbers.
			if (((A[i]>>(7-bit)) & 1) == 1) 	
			{

				// If n is divisible by x, then it is
				// not prime.
				if (n % x == 0)
				{
					std::cout << std::endl;
					std::cout << "[" << rank << "] " << n << " isn't prime. " << std::endl;
					std::cout << "[" << rank << "] "; print(A, 1);
					return 0;
				}	
			}
			std::cout << std::endl;
		}		
	}
	if (n >= low_n && n <= high_n)
	{
		int i = n / 16;
		int bit = (n - (16 * (n/16)))/2; 			
		A[i] = (A[i] | 128>>bit);	
	}	
	std::cout << "[" << rank << "] " << n << " is prime. " << std::endl;
	std::cout << "[" << rank << "] "; print(A, 1);
	std::cout << "!" << std::endl;
	return 1;
}

int main (int argc, char* argv[])
{

	if (argv[1] == 0) return 0;
	////////////////////// MPI ////////////////////////
	int numprocs, rank, namelen;        
        char cpu_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc,&argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Get_processor_name(cpu_name, &namelen);
	/////////////////////////////////////////////////

	// A [] - an array representing odd numbers 
	//	- actually, each bit will represent an 
	//	odd number.
	// A_local [] - a local array, each process will
	//	only be responsible for its section of 
	//	the array, and changes will be made to
	//	A_local, and ORed into A[] of process 0.
	int N = atoi(argv[1]);

	int sz = N / BYTE_SIZE;						// sz = size of A / A_local
	int part_sz = sz / numprocs == 0 ? (sz/numprocs) + 1: sz/numprocs;	// part_sz = the size that 
	byte A [sz];								
	byte A_local [sz];						
									// 	this process is responsible for
	int xtra = sz - (part_sz * numprocs);					// xtra - the number of processes 
	int xclude = xtra < 0? abs(xtra) : 0;
	if (xtra < 0) xtra = 0;							// 	that must have one extra piece of
										//	the array to manage.
	if (rank == 0) std::cout << "sz=" << sz << " part_sz=" << part_sz << " xtra=" << xtra << " xclude=" << xclude << std::endl;
	// Calculation of low and high indeces
	// that the local process will handle
	int low 	= rank < xtra? rank * (part_sz + 1) : (rank * part_sz) + xtra;
	int high 	= rank < xtra? low + part_sz 	    : low + part_sz - 1;
	
	if (rank > (numprocs - xclude - 1)) 
	{
		MPI_Finalize();
		return 0;
	}
	A_local[0] = 128;
	for (int i = 1; i < N; i += 2)	
	{
		can_divide(rank, A_local, low, high, i);
	}
	print(A_local, sz); std::cout << std::endl;
	// If we have 'left over' processes that are not
	// needed, they will skip all operations.
	/*
	if (rank < A_part_num)
	{
		int low = A_part_size * rank;
		int high;

		if (rank == numprocs - 1)
		{
			high = A_size - 1;
		}
		else
		{
			high = low + (A_part_size - 1);
		}

		byte A_GLOBAL [A_size];
		byte A_LOCAL [A_size];
		
		std::cout << "[" << rank << "] (" << low << " -> " << high << ")" << std::endl;
	}
	*/
	MPI_Finalize ();
	return 0;

}
