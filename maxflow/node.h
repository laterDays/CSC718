#ifndef _NODE_H_
#define _NODE_H_

#include <unordered_map>
#include <sstream>
#include "edge.h"
#include "list.h"
template <typename T> class node
{
    private:
        T value;
        std::unordered_map<T,edge<node<T>,T>*> edge_directory;	// edge_directory - listing of all edges in children
        list<edge<node<T>,T> >* children;						// children -
        int level;												// level - used in level graph generation
    public:
        node (T val);
        ~node();

        T getValue ();

        void addChild (node<T>* n, int label);					// addChild - creates an edge (parent->child), adds to children
        void removeChild (node<T>* n);							// removeChild - removes the edge (parent->n)
        void removeEdge (edge<node<T>,T>* e);					// removeEdge - method that removes child by way of its edge
        edge<node<T>,T>* getEdge (int n);
        list<edge<node<T>,T> >* getChildren ();					// getChildren - returns the edge list

        void setLevel (int l);									// setLevel - used to set node levels when generating
        int getLevel ();										//	level graphs

        std::string toString();
        void deleteChildren();									// deleteChildren - used in the chain of graph destruction
};																// in order to bypass the destruction of the nodes

template <typename T>
node<T>::node (T val)
{
    value = val;
    children = new list<edge<node<T>,T> >();
    level = -1;
}
template <typename T>
node<T>::~node ()
{
    delete children;
}
template <typename T>
void node<T>::deleteChildren ()
{
	if (children != 0) delete children;
}

template <typename T>
T node<T>::getValue ()
{
	return value;
}

template <typename T>
void node<T>::addChild (node<T>* n, int label)
{
	edge<node<T>,T>* e = new edge<node<T>,T> (this, n, label);	// create edge
    children->add(e);											// add it to list of children
    edge_directory.insert({{n->getValue(), e}});				// add edge to the edge_directory
}
template <typename T>
void node<T>::removeChild (node<T>* n)
{
	edge<node<T>,T> e = edge_directory.at(n);					// use edge_directory to find edge
    edge_directory.erase(n->getValue());						// erase it from edge_directory
    children->remove(e);										// remove it from children
}
template <typename T>
void node<T>::removeEdge (edge<node<T>,T>* e)
{
    edge_directory.erase(e->getn()->getValue());
    children->remove(e);
}
template <typename T>
edge<node<T>,T>* node<T>::getEdge (int n)
{
	try
	{
		return edge_directory.at(n);
	}
    catch (std::exception& e)
    {
    	return 0;												// return 0 if the edge doesn't exist
    }
}
template <typename T>
list<edge<node<T>,T> >* node<T>::getChildren ()
{
    return children;
}

template <typename T>
void node<T>::setLevel (int l)
{
    level = l;
}
template <typename T>
int node<T>::getLevel ()
{
    return level;
}


template <typename T>
std::string node<T>::toString ()
{
    std::string result;
    std::ostringstream c;
    c << value;
    return c.str();
}

#endif
