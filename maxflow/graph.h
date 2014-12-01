#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <climits>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "edge.h"
#include "list.h"
#include "node.h"
#include "node_fun.h"
/*
 *
 * 	graph -
 *
 * 	***Important*** - this is currently configured so that source nodes must be labeled '0'!
 *
 *
 */
template <class N, typename T>
class graph
{
    private:
        std::unordered_map<int,N*> node_directory;								// node_directory - maps the int name of the node to its object
        std::unordered_set<N*> children_seen;									// children_seen - used to prevent circular traversals

        edge<N,T>* s;															// s - source edge
        N* t;																	// t - sink node

        N* add_unique_node (int label);											// add_unique_node - creates nodes that don't exist in this graph

        void traverse (node_fun<N,T>* f, edge<N,T>* e);							// traverses the graph (depth first) - calling node_fun parameters on edge e
        																		// 		called by public traverse ().
        void delete_children(edge<N,T>* chd);

        /////////////////////////////////////////////////////////////////////
        //
        // Dinics Functions
        //
        void update_path (graph<N,T>* G, list<edge<N,T>>* path, int min);		// update_path - removes/decreases flow on the given paths from graph G
        																		//
        void mk_residual_path (graph<N,T>* G, list<edge<N,T>>* path, int min);	// mk_residual_path - creates residual edges on graph G, using the
        																		//		passed path and min
        int advance (edge<N,T>* e, list<edge<N,T>>* path);						// advance - recursive partner of public advance. This traverses
        																		//		the graph and decides whether to add an edge to the list, path.
        //
        /////////////////////////////////////////////////////////////////////

    public:
        // Construct / Deconstruct
        graph ();
        graph (int E[][3], int count, T sink);
        ~graph ();									// Does not destroy nodes

        /////////////////////////////////////////////////////////////////////
		// Construction methods
		void addEdge (T p, T n, T label);
		edge<N,T>* getEdge (T p, T n);
		void removeEdge (T p, T n);
		void setSource (T src);
		void setSink (T sink);
		int getSink ();


		/////////////////////////////////////////////////////////////////////
		// Level graph generation methods
        void setLevel (int n, int lvl);
        int getLevel (int n);


        /////////////////////////////////////////////////////////////////////
        // General functions
        void traverse (node_fun<N,T>* f);			// traverse - depth first, applying node_fun<N,T>* to all edges
        void traverse_bf (node_fun<N,T>* f);		// traverse - breadth first, applying node_fun<N,T>*


        /////////////////////////////////////////////////////////////////////
        // Dinics Functions
        void advance ();							// advance - advances through the graph (and backs out) to find paths
        int rmv_aug_paths (graph<N,T>* G);			// rmv_aug_paths - removes augmenting paths from graph G


        /////////////////////////////////////////////////////////////////////
        std::string toString();
        bool within (graph<N,T>* G);				//  within - used to test if this graph is contained within G
};													// 		i.e. all nodes and edges in this graph are also within G

template <class N, typename T>
graph<N,T>::graph ()
{
	s = 0;
	t = 0;
}
template <class N, typename T>
graph<N,T>::graph (int E[][3], int count, T sink)
{
    for (int e=0; e < count; e++)
    {
        addEdge(E[e][0], E[e][1], E[e][2]);			// call addEdge() for all rows in the passed 2D array
    }
    try
    {
        setSource(0);								// set s
        setSink(sink);								// set t
    }
    catch (std::exception& e)
    {
        std::cout << "Source or sink nodes not correct: " << e.what() << std::endl;
    }
}

