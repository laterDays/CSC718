***************************************************************************************************************
**************************************************************************************************** * *** * * 
**										******  *** *  **** *
**	DASON ADAMOS								*******  * *    *
**	CSC 718			SIEVE - README					* ***  * *    
**	11NOV2014								* *  ***   *   
**										****
*******************************************************************************************  * 
******************************************************************************************

. . . . . . . . . . . . . . . . INTRODUCTION . . . . . . . . . . . . . . . . 

The "Sieve of Eratosthenes" algorithm is used to find all prime numbers up to 
some number, N.

	create sieve (array) from 2, 3, 4 ... N
	set k = 2
	loop
		mark multiples of k -> k*k, k*(k+1), k*(k+2), ... up to N
		k = smallest unmarked index
	until k^2 > N
	* unmarked entries in the array are primes

This tar file includes the following files: sieve2.c, sieve3.c, and sieve4.c. 
All of these files include implementations of the sieve of Eratosthenes algorithm
with implementation differences. In general, these files compute primes in the
following fashion:

	1	for each process:
	
	2	create sieve 'marked[]'
	3	prime = 3
	4	process 0: index = 0
	5	loop
	6		first = first index to mark
	7		for multiples of prime, marked[] = 1
	8		process 0: increments index until an unmarked index is found
	9			prime = index
	10			Broadcast the prime to other processes
	11	until prime^2 > N
	

sieve2.c - All even numbers past 2 are not primes. Since we know they are not
	primes, we can automatically exclude them from consideration. sieve2.c
	implements the sieve algorithm with consideration to odd numbers only.
	- Many changes were done in order to ensure that numbers are represented
	correctly:
		- the first element in the seive represents 3, not 2
		- difference between array elements is 2, not 1
		- block low values / high values needed to be adjusted

sieve3.c - This program eliminated the broadcast step on line 10 in the above 
	pseudo-code. In order to eliminate the broadcast, each process needed
	to have its own array that represented numbers 3 to sqrt(N) - I called
	this the 'seed'.

	- Changed lines marked (*):

	1	for each process:
	
	2*	create sieve 'marked[]' with two sections
			seed: 3 to sqrt(N)
			block: this processors share of numbers between sqrt(N) to N
	3	prime = 3
	4*	index = 0
	5	loop
	6		first = first index to mark
	7		for multiples of prime, marked[] = 1
	8*		increment index until an unmarked index is found
	9			prime = index
	10	until prime^2 > N

. . . . . . . . . . . . . . . . COMPILATION. . . . . . . . . . . . . . . . .

The included files are:
	build_sieves
	README.txt
	sieve2.c
	sieve3.c
	sieve4.cpp
You may compile the 3 program files using the provided file 'build_sieves'. 
However, you may have to chmod the file before execution:

user@host-$ chmod +x build_sieves
user@host-$ ./build_sieves

Instead, if you would like to build each file separately, you may type the 
following commands:

user@host-$ mpicc sieve2.c -o sieve2 -lm
user@host-$ mpicc sieve3.c -o sieve3 -lm
user@host-$ mpic++ sieve4.cpp -o sieve4

. . . . . . . . . . . . . . . . EXECUTION. . . . . . . . . . . . . . . . . .

All of the generated programs may be run from the command prompt using 
mpirun. Remember to provide the target N value as an argument.

user@host-$ mpirun sieve2 10000
user@host-$ mpirun sieve3 10000
user@host-$ mpirun sieve4 10000

If you would like to run the program on multiple machines, set up a machinefile 
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

After creating this file, you may run mpirun in the following fashion:

user@host-$ mpirun -machinefile machines -n 4 sieve2 10000

The above executes sieve2 with N = 10000 with the following:
	-machinefile: run on the machines specified in the machinefile 'machines' 
	-n: with 4 processes

. . . . . . . . . . . . . . . . RESULTS. . . . . . . . . . . . . . . . . . .

sieve2.c

	The following is the output from running sieve2 over dsu:

	dsu@jefferson:~/Dev/sieve$ mpirun -machinefile machines sieve2 1000
	[3-washington] ARRAY: low[0]=753, high[124]=1001
	[2-Roosevelt] ARRAY: low[0]=503, high[124]=751
	[1-lincoln] ARRAY: low[0]=253, high[124]=501
	[0-jefferson] ARRAY: low[0]=3, high[124]=251
	168 primes are less than or equal to 1000
	Total elapsed time:   0.005198

	The program outputs the array bounds each processor is responsible for
	marking. for example, the first line shows that the host, washington,
	has an array where its lowest element represents the number 753 and its
	highest element (index 124) represents the number 1001. The second-to-last
	line shows the number of primes and the last line shows the elapsed time
	in seconds.

sieve3.c

	The following is the output from running sieve3 over dsu machines:

	[0-jefferson] SEED:  low[     0]=     3, high[    15]=    33 (16)
	[3-washington] ARRAY: low[    16]=   759, high[   136]=   999 (121)
	[2-Roosevelt] ARRAY: low[    16]=   517, high[   136]=   757 (121)
	[1-lincoln] ARRAY: low[    16]=   275, high[   136]=   515 (121)
	[0-jefferson] ARRAY: low[    16]=    35, high[   135]=   273 (120)
	168 primes are less than or equal to 1000
	Total elapsed time:   0.002840

	There is a new type of array shown here, named SEED. This array represents
	numbers from 3 to sqrt(n), and although only the host, jefferson, is showing
	its information, all of the other hosts also own a copy of the seed. This 
	array is called the seed because it is the source from which all processes
	find the prime whose multiples it must mark.


. . . . . . . . . . . . . . . . BENCHMARK. . . . . . . . . . . . . . . . . . .
	
The following benchmark data was found for n=100,000,000. Significant improvement
was seen from sieve1->sieve2 and from sieve2->sieve3. An attempt at creating 
sieve4.cpp was created, but benchmarks revealed it was lacking in performance,
and therefore, I have considered it a failure.

	n=1	n=2	n=3	n=4
sieve1	19.17	 8.55    4.85    3.63
sieve2	 7.61    3.40    2.37    1.96
sieve3   7.18	 3.13    2.10    1.59
sieve4	21.56    8.84	 4.51    3.30

