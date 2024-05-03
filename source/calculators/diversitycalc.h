//
//  diversitycalc.h
//  Mothur
//
//  Created by Sarah Westcott on 5/23/19.
//  Copyright © 2019 Schloss Lab. All rights reserved.
//

#ifndef diversitycalc_h
#define diversitycalc_h

#include "mothurout.h"
#include "datastructures/sabundvector.hpp"
#include "utils.hpp"

/***********************************************************************/
struct acceptRatioPos  {
    double acceptRatio;
    bool high;
    int pos;

    acceptRatioPos() { pos = 0; acceptRatio = 1.0; high = false; }
    acceptRatioPos(double ac, int po, bool h) : acceptRatio(ac), pos(po), high(h) {}
    ~acceptRatioPos() = default;
};

/***********************************************************************/

inline bool operator< (const acceptRatioPos& lhs, const acceptRatioPos& rhs){ return rhs.acceptRatio > lhs.acceptRatio; }
inline bool operator> (const acceptRatioPos& lhs, const acceptRatioPos& rhs){ return rhs.acceptRatio < lhs.acceptRatio; }
inline bool operator<=(const acceptRatioPos& lhs, const acceptRatioPos& rhs){ return !(lhs.acceptRatio > rhs.acceptRatio); }
inline bool operator>=(const acceptRatioPos& lhs, const acceptRatioPos& rhs){ return !(lhs.acceptRatio < rhs.acceptRatio); }


/***********************************************************************/


class DiversityCalculator {

public:
    DiversityCalculator(bool rs){ m = MothurOut::getInstance();  requiresSamples = rs; }
    virtual ~DiversityCalculator(){};

    virtual string getTag() = 0;
    virtual bool requiresSample() { return requiresSamples; }
    virtual vector<double> getValues(int, vector<mcmcSample>&)  { return results;   }
    virtual vector<string> getValues(SAbundVector* rank)        { return outputs;   }
    virtual void getValues(SAbundVector* rank, vector<double>& ) {         }

protected:
    Utils util;
    MothurOut* m;

    bool requiresSamples;
    vector<double> results;
    vector<string> outputs;


};
/***********************************************************************/


#endif /* diversitycalc_h */