template <class N, typename T>
void graph<N,T>::addEdge (T p, T c, T label)
{
	N* np = add_unique_node(p);						// create parent node if necessary
	N* nc = add_unique_node(c);						// create child node if necessary
	np->addChild(nc, label);						// add the child to the parent
}
template <class N, typename T>
edge<N,T>* graph<N,T>::getEdge (T p, T c)
{
	edge<N,T>* e;
	if (p == 0 && c == 0)
	{
		return s;									// return s (s is the edge 0->0)
	}
	else
	{
		e = (node_directory.at(p))->getEdge(c);		// otherwise look up the node and
		return e;									// ask it to return the correct edge
	}
	return 0;
}
template <class N, typename T>
void graph<N,T>::removeEdge (T p, T c)
{
	N* p_ = node_directory.at(p);					// look up the node
	p_->removeChild(node_directory.at(c));			// ask it to remove the correct child
}
template <class N, typename T>
void graph<N,T>::setSource (T src)
{
	try
	{
		s = new edge<N,T>(0, node_directory.at(src), INT_MAX);	// set source if it exists
	}
    catch (std::exception& e)
    {
    	s = new edge<N,T>(0, add_unique_node(src), INT_MAX);	// or make s and set it to source
    }
}
template <class N, typename T>
void graph<N,T>::setSink (T sink)
{
	try
	{
		t = node_directory.at(sink);				// set sink if it exits
	}
    catch (std::exception& e)
    {
    	t = add_unique_node(sink);					// or make it and set it to t
    }
}
template <class N, typename T>
int graph<N,T>::getSink ()
{
	return t->getValue();
}
template <class N, typename T>
graph<N,T>::~graph ()
{
	delete_children(s);								// this delete will not destroy the nodes themselves
}
template <class N, typename T>
void graph<N,T>::delete_children (edge<N,T>* e)
{
	list<edge<N,T>>* chd = e->getn()->getChildren();
	chd->begin();
	while (chd->hasNext())
	{
		delete_children(chd->next());
	}
	delete e;
}

template <class N, typename T>
void graph<N,T>::setLevel (int n, int lvl)
{
	(node_directory.at(n))->setLevel(lvl);
}
template <class N, typename T>
int graph<N,T>::getLevel (int n)
{
	int lvl = -1;
	try
	{
		lvl = (node_directory.at(n))->getLevel();	// get the level by referencing the
	}												//	node directory
	catch (std::exception& e)
	{

	}
	return lvl;
}

template <class N, typename T>
N* graph<N,T>::add_unique_node (int label)
{
    N* n;
    try
    {
        n = node_directory.at(label);				// return the node if it already exists
    }
    catch (std::exception& e)
    {
        n = new node<int>(label);					// if it doesn't exist, make it
        node_directory.insert({{label,n}});			// note it in the node_directory
    }
    if (n == 0)
    {
        std::cout << "Wait a sec...";
    }
    return n;
}

