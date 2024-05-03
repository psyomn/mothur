//
//  kmerdist.cpp
//  Mothur
//
//  Created by Sarah Westcott on 3/29/21.
//  Copyright © 2021 Schloss Lab. All rights reserved.
//

#include "calculators/kmerdist.hpp"
#include "datastructures/kmer.hpp"

/***********************************************************************/
KmerDist::KmerDist(int k) {
    try {
        m = MothurOut::getInstance();
        
        int power4s[14] = { 1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864 };
        
        kmerSize = k;
        maxKmer = power4s[kmerSize];
    }
    catch(exception& e) {
        m->errorOut(e, "KmerDist", "KmerDist");
        exit(1);
    }
}
/***********************************************************************/

double KmerDist::calcDist(Sequence A, Sequence B){
    try {
        string seqA = A.getUnaligned();
        string seqB = B.getUnaligned();
        
        int numKmers = min(seqA.length(), seqB.length()) - kmerSize + 1;
        
        Kmer kmer(kmerSize);
        
        int numSeqAKmers = seqA.length() - kmerSize + 1;
            
        vector<int> seqAKmers(maxKmer+1,0);
        for(int j=0;j<numSeqAKmers;j++){                        //    ...step though the sequence and get each kmer...
            int kmerNumber = kmer.getKmerNumber(seqA, j);
            seqAKmers[kmerNumber] = 1;
        }    
        
        int numSeqBKmers = seqB.length() - kmerSize + 1;
            
        int numMatchingKmers = 0;
        vector<int> seqBKmers(maxKmer+1,0);
        for(int j=0;j<numSeqBKmers;j++){                        //    ...step though the sequence and get each kmer...
            int kmerNumber = kmer.getKmerNumber(seqB, j);
            if ((seqBKmers[kmerNumber] == 0) && (seqAKmers[kmerNumber] == 1)) { //this kmer is present in seqA, and we haven't already counted it
                numMatchingKmers++;
            }
            seqBKmers[kmerNumber] = 1;
        }
        
        double dist = abs(log(0.1 + (numMatchingKmers / (float) numKmers)));
            
        return dist;
    }
    catch(exception& e) {
        m->errorOut(e, "KmerDist", "calcDist");
        exit(1);
    }
}
/***********************************************************************/

vector<double> KmerDist::calcDist(vector<kmerCount> A, vector<int> B, int length){
    try {
        int numAKmers = 0;
        int numMatchingKmers = 0;
        int numUniqueMatchingKmers = 0;
        
        int numKmers = length - kmerSize + 1;
        
        for (int i = 0; i < A.size(); i++) {
            
            numAKmers += A[i].kCount;
            
            if (B[A[i].kmerNumber] != 0) { //does sequence B contain this kmer
                numMatchingKmers += min(A[i].kCount, B[A[i].kmerNumber]);
                numUniqueMatchingKmers++;
            }
        }
    
        double dist1 = log(1.0 - (numMatchingKmers / (float) numKmers));
        double dist2 = log(1.0 - (numUniqueMatchingKmers / (float) numKmers));
              
        vector<double> dists; dists.push_back(dist1); dists.push_back(dist2);
        return dists;
    }
    catch(exception& e) {
        m->errorOut(e, "KmerDist", "calcDist");
        exit(1);
    }
}
/***********************************************************************/
