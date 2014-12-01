***************************************************************************************************************
**************************************************************************************************** * *** * * 
**										******  *** *  **** *
**	DASON ADAMOS								*******  * *    *
**	CSC 718			CONSPRIME_MPI - README				* ***  * *    
**	15NOV2014								* *  ***   *   
**										****
*******************************************************************************************  * 
******************************************************************************************

. . . . . . . . . . . . . . . . INTRODUCTION. . . . . . . . . . . . . . . . 

The MPI version of the previous consprime file uses the same algirthm:

Algorithm - In order to calculate the consecutive odd prime numbers, the following
method is used:

	get_primes_to n
		if n > 1, add 2 to "prime list"
		if n > 2, add 3 to "prime list"
		from every odd, i, from 5 up to n
			for every prime, p, in the "prime list" (up to sqrt(i))
				if i mod p is 0, i isn't prime
			if i is prime,
				if it is a consecutive prime add i 
				to the "consecutive prime list"
			
				add i to the list of primes

In order to involve multiple processes, some changes are made. The last line
is changed so that only one process adds the new prime to its list of primes,
therefore when each process checks a number for primality, it only checks the 
number against its own share of prime numbers. This creates another requirement,
after each prime checks for primality, all processes must must come to an agreement
on primality (through MPI_Allreduce).

	>> P = 0
	get_primes_to n
		if n > 1, add 2 to "prime list"
		if n > 2, add 3 to "prime list"
		from every odd, i, from 5 up to n
			for every prime, p, in the "prime list" (up to sqrt(i))
				if i mod p is 0, i isn't prime
			>> MPI_Allreduce() Check with all processes, is i prime for 
				all processes?
			if i is prime,
				if it is a consecutive prime add i 
				to the "consecutive prime list"
			
				>> add i to P's list of primes
				>> change P to next process

. . . . . . . . . . . . . . . . COMPILATION. . . . . . . . . . . . . . . . .

Do the following for each machine:

1. Extract consprime_mpi.cpp into an appropriate directory (the same directory
for each machine), cd into that directory and execute the following command:

	user@host-$ mpic++ consprime_mpi.cpp -o consprime_mpi

2. This command generates the executable 'consprime_mpi'.

. . . . . . . . . . . . . . . . EXECUTION. . . . . . . . . . . . . . . . .

1. If you would like to run the program on multiple machines, set up a machinefile 
on the machine you will execute the program from (e.g.  your localhost). Create
a file that contains the host or user@host combination (provide information you 
would normally use in order to engage an ssh connection).

	user@host-$ vim machines
	
	. . . . . . in vim . . . . . . .

	localhost
	host1
	host2
	...
	~
	~
	~
	. . . . . . . . . . . . . . . . 

	user@host-$

2. In order to execute the program, type the following on your main machine:

	user@host-$ mpirun -machinefile machines consprime_mpi 1000000
	
	* The above command will find consecutive primes up to n=1000000,
	but you may provide any positive integer as an argument.
	
3. The console displays the following message:

	[0] Number consecutive primes: 8169 0.552216seconds.

The program states that process 0 found 8169 consecutive prime numbers in ~.55 seconds.

. . . . . . . . . . . . . . . . RESULTS. . . . . . . . . . . . . . . . .

1. The output states the number of consecutive primes, unline the non-mpi version,
it does not list all the consecutive primes for the sake of performance.

. . . . . . . . . . . . . . MAIN RESULTS. . . . . . . . . . . . . . . . .

1. For a problem size of 1,000,000, there are 8169 consecutive odd primes. As shown
in the following console output:

	user@host-$ mpirun -machinefile consprime_mpi 1000000
	[0] Number consecutive primes: 8169 0.552216seconds.

Tests have shown that the performance of this MPI version of the algorithm is actually
slower than the single process version for two reasons:

	- Less work per iteration - This altorithm checks for divisibility of all
	odd numbers starting from 5, and each iteration ends with an MPI_Allreduce. 
	That is:
		i = 5, check for divisibility by 2,3
		MPI_Allreduce()
		i = 7, check for divisibility by 2,3,5
		MPI_Allreduce()
		i = 9, check for divisibility by 2,3
		MPI_Allreduce()
		i = 11, check for divisibility by 2,3,5,7,8
		...
	You can see from this algorithm that each process does less work and more
	communication per iteration than the sieve of Eratosthenes.
	
	- Communication overhead - This reason is tightly linked with the above. 
	We can see that communication occurs often, however, not only is communication 
	required every iteration, the type of communication is also heavy, it is an
	MPI_Allreduce(), rather than just an MPI_Reduce(), or MPI_Bcast().

Although the algorithm is not conducive to parallel treatment, I chose to work with it -
if not, I would just recreate the sieve of Eratosthenes. Nevertheless, my development
using this altorithm was not as successful as I had anticipated, leading to poor 
performance.



		

