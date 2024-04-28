#ifndef CLUSTER_H
#define CLUSTER_H

#include "datastructures/sparsedistancematrix.h"
#include "datastructures/rabundvector.hpp"
#include "datastructures/listvector.hpp"

#include "mothurout.h"

class ListVector;

class Cluster {
	
public:
	Cluster(RAbundVector*, ListVector*, SparseDistanceMatrix*, float, string, float);
    Cluster() { m = MothurOut::getInstance(); }
    virtual ~Cluster() = default;
    virtual bool update(double&);
	virtual string getTag() = 0;
	virtual void setMapWanted(bool m);  
	virtual map<string, int> getSeqtoBin()  {  return seq2Bin;	}
    
protected:	    
	virtual bool updateDistance(PDistCell& colCell, PDistCell& rowCell) = 0;
    
	virtual void clusterBins();
	virtual void clusterNames();
	virtual void updateMap();
	
	RAbundVector* rabund;
	ListVector* list;
	SparseDistanceMatrix* dMatrix;	
	
	ull smallRow, smallCol, nRowCells, nColCells;
	float smallDist, adjust, cutoff;
	bool mapWanted;
	map<string, int> seq2Bin;
	string method;
	
	MothurOut* m;
    Utils util;
};

/***********************************************************************/

class CompleteLinkage : public Cluster {
public:
	CompleteLinkage(RAbundVector*, ListVector*, SparseDistanceMatrix*, float, string, float);
	bool updateDistance(PDistCell& colCell, PDistCell& rowCell);
	string getTag();
	
private:
    
};

/***********************************************************************/

class SingleLinkage : public Cluster {
public:
	SingleLinkage(RAbundVector*, ListVector*, SparseDistanceMatrix*, float, string, float);
    //void update(double&);
	bool updateDistance(PDistCell& colCell, PDistCell& rowCell);
	string getTag();
	
private:
    
};

/***********************************************************************/

class AverageLinkage : public Cluster {
public:
	AverageLinkage(RAbundVector*, ListVector*, SparseDistanceMatrix*, float, string, float);
	bool updateDistance(PDistCell& colCell, PDistCell& rowCell);
	string getTag();
	
private:
	int saveRow;
	int saveCol;
	int rowBin;
	int colBin;
	int totalBin;
    
};

/***********************************************************************/

class WeightedLinkage : public Cluster {
public:
	WeightedLinkage(RAbundVector*, ListVector*, SparseDistanceMatrix*, float, string, float);
	bool updateDistance(PDistCell& colCell, PDistCell& rowCell);
	string getTag();
	
private:
	int saveRow;
	int saveCol;	
};

/***********************************************************************/



#endif
