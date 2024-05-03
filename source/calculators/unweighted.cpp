/*
 *  unweighted.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 2/9/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "unweighted.h"

/**************************************************************************************************/
Unweighted::Unweighted(bool r, vector<string> G) : includeRoot(r), Groups(G) {
    try {
        int numGroups = Groups.size();
        
        //calculate number of comparisons i.e. with groups A,B,C = AB, AC, BC = 3;
        for (int i=0; i<numGroups; i++) {
            for (int l = 0; l < i; l++) {
                vector<string> groups; groups.push_back(Groups[i]); groups.push_back(Groups[l]);
                namesOfGroupCombos.push_back(groups);
            }
        }
    }
    catch(exception& e) {
        m->errorOut(e, "Unweighted", "Unweighted");
        exit(1);
    }
}
/**************************************************************************************************/

EstOutput Unweighted::getValues(Tree* t, int p) {
	try {
		processors = p;
    
		return (createProcesses(t));
	}
	catch(exception& e) {
		m->errorOut(e, "Unweighted", "getValues");
		exit(1);
	}
}
/***********************************************************************/
struct unweightedData {
    int start;
    int num, count;
    MothurOut* m;
    EstOutput results;
    vector< vector<string> > namesOfGroupCombos;
    vector<vector<int> > randomizedTreeNodes;
    Tree* t;
    CountTable* ct;
    bool includeRoot;
    vector<string> Treenames;
    Utils util;
    
    unweightedData(){}
    unweightedData(int st, int en, vector< vector<string> > ngc, Tree* tree, CountTable* count, bool ir) {
        m = MothurOut::getInstance();
        start = st;
        num = en;
        namesOfGroupCombos = ngc;
        t = tree;
        ct = count;
        includeRoot = ir;
        Treenames = tree->getTreeNames();
        results.resize(num);
        count = 0;
    }

