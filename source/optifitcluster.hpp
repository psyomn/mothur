//
//  optifitcluster.hpp
//  Mothur
//
//  Created by Sarah Westcott on 5/10/18.
//  Copyright © 2018 Schloss Lab. All rights reserved.
//

#ifndef optifitcluster_hpp
#define optifitcluster_hpp

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
#include "opticluster.h"

/***********************************************************************/

class OptiFitCluster : public Cluster {
    
public:
    
    OptiFitCluster(OptiData* mt, ClusterMetric* met, long long ns);
    ~OptiFitCluster() = default;
    
    int initialize(double& value, bool randomize, vector<vector< string > > existingBins, vector<string>, string, bool);
    bool update(double&); //returns whether list changed and MCC
    bool updateDistance(PDistCell& colCell, PDistCell& rowCell) { return false; } //inheritance compliant
    string getTag() { string tag = "optifit_" + metric->getName(); return tag; }
    long long getNumBins();
    long long getNumFitBins();
    
    vector<double>  getStats( double&, double&,  double&,  double&);  //combo stats
    vector<double>  getFitStats( double&,  double&,  double&,  double&); //fitted seqs stats
    
    ListVector* getList();
    ListVector* getFittedList(string, bool);
    set<string> getUnfittedNames() { return unfittedNames; }
    
protected:
    MothurOut* m;
    Utils util;
    ClusterMetric* metric;
    OptiData* matrix;
    
    map<long long, long long> seqBin; //sequence# -> bin#
    vector<long long> randomizeSeqs;
    vector< vector<long long> > bins; //bin[0] -> seqs in bin[0]
    map<long long, string> binLabels; //for fitting - maps binNumber to existing reference label
    long long maxRefBinNumber;
    bool closed, denovo;
    set<string> unfittedNames;
    
    double fittruePositives, fittrueNegatives, fitfalsePositives, fitfalseNegatives, combotruePositives, combotrueNegatives, combofalsePositives, combofalseNegatives;
    long long  numFitSeqs, insertLocation, numFitSingletons;
    long long  numComboSeqs, numComboSingletons;
    
    int findInsert();
    vector<double> getCloseFarCounts(long long seq, long long newBin);
    vector<double> getCloseFarFitCounts(long long seq, long long newBin);
    ListVector* clusterUnfitted(OptiData*, string);
    
};


#endif /* optifitcluster_hpp */
