***************************************************************************************************************
**************************************************************************************************** * *** * * 
**										******  *** *  **** *
**	DASON ADAMOS								*******  * *    *
**	CSC 718			CONSPRIME - README				* ***  * *    
**	19OCT2014								* *  ***   *   
**										****
*******************************************************************************************  * 
******************************************************************************************

. . . . . . . . . . . . . . . . INTRODUCTION. . . . . . . . . . . . . . . . 

The program included in consprime.cpp creates a list of consecutive odd primes.
For example, 1, 2, 3, and 5 are all odd numbers, but the only consecutive odd
primes in this set are 3 and 5. Likewise, 5 and 7 are also consecutive odd 
prime numbers.

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

			
. . . . . . . . . . . . . . . . COMPILATION. . . . . . . . . . . . . . . . .

1. Extract consprime.cpp into an appropriate directory, cd into that directory
and execute the following command:

	user@host-$ g++ consprime.cpp -o consprime

2. This command generates the executable 'consprime'.

. . . . . . . . . . . . . . . . EXECUTION. . . . . . . . . . . . . . . . .

1. In order to execute consprime, type './consprime' at the bash prompt.

	user@hos-$ ./consprime
	
2. The console displays the following message:

	consprime - finds all consecutive odd prime numbers up to a given number, n.
	Results are also written to 'out.txt'.
	Enter in a number, n (or 'exit' to quit)= 

3. From this point you can type in any number. The following is displayed when
the number '9' is entered.

	Enter in a number, n (or 'exit' to quit)=9
	3,5
	5,7
	[2] Consecutive odd primes.
	Enter in a number, n (or 'exit' to quit)=

4. As can be seen, the program will list all consecutive odd primes to the console.

. . . . . . . . . . . . . . . . RESULTS. . . . . . . . . . . . . . . . .

1. The output lists all consecutive pairs of odd primes. If there are 3 consecutive
odd prime numbers in a row, the program will list two entries for this case. For
example, EXECUTION(4) showed that when requesting the list of consecutive primes up
to 9, the program lists: 

	3,5
	5,7

Because 3, 5, and 7 are all consecutive odd primes.

2. The last line in the console displays the total number of consecutive odd prime
numbers between brackets "[]":

	[2] Consecutive odd primes.

3. Also, consprime outputs the list of consecutive odd primes to "out.txt" which you
will find in the directory in which the program is located.

	user@host-$ ls
	consprime  consprime.cpp  out.txt
	user@host-$ vim out.txt

	. . . . . in vim . . . . . . .
	3,5
	5,7
	~
	~
	~
	...

. . . . . . . . . . . . . . MAIN RESULTS. . . . . . . . . . . . . . . . .

1. For a problem size of 1,000,000, there are 8169 consecutive odd primes. As shown
in the following console output:

	...
	998687,998689
	999329,999331
	999431,999433
	999611,999613
	999959,999961
	[8169] Consecutive odd primes.

The 'out.txt' file for n=1,000,000 has been saved as 'out_1000000.txt'.

