**********************************************************************
*********************************************************** * *** * *
** 						****** *** * **** *
** DASON ADAMOS 				******* * * *
** CSC 718 		HPS - README 		* *** * *
** 09DEC2014 					* * *** *
** 						****
***************************************************** *
*******************************************************

. . . . . . . . . . . INTRODUCTION. . . . . . . . . . . . .

The programs included in this project are all variants of the original 
file, hps.c. This program calculates the sum of the harmonic progression:

	1/1 + 1/2 + 1/3 + ... 1/n

The other files are:

	mpihps.c - MPI - hps.c was rewritten to take advantage of the 
	Message Passing Interface.
		- Execution of the main for loop (mpihps.c, line 69)
		is divided between all threads.
		- Results are reduced into an array in process 0.
		- Process 0 performs the rest of the administrative
		tasks (rolling over excess and rounding).

	mphps.c - OpenMP - hps.c rewritten to fork threads in order to
	execute for loops in parallel.
		- Execution of initializing (mphps.c, line 56) and 
		main for loop (mphps.c, line 61) executed with 
		parallel threads ("pragma omp parallel for").
		- The administrative tasks (rolling over excess and
		rounding) are completed in a sequential manner.


. . . . . . . . . . . COMPILATION. . . . . . . . . . . . .



--==: Open MPI :==--

0. MPI - Download the latest distribution of Open MPI for your working
platform (e.g. openmpi-*.tar.gz). 
	- Follow the instruction for installation at http://www.open-mpi
	.org/faq/?category=building. 
	- Set the LD_LIBRARY_PATH environment variable to the library 
	installed by Open MPI (it may be at /usr/local/lib).
	- Check if it is installed correctly by typing mpirun. Your 
	system should not say that the program cannot be found.

1. For each machine that will be running the mpihps.c, ensure that the
file is downloaded at the same exact location in each machine. Execute
the following on each system:

	user@host-$ mpicc mpihps.c -o mpihps

2. Set up the machinefile on the machine you will execute the program from (e.g. 
your localhost). Create a file that contains the host or user@host combination 
(provide information you would normally use in order to engage an ssh connection).

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



--==: OpenMP :==--

1. Simply use the -fopenmp compiler flag when using gcc to compile the file:

user@host-$ gcc mphps.c -o mphps -fopenmp

2. The above line will generate mphps:

user@host-$ ls
mphps
mphps.c
...


. . . . . . . . . . . EXECUTION. . . . . . . . . . . . .


--==: Open MPI :==--

1. You may execute the program using OpenMPI's mpirun:

	user@host-$ mpirun -machinefile machines -n 2 mpihps

This will execute mpihps on 2 processes using the machinefile named "machines"



--==: OpenMP :==--

1. I have set up this program to take the number of threads for
execution as an argument:

	user@host-$ mphps [n value] [d # trailing digis] [t # threads]
	user@host-$ mphps 100 10 4

The above will execute

	1/1 + 1/2 + 1/3 + ... + 1/99 + 1/100 

with 10 decimal digits, using 4 threads.



. . . . . . . . . . . RESULTS . . . . . . . . . . . . .

The following results are stdout from running the command where

1/1 + 1/2 + 1/3 + ... 1/n	n = 1,000,000

decimal places are 100		d = 100


--==: Sequential :==--

dsu@jefferson:~/Dev/CSC718/hps$ mpirun -np 1 hps 1000000 100
14.3927267228657236313811274931885876766448000137443116534184330458129585075179950035682981759472191007 in    2.389022 sec.



--==: Open MPI :==--

[1 processor]
dsu@jefferson:~/Dev/CSC718/hps$ mpirun -machinefile machines -np 1 mpihps 1000000 100
[0] starting on jefferson
14.3927267228657236313811274931885876766448000137443116534184330458129585075179950035682981759472191008 in    2.353957 sec.

[2 processors]
dsu@jefferson:~/Dev/CSC718/hps$ mpirun -machinefile machines -np 2 mpihps 1000000 100
[0] starting on jefferson
[1] starting on lincoln
14.3927267228657236313811274931885876766448000137443116534184330458129585075179950035682981759472191008 in    1.580353 sec.

[3 processors]
dsu@jefferson:~/Dev/CSC718/hps$ mpirun -machinefile machines -np 3 mpihps 1000000 100
[0] starting on jefferson
[1] starting on lincoln
[2] starting on Roosevelt
14.3927267228657236313811274931885876766448000137443116534184330458129585075179950035682981759472191008 in    1.031743 sec.

[4 processors]
dsu@jefferson:~/Dev/CSC718/hps$ mpirun -machinefile machines -np 4 mpihps 1000000 100
[0] starting on jefferson
[2] starting on Roosevelt
[1] starting on lincoln
[3] starting on washington
14.3927267228657236313811274931885876766448000137443116534184330458129585075179950035682981759472191008 in    1.091109 sec.



--==: OpenMP :==--

* Lack of precision experienced

[1 thread]
user@host-$ ./mphps 1000000 100 1
14.3927267228657236313811274931885876766448000137443116534184330458129585075179950035682981759472191007 in 2.305534 sec.

[2 threads]
user@host-$ ./mphps 1000000 100 2
14.3810941917888896938853592420961238626329943129576442581979122186708369619361972861572934931893774583 in 1.300271 sec.

[3 threads]
user@host-$ ./mphps 1000000 100 3
14.0641824361351050211938214684472197789713079774138249656718396753092850743106710885552153054117615422 in 1.200059 sec.

[4 threads]
user@host-$ ./mphps 1000000 100 4
14.0372325485647678608647319487408421281198707304267800100357516557719233010124084681179606053263032938 in 0.935458 sec.

