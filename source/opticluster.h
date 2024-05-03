//
//  opticluster.h
//  Mothur
//
//  Created by Sarah Westcott on 4/20/16.
//  Copyright (c) 2016 Schloss Lab. All rights reserved.
//

#ifndef __Mothur__opticluster__
#define __Mothur__opticluster__

#include "cluster.hpp"
#include "datastructures/optimatrix.h"
#include "calculators/calculator.h"
#include "calculators/mcc.hpp"
#include "calculators/sensitivity.hpp"
#include "calculators/specificity.hpp"
#include "calculators/fdr.hpp"
#include "calculators/npv.hpp"
#include "calculators/ppv.hpp"
#include "calculators/f1score.hpp"
#include "calculators/tp.hpp"
#include "calculators/fp.hpp"
#include "calculators/fpfn.hpp"
#include "calculators/tptn.hpp"
#include "calculators/tn.hpp"
#include "calculators/fn.hpp"
#include "calculators/accuracy.hpp"

/***********************************************************************/

/*
 #ifdef UNIT_TEST
     friend class TestOptiCluster;
     OptiCluster() : Cluster() { m = MothurOut::getInstance(); truePositives = 0; trueNegatives = 0; falseNegatives = 0; falsePositives = 0;  } //for testing class
     void setVariables(OptiData* mt, ClusterMetric* met) { matrix = mt; metric = met; }
 #endif
     
 */

class OptiCluster : public Cluster {

public:
    
    OptiCluster(OptiData* mt, ClusterMetric* met, long long ns);
    ~OptiCluster() = default;
    
    bool updateDistance(PDistCell& colCell, PDistCell& rowCell) { return false; } 
    string getTag() { string tag = "opti_" + metric->getName(); return tag; }
    long long getNumBins();
    int initialize(double&, bool, string);  //randomize and place in "best" OTUs
    
    bool update(double&); //returns whether list changed and MCC
    vector<double> getStats( double&,  double&,  double&,  double&);
    ListVector* getList();
    
protected:
    OptiData* matrix;
    ClusterMetric* metric;
    vector<int> randomizeSeqs;
    vector< vector<long long> > bins; //bin[0] -> seqs in bin[0]
    map<long long, string> binLabels; //for fitting - maps binNumber to existing reference label
    map<long long, long long> seqBin; //sequence# -> bin#
    
    long long numSeqs, insertLocation, numSingletons;
    double truePositives, trueNegatives, falsePositives, falseNegatives;
    
    long long findInsert();
    vector<double> getCloseFarCounts(long long seq, long long newBin);
    vector<double> getFitStats( long long&, long long&, long long&, long long&);
};

#endif /* defined(__Mothur__opticluster__) */
