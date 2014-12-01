#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
typedef std::pair<int, int> npair;
std::list<int> primes;									// list of primes
std::list<npair> c_primes;

void get_primes_to (int n)
{
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
		bool is_prime = true;
		//std::cout << "get_primes_to: check " << i << " for factors up to " << (sqrt_i + 1) << std::endl;
		for (std::list<int>::iterator p = primes.begin(); p != primes.end() && *p < (sqrt_i + 1); ++p)
		{
			//std::cout << "get_primes_to: " << i << " % " << *p << std::endl;
			if (i % *p == 0)
			{
				is_prime = false;
				break;
			}
		}

		// If it is prime, let us add it to the list of primes.
		// Let us also add it to the list of consecutive (odd)
		// primes if it was consecutive.
		if (is_prime)
		{
			// Let note the consecutive prime, before we add
			// it to the list of primes.
			if (i == (primes.back() + 2))
			{
				c_primes.push_back(npair(primes.back(), i));
			}

			// Now, add it..
			primes.push_back(i);
		}
	}

	// Add it at the end. All numbers are divisible by 1,
	// this will mess up the previous calculations.
	primes.push_front(1);
}

void print_primes ()
{
	for (std::list<int>::iterator p = primes.begin(); p != primes.end(); ++p)
	{
		std::cout << *p << std::endl;
	}
}
void print_consprimes (std::string file_name)
{
	std::fstream fs;
	fs.open (file_name.c_str(), std::fstream::out);

	// Interate through the list of consecutive odd primes, outputting the information
	// to the provided file.
	for (std::list<npair>::iterator pp = c_primes.begin(); pp != c_primes.end(); ++pp)
	{
		std::cout << pp->first << "," << pp->second << std::endl;
		fs << pp->first << "," << pp->second << std::endl;
	}
	fs.close();
}
int main (int argc, char *argv[])
{
	double t1, t2;
	std::string input;
	std::cout << "consprime - finds all consecutive odd prime numbers up to a given number, n.\n";
	std::cout << "Results are also written to 'out.txt'.\nEnter in a number, n (or 'exit' to quit)=";
	std::cin >> input;
	while (input != "exit")
	{
		t1 = MPI_Wtime();
		// Call get_primes_to() - the main algorithm
		get_primes_to (atoi(input.c_str()));					

		// Print the results.
		//print_consprimes("out.txt");
		t2 = MPI_Wtime();
		std::cout << "[" << c_primes.size() << "] Consecutive odd primes " << (t2-t1) << " seconds.\n";
		std::cout << "[" << primes.size() << "] Primes.\n";
		// Clear the data structures (lists) in preparation 
		// for the next round.
		primes.clear();
		c_primes.clear();

		// Prompt for the next round.
		std::cout << "Enter in a number, n (or 'exit' to quit)=";
		std::cin >> input;
	}
	return 0;
}