    unweightedData(int st, int en, vector< vector<string> > ngc, Tree* tree, CountTable* count, bool ir, vector<vector<int> > randomTreeNodes) {
        m = MothurOut::getInstance();
        start = st;
        num = en;
        namesOfGroupCombos = ngc;
        randomizedTreeNodes = randomTreeNodes;
        t = tree;
        ct = count;
        includeRoot = ir;
        Treenames = tree->getTreeNames();
        results.resize(num);
        count = 0;
    }
};
/**************************************************************************************************/
int findNodeBelonging(MothurOut* m, vector<string>& namesOfGroupCombos, map< string, vector<int> >& groupNodeInfo) {
    try {
     
     int nodeBelonging = -1;
     for (int g = 0; g < namesOfGroupCombos.size(); g++) {
         if (groupNodeInfo[namesOfGroupCombos[g]].size() != 0) { nodeBelonging = groupNodeInfo[namesOfGroupCombos[g]][0]; break; }
     }
     
     //sanity check
     if (nodeBelonging == -1) {
         m->mothurOut("[WARNING]: cannot find a nodes in the tree from grouping ");
         for (int g = 0; g < namesOfGroupCombos.size()-1; g++) { m->mothurOut(namesOfGroupCombos[g] + "-"); }
         m->mothurOut(namesOfGroupCombos[namesOfGroupCombos.size()-1]);
         m->mothurOut(", skipping.\n");
     }
     
     return nodeBelonging;
 }
 catch(exception& e) {
     m->errorOut(e, "Weighted", "findNodeBelongingToThisComparison");
     exit(1);
 }
}
/**************************************************************************************************/
void getRoot2(MothurOut* m, Tree* t, int v, vector<string> grouping, set<int>& rootForGrouping) {
    try {
        //you are a leaf so get your parent
        int index = t->tree[v].getParent();
        
        //my parent is a potential root
        rootForGrouping.insert(index);
        
        //while you aren't at root
        while(t->tree[index].getParent() != -1){
            
            if (m->getControl_pressed()) {  return; }
            
            //am I the root for this grouping? if so I want to stop "early"
            //does my sibling have descendants from the users groups?
            //if so I am not the root
            int parent = t->tree[index].getParent();
            int lc = t->tree[parent].getLChild();
            int rc = t->tree[parent].getRChild();
            
            int sib = lc;
            if (lc == index) { sib = rc; }
            
            map<string, int>::iterator itGroup;
            int pcountSize = 0;
            for (int j = 0; j < grouping.size(); j++) {
                map<string, int>::iterator itGroup = t->tree[sib].pcount.find(grouping[j]);
                if (itGroup != t->tree[sib].pcount.end()) { pcountSize++; if (pcountSize > 1) { break; } }
            }
            
            //if yes, I am not the root
            if (pcountSize != 0) {
                rootForGrouping.clear();
                rootForGrouping.insert(parent);
            }
            
            index = parent;
        }
        
        //get all nodes above the root to add so we don't add their u values above
        index = *(rootForGrouping.begin());
        while(t->tree[index].getParent() != -1){
            int parent = t->tree[index].getParent();
            rootForGrouping.insert(parent);
            
            index = parent;
        }
        
        
        return;
    }
    catch(exception& e) {
        m->errorOut(e, "Weighted", "getRoot2");
        exit(1);
    }
}
/**************************************************************************************************/
void driverUnweighted(unweightedData* params) {
    try {
        params->count = 0;

        for (int h = params->start; h < (params->start+params->num); h++) {
            
            if (params->m->getControl_pressed()) { break; }
            
            double UniqueBL=0.0000;  //a branch length is unique if it's chidren are from the same group
            double totalBL = 0.00;	//all branch lengths
            double UW = 0.00;		//Unweighted Value = UniqueBL / totalBL;
            set<int> rootBranches; //if not including root this will hold branches that are "above" the root for this comparison

            int nodeBelonging = findNodeBelonging(params->m, params->namesOfGroupCombos[h], params->t->groupNodeInfo);
            
            if (nodeBelonging != -1) {

                //fills rootBranches to exclude, if including the root then rootBranches should be empty.
                if (!params->includeRoot) { getRoot2(params->m, params->t, nodeBelonging, params->namesOfGroupCombos[h], rootBranches); }

                for(int i=0;i<params->t->getNumNodes();i++){
                    
                    if (params->m->getControl_pressed()) {  break; }
                    
                    //pcountSize = 0, they are from a branch that is entirely from a group the user doesn't want
                    //pcountSize = 2, not unique to one group
                    //pcountSize = 1, unique to one group
                    
                    int pcountSize = 0;
                    for (int j = 0; j < params->namesOfGroupCombos[h].size(); j++) {
                        map<string, int>::iterator itGroup = params->t->tree[i].pcount.find(params->namesOfGroupCombos[h][j]);
                        if (itGroup != params->t->tree[i].pcount.end()) { pcountSize++; if (pcountSize > 1) { break; } }
                    }
                    
                    
                    //unique calc
                    if (pcountSize == 0) { }
                    else if (!params->util.isEqual(params->t->tree[i].getBranchLength(), -1) && (pcountSize == 1) && (rootBranches.count(i) == 0)) { //you have a unique branch length and you are not the root
                        UniqueBL += abs(params->t->tree[i].getBranchLength());
                    }
                    
                    //total calc
                    if (pcountSize == 0) { }
                    else if (!params->util.isEqual(params->t->tree[i].getBranchLength(), -1) && (pcountSize != 0) && (rootBranches.count(i) == 0)) { //you have a branch length and you are not the root
                        totalBL += abs(params->t->tree[i].getBranchLength());
                    }
                }
                
                UW = (UniqueBL / totalBL);
                
                if (isnan(UW) || isinf(UW)) { UW = 0; }
                
                params->results[params->count] = UW;
            }
            params->count++;
        }
    }
    catch(exception& e) {
        params->m->errorOut(e, "Unweighted", "driverUnweighted");
        exit(1);
    }
}
/**************************************************************************************************/

