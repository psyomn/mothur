#ifndef Mothur_treereader_h
#define Mothur_treereader_h

//
//  treereader.h
//  Mothur
//
//  Created by Sarah Westcott on 4/11/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "mothurout.h"
#include "datastructures/tree.h"
#include "datastructures/counttable.h"

class TreeReader {

public:

    TreeReader(string tf, string cf);
    TreeReader(string tf, string gf, string nf);
	~TreeReader() = default;

    vector<Tree*> getTrees()            { return trees;     }

private:
    MothurOut* m;
	vector<Tree*> trees;
    CountTable* ct;
    vector<string> Treenames;

    string treefile, groupfile, namefile, countfile;

    bool readTrees();
    int readNamesFile();
};



#endif
