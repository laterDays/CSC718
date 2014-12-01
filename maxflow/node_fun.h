#ifndef _NODE_FUN_H_
#define _NODE_FUN_H_
/*
 *
 *	node_fun - a function used for traversing the
 *	graph. Graph contains a traverse() method, which
 *	takes parameter of type node_fun<N,T>.
 *
 */
template <class N, typename T>
class node_fun
{
	public:
		virtual void operator() (edge<N,T>* n) = 0;		// operator() - main function to apply to edge
		virtual void on_branch (edge<N,T>* n) = 0;		// on_branch - called when graph branches to children
		virtual void on_join (edge<N,T>* n) = 0;		// on_join - called when graph traversal return from children
        virtual void on_backout (edge<N,T>* n) = 0;		// on_backout - called backing out from a node whose
};														//		children have already been traversed.
#endif
