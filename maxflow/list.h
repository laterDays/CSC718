#ifndef _LIST_H_
#define _LIST_H_

#include <iostream>
#include <string>
#include <unordered_map>
/*
 *
 *	template <typename T> class list
 *		- this is a doubly-linked list composed of list_nodes which
 *		contain the item.
 *
 *				head							  tail
 *		 		|								  |
 *		 		list_node		 list_node		  list_node
 *			 	[value ]--next-->[value ]--next-->[value ]--next--> 0
 *	0 <-prev----[	   ]<-prev---[	    ]<-prev---[	     ]
 *
 */
template <typename T> class list
{
	private:
		/////////////////////////////////////////////////////////////////////
		//
	    // list_node<U>
		//
		template <class U> class list_node
		{
			private:
				U* value;							// value - the contained object
				list_node<U>* next;
				list_node<U>* previous;
			public:
				list_node(U* val);
				~list_node();

				U* getValue ();

				void setNext (list_node<T>* n);		// setNext - used to construct the list
				list_node<U>* getNext ();

				void setPrev (list_node<U>* p);		// setPrev - used to construct the list
				list_node<U>* getPrev ();

				std::string toString ();
		};
		//
		/////////////////////////////////////////////////////////////////////

        list_node<T>* head;							// head - front of the list
        list_node<T>* tail;							// tail - end of the list
        int N;										// N - size of the list
        list_node<T>* it;							// it - iterator pointer
        list_node<T>* it_prev;						// it_prev - previous iterated item

        std::unordered_map<T*,list_node<T>*> node_directory;	// listing of all list_nodes
        														// indexed by the item it holds
    public:
        list ();
        ~list();
        void add (T* n);							// add - item to this list
        void remove (T* n);							// remove - item from this list

        // Iterations ////
        void begin ();								// begin - call to reset the iterator
        bool hasNext ();							// hasNext - returns true if there is a next item
        T* next ();									// next - retrieved item, advance iterator

        //////////////////
        int  size ();								// size - returns # of items in the list
        std::string toString ();
};
template <class T>
template <class U>
list<T>::list_node<U>::list_node (U* val)
{
	value = val;
	previous = next = 0;
}
template <class T>
template <class U>
list<T>::list_node<U>::~list_node ()
{
	next = 0;
}
template <class T>
template <class U>
U* list<T>::list_node<U>::getValue()
{
	return value;
}
template <class T>
template <class U>
void list<T>::list_node<U>::setNext (list_node<T>* n)
{
	next = n;
}
template <class T>
template <class U>
list<T>::list_node<U>* list<T>::list_node<U>::getNext ()
{
	return next;
}
template <class T>
template <class U>
void list<T>::list_node<U>::setPrev (list_node<U>* p)
{
	previous = p;
}
template <class T>
template <class U>
list<T>::list_node<U>* list<T>::list_node<U>::getPrev ()
{
	return previous;
}
template <class T>
template <class U>
std::string list<T>::list_node<U>::toString ()
{
	return value->toString();
}

template <typename T>
list<T>::list ()
{
    head = tail = it_prev = it = 0;
    N = 0;
}
template <typename T>
list<T>::~list ()
{
	list_node<T>* ptr;
	list_node<T>* mark;

	ptr = head;					// iterates accross the list
	while (ptr != 0)			// to delete the list_nodes
	{
		mark = ptr;
		ptr = ptr->getNext();
		delete mark;
	}
}
template <typename T>
void list<T>::add (T* n)
{
    list_node<T>* node = new list_node<T>(n);
    if (head == 0)				// if there is nothing in the list
    {
        head = tail = node;		// set head and tail to the new node
    }
    else
    {
        tail->setNext(node);	// if the node isn't at the front,
        node->setPrev(tail);	// add it to the back, set next,
        tail = node;			// previous, and tail accordingly
    }
    node_directory.insert({{n,node}});
    N++;
}
template <typename T>
void list<T>::remove (T* n)
{
	list_node<T>* ln = node_directory.at(n);

	if (ln == head)				// if the node is the head
	{
		if (it == ln)
		{
			it = it->getNext();		// adjust iterator pointer as necessary
		}
		head = head->getNext();		// the new head is whatever is next
		if(head != 0)
        {
            head->setPrev(0);
        }
        ln->setNext(0);
	}
	else if (ln == tail)		// if the node is the tail
	{
		if (it == ln)
		{
			it = 0;					// adjust iterator pointer as necessary
		}
		tail = tail->getPrev();		// move tail pointer to the previous
		tail->setNext(0);			// node
		ln->setPrev(0);
	}
	else
	{							// if the node is in the middle
		if (it == ln)
		{
			it = it->getNext();		// adjust iterator pointer
		}
		list_node<T>* prv = ln->getPrev();
		list_node<T>* nxt = ln->getNext();
		prv->setNext(nxt);			// arrange pointers to skip over
		nxt->setPrev(prv);			// this node
		ln->setNext(0);
		ln->setPrev(0);
	}
	N--;
}
template <typename T>
std::string list<T>::toString ()
{
    std::string result;
    list_node<T>* ptr = head;
    while (ptr != 0)
    {
        result += ptr->toString() + ",";
        ptr = ptr->getNext();
    }
    return result;
}
template <typename T>
void list<T>::begin()
{
    it = head;
}
template <typename T>
bool list<T>::hasNext()
{
    return it != 0;
}
template <typename T>
int list<T>::size()
{
    return N;
}
template <typename T>
T* list<T>::next()
{
	it_prev = it;
	it = it->getNext ();
    return it_prev->getValue();
}

#endif
