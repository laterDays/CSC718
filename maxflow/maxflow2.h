#ifndef _MAXFLOW2_H_
#define _MAXFLOW2_H_

#include "DinicsImproved.h"

class maxflow2
{
	public:
		static void run (int E[][3], int edge_count, int sink, std::string oFile);	// run the maxflow algorithm on
};																					// the supplied data

void maxflow2::run (int E[][3], int edge_count, int sink, std::string oFile)
{
    graph<node<int>,int>* G1 = new graph<node<int>,int> (E, edge_count, sink);	// create a graph
    DinicsImproved::run (G1, sink, oFile);														// run the maxflow algorithm
}
#endif
