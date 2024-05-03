//
//  optidb.hpp
//  Mothur
//
//  Created by Sarah Westcott on 3/26/20.
//  Copyright © 2020 Schloss Lab. All rights reserved.
//

#ifndef optidb_hpp
#define optidb_hpp

#include "sequence.hpp"
#include "searchdatabase.hpp"

class OptiDB : public SearchDatabase {

public:
    
    OptiDB(string, string); //reference file name for shortcut file name generation, version
    ~OptiDB() = default;
    
    void addSequence(Sequence);
    void generateDB();
    void readDB(ifstream&);
    
    vector<int> getIndicatorColumns() { return indicatorColumns; }
    map<int, int> getFilteredIndicatorColumns(string f, vector<int>& results) { return filterIndicatorColumns(f, results); }

    
    vector< vector<int> > get(int i, char& allSame); //A,T,G,C,-,N returns vector[6][numSeqsWithBase] -> vector[0] = vector of indexes of reference with A in location i, vector[1] = vector of indexes of reference with T in location i,ect. If allSame!='x', all characters are the same in this column, and will return blank vector. ie if allSame='A', every reference in this location is an A
    
    vector<int> findClosestSequences(Sequence*, int, vector<float>&) const { return nullIntVector; }
   
    
    
private:

    string optiDBName, version;

    int alignedLength, numBases;
    classifierOTU reference;
    map<char, int> baseMap;
    vector<int> indicatorColumns;
    
    void convertSequences();
    void calcIndicatorColumns();
    map<int, int> filterIndicatorColumns(string, vector<int>&);
    
    //only used when generating db, not when reading shortcut files
    set<int> lengths;
    vector<Sequence> refs;
    
    
};


#endif /* optidb_hpp */