EstOutput Unweighted::createProcesses(Tree* t) {
	try {
        vector<linePair> lines;
        int remainingPairs = namesOfGroupCombos.size();
        if (remainingPairs < processors) { processors = remainingPairs; }
        int startIndex = 0;
        for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
            int numPairs = remainingPairs; //case for last processor
            if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
            lines.push_back(linePair(startIndex, numPairs)); //startIndex, numPairs
            startIndex = startIndex + numPairs;
            remainingPairs = remainingPairs - numPairs;
        }
        
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<unweightedData*> data;
        vector<string> Treenames; Treenames = t->getTreeNames();
        CountTable* ct = t->getCountTable();
        
        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            CountTable* copyCount = new CountTable();
            copyCount->copy(ct);
            Tree* copyTree = new Tree(copyCount, Treenames);
            copyTree->getCopy(t);
            
            unweightedData* dataBundle = new unweightedData(lines[i+1].start, lines[i+1].end, namesOfGroupCombos, copyTree, copyCount, includeRoot);
            data.push_back(dataBundle);
            
            workerThreads.push_back(new std::thread(driverUnweighted, dataBundle));
        }
        
        CountTable* copyCount = new CountTable();
        copyCount->copy(ct);
        Tree* copyTree = new Tree(copyCount, Treenames);
        copyTree->getCopy(t);
        
        unweightedData* dataBundle = new unweightedData(lines[0].start, lines[0].end, namesOfGroupCombos, t, ct, includeRoot);
        driverUnweighted(dataBundle);
        EstOutput results = dataBundle->results;
        delete copyTree; delete copyCount;
        delete dataBundle;
        
        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            
            for (int j = 0; j < data[i]->results.size(); j++) {  results.push_back(data[i]->results[j]);  }
            if (data[i]->count != data[i]->num) { //you didn't complete your tasks
                m->mothurOut("[ERROR]: thread " + toString(i+1) + " failed to complete it's tasks, quitting.\n");
                m->setControl_pressed(true);
            }
            
            delete data[i]->t; delete data[i]->ct; delete data[i]; delete workerThreads[i];
        }
        
        return results;
	}
	catch(exception& e) {
		m->errorOut(e, "Unweighted", "createProcesses");
		exit(1);
	}
}
/**************************************************************************************************/

EstOutput Unweighted::getValues(Tree* t, vector<vector<int> >& randomTreeNodes, int p) {
 try {
		processors = p;
    
        return (createProcesses(t, randomTreeNodes));
	}
	catch(exception& e) {
		m->errorOut(e, "Unweighted", "getValues");
		exit(1);
	}
}
/**************************************************************************************************/
void driverRandomCalcs(unweightedData* params) {
    try {
        params->count = 0;
       
        vector<string> Treenames = params->t->getTreeNames();
        Tree* copyTree = new Tree(params->ct, Treenames);
        
        for (int h = params->start; h < (params->start+params->num); h++) {
            
            if (params->m->getControl_pressed()) { break; }
            
            //copy random tree passed in
            copyTree->getCopy(params->t);
            
            //swap labels in the groups you want to compare
            copyTree->assembleRandomUnifracTree(params->randomizedTreeNodes[h]);
            
            double UniqueBL=0.0000;  //a branch length is unique if it's chidren are from the same group
            double totalBL = 0.00;	//all branch lengths
            double UW = 0.00;		//Unweighted Value = UniqueBL / totalBL;
            //find a node that belongs to one of the groups in this combo
            set<int> rootBranches; //if not including root this will hold branches that are "above" the root for this comparison

            int nodeBelonging = findNodeBelonging(params->m, params->namesOfGroupCombos[h], params->t->groupNodeInfo);
            
            if (nodeBelonging != -1) {

                //fills rootBranches to exclude, if including the root then rootBranches should be empty.
                if (!params->includeRoot) { getRoot2(params->m, params->t, nodeBelonging, params->namesOfGroupCombos[h], rootBranches); }

                for(int i=0;i<copyTree->getNumNodes();i++){
                    
                    if (params->m->getControl_pressed()) {  break; }
                    
                    //pcountSize = 0, they are from a branch that is entirely from a group the user doesn't want
                    //pcountSize = 2, not unique to one group
                    //pcountSize = 1, unique to one group
                    
                    int pcountSize = 0;
                    for (int j = 0; j < params->namesOfGroupCombos[h].size(); j++) {
                        map<string, int>::iterator itGroup = copyTree->tree[i].pcount.find(params->namesOfGroupCombos[h][j]);
                        if (itGroup != copyTree->tree[i].pcount.end()) { pcountSize++; if (pcountSize > 1) { break; } }
                    }
                    
                    //unique calc
                    if (pcountSize == 0) { }
                    else if (!params->util.isEqual(copyTree->tree[i].getBranchLength(), -1) && (pcountSize == 1) && (rootBranches.count(i) == 0)) { //you have a unique branch length and you are not the root
                        UniqueBL += abs(copyTree->tree[i].getBranchLength());
                    }
                    
                    //total calc
                    if (pcountSize == 0) { }
                    else if (!params->util.isEqual(copyTree->tree[i].getBranchLength(), -1) && (pcountSize != 0) && (rootBranches.count(i) == 0)) { //you have a branch length and you are not the root
                        totalBL += abs(copyTree->tree[i].getBranchLength()); 
                    }
                    
                }
                
                UW = (UniqueBL / totalBL);
                if (isnan(UW) || isinf(UW)) { UW = 0; }
                
                params->results[params->count] = UW;
            }
            params->count++;
        }
        
        delete copyTree;
    }
    catch(exception& e) {
        params->m->errorOut(e, "Unweighted", "driverRandomCalcs");
        exit(1);
    }
}
/**************************************************************************************************/

