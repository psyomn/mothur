#ifndef DISPLAY_H
#define DISPLAY_H

#include "datastructures/sabundvector.hpp"

/***********************************************************************/

class Display {

public:
	virtual void update(SAbundVector& rank) = 0;
    virtual void update(vector<SharedRAbundVector*> shared, int numSeqs) {}
    virtual void update(vector<SharedRAbundVector*>& shared, int numSeqs, bool pairs, map<string, int>) {}
	virtual void init(string) = 0;
	virtual void reset() = 0;
	virtual void close() = 0;
    virtual bool isCalcMultiple() = 0;
	virtual void setAll(bool){}
	virtual bool hasLciHci(){ return false; }
	virtual bool getAll()	{	bool a; return a;	}
	virtual bool calcNeedsAll()    { bool a; return a;	}
	virtual string getName() { return ""; };
    virtual ~Display() = default;
	Display() {  m = MothurOut::getInstance();  }

protected:
	MothurOut* m;

};

/***********************************************************************/

#endif
