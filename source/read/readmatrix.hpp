#ifndef READMATRIX_HPP
#define READMATRIX_HPP

/*
 *  readmatrix.hpp
 *  
 *
 *  Created by Pat Schloss on 8/13/08.
 *  Copyright 2008 Patrick D. Schloss. All rights reserved.
 *
 */

#include "mothur.h"
#include "datastructures/listvector.hpp"
#include "datastructures/nameassignment.hpp"
#include "datastructures/counttable.h"
#include "datastructures/sparsedistancematrix.h"
#include "utils.hpp"


class ReadMatrix {

public:
	ReadMatrix(){ DMatrix = new SparseDistanceMatrix(); m = MothurOut::getInstance();  }
	virtual ~ReadMatrix() = default;
	virtual int read(NameAssignment*){ return 1; }
    virtual int read(CountTable*){ return 1; }
	
	void setCutoff(float c)			{	cutoff = c;		}
    SparseDistanceMatrix* getDMatrix()		{	return DMatrix;		}
	ListVector* getListVector()		{	return list;	}

	int successOpen;
	
protected:
    SparseDistanceMatrix* DMatrix;
	ListVector* list;
	float cutoff;
	MothurOut* m;
	bool sim;
    Utils util;
};



#endif
