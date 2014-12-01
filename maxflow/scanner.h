#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <fstream>
#include <iostream>
#include <list>
#include <vector>
/*
 *
 * 	scanner - used to read lines from "sources.txt" and turn them into
 *		a list of vectors  (vector element 0 is the first node, element 1 is the second).
 *
 */
class scanner
{
	private:
		std::fstream ifile;
		std::vector<int>* str_to_vector (std::string line);
	public:
		scanner (const char *inFile);				// initialize with input file
		std::list<std::vector<int>*>* scan ();		// scan the file, return the list of edges (in vector form)

};

scanner::scanner (const char *inFile)
{
	ifile.open(inFile);
	if(!ifile.is_open()) std::cout << "File did not open.\n";
}

std::list<std::vector<int>*>* scanner::scan ()
{
	std::list<std::vector<int>*>* edges = new std::list<std::vector<int>*>();	// this will hold the data
	std::string line;

	if (ifile.is_open())
	{
		std::cout << "Beginning scan.\n";
		while (!ifile.eof())
		{
			std::getline(ifile,line);
			if (line.compare("") == 0) break;
			edges->push_back(str_to_vector (line));			// add the vector to the end of the list
		}
		std::cout << "End scan.\n";
		return edges;
	}
	else
	{
		std::cout << "Cannot scan. File not open.\n";
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////
std::vector<int>* scanner::str_to_vector (std::string line)
{
	std::vector<int>* v = new std::vector<int>();
	std::string a = "";
	int n;

	for ( std::string::iterator it=line.begin(); it <= line.end(); ++it)
	{
		if(*it != ',' && *it != '\0')
		{
			a += *it;						// append numbers onto a
		}									// until a , or null is reached
		else
		{
			n = atoi(a.c_str());			// turn 'a' into a number
			a = "";
			v->push_back(n);				// and add it to the vector
		}
	}
	return v;
}

#endif
