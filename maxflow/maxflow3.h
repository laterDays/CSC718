#ifndef _MAXFLOW3_H_
#define _MAXFLOW3_H_

#include <climits>
#include <list>
#include <vector>
#include "graph.h"

class maxflow3
{
	private:
		graph<node<int>,int>* G;
		int src;
		int sink;
		void make_graph (std::list<std::vector<int>*>* edges);	// Construct the graph from the list of vectors
	public:
		maxflow3(std::list<std::vector<int>*>* edges);			// constructs the graph for maxflow3
		~maxflow3();
		void run (std::string oFile);											// runs the maxflow algorithm from DinicsImproved.h

};

maxflow3::maxflow3(std::list<std::vector<int>*>* edges)
{
	src = 0;
	sink = 513;
	G = new graph<node<int>,int>();
	make_graph (edges);
}
maxflow3::~maxflow3(){
	delete G;
}
void maxflow3::make_graph (std::list<std::vector<int>*>* edges)
{
	int p, n;
	// Strip the data from the list of vectors
	// It should look like this
	// list:
	//	{0 , 1}	vector
	//	{3 , 4}
	//	... etc
	// Found out the min is 1 and the max is 256
	G->setSource(src);
	G->setSink(sink);
	for (std::list<std::vector<int>*>::iterator e=edges->begin(); e != edges->end(); e++)
	{
		p = (*e)->at(0);		// 1 to 256
		n = (*e)->at(1) + 256;	// 257 to 512

		G->addEdge(p, n, INT_MAX);	// All edges added as max value (infinite) as
	}
	for (int i = 1; i < 257; i++)
	{
		G->addEdge(src,i,1);
		G->addEdge(i+256,sink,1);
	}
}

void maxflow3::run (std::string oFile)
{
	DinicsImproved::run (G,sink,oFile);
}

#endif