EstOutput Unweighted::createProcesses(Tree* t, vector<vector<int> >& randomTreeNodes) {
	try {
        vector<linePair> lines;
        int remainingPairs = namesOfGroupCombos.size();
        if (remainingPairs < processors) { processors = remainingPairs; }
        int startIndex = 0;
        for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
            int numPairs = remainingPairs; //case for last processor
            if (remainingProcessors != 1) { numPairs = ceil(remainingPairs / remainingProcessors); }
            lines.push_back(linePair(startIndex, numPairs)); //startIndex, numPairs
            startIndex = startIndex + numPairs;
            remainingPairs = remainingPairs - numPairs;
        }
        
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<unweightedData*> data;
        vector<string> Treenames; Treenames = t->getTreeNames();
        CountTable* ct = t->getCountTable();
        
        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            CountTable* copyCount = new CountTable();
            copyCount->copy(ct);
            Tree* copyTree = new Tree(copyCount, Treenames);
            copyTree->getCopy(t);
            
            unweightedData* dataBundle = new unweightedData(lines[i+1].start, lines[i+1].end, namesOfGroupCombos, copyTree, copyCount, includeRoot, randomTreeNodes);
            data.push_back(dataBundle);
            
            workerThreads.push_back(new std::thread(driverRandomCalcs, dataBundle));
        }
        
        unweightedData* dataBundle = new unweightedData(lines[0].start, lines[0].end, namesOfGroupCombos, t, ct, includeRoot, randomTreeNodes);
        driverRandomCalcs(dataBundle);
        EstOutput results = dataBundle->results;
        delete dataBundle;
        
        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            
            for (int j = 0; j < data[i]->results.size(); j++) {  results.push_back(data[i]->results[j]);  }
            if (data[i]->count != data[i]->num) { //you didn't complete your tasks
                m->mothurOut("[ERROR]: thread " + toString(i+1) + " failed to complete it's tasks, quitting.\n");
                m->setControl_pressed(true);
            }
            
            delete data[i]->t;
            delete data[i]->ct;
            delete data[i];
            delete workerThreads[i];
        }
        
        return results;
	}
	catch(exception& e) {
		m->errorOut(e, "Unweighted", "createProcesses");
		exit(1);
	}
}
/**************************************************************************************************/