template <class N, typename T>
void graph<N,T>::traverse(node_fun<N,T>* f)
{
    traverse(f, s);
    children_seen.clear();
}
template <class N, typename T>
void graph<N,T>::traverse(node_fun<N,T>* f,edge<N,T>* e)
{
	N* n_val = e->getn();
	list<edge<N,T>>* chd;

    (*f)(e);													// call node_fun.operator() on edge e

    chd = n_val->getChildren();
    if (chd->size() == 0 || (children_seen.count(n_val) > 0))	// if there are no children or if these
    {															// children have been explored
        f->on_backout(e);										// call node_fun.on_backout() and return
        return;
    }
    children_seen.insert(n_val);								// note children_seen
    f->on_branch(e);
    chd->begin();
    while (chd->hasNext())
    {
        traverse(f, chd->next());								// recurse, traverse (f, children)
    }
    f->on_join(e);												// call node_fun.on_join()
    return;
}
template <class N, typename T>
void graph<N,T>::traverse_bf(node_fun<N,T>* f)
{
    list<edge<N,T>>* chd;
    std::queue<edge<N,T>*> eq;		// nq - node queue
    edge<N,T>* e = s;
    N* n;

    eq.push(e);                     // node, parent
    while (eq.size() > 0)
    {
        e = eq.front(); eq.pop();
        n = e->getn();

        (*f)(e);							// call the node_fun<N,T>* f

        if (children_seen.count(n) == 0)	// If the node (not the parent) wasn't seen yet
        {
            chd = n->getChildren();			//	children = n->children();
            children_seen.insert(n);		//	note that we've seen n's children
            chd->begin();					//	Iterate...
            while(chd->hasNext())
            {
                e = chd->next();
                eq.push(e);					// add the children to the end of the queue
            }								// so we can see them later
        }
    }
    children_seen.clear();
}
template <class N, typename T>
int graph<N,T>::advance (edge<N,T>* e, list<edge<N,T>>* path)
{
    int min;

    if (e->getn() == t)
    {
        path->add(e);
        return e->getLabel();
    }
	// goal: this add to the path, and readjust the min so,
	// get this edge's children
    list<edge<N,T>>* chd = e->getn()->getChildren();
	// if there are no children, it is a dead end -> back out;
    if (chd->size() == 0)
    {
        return -1;
    }
    else
    {
        // attempt an advance on all its children
        chd->begin();
        while(chd->hasNext())
        {
            min = advance(chd->next(), path);
            if (min != -1) break;
        }
        // If an advance does succeed
        // add this edge to the path and update min
        if (min != -1)
        {
            path->add(e);
            if (e->getLabel() < min)
            {
                min = e->getLabel();
            }
            return min;
        }
        else
        {
            return -1;
        }
    }

}
template <class N, typename T>
int graph<N,T>::rmv_aug_paths (graph<N,T>* G)
{
	int min = INT_MAX;
	int flow = 0;
    edge<N,T>* cur;
	list<edge<N,T>>* path = new list<edge<N,T>>();

	do
	{
        min = advance (s, path);						// advance through the graph
        												// 		save path to path
		if (min != -1)	// minn_n is a list_node, t is a node
		{
			flow += min;

            update_path (this, path, min);				// update the level graph with this path
			mk_residual_path (G, path, min);			// update the original graph to be a residual graph

			delete path;
			path = new list<edge<N,T>>();

			min = INT_MAX;
		}
	} while (cur->getn() == t);
    return flow;
}
template <class N, typename T>
void graph<N,T>::update_path (graph<N,T>* G, list<edge<N,T>>* path, int min)
{
	edge<N,T>* e = 0;
	edge<N,T>* e_other = 0;
    N* p;
	int cap;
    int p_;
    int n_;

	path->begin();
	while(path->hasNext())
	{
		e = path->next();									// get the next edge in the path
		p_ = e->getp() == 0 ? 0 : e->getp()->getValue();
		e_other = G->getEdge(p_, e->getn()->getValue());	// e_other - edge in graph G, not this
															//		which has same parent and child
		cap = e_other->getLabel();
		cap -= min;											// reduce the amount of flow through
		e_other->setLabel(cap);								// that edge.

		if (cap == 0 && e != s)								// if the capacity is 0
		{													// remove the edge
            p = e_other->getp();
			p->removeEdge(e_other);
		}
	}
}
template <class N, typename T>
void graph<N,T>::mk_residual_path (graph<N,T>* G, list<edge<N,T>>* path, int min)
{
	edge<N,T>* e = 0;
	edge<N,T>* e_other = 0;
	edge<N,T>* e_other_back = 0;
    N* p;
	int cap;
    std::string e_rmv;
    int p_;
    int n_;

	path->begin();
	while(path->hasNext())
	{
		e = path->next();									// get the edge in graph G, not this
		p_ = e->getp() == 0 ? 0 : e->getp()->getValue();	// with the same parent and child
		n_ = e->getn()->getValue();

		/////////////////////////////////////////////////////////////////////////
		// Decrease the forward path

		e_other = G->getEdge(p_, n_);						// decrease the capacity of that edge
		cap = e_other->getLabel();
  		cap -= min;
        e_other->setLabel(cap);

        // Remove if zero
        if (cap == 0 && e != s)								// and remove it if 0
		{
            p = e_other->getp();
            e_rmv = e->toString();
			p->removeEdge(e_other);
		}

		/////////////////////////////////////////////////////////////////////////
		// Increase the back path

		e_other_back = G->getEdge(n_, p_);					// increase the backwards edge
		if (e_other_back == 0)								// create one if there isn't one
		{
			// Add if zero
			cap = min;
			G->addEdge(n_,p_,cap);
			e_other_back = G->getEdge(n_,p_);
		}
		else
		{
			cap = e_other_back->getLabel();					// otherwise increase the
			cap += min;										// flow through it
			e_other_back->setLabel(cap);
		}
	}
}
template <class N, typename T>
std::string graph<N,T>::toString ()
{
	std::string result;

	for ( auto it = node_directory.begin(); it != node_directory.end(); ++it )
	{
		N* n = it->second;
		result += n->toString() + "{" + n->getChildren()->toString() + "}\n";
	}
	return result;
}
template <class N, typename T>
bool graph<N,T>::within(graph<N,T>* G)
{
	list<edge<N,T>>* chd;
	edge<N,T>* e;
	T p_val;
	T n_val;
	for ( auto it = node_directory.begin(); it != node_directory.end(); ++it )	// for every node in the node_directory
	{
		N* n = it->second;														// n = the node
		chd = n->getChildren();													// chd = the node's children

		chd->begin();															// iterate through the children...
		while(chd->hasNext())
		{
			e = chd->next();
			p_val = e->getp()->getValue();										// p_val = parent node number
			n_val = e->getn()->getValue();										// n_val = child node number
			if(G->getEdge(p_val,n_val) == 0)
			{
				return false;													// if the graph G doesn't have the node
			}																	// that exists in this graph, then
		}																		// this graph is not contained in graph G
	}
	return true;
}
#endif
