#ifndef INPUTDATA_H
#define INPUTDATA_H

#include "datastructures/listvector.hpp"
#include "datastructures/ordervector.hpp"
#include "datastructures/sharedlistvector.h"
#include "datastructures/sharedordervector.h"
#include "datastructures/sharedrabundvectors.hpp"
#include "datastructures/sharedrabundfloatvectors.hpp"

#include "utils.hpp"

class InputData {

public:
	InputData(string, string, vector<string>);
	InputData(string, string, string);
	~InputData();
	ListVector* getListVector();
	ListVector* getListVector(string);  //pass the label you want
	ListVector* getListVector(string, bool);  //pass the label you want, reset filepointer
	SharedListVector* getSharedListVector();
	SharedListVector* getSharedListVector(string);  //pass the label you want
	OrderVector* getOrderVector();
	OrderVector* getOrderVector(string); //pass the label you want
	SharedOrderVector* getSharedOrderVector();
	SharedOrderVector* getSharedOrderVector(string);  //pass the label you want
	SAbundVector* getSAbundVector();
	SAbundVector* getSAbundVector(string);  //pass the label you want
	RAbundVector* getRAbundVector();
	RAbundVector* getRAbundVector(string);  //pass the label you want
	SharedRAbundVectors* getSharedRAbundVectors();
	SharedRAbundVectors* getSharedRAbundVectors(string);  //pass the label you want
	SharedRAbundFloatVectors* getSharedRAbundFloatVectors();
	SharedRAbundFloatVectors* getSharedRAbundFloatVectors(string);  //pass the label you want
    SharedCLRVectors* getSharedCLRVectors();
    SharedCLRVectors* getSharedCLRVectors(string);  //pass the label you want

private:
    Utils util;
	string format;
	ifstream fileHandle;
	DataVector* input;
	ListVector* list;
	SharedListVector* SharedList;
	OrderVector* output;
	SharedOrderVector* SharedOrder;
	SAbundVector* sabund;
	RAbundVector* rabund;
	map<string,int> orderMap;
	string filename;
	MothurOut* m;
    vector<string> currentLabels;
    vector<string> groups;
    string nextDistanceLabel;
    string otuTag;
};


#endif
