#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
typedef std::pair<int, int> npair;
//std::list<int> all_primes;	
std::list<int> primes;			// list of primes
int num_consecutive = 0;

void get_primes_to (int n, int rank, int numprocs)
{
	int P = 0; 
	int* is_local_prime = (int *)malloc(sizeof(int));
	int* is_prime = (int *)malloc(sizeof(int));
	int last_prime;

	// A loop over all of the numbers up to the desired number.
	// Each loop will check to see if it is a prime, by checking
	// to see if it is divisible by a list of primes up to
	// that has been accumulated thus far, but we will only 
	// check up to prime <= sqrt(n). If the number is prime, it will
	// be added to the list of primes.

	// Add 2,3 to primes if n is large enough.
	// Get these cases out of the way so that we do not have
	// to test for them for every iteration of the following 
	//major loop.
	if (n > 1) primes.push_back(2);
	if (n > 2) primes.push_back(3);

	// For the rest of the numbers (odd numbers)...
	for (int i = 5; i <= n; i += 2)
	{
		// See if they are divisible by numbers in our list of
		// primes, only up to sqrt(n) - don't check past sqrt(n)
		// because doing so is a repetition of effort.
		double sqrt_i = sqrt(i);
		*is_local_prime = 1;
		//std::cout << "get_primes_to: check " << i << " for factors up to " << (sqrt_i + 1) << std::endl;
		for (std::list<int>::iterator p = primes.begin(); p != primes.end() && *p < (sqrt_i + 1); ++p)
		{
			//std::cout << "get_primes_to: " << i << " % " << *p << std::endl;
			if (i % *p == 0)
			{
				*is_local_prime = 0;
				break;
			}
		}
		
		// If it is prime, let us add it to this processes'
		// list of primes, only if it is it's turn (as 
		// designated by P.

		// All processes send out local results on primality.
		// They all also reduce to determine the group
		// consensus on primality.
		MPI_Allreduce(is_local_prime, is_prime, 1, MPI_INT, MPI_LAND, MPI_COMM_WORLD);
		if (*is_prime) 
		{
			// Processor P will add the new prime to the list
			// of primes it will use to determine primality
			if (rank == P)
			{
				//std::cout << "[" << rank << "] " << i << " OK\n";
				primes.push_back(i);
			}

			// Process 0 will add to list of consecutive primes to its list.
			if (rank == 0)
			{
				if (i == (last_prime + 2)) num_consecutive++;
			}
			// If there was a prime, then all processes
			// need to advance P, which indicates who
			// will add the next prime to their local 
			// list.
			P = (P + 1) < numprocs ? P + 1 : 0;

			// Reassign last_prime, this will be used
			// to determine the number of consecutive
			// primes.
			last_prime = i;
		}

	}

	// Add it at the end. All numbers are divisible by 1,
	// this will mess up the previous calculations.
	primes.push_front(1);
	if (rank == 0) num_consecutive++;	// for 1
}

void print_primes ()
{
	for (std::list<int>::iterator p = primes.begin(); p != primes.end(); ++p)
	{
		std::cout << *p << std::endl;
	}
}
int main (int argc, char *argv[])
{
	int N = atoi(argv[1]);
	double t1, t2;
	
	////////////////////////////////////////////////
	int numprocs, rank, namelen;
        char cpu_name[MPI_MAX_PROCESSOR_NAME];

        MPI_Init(&argc,&argv);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Get_processor_name(cpu_name, &namelen);
        /////////////////////////////////////////////////

	if (rank == 0) t1 = MPI_Wtime();
	// Call get_primes_to() - the main algorithm
	get_primes_to (N, rank, numprocs);					

	// Print the results.
	//print_consprimes("out.txt");
	//std::cout << "[" << c_primes.size() << "] Consecutive odd primes.\n";

	MPI_Barrier(MPI_COMM_WORLD);
	
	if (rank == 0)
	{
		t2 = MPI_Wtime();
		std::cout << "[0] Number consecutive primes: " << num_consecutive << " " << (t2-t1) << "seconds.\n";	
	}

	// Clear the data structures (lists) in preparation 
	// for the next round.
	primes.clear();

	MPI_Finalize();
	return 0;
}
