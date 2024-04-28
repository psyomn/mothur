#ifndef CLUSTERCLASSIC_H
#define CLUSTERCLASSIC_H

#include "datastructures/counttable.h"
#include "datastructures/nameassignment.hpp"
#include "datastructures/listvector.hpp"
#include "datastructures/rabundvector.hpp"

#include "mothurout.h"

/*
 *  clusterclassic.h
 *  Mothur
 *
 *  Created by westcott on 10/29/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */


class ClusterClassic {
	
public:
	ClusterClassic(float, string, bool);
	void readPhylipFile(string, NameAssignment*);
    void readPhylipFile(string, CountTable*);
	void update(double&);
	double getSmallDist() { return smallDist; }	
	int getNSeqs() { return nseqs; }	
	ListVector* getListVector() { return list; }
	RAbundVector* getRAbundVector() { return rabund; }		
	string getTag() { return tag; }
	void setMapWanted(bool m);  
	map<string, int> getSeqtoBin()  {  return seq2Bin;	}

private:	
	double getSmallCell();
	void clusterBins();
	void clusterNames();
	void updateMap();
	void print();
	
	RAbundVector* rabund;
	ListVector* list;
	vector< vector<float> > dMatrix;	
	
	int smallRow;
	int smallCol, nseqs;
	double smallDist;
	bool mapWanted, sim;
	double cutoff, aboveCutoff;
	map<string, int> seq2Bin;
	string method, tag;
	
	MothurOut* m;
};

#endif


