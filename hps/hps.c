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

int main(int argc, const char **argv) 
{
    long unsigned int *digits;
    long unsigned int remainder, div, mod;
    int i, digit; 
    double elapsed_time; 			/* execution time */
    elapsed_time = -MPI_Wtime();

    //Sequential Method
    if (argc != 3)
    {
	printf("usage: hps n d\n");
	printf("    n: value of N (Example: 5 => 1/1 + 1/2 + 1/3 + 1/4 + 1/5)\n");
	printf("    d: numbers of Digits (Example: 5 => 0,xxxxx) \n");
	exit(-1);
    }

    int n = atoi((char*)argv[1]);	// value of N
    int d = atoi((char*)argv[2]);	// numbers of digits

    digits = (long unsigned int *)malloc(sizeof(long unsigned int) * (d+11));
    if (!digits)
    {
	printf("memory allocation failed\n");
	exit(-1);
    }

    // set digits to 0
    for (digit = 0; digit < d + 11; ++digit)
        digits[digit] = 0;

    for (i = 1; i <= n; ++i) 
    {
        remainder = 1;
        for (digit = 0; digit < d + 11 && remainder; ++digit) 	{
            div = remainder / i;
            mod = remainder % i;
            digits[digit] += div;
            remainder = mod * 10;
        }
    }

    // step r1: init regroupng, from d+11-1 to 1
    for (i = d + 11 - 1; i > 0; --i) 
    {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    // step r2: skip all the digiths after d
    if (digits[d + 1] >= 5) 
    {
        ++digits[d];
    }

    // step r3: regrouping again because of r2
    for (i = d; i > 0; --i) 
    {
        digits[i - 1] += digits[i] / 10;
        digits[i] %= 10;
    }

    // print sum
    printf("%ld.", digits[0]);

    for (i = 1; i <= d; ++i) 
    {
	printf("%ld",digits[i]);
    }
    elapsed_time += MPI_Wtime();
    printf(" in  %10.6f sec.\n", elapsed_time);

    free(digits);

    return 0;
}
