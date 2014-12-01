#ifndef _MAKE_LVL_GRAPH_H_
#define _MAKE_LVL_GRAPH_H_

#include "edge.h"
#include "node_fun.h"
#include "graph.h"

/*
 *
 * 	make_lvl_graph - generates a level graph. This object must be passed to
 * 		a graphs traverse() method. Afterwards, getGraph() can be used to
 * 		retrieve the graph that was generated.
 *
 * 		E.g.
 *
 * 		graph<node<int>,int>* G;
 * 		graph<node<int>,int>* Gl;
 * 		...
 *		G->traverse_bf(makelvlGraph);
 *		Gl = makelvlGraph->getGraph();
 *		...
 */
template <class N, typename T>
class make_lvl_graph : public node_fun<N,T>
{
	private:
		graph<N,T>* Gl;							// Gl - the level graph (under construction)

	public:
		make_lvl_graph(int sink);
		~make_lvl_graph();

		void operator() (edge<N,T>* e);			// operator() - will add the edge to Gl if the
		void on_branch (edge<N,T>* n);			//		node level is 1+ the parent level
		void on_join (edge<N,T>* n);			// on_branch, on_join, on_backout - NOT USED
        void on_backout (edge<N,T>* n);

        graph<N,T>* getGraph ();				// getGraph - retrieve the graph that was generated
};
template <class N, typename T>
make_lvl_graph<N,T>::make_lvl_graph (int sink)
{
	Gl = new graph<N,T>();
	Gl->setSink(sink);
	//level = 0;
}
template <class N, typename T>
make_lvl_graph<N,T>::~make_lvl_graph ()
{

}

template <class N, typename T>
void make_lvl_graph<N,T>::operator() (edge<N,T>* e)
{
	int n_val = e->getn()->getValue();							//	n_val = int name of node
	int p_val = (e->getp() == 0 ? 0 : e->getp()->getValue());	// 	p_val = int name of parent

    if (e->getp() == 0)											//	if there is no parent
    {
        Gl->setSource(n_val);									//		it is the source
        Gl->setLevel(n_val,0);									//		set its level to 0
    }
    else
    {
    	int n_lvl = Gl->getLevel(n_val);						// n_lvl - node level
		int p_lvl = Gl->getLevel(p_val);						// p_lvl - parent level

		if (n_lvl == -1 || n_lvl == (p_lvl + 1))				// if the node doesn't have a level (-1) or if n_lvl = p_lvl + 1
		{
	    	Gl->addEdge(p_val, n_val, e->getLabel());			//		add it to the level graph
	    	Gl->setLevel(n_val, p_lvl + 1);						//		set the level of n_val = p_lvl + 1
		}
    }
}


template <class N, typename T>
void make_lvl_graph<N,T>::on_branch (edge<N,T>* n)
{

}
template <class N, typename T>
void make_lvl_graph<N,T>::on_join (edge<N,T>* n)
{

}
template <class N, typename T>
void make_lvl_graph<N,T>::on_backout (edge<N,T>* n)
{

}
template <class N, typename T>
graph<N,T>* make_lvl_graph<N,T>::getGraph ()
{
	graph<N,T>* tmp = Gl;
	Gl = new graph<N,T>();
	Gl->setSink(tmp->getSink());
	return tmp;
}
#endif
