#include <iostream>		// cout
#include <mpi.h>
#include <stdlib.h>		// malloc
#define INTERVALS 1000000

int main (int argc, char* argv[])
{


	// Actually, just let each part calculate the
	// range that they will be using. For now, let
	// assume a range of 0.0 - 1.0 and 1000000 intervals
	// use the rank to determine which part each processor
	// will integrate over.
	// e.g. if n = 2,
	//			rank 0 - integrate from 0.0 to 0.5
	//			rank 1 - integrate from 0.5 to 1.0
	// * figure out bounds later.

	int numprocs, rank, namelen;							// MPI related variables
	char cpu_name[MPI_MAX_PROCESSOR_NAME];

	double ysum = 0.0;										// Integral related variables
	double xi;

															// Reduction related variables
	double* part_sum = (double *)malloc(sizeof(double));	// Reduce will gather all 'part_sum's
	double* sum = (double *)malloc(sizeof(double));			// and total to 'sum'

	double t1, t2;											// <Performance> beginning(t1) and end(t2) times

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Get_processor_name(cpu_name, &namelen);

	if (rank == 0) t1 = MPI_Wtime();			// <Performance> stopwatch on processor 0,
												// which will receive the reduction result.

												// So we now have
												// the numprocs, which will tell us how many partitions of
												//				 the whole graph we will make
												// the rank, which will tell us which partition the current
												//			 processor will integrate over.

	double part_sz = 1.0 / numprocs;			// Use numprocs to determine the how wide
												// of a range of x values each processor
												// will integrate over.

	xi = rank * part_sz;						// Set the starting x (x initial) for this process

	// Console output
	std::cout << "process[" << rank << "] on cpu[" << cpu_name << "] range " << xi << "\n";


	//////////////////////////////////////////////
	//
	// The integral
	for (double x = xi + ((1.0/INTERVALS)*0.5); x < (xi + part_sz); x += 1.0/INTERVALS)
	{
		ysum += 4.0 / (1.0 + x*x);
	}
	part_sum[0] = ysum * (1.0/INTERVALS);


	std::cout << "process[" << rank << "] on cpu[" << cpu_name << "] calculated area: " << part_sum[0] << ".\n";
	//
	//////////////////////////////////////////////


	// 	Reduce
	// 	Collect all sums (part_sum) from each process
	//	and sum (MPI_SUM) them, storing the overall
	//	total into the variable 'sum'
	MPI_Reduce(part_sum, sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


	// 	Reveal the value of sum that is sent to
	//	process 0, due to the MPI_Reduce
	if (rank == 0)
	{
		t2 = MPI_Wtime();						// <Performance> get the time after process
												// 0 receives reduction result.
		std::cout << "process[" << rank << "] on cpu[" << cpu_name << "] Total area: " << *sum << " in " << (t2 - t1) << "seconds.\n";
	}


	MPI_Finalize();

	return 0;
}
