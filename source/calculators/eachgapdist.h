#ifndef EACHGAPDIST_H
#define EACHGAPDIST_H
/*
 *  eachgapdist.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 5/7/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */


#include "calculator.h"

/**************************************************************************************************/

class eachGapDist : public DistCalc {
	
public:
	
    eachGapDist(double c) : DistCalc(c) {}
	
    double calcDist(Sequence A, Sequence B);
    
    vector<double> calcDist(Sequence A, classifierOTU otu, vector<int> cols);
    
    string getCitation() { return "http://mothur.org"; }

	
};

/**************************************************************************************************/

#endif
