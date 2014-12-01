***************************************************************************************************************
**************************************************************************************************** * *** * * 
**										******  *** *  **** *
**	DASON ADAMOS								*******  * *    *
**	CSC 718			INTEGRAL - README				* ***  * *    
**	17OCT2014								* *  ***   *   
**										****
*******************************************************************************************  * 
******************************************************************************************

. . . . . . . . . . . . . . . . INTRODUCTION. . . . . . . . . . . . . . . . 

integral.cpp is a simple OpenMPI program that estimates the value of the integral:

	Integral( ___4___ ) dx
	 0->1	  1 + x^2

The estimation of the integral is completed by approximating the shape of the graph
with very thin rectangles, whose height is:

      y=___4___
	1 + x^2

and whose width is (1 / 1,000,000), then summing the area of these triangles. The way
this program will separate the integration of this area will depend on how many 
processes are spawned according to the following example:

if 1 process  -> process 0 will integrate from x = 0.0 to x = 1.0

if 2 processes-> process 0 will integrate from x = 0.0 to x = 0.5
		 process 1 will integrate from x = 0.5 to x = 1.0

if 4 processes-> process 0 will integrate from x = 0.00 to x = 0.25
		 process 1 will integrate from x = 0.25 to x = 0.50
		 process 2 will integrate from x = 0.50 to x = 0.75
		 process 3 will integrate from x = 0.75 to x = 1.00

Rather than having one process calculate the ranges for each process, each process
will use use the MPI_Comm_size() and MPI_Comm_rank() to determine the interval it 
should integrate over. For example if there are 4 processes, each process will 
know that there are 4 processes from MPI_Comm_size() and therefore that they 
will have to integrate 1/4 of the total range. I have set up the program such that
each process will use MPI_Comm_rank() in order to determine which partial interval
to integrate over. Rank 0 will integrate over the first interval, rank 1 over the
second interval, rank 2 over the third interval...etc.

Rank 0 will receive the partial sums from all other processes, display the total
sum and also the time of completion.

. . . . . . . . . . . . . . . . COMPILATION. . . . . . . . . . . . . . . . .

0. There is only one source file for this program: integral.cpp

1. The setup for OpenMPI depends on how your machines are set up. This setup will
cover a cluster that doesn't utilize NFS. If this is the case, ensure that all
machines have a correctly set up OpenMPI environment. 

- Ensure the LD_LIBRARY_PATH is corretly set for non-interactive login.

- Ensure the source file exists on all machines in the same exact path. You may
distribute the file to your other machines using scp:

	user@host-$ scp integral.cpp user@remotehost:/home/user/path

	*where path is where you would like to place integral.cpp

- Ensure passwordless ssh is set up.

