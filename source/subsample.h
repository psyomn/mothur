#ifndef Mothur_subsample_h
#define Mothur_subsample_h

//
//  subsample.h
//  Mothur
//
//  Created by Sarah Westcott on 4/2/12.
//  Copyright (c) 2012 Schloss Lab. All rights reserved.
//

#include "datastructures/rabundvector.hpp"
#include "datastructures/tree.h"
#include "datastructures/counttable.h"
#include "datastructures/sharedrabundvectors.hpp"

#include "mothurout.h"


//subsampling overwrites the sharedRabunds.  If you need to reuse the original use the getSamplePreserve function.

class SubSample {

    public:

        SubSample() { m = MothurOut::getInstance(); }
        ~SubSample() = default;

        vector<string> getSample(SharedRAbundVectors*&, int); //returns the bin labels for the subsample. Overwrites original vector passed in, if you need to preserve it deep copy first.
        vector<string> getSampleWithReplacement(SharedRAbundVectors*&, int); //returns the bin labels for the subsample. Overwrites original vector passed in, if you need to preserve it deep copy first.

        vector<string> getSample(vector<SharedRAbundVector*>&, int, vector<string>); //returns the bin labels for the subsample. Overwrites original vector passed in, if you need to preserve it deep copy first.
        vector<string> getSampleWithReplacement(vector<SharedRAbundVector*>&, int, vector<string>); //returns the bin labels for the subsample. Overwrites original vector passed in, if you need to preserve it deep copy first.

        Tree* getSample(Tree*, CountTable*, CountTable*, int, vector<string>&); //creates new subsampled tree. Uses first counttable to fill new counttable with sabsampled seqs. Sets groups of seqs not in subsample to "doNotIncludeMe".
        Tree* getSampleWithReplacement(Tree*, CountTable*, CountTable*, int, vector<string>&); //creates new subsampled tree. Uses first counttable to fill new counttable with sabsampled seqs. Sets groups of seqs not in subsample to "doNotIncludeMe".

        int getSample(SAbundVector*&, int); //destroys sabundvector passed in, so copy it if you need it
        int getSampleWithReplacement(SAbundVector*&, int); //destroys sabundvector passed in, so copy it if you need it

        int getSample(RAbundVector*&, int); //destroys sabundvector passed in, so copy it if you need it
        int getSampleWithReplacement(RAbundVector*&, int); //destroys sabundvector passed in, so copy it if you need it

        CountTable getSample(CountTable&, int, vector<string>, bool persample); // if persample then subsample 'size' members from each group - bygroup(same number sampled from each group), returns subsampled. If not persample then subsample 'size' members from the set of groups passed in.
        CountTable getSampleWithReplacement(CountTable&, int, vector<string>, bool persample); // if persample then subsample 'size' members from each group - bygroup(same number sampled from each group), returns subsampled. If not persample then subsample 'size' members from the set of groups passed in.

        GroupMap getSample(GroupMap&, int, vector<string> groupsWanted, bool persample); // if persample then subsample 'size' members from each group - bygroup(same number sampled from each group), returns subsampled. If not persample then subsample 'size' members from the set of groups passed in.
        GroupMap getSample(GroupMap&, int size); //returns subsampled GroupMap with 'size' members


        set<long long> getWeightedSample(map<long long, long long>&, long long); //map of sequence names -> weight (could be abundance or some other measure), num to sample

    private:

        MothurOut* m;
        Utils util;
        map<string, string> deconvolute(map<string, string> wholeSet, vector<string>& subsampleWanted); //returns new nameMap containing only subsampled names, and removes redundants from subsampled wanted because it makes the new nameMap.
        GroupMap getSample(GroupMap&, int, vector<string> groupsWanted);
        CountTable getSample(CountTable&, int, vector<string>); //subsample a countTable bygroup(same number sampled from each group), returns subsampled countTable
        CountTable getSampleWithReplacement(CountTable&, int, vector<string>); //subsample a countTable bygroup(same number sampled from each group), returns subsampled countTable

};

#endif
