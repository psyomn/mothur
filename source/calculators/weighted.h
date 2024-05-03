#ifndef WEIGHTED_H
#define WEIGHTED_H


/*
 *  weighted.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "treecalculator.h"

/***********************************************************************/

class Weighted : public TreeCalculator  {

	public:
        Weighted( bool r, vector<string> G);
		~Weighted() = default;

		EstOutput getValues(Tree*, string, string);
		EstOutput getValues(Tree*, int);

	private:
        Utils util;
        int processors;
        bool includeRoot;
        vector<string> Groups;
        vector< vector<string> > namesOfGroupCombos;

		EstOutput createProcesses(Tree*);
};
/**************************************************************************************************/

#endif
