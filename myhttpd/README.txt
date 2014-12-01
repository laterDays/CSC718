***************************************************************************************************************
**************************************************************************************************** * *** * * 
**										******  *** *  **** *
**	DASON ADAMOS								*******  * *    *
**	CSC 718			MYHTTPD - README				* ***  * *    
**	17OCT2014								* *  ***   *   
**										****
*******************************************************************************************  * 
******************************************************************************************



. . . . . . . . . . . . . . . . INTRODUCTION. . . . . . . . . . . . . . . . 

The files included in this tar file includes:
	
myhttpd1 - corrected simple web server which now includes function call to 
	   httpHandler ().
	   Console output: informs users of any connections.

myhttpd2 - multiple thread version of the simple web server (myhttpd1 + threads)
	   Console output: informs users of connections on each thread.

myhttpd3 - daemon version of the multiple thread simple web server (myhttpd2 + daemonize).
	   System log output: activity logged under process "myhttpd".


The above files are additions to a simple web server program distributed as
myhttpd.cpp. The program serves http requests on port 8080 (default) and
as mentioned in the above, provides the server user with information 
regarding the web service (e.g. connection attempts, errors, etc.). Output
is communicated via console or syslog.

. . . . . . . . . . . . . . . . COMPILATION. . . . . . . . . . . . . . . . 

Contents of maxflow.tar.gz file:

	README.txt
	myhttpd1.cpp
	myhttpd2.cpp
	myhttpd3.cpp

0. Extract all files into an appropriately named folder (e.g. myhttpd). cd into
this directory. Compilation for each file is done with the following commands
from the bash prompt:

user@host-$ g++ myhttpd1.cpp -o myhttpd1
user@host-$ g++ myhttpd2.cpp -o myhttpd2 -lpthread
user@host-$ g++ myhttpd3.cpp -o myhttpd3 -lpthread

* The compilation flag -lpthread is included because the above files use posix
threads.	

. . . . . . . . . . . . . . . . EXECUTION. . . . . . . . . . . . . . . . .

1.a. Executing myhttpd1. Type "./myhttpd1" into the bash prompt. The following text
will be displayed.

user@host-$ ./myhttpd1
myhttpd server listening on port 8080
waiting for a connection

1.b. You may view the hosted web page by typing "localhost:8080" into the address 
bar of any web browser. Your web browser should open a page with a single line,
"Welcome to my first page!". The console should display text similar to the following:

Received connection from 127.0.0.1
Received bytes 292
Received string "GET / HTTP/1.1
Host: localhost:8080
...
Connection: keep-alive

"
Sent bytes 137

2.a. Executing myhttpd2. Type "./myhttpd2" into the bash prompt. The console 
displays the same text as before:

user@host-$ ./myhttpd2
myhttpd server listening on port 8080
waiting for a connection

2.b. However, upon connection, the console will dislay information different from
before. The following is an example of two nearly simultaneous http requests:

Received connection from 127.0.0.1
Launched thread 140378024019712.
waiting for a connection
[4] Begin.
[4] Sent bytes 137
Received connection from 127.0.0.1
Launched thread 140378015627008.
waiting for a connection
[5] Begin.
[5] Sent bytes 137
[4] End.
[5] End.

Notes:
- The originating thread displays the thread identifier of the thread it created
- The launched thread displays the descriptor of the socket it is using between
the "[ ]" braces.
- A call to sleep() was inserted to make it easier to display the overlapping
execution of threads. In the above example, we can see that the thread 1, which 
utilizes socket (4), finishes in in the midst of thread 2's execution.

3.a. Execution of myhttpd3. Type "./myhttpd3" into the bash prompt. Nothing will
appear on the console.

3.b. The web server writes to your syslog. In my case, the log is located in 
/var/log/syslog. You can view the syslog entries with root/sudoer permissions:

user@host-$ sudo tail -30 /var/log/syslog

...
Oct 17 20:44:22 HOST myhttpd[4052]: daemon attempting to begin.
Oct 17 20:44:22 HOST myhttpd[4052]: myhttpd server listening on port 8080
Oct 17 20:44:22 HOST myhttpd[4052]: waiting for a connection
Oct 17 20:44:58 HOST myhttpd[4052]: Received connection from 127.0.0.1
Oct 17 20:44:58 HOST myhttpd[4052]: waiting for a connection
Oct 17 20:44:58 HOST myhttpd[4052]: [2] Received bytes 318
Oct 17 20:44:58 HOST myhttpd[4052]: [2] Sent bytes 137
Oct 17 20:44:59 HOST myhttpd[4052]: Received connection from 127.0.0.1
Oct 17 20:44:59 HOST myhttpd[4052]: waiting for a connection
Oct 17 20:44:59 HOST myhttpd[4052]: [3] Received bytes 318
Oct 17 20:44:59 HOST myhttpd[4052]: [3] Sent byte

The syslog displays the same information we saw in myhttpd2. We can also search
the syslog for entries using the following command:

user@host-$ sudo grep myhttpd /var/log/syslog

3.c. Stopping the daemon.

user@host-$ ps -A | grep myhttp
5123 ?		00:00:00 myhttpd3

The above command reveals that the daemon is indeed running. In order to kill this
daemon on an Ubuntu system or any system that contains the "start-stop-daemon"
command:

user@host-$ start-stop-daemon --stop --name myhttpd3 --retry 1

For some reason, the daemon does not exit without the "--retry" flag.




