#ifndef _EDGE_H_
#define _EDGE_H_

#include <sstream>
#include <string>
#include <typeinfo>

template <class N, typename T> class edge
{
private:
	N* n;											// n - node
	N* p;											// p - parent node
	T label;										// label - can be used for noting capacity
public:
	edge (N* parent, N* node, T label_);
	~edge ();

	N* getp();										// getp - get parent
	N* getn();										// getn - get child

	void setLabel (T label_);
	T getLabel ();

	std::string toString ();
};

template <class N, typename T>
edge<N,T>::edge (N* parent, N* node, T label_)
{
	p = parent;
	n = node;
	label = label_;
}
template <class N, typename T>
edge<N,T>::~edge ()
{
	if (p != 0) p->deleteChildren();				// on destruction, delete
	if (n != 0) n->deleteChildren();				// only the edges (skip nodes)
}													// this does introduce a memory leak, but
template <class N, typename T>						// prevents the deletion of graphs from
N* edge<N,T>::getp ()								// deleting its contents
{
	return p;
}
template <class N, typename T>
N* edge<N,T>::getn ()
{
	return n;
}
template <class N, typename T>
void edge<N,T>::setLabel (T label_)
{
	label = label_;
}
template <class N, typename T>
T edge<N,T>::getLabel ()
{
	return label;
}
template <class N, typename T>
std::string edge<N,T>::toString ()
{
	std::ostringstream c;
	std::string p_str = (p == 0 ? "0" : p->toString());	// if there is no parent, its string is "0"
	std::string n_str = n->toString();
	if (typeid(label) == typeid(int));
	{
		c << label;
	}
	return p_str + "-" + c.str() + "->" + n_str + "";	// e.g. 0-3->2 means node 0 has edge to 2 of label 3
}

#endif
