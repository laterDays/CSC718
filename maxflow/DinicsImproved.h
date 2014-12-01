#ifndef _DINICSIMPROVED_H_
#define _DINICSIMPROVED_H_
#include <iostream>
#include <fstream>
#include "graph.h"
#include "make_lvl_graph.h"
#include "scanner.h"
/*
 *
 *	DinicsImproved - the algorithm used to find the maxflow. The function,
 *		run () returns the residual graph created through the process and
 *		outputs:
 *			the original graph, G,
 *			the level graph, Gl
 *			and the total flow
 *
 *		The graph output is of the form
 *			0 {0-10->1,0-9->8,...}
 *			...
 *			where 0 is the parent node, the items in the {} are edges and
 *			each edge is read:
 *				0-10->1		edge 0 to 1, of label 10
 */
class DinicsImproved
{
	public:
		static graph<node<int>,int>* run (graph<node<int>,int>* G, int sink, std::string oFile);
};

graph<node<int>,int>* DinicsImproved::run (graph<node<int>,int>* G, int sink, std::string oFile)
{
	std::ofstream oFile_;
    int flow = 0;
    int total_flow = 0;
	graph<node<int>,int>* Gl;
    make_lvl_graph<node<int>,int>* makelvlGraph = new make_lvl_graph<node<int>,int>(sink);

    do{
    	G->traverse_bf(makelvlGraph);												// traverse the original graph to construct a level graph
		Gl = makelvlGraph->getGraph();												// retrieve the level graph

		flow = Gl->rmv_aug_paths(G);												// remove the augmenting paths and get flow
		total_flow += flow;															// augment flow
    } while (flow > 0);																// continue while the flow increases

	std::cout << "[AFTER(" << total_flow << ")] G: \n" << G->toString() << "\n";	// print graph G
	std::cout << "[AFTER(" << total_flow << ")] Gl: \n" << Gl->toString() << "\n";	// print graph Gl (level graph)
	std::cout << "[*] Flow = " << total_flow << std::endl;							// print total flow

    oFile_.open(oFile);
    if(oFile_.is_open())
    {
    	oFile_ << "[AFTER(" << total_flow << ")] G: \n" << G->toString() << "\n";	// print graph G
    	oFile_ << "[AFTER(" << total_flow << ")] Gl: \n" << Gl->toString() << "\n";	// print graph Gl (level graph)
    	oFile_ << "[*] Flow = " << total_flow << std::endl;
    	oFile_.close();
    }
	return G;
}
#endif