- Ensure appropriate iptables rules are set up. You must allow INPUT and OUTPUT
connections betweem communication paths (e.g. if you allow all OUTPUT for each
machine, but filter INPUT, you must create rules to accept connections from 
host2 on host1 and to accept communication from host 1 on host2.



2. Use OpenMPI to compile the program on all your machines, again, ensure the 
executable is located in the same path for all machines.

	user@host-$ mpic++ integral.cpp -o integral



3. Set up the machinefile on the machine you will execute the program from (e.g. 
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


. . . . . . . . . . . . . . . . EXECUTION. . . . . . . . . . . . . . . . .

3. Execute the program using OpenMPI:

	user@host-$ mpirun -machinefile machines -n 2 integral

In the above command, we executed integral using the machines designated in
our machinefile, and using 2 processes (specified with -n flag).


. . . . . . . . . . . . . . . . RESULTS. . . . . . . . . . . . . . . . .

The following are outputs for different values of n = #processes as set
with the "-n" flag.

user@HOST0-$ mpirun -machinefile machines -n 1 integral
process[0] on cpu[HOST0] range 0
process[0] on cpu[HOST0] calculated area: 3.14159.
process[0] on cpu[HOST0] Total area: 3.14159 in 0.00436807seconds.

user@HOST0-$ mpirun -machinefile machines -n 2 integral
process[0] on cpu[HOST0] range 0
process[1] on cpu[HOST0] range 0.5
process[0] on cpu[HOST0] calculated area: 1.85459.
process[0] on cpu[HOST0] Total area: 3.14159 in 0.00234509seconds.
process[1] on cpu[HOST0] calculated area: 1.287.

user@HOST0-$ mpirun -machinefile machines -n 3 integral
process[0] on cpu[HOST0] range 0
process[1] on cpu[HOST0] range 0.333333
process[2] on cpu[HOST0] range 0.666667
process[0] on cpu[HOST0] calculated area: 1.287.
process[1] on cpu[HOST0] calculated area: 1.06501.
process[2] on cpu[HOST0] calculated area: 0.789582.
process[0] on cpu[HOST0] Total area: 3.14159 in 0.00159097seconds.

user@HOST0-$ mpirun -machinefile machines -n 4 integral
process[0] on cpu[HOST0] range 0
process[1] on cpu[HOST0] range 0.25
process[2] on cpu[HOST0] range 0.5
process[3] on cpu[HOST0] range 0.75
process[1] on cpu[HOST0] calculated area: 0.874676.
process[2] on cpu[HOST0] calculated area: 0.719414.
process[3] on cpu[HOST0] calculated area: 0.567588.
process[0] on cpu[HOST0] calculated area: 0.979915.
process[0] on cpu[HOST0] Total area: 3.14159 in 0.001266seconds.

All the above data sets show the utilization of a single 4 core processor.
The following data sets start to involve other machines.

user@HOST0-$ mpirun -machinefile machines -n 5 integral
process[1] on cpu[HOST0] range 0.2
process[2] on cpu[HOST0] range 0.4
process[3] on cpu[HOST0] range 0.6
process[0] on cpu[HOST0] range 0
process[2] on cpu[HOST0] calculated area: 0.639652.
process[3] on cpu[HOST0] calculated area: 0.537286.
process[0] on cpu[HOST0] calculated area: 0.789582.
process[1] on cpu[HOST0] calculated area: 0.732443.
process[0] on cpu[HOST0] Total area: 3.14159 in 0.015425seconds.
process[4] on cpu[HOST1] range 0.8
process[4] on cpu[HOST1] calculated area: 0.442629.

user@HOST0-$ mpirun -machinefile machines integral
process[0] on cpu[HOST0] range 0
process[1] on cpu[HOST0] range 0.0833333
process[2] on cpu[HOST0] range 0.166667
process[3] on cpu[HOST0] range 0.25
process[1] on cpu[HOST0] calculated area: 0.328028.
process[2] on cpu[HOST0] calculated area: 0.319319.
process[0] on cpu[HOST0] calculated area: 0.332564.
process[3] on cpu[HOST0] calculated area: 0.307086.
process[4] on cpu[HOST1] range 0.333333
process[5] on cpu[HOST1] range 0.416667
process[6] on cpu[HOST1] range 0.5
process[7] on cpu[HOST1] range 0.583333
process[4] on cpu[HOST1] calculated area: 0.292161.
process[5] on cpu[HOST1] calculated area: 0.275425.
process[7] on cpu[HOST1] calculated area: 0.239712.
process[6] on cpu[HOST1] calculated area: 0.257706.
process[0] on cpu[HOST0] Total area: 3.14158 in 0.051528seconds.
process[9] on cpu[HOST2] range 0.75
process[10] on cpu[HOST2] range 0.833333
process[11] on cpu[HOST2] range 0.916667
process[8] on cpu[HOST2] range 0.666667
process[10] on cpu[HOST2] calculated area: 0.188835.
process[11] on cpu[HOST2] calculated area: 0.173803.
process[9] on cpu[HOST2] calculated area: 0.204948.
process[8] on cpu[HOST2] calculated area: 0.221993.


