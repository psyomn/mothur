#ifndef UNWEIGHTED_H
#define UNWEIGHTED_H


/*
 *  unweighted.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "treecalculator.h"

/***********************************************************************/

class Unweighted : public TreeCalculator  {

	public:
        Unweighted(bool r, vector<string> g);
		~Unweighted() = default;

		EstOutput getValues(Tree*, int);
		EstOutput getValues(Tree*, vector<vector<int> >&, int);

	private:
		vector< vector<string> > namesOfGroupCombos;
        vector<string> Groups;
		int processors;
		bool includeRoot;

		EstOutput createProcesses(Tree*);
		EstOutput createProcesses(Tree*, vector<vector<int> >&);
};

/**************************************************************************************************/

#endif
