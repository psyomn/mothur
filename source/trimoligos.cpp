/*
 * trimoligos.cpp
 * Mothur
 *
 * Created by westcott on 9/1/11.
 * Copyright 2011 Schloss Lab. All rights reserved.
 *
 */

#include "trimoligos.h"

#include "datastructures/alignment.hpp"
#include "needlemanoverlap.hpp"

/********************************************************************/
//strip, pdiffs, bdiffs, primers, barcodes, revPrimers
TrimOligos::TrimOligos(int p, int b, int l, int s, map<int, oligosPair> pr, map<int, oligosPair> br, bool hi){
    try {
        m = MothurOut::getInstance();
        preProcess(p,b,l,s,pr,br,hi);
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "TrimOligos");
        exit(1);
    }
}
/********************************************************************/
TrimOligos::TrimOligos(int p, int rd, int b, map<string, int> pr, map<string, int> br, vector<string> r){
    try {
        m = MothurOut::getInstance();
        int l = 0; int s = 0; vector<string> lk, sp;
        preProcess(p,b,l,s,rd,pr,br,r,lk,sp);
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "TrimOligos");
        exit(1);
    }
}
/********************************************************************/
TrimOligos::TrimOligos(int p, int b, int l, int s, map<string, int> pr, map<string, int> br, vector<string> r, vector<string> lk, vector<string> sp){
    try {
        m = MothurOut::getInstance();
        preProcess(p,b,l,s,0,pr,br,r,lk,sp);
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "TrimOligos");
        exit(1);
    }
}
/********************************************************************/
int TrimOligos::preProcess(int p, int b, int l, int s, int rd, map<string, int> pr, map<string, int> br, vector<string> r, vector<string> lk, vector<string> sp){
    try {
        paired = false;
        hasIndex = false;

        pdiffs = p;
        bdiffs = b;
        ldiffs = l;
        sdiffs = s;
        rdiffs = rd;

        barcodes = br;
        primers = pr;
        revPrimer = r;
        linker = lk;
        spacer = sp;
        maxFBarcodeLength = 0;
        for(map<string,int>::iterator it=barcodes.begin();it!=barcodes.end();it++){
            if(it->first.length() > maxFBarcodeLength){
                maxFBarcodeLength = it->first.length();
            }
        }
        maxFPrimerLength = 0;
        map<string,int>::iterator it;
        for(it=primers.begin();it!=primers.end();it++){
            if(it->first.length() > maxFPrimerLength){
                maxFPrimerLength = it->first.length();
            }
        }

        maxLinkerLength = 0;
        for(int i = 0; i < linker.size(); i++){
            if(linker[i].length() > maxLinkerLength){
                maxLinkerLength = linker[i].length();
            }
        }

        maxSpacerLength = 0;
        for(int i = 0; i < spacer.size(); i++){
            if(spacer[i].length() > maxSpacerLength){
                maxSpacerLength = spacer[i].length();
            }
        }

        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "preProcess");
        exit(1);
    }
}
/********************************************************************/
//strip, pdiffs, bdiffs, primers, barcodes, revPrimers
int TrimOligos::preProcess(int p, int b, int l, int s, map<int, oligosPair> pr, map<int, oligosPair> br, bool hi){
    try {
        pdiffs = p;
        bdiffs = b;
        ldiffs = l;
        sdiffs = s;
        rdiffs = 0;
        paired = true;
        hasIndex = hi;

        maxFBarcodeLength = 0;
        for(map<int,oligosPair>::iterator it=br.begin();it!=br.end();it++){
            string forward = it->second.forward;
            map<string, vector<int> >::iterator itForward = ifbarcodes.find(forward);

            if(forward.length() > maxFBarcodeLength){ maxFBarcodeLength = forward.length(); }

            if (itForward == ifbarcodes.end()) {
                vector<int> temp; temp.push_back(it->first);
                ifbarcodes[forward] = temp;
            }else { itForward->second.push_back(it->first); }
        }

        maxFPrimerLength = 0;
        for(map<int,oligosPair>::iterator it=pr.begin();it!=pr.end();it++){
            string forward = it->second.forward;
            map<string, vector<int> >::iterator itForward = ifprimers.find(forward);

            if(forward.length() > maxFPrimerLength){ maxFPrimerLength = forward.length(); }

            if (itForward == ifprimers.end()) {
                vector<int> temp; temp.push_back(it->first);
                ifprimers[forward] = temp;
            }else { itForward->second.push_back(it->first); }
        }

        maxRBarcodeLength = 0;
        for(map<int,oligosPair>::iterator it=br.begin();it!=br.end();it++){
            string forward = it->second.reverse;
            map<string, vector<int> >::iterator itForward = irbarcodes.find(forward);

            if(forward.length() > maxRBarcodeLength){ maxRBarcodeLength = forward.length(); }

            if (itForward == irbarcodes.end()) {
                vector<int> temp; temp.push_back(it->first);
                irbarcodes[forward] = temp;
            }else { itForward->second.push_back(it->first); }
        }

        maxRPrimerLength = 0;
        for(map<int,oligosPair>::iterator it=pr.begin();it!=pr.end();it++){
            string forward = it->second.reverse;
            map<string, vector<int> >::iterator itForward = irprimers.find(forward);

            if(forward.length() > maxRPrimerLength){ maxRPrimerLength = forward.length(); }

            if (itForward == irprimers.end()) {
                vector<int> temp; temp.push_back(it->first);
                irprimers[forward] = temp;
            }else { itForward->second.push_back(it->first); }
        }

        ipbarcodes = br;
        ipprimers = pr;

        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "preProcess");
        exit(1);
    }
}
//********************************************************************/
vector<int> TrimOligos::findForward(Sequence& seq, int& primerStart, int& primerEnd){
    try {

        string rawSequence = seq.getUnaligned();
        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX); //no matches found

        if (primers.size() == 0) {  return success; }

        for(map<string,int>::iterator it=primers.begin();it!=primers.end();it++){
            string oligo = it->first;

            if(rawSequence.length() < oligo.length()) { break; }

            //search for primer
            int olength = oligo.length();
            for (int j = 0; j < rawSequence.length()-olength; j++){
                if (m->getControl_pressed()) { primerStart = 0; primerEnd = 0; return success; }
                string rawChunk = rawSequence.substr(j, olength);

                if(compareDNASeq(oligo, rawChunk)) {
                    primerStart = j;
                    primerEnd = primerStart + olength;
                    success[0] = 0;
                    success[1] = 0;
                    return success;
                }

            }
        }

        primerStart = 0; primerEnd = 0;
        //if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;

            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFPrimerLength+pdiffs+1));

            for(map<string,int>::iterator it=primers.begin();it!=primers.end();it++){

                string prim = it->first;
                //search for primer
                int olength = prim.length();
                if (rawSequence.length() < olength+pdiffs) {} //ignore primers too long for this seq
                else{
                    for (int j = 0; j < rawSequence.length()-(olength+pdiffs); j++){

                        string oligo = it->first;

                        if (m->getControl_pressed()) { delete alignment; primerStart = 0; primerEnd = 0; return success; }

                        string rawChunk = rawSequence.substr(j, olength+pdiffs);

                        //if (m->getDebug()) { m->mothurOut(oligo + '\n' + rawChunk + '\n');  }

                        //use needleman to align first primer.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawChunk);
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();

                        int alnLength = oligo.length();

                        //if (m->getDebug()) { m->mothurOut(oligo + '\n' + temp + '\n');  }

                        for(int i=oligo.length()-1;i>=0;i--){
                            if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                        }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);

                        //if (m->getDebug()) { m->mothurOut(oligo + '\n' + temp + '\n');  }

                        int numDiff = countDiffs(oligo, temp);

                        if (m->getDebug()) {
                            if (numDiff < minDiff) {
                                m->mothurOut("[DEBUG]: location = " + toString(j+1) + "  " + oligo + '\n' + temp + '\n' + "diffs = " + toString(numDiff) + '\n');
                            }
                        }

                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            primerStart = j;
                            primerEnd = primerStart + alnLength;
                        }else if(numDiff == minDiff){ minCount++; }
                    }
                }
            }

            delete alignment;

            if(minDiff > pdiffs)	{	primerStart = 0; primerEnd = 0; success[0] = minDiff;  success[1] = MOTHURMAX; return success;	}	//no good matches
            else if(minCount > 1)	{	minDiff = MOTHURMAX; primerStart = 0; primerEnd = 0; success[0] = minDiff; success[1] = pdiffs + 10000; return success;	}	//can't tell the difference between multiple primers
            else{  success[0] = minDiff; success[1] = 0; return success; }
        }

        return success;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripForward");
        exit(1);
    }
}
//******************************************************************/
vector<int> TrimOligos::findReverse(Sequence& seq, int& primerStart, int& primerEnd){
    try {

        string rawSequence = seq.getUnaligned();
        int maxRevPrimerLength = revPrimer[0].length();
        vector<int> success;
        success.push_back(rdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX); //no matches found

        if (revPrimer.size() == 0) { return success; }

        for(int i=0;i<revPrimer.size();i++){
            string oligo = revPrimer[i];
            if(rawSequence.length() < oligo.length()) { break; }

            if (oligo.length() > maxRevPrimerLength) { maxRevPrimerLength = oligo.length(); }

            //search for primer
            int olength = oligo.length();
            for (int j = rawSequence.length()-olength; j >= 0; j--){
                if (m->getControl_pressed()) { primerStart = 0; primerEnd = 0; return success; }
                string rawChunk = rawSequence.substr(j, olength);

                if(compareDNASeq(oligo, rawChunk)) {
                    primerStart = j;
                    primerEnd = primerStart + olength;

                    success[0] = 0;
                    success[1] = 0;
                    return success;
                }

            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((rdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRevPrimerLength+rdiffs+1));

            //can you find the revPrimer
            int minDiff = MOTHURMAX; int minCount = 1;

            string rawRSequence = reverseOligo(seq.getUnaligned());

            for(int i=0;i<revPrimer.size();i++){

                if (rawSequence.length() < revPrimer[i].length()+rdiffs) {} //ignore primers too long for this seq
                else{
                    //undefined if not forced into an int.
                    int stopSpot = rawRSequence.length()-(revPrimer[i].length()+rdiffs);

                    for (int j = 0; j < stopSpot; j++){

                        string oligo = reverseOligo(revPrimer[i]);
                        string rawChunk = rawRSequence.substr(j,oligo.length()+rdiffs);

                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawChunk);
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();


                        int alnLength = oligo.length();
                        for(int k=oligo.length()-1;k>=0;k--){ if(oligo[k] != '-'){	alnLength = k+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);

                        int numDiff = countDiffs(oligo, temp);
                        if (alnLength == 0) { numDiff = rdiffs + 1000; }

                        ;
                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            primerEnd = rawRSequence.length() - j;
                            primerStart = primerEnd - alnLength;
                        }else if(numDiff == minDiff){
                            minCount++;
                        }
                    }
                }
            }

            delete alignment;

            if(minDiff > rdiffs)	{	primerStart = 0; primerEnd = 0; success[0] = minDiff;  success[1] = MOTHURMAX; return success;	}	//no good matches
            else if(minCount > 1)	{	primerStart = 0; primerEnd = 0; success[0] = MOTHURMAX; success[1] = rdiffs + 10000; return success;	}	//can't tell the difference between multiple primers
            else{  success[0] = minDiff; success[1] = 0; return success; }
        }

        return success;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "findReverse");
        exit(1);
    }
}
//*******************************************************************/
string TrimOligos::getCodeValue(int code, int diffs){
    try {

        string value = "unknown";
        if (code == 0)                      { value = "match"; }
        else if (code == (diffs+10000))     { value = "multipleMatches"; }
        else if (code == MOTHURMAX)               { value = "noMatch"; }
        else if (code == (diffs+1000))      { value = "shortSeq"; }

        return value;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "getCodeValue");
        exit(1);
    }
}
//*******************************************************************/

vector<int> TrimOligos::stripBarcode(Sequence& seq, QualityScores& qual, int& group){
    try {
        vector<int> success;
        if (paired) { success = stripPairedBarcode(seq, qual, group); return success; }

        string rawSequence = seq.getUnaligned();
        success.push_back(bdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX); //no matches found

        if (barcodes.size() == 0) { return success;  }

        //can you find the barcode
        for(map<string,int>::iterator it=barcodes.begin();it!=barcodes.end();it++){
            string oligo = it->first;
            if(rawSequence.length() < oligo.length()){	//let's just assume that the barcodes are the same length
                success[0] = rawSequence.length();
                success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                group = it->second;
                seq.setUnaligned(rawSequence.substr(oligo.length()));

                if(qual.getName() != ""){
                    qual.trimQScores(oligo.length(), -1);
                }

                success[0] = 0; success[1] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((bdiffs == 0) || (success[0] == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFBarcodeLength+bdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minGroup = -1;
            int minPos = 0;

            for(map<string,int>::iterator it=barcodes.begin();it!=barcodes.end();it++){
                string oligo = it->first;
                // int length = oligo.length();

                if(rawSequence.length() < maxFBarcodeLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawSequence.length();
                    success[1] = bdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+bdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);
                int numDiff = countDiffs(oligo, temp);

                if (m->getDebug()) { m->mothurOut("[DEBUG]: " + seq.getName() + " aligned fragment =" + temp + ", barcode =" + oligo + ", numDiffs = " + toString(numDiff) + "\n"); }

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minGroup = it->second;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > bdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else if(minCount > 1)	{	success[0] = MOTHURMAX; success[1] = bdiffs + 10000;	}	//can't tell the difference between multiple barcodes
            else{	//use the best match
                group = minGroup;
                seq.setUnaligned(rawSequence.substr(minPos));

                if(qual.getName() != ""){
                    qual.trimQScores(minPos, -1);
                }
                success[0] = minDiff; success[1] = 0;
            }

            delete alignment;

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripBarcode");
        exit(1);
    }
}
//*******************************************************************/
vector<int> TrimOligos::stripBarcode(Sequence& forwardSeq, Sequence& reverseSeq, int& group){
    try {
        vector<int> success;
        //look for forward barcode
        string rawFSequence = forwardSeq.getUnaligned();
        string rawRSequence = reverseSeq.getUnaligned();
        success.push_back(bdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(bdiffs + 1000);
        success.push_back(MOTHURMAX);

        if ((ifbarcodes.size() == 0) || (irbarcodes.size() == 0)) { return success;  }

        //can you find the forward barcode
        for(map<int,oligosPair>::iterator it=ipbarcodes.begin();it!=ipbarcodes.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            if(rawFSequence.length() < foligo.length()){	//let's just assume that the barcodes are the same length
                success[0] = rawFSequence.length();
                success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                break;
            }
            if(rawRSequence.length() < roligo.length()){	//let's just assume that the barcodes are the same length
                success[2] = rawRSequence.length();
                success[3] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                break;
            }

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length()))) {
                    group = it->first;
                    reverseSeq.setUnaligned(rawRSequence.substr(roligo.length())); //trim reverse
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) {
                    group = it->first;
                    forwardSeq.setUnaligned(rawFSequence.substr(foligo.length())); //trim forward
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length())))) {
                group = it->first;
                forwardSeq.setUnaligned(rawFSequence.substr(foligo.length()));
                reverseSeq.setUnaligned(rawRSequence.substr(roligo.length()));
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((bdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFBarcodeLength+bdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */

            for(map<string, vector<int> >::iterator it=ifbarcodes.begin();it!=ifbarcodes.end();it++){
                string oligo = it->first;

                if(rawFSequence.length() < maxFBarcodeLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawFSequence.length();
                    success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                    break;
                }

                if (oligo != "NONE") {

                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawFSequence.substr(0,oligo.length()+bdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (m->getDebug()) { m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + ", barcode=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                    if (alnLength == 0) { numDiff = bdiffs + 1000; }


                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }
            }


            if(minDiff > bdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else{
                success[0] = minDiff;

                //check for reverse match
                delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRBarcodeLength+bdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                for(map<string, vector<int> >::iterator it=irbarcodes.begin();it!=irbarcodes.end();it++){
                    string oligo = it->first;

                    if(rawRSequence.length() < maxRBarcodeLength){	//let's just assume that the barcodes are the same length
                        success[2] = rawRSequence.length();
                        success[3] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                        break;
                    }

                    if (oligo != "NONE") {
                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+bdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();

                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);

                        if (m->getDebug()) { m->mothurOut("[DEBUG]: reverse " + forwardSeq.getName() + " aligned fragment=" + temp + ", barcode=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                        if (alnLength == 0) { numDiff = bdiffs + 1000; }


                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }

                }

                if(minDiff > bdiffs)	{	success[2] = minDiff;  success[3] = MOTHURMAX;	}	//no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = minRPos[0];
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        forwardSeq.setUnaligned(rawFSequence.substr(fStart));
                        reverseSeq.setUnaligned(rawRSequence.substr(rStart));
                        success[1] = 0; success[2] = minDiff; success[3] = 0;
                    }else { success[1] = bdiffs + 10000; success[2] = minDiff; success[3] = bdiffs + 10000;	}
                }
            }

            delete alignment;
        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripIBarcode");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripIndexBarcode(Sequence& forwardSeq, Sequence& reverseSeq, int& group){
    try {
        vector<int> success;
        //look for forward barcode
        string rawFSequence = forwardSeq.getUnaligned();
        string rawRSequence = reverseSeq.getUnaligned();
        success.push_back(bdiffs + 1000);    //guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(bdiffs + 1000);
        success.push_back(MOTHURMAX);
        //cout << "SEQUENCE " << endl << forwardSeq.getName() << '\n' << rawFSequence << endl << rawRSequence << endl;
        //can you find the forward barcode

        if ((ifbarcodes.size() == 0) || (irbarcodes.size() == 0)) { return success; }

        for(map<int,oligosPair>::iterator it=ipbarcodes.begin();it!=ipbarcodes.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            //if (m->getDebug()) { m->mothurOut("[DEBUG]: " + toString(it->first) + " barcode pair = '" + foligo + " " + roligo + "'\n"); m->mothurOut("[DEBUG]: sequence pair = '" + rawFSequence + " " + rawRSequence + "'\n");}

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length()))) {
                    group = it->first;
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) {
                    group = it->first;
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length())))) {
                if(rawRSequence.length() < roligo.length()){    //let's just assume that the barcodes are the same length
                    success[2] = rawRSequence.length();
                    success[3] = bdiffs + 1000;    //if the sequence is shorter than the barcode then bail out
                    break;
                }

                if(rawFSequence.length() < foligo.length()){    //let's just assume that the barcodes are the same length
                    success[0] = rawFSequence.length();
                    success[1] = bdiffs + 1000;    //if the sequence is shorter than the barcode then bail out
                    break;
                }

                group = it->first;
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((bdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFBarcodeLength+bdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */
            for(map<string, vector<int> >::iterator it=ifbarcodes.begin();it!=ifbarcodes.end();it++){
                string oligo = it->first;
                //cout << "trying forward oligos " << oligo << endl;
                if (oligo != "NONE") {

                    if(rawFSequence.length() < maxFBarcodeLength){    //let's just assume that the barcodes are the same length
                        success[0] = rawFSequence.length();
                        success[1] = bdiffs + 1000;    //if the sequence is shorter than the barcode then bail out
                        break;
                    }

                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawFSequence.substr(0,oligo.length()+bdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){    alnLength = i+1;    break;    } }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (alnLength == 0) { numDiff = bdiffs + 1000; }
                    //if (m->getDebug()) { m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + ", barcode=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                    if (m->getDebug()) {
                        if (numDiff < minDiff) {
                            m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + "\nbarcode=" + oligo + "\nnumDiffs=" + toString(numDiff) + ".\n");
                        }
                    }

                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }
            }


            if(minDiff > bdiffs)    {    success[0] = minDiff;  success[1] = MOTHURMAX;    }    //no good matches
            else{
                success[0] = minDiff; //set forward barcode diffs

                //check for reverse match
                delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRBarcodeLength+bdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                for(map<string, vector<int> >::iterator it=irbarcodes.begin();it!=irbarcodes.end();it++){
                    string oligo = it->first;
                    //cout << "trying reverse oligos " << oligo << endl;
                    if (oligo != "NONE") {

                        if(rawRSequence.length() < maxRBarcodeLength){    //let's just assume that the barcodes are the same length
                            success[2] = rawRSequence.length();
                            success[3] = bdiffs + 1000;    //if the sequence is shorter than the barcode then bail out
                            break;
                        }

                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+bdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();
                        //cout << oligo <<endl;
                        //cout << temp << endl;
                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){    alnLength = i+1;    break;    } }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);
                        if (alnLength == 0) { numDiff = bdiffs + 1000; }

                        if (m->getDebug()) {
                            if (numDiff < minDiff) {
                                m->mothurOut("[DEBUG]: reverse " + reverseSeq.getName() + " aligned fragment=" + temp + "\nbarcode=" + oligo + "\nnumDiffs=" + toString(numDiff) + ".\n");
                            }
                        }

                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }

                }

                if(minDiff > bdiffs)    {    success[2] = minDiff;  success[3] = MOTHURMAX;    }    //no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = minRPos[0];
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        success[1] = 0; success[2] = minDiff; success[3] = 0;
                    }else { minDiff = MOTHURMAX; success[1] = bdiffs + 10000; success[2] = minDiff; success[3] = bdiffs + 10000;    } //too many matches
                }
            }

            delete alignment;
        }
        //exit(1);
        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripIBarcode");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripBarcode(Sequence& forwardSeq, Sequence& reverseSeq, QualityScores& forwardQual, QualityScores& reverseQual, int& group){
    try {
        vector<int> success;
        //look for forward barcode
        string rawFSequence = forwardSeq.getUnaligned();
        string rawRSequence = reverseSeq.getUnaligned();
        success.push_back(bdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(bdiffs + 1000);
        success.push_back(MOTHURMAX);
        //cout << "SEQUENCE " << endl << forwardSeq.getName() << '\n' << rawFSequence << endl << rawRSequence << endl;
        //can you find the forward barcode

        if ((ifbarcodes.size() == 0) || (irbarcodes.size() == 0)) { return success; }

        for(map<int,oligosPair>::iterator it=ipbarcodes.begin();it!=ipbarcodes.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            //if (m->getDebug()) { m->mothurOut("[DEBUG]: " + toString(it->first) + " barcode pair = '" + foligo + " " + roligo + "'\n"); m->mothurOut("[DEBUG]: sequence pair = '" + rawFSequence + " " + rawRSequence + "'\n");}

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length()))) {
                    group = it->first;
                    if (!hasIndex) { //if you are using index file then just matching
                        reverseSeq.setUnaligned(rawRSequence.substr(roligo.length())); //trim reverse
                        reverseQual.trimQScores(roligo.length(), -1);
                    }
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) {
                    group = it->first;
                    if (!hasIndex) { //if you are using index file then just matching
                        forwardSeq.setUnaligned(rawFSequence.substr(foligo.length())); //trim forward
                        forwardQual.trimQScores(foligo.length(), -1);
                    }
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length())))) {
                if(rawRSequence.length() < roligo.length()){	//let's just assume that the barcodes are the same length
                    success[2] = rawRSequence.length();
                    success[3] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                    break;
                }

                if(rawFSequence.length() < foligo.length()){	//let's just assume that the barcodes are the same length
                    success[0] = rawFSequence.length();
                    success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                    break;
                }

                group = it->first;
                if (!hasIndex) { //if you are using index file then just matching
                    forwardSeq.setUnaligned(rawFSequence.substr(foligo.length()));
                    reverseSeq.setUnaligned(rawRSequence.substr(roligo.length()));
                    forwardQual.trimQScores(foligo.length(), -1);
                    reverseQual.trimQScores(roligo.length(), -1);
                }
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((bdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFBarcodeLength+bdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */
            for(map<string, vector<int> >::iterator it=ifbarcodes.begin();it!=ifbarcodes.end();it++){
                string oligo = it->first;

                if (oligo != "NONE") {

                    if(rawFSequence.length() < maxFBarcodeLength){	//let's just assume that the barcodes are the same length
                        success[0] = rawFSequence.length();
                        success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                        break;
                    }

                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawFSequence.substr(0,oligo.length()+bdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (alnLength == 0) { numDiff = bdiffs + 1000; }
                    //if (m->getDebug()) { m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + ", barcode=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                    if (m->getDebug()) {
                        if (numDiff < minDiff) {
                            m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + "\nbarcode=" + oligo + "\nnumDiffs=" + toString(numDiff) + ".\n");
                        }
                    }

                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }
            }


            if(minDiff > bdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else{
                success[0] = minDiff; //set forward barcode diffs

                //check for reverse match
                delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRBarcodeLength+bdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                for(map<string, vector<int> >::iterator it=irbarcodes.begin();it!=irbarcodes.end();it++){
                    string oligo = it->first;

                    if (oligo != "NONE") {

                        if(rawRSequence.length() < maxRBarcodeLength){	//let's just assume that the barcodes are the same length
                            success[2] = rawRSequence.length();
                            success[3] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                            break;
                        }

                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+bdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();
                        //cout << oligo <<endl;
                        //cout << temp << endl;
                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);
                        if (alnLength == 0) { numDiff = bdiffs + 1000; }

                        if (m->getDebug()) {
                            if (numDiff < minDiff) {
                                m->mothurOut("[DEBUG]: reverse " + reverseSeq.getName() + " aligned fragment=" + temp + "\nbarcode=" + oligo + "\nnumDiffs=" + toString(numDiff) + ".\n");
                            }
                        }

                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }

                }

                if(minDiff > bdiffs)	{	success[2] = minDiff;  success[3] = MOTHURMAX;	}	//no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = minRPos[0];
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        if (!hasIndex) { //if you are using index file then just matching
                            forwardSeq.setUnaligned(rawFSequence.substr(fStart));
                            reverseSeq.setUnaligned(rawRSequence.substr(rStart));
                            forwardQual.trimQScores(fStart, -1);
                            reverseQual.trimQScores(rStart, -1);
                        }
                        success[1] = 0; success[2] = minDiff; success[3] = 0;
                    }else { minDiff = MOTHURMAX; success[1] = bdiffs + 10000; success[2] = minDiff; success[3] = bdiffs + 10000;	} //too many matches
                }
            }

            if (alignment != nullptr) { delete alignment; }
        }
        //exit(1);
        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripIBarcode");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripPairedBarcode(Sequence& seq, QualityScores& qual, int& group){
    try {
        vector<int> success;
        int rMinDiff=0, fMinDiff=0;

        //look for forward barcode
        string rawSeq = seq.getUnaligned();

        success.push_back(bdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(bdiffs + 1000);
        success.push_back(MOTHURMAX);

        if ((ifbarcodes.size() == 0) || (irbarcodes.size() == 0)) { return success; }

        //can you find the forward barcode
        for(map<int,oligosPair>::iterator it=ipbarcodes.begin();it!=ipbarcodes.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            if (rawSeq.length() < (foligo.length() + roligo.length())) {
                success[0] = rawSeq.length();
                success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                success[2] = rawSeq.length();
                success[3] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                break;
            }

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawSeq.substr(rawSeq.length()-roligo.length(),roligo.length()))) {
                    group = it->first;
                    seq.setUnaligned(rawSeq.substr(0, (rawSeq.length()-roligo.length()))); //trim reverse
                    if(qual.getName() != ""){ qual.trimQScores(-1, rawSeq.length()-roligo.length()); }
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawSeq.substr(0,foligo.length()))) {
                    group = it->first;
                    seq.setUnaligned(rawSeq.substr(foligo.length())); //trim forward
                    if(qual.getName() != ""){ qual.trimQScores(foligo.length(), -1); }
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawSeq.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawSeq.substr(rawSeq.length()-roligo.length(),roligo.length())))) {
                if(rawSeq.length() < foligo.length()){	//let's just assume that the barcodes are the same length
                    success[0] = rawSeq.length();
                    success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                    break;
                }
                if(rawSeq.length() < roligo.length()){	//let's just assume that the barcodes are the same length
                    success[2] = rawSeq.length();
                    success[3] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                    break;
                }

                group = it->first;
                string trimmedSeq = rawSeq.substr(foligo.length()); //trim forward barcode
                seq.setUnaligned(trimmedSeq.substr(0,(trimmedSeq.length()-roligo.length()))); //trim reverse barcode
                if(qual.getName() != ""){
                    qual.trimQScores(-1, rawSeq.length()-roligo.length());
                    qual.trimQScores(foligo.length(), -1);
                }
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((bdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFBarcodeLength+bdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */

            for(map<string, vector<int> >::iterator it=ifbarcodes.begin();it!=ifbarcodes.end();it++){
                string oligo = it->first;

                if (oligo != "NONE") {
                    if(rawSeq.length() < maxFBarcodeLength){	//let's just assume that the barcodes are the same length
                        success[0] = rawSeq.length();
                        success[1] = bdiffs + 1000;	//if the sequence is shorter than the barcode then bail out
                        break;
                    }

                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawSeq.substr(0,oligo.length()+bdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (alnLength == 0) { numDiff = bdiffs + 1000; }


                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }
            }

            fMinDiff = minDiff;

            if(minDiff > bdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else{
                success[0] = minDiff; //set forward barcode diffs

                //check for reverse match
                 delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRBarcodeLength+bdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                string rawRSequence = reverseOligo(seq.getUnaligned());

                for(map<string, vector<int> >::iterator it=irbarcodes.begin();it!=irbarcodes.end();it++){
                    string oligo = it->first;
                    if (oligo != "NONE") { oligo = reverseOligo(oligo); }

                    if (oligo != "NONE") {

                        if(rawRSequence.length() < maxRBarcodeLength){	//let's just assume that the barcodes are the same length
                            success[2] = rawRSequence.length();
                            success[3] = bdiffs + 1000;
                            break;
                        }

                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+bdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();

                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);
                        if (alnLength == 0) { numDiff = bdiffs + 1000; }


                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }
                }

                if(minDiff > bdiffs)	{	success[2] = minDiff;  success[3] = MOTHURMAX;	}	//no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = rawSeq.length() - minRPos[0];
                        if (fStart > rStart) { foundMatch = false; } //only barcodes not a good sequence
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        string trimmedSeq = rawSeq.substr(0, rStart); //trim reverse barcode
                        seq.setUnaligned(trimmedSeq.substr(fStart)); //trim forward barcode
                        if(qual.getName() != ""){
                            qual.trimQScores(-1, rStart);
                            qual.trimQScores(fStart, -1);
                        }
                        success[1] = 0; success[2] = minDiff; success[3] = 0;

                    }else { minDiff = MOTHURMAX; success[1] = bdiffs + 10000; success[2] = minDiff; success[3] = bdiffs + 10000;	} //too many matches
                }
            }
            rMinDiff = minDiff;

            delete alignment;
        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripPairedBarcode");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripPairedPrimers(Sequence& seq, QualityScores& qual, int& group, bool keepForward){
    try {
        int rMinDiff=0, fMinDiff=0;

        //look for forward
        string rawSeq = seq.getUnaligned();

        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(pdiffs + 1000);
        success.push_back(MOTHURMAX);

        if ((ifprimers.size() == 0) || (irprimers.size() == 0)) { return success; }

        //can you find the forward
        for(map<int,oligosPair>::iterator it=ipprimers.begin();it!=ipprimers.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            if(rawSeq.length() < foligo.length()){	//let's just assume that the barcodes are the same length
                success[0] = rawSeq.length();
                success[1] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                break;
            }
            if(rawSeq.length() < roligo.length()){	//let's just assume that the barcodes are the same length
                success[2] = rawSeq.length();
                success[3] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                break;
            }

            if (rawSeq.length() < (foligo.length() + roligo.length())) {
                success[0] = rawSeq.length();
                success[1] = pdiffs + 1000;
                success[2] = rawSeq.length();
                success[3] = pdiffs + 1000;
                break;
            }

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawSeq.substr(rawSeq.length()-roligo.length(),roligo.length()))) {
                    group = it->first;
                    if (!keepForward) {
                        seq.setUnaligned(rawSeq.substr(0, (rawSeq.length()-roligo.length()))); //trim reverse
                        if(qual.getName() != ""){ qual.trimQScores(-1, rawSeq.length()-roligo.length()); }
                    }
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawSeq.substr(0,foligo.length()))) {
                    group = it->first;
                    if (!keepForward) {
                        seq.setUnaligned(rawSeq.substr(foligo.length())); //trim forward
                        if(qual.getName() != ""){ qual.trimQScores(foligo.length(), -1); }
                    }
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawSeq.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawSeq.substr(rawSeq.length()-roligo.length(),roligo.length())))) {
                group = it->first;
                if (!keepForward) {
                    string trimmedSeq = rawSeq.substr(foligo.length()); //trim forward barcode
                    seq.setUnaligned(trimmedSeq.substr(0,(trimmedSeq.length()-roligo.length()))); //trim reverse barcode
                    if(qual.getName() != ""){
                        qual.trimQScores(-1, rawSeq.length()-roligo.length());
                        qual.trimQScores(foligo.length(), -1);
                    }
                }
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFPrimerLength+pdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */

            for(map<string, vector<int> >::iterator it=ifprimers.begin();it!=ifprimers.end();it++){
                string oligo = it->first;

                if(rawSeq.length() < maxFPrimerLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawSeq.length();
                    success[1] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                    break;
                }

                if (oligo != "NONE") {
                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawSeq.substr(0,oligo.length()+pdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (alnLength == 0) { numDiff = pdiffs + 1000; }


                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }

            }

            fMinDiff = minDiff;


            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else{
                success[0] = minDiff; //set forward primer diffs

                //check for reverse match
                delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRPrimerLength+pdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                string rawRSequence = reverseOligo(seq.getUnaligned());

                for(map<string, vector<int> >::iterator it=irprimers.begin();it!=irprimers.end();it++){
                    string oligo = it->first;
                    if (oligo != "NONE") { oligo = reverseOligo(oligo); }

                    if(rawRSequence.length() < maxRPrimerLength){	//let's just assume that the barcodes are the same length
                        success[2] = rawRSequence.length();
                        success[3] = pdiffs + 1000;
                        break;
                    }

                    if (oligo != "NONE") {
                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+pdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();

                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);
                        if (alnLength == 0) { numDiff = pdiffs + 1000; }


                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }
                }

                if(minDiff > pdiffs)	{	success[2] = minDiff;  success[3] = MOTHURMAX;	}	//no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = rawSeq.length() - minRPos[0];
                        if (fStart > rStart) { foundMatch = false; } //only primers not a good sequence
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        if (!keepForward) {
                            string trimmedSeq = rawSeq.substr(0, rStart); //trim reverse barcode
                            seq.setUnaligned(trimmedSeq.substr(fStart)); //trim forward barcode
                            if(qual.getName() != ""){
                                qual.trimQScores(-1, rStart);
                                qual.trimQScores(fStart, -1);
                            }
                        }
                        success[1] = 0; success[2] = minDiff; success[3] = 0;

                    }else { minDiff = MOTHURMAX; success[1] = pdiffs + 10000; success[2] = minDiff; success[3] = pdiffs + 10000;	} //too many matches
                }
            }

            rMinDiff = minDiff;

            delete alignment;
        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripPairedPrimers");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripForward(Sequence& forwardSeq, Sequence& reverseSeq, QualityScores& forwardQual, QualityScores& reverseQual, int& group){
    try {
        //look for forward barcode
        string rawFSequence = forwardSeq.getUnaligned();
        string rawRSequence = reverseSeq.getUnaligned();

        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(pdiffs + 1000);
        success.push_back(MOTHURMAX);

        if ((ifprimers.size() == 0) || (irprimers.size() == 0)) { return success; }

        //can you find the forward barcode
        for(map<int,oligosPair>::iterator it=ipprimers.begin();it!=ipprimers.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            if(rawFSequence.length() < foligo.length()){	//let's just assume that the barcodes are the same length
                success[0] = rawFSequence.length();
                success[1] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                break;
            }
            if(rawRSequence.length() < roligo.length()){	//let's just assume that the barcodes are the same length
                success[2] = rawRSequence.length();
                success[3] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                break;
            }

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length()))) {
                    group = it->first;
                    reverseSeq.setUnaligned(rawRSequence.substr(roligo.length())); //trim reverse
                    reverseQual.trimQScores(roligo.length(), -1);
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) {
                    group = it->first;
                    forwardSeq.setUnaligned(rawFSequence.substr(foligo.length())); //trim forward
                    forwardQual.trimQScores(foligo.length(), -1);
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length())))) {
                group = it->first;
                forwardSeq.setUnaligned(rawFSequence.substr(foligo.length()));
                reverseSeq.setUnaligned(rawRSequence.substr(roligo.length()));
                forwardQual.trimQScores(foligo.length(), -1);
                reverseQual.trimQScores(roligo.length(), -1);
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFPrimerLength+pdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */

            for(map<string, vector<int> >::iterator it=ifprimers.begin();it!=ifprimers.end();it++){
                string oligo = it->first;

                if(rawFSequence.length() < maxFPrimerLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawFSequence.length();
                    success[1] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                    break;
                }

                if (oligo != "NONE") {
                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawFSequence.substr(0,oligo.length()+pdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (alnLength == 0) { numDiff = pdiffs + 1000; }


                    if (m->getDebug()) { m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + ", primer=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }
            }


            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else{
                success[0] = minDiff; //set forward primer diffs

                //check for reverse match
                delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRPrimerLength+pdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                for(map<string, vector<int> >::iterator it=irprimers.begin();it!=irprimers.end();it++){
                    string oligo = it->first;

                    if(rawRSequence.length() < maxRPrimerLength){	//let's just assume that the barcodes are the same length
                        success[2] = rawRSequence.length();
                        success[3] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                        break;
                    }

                    if (oligo != "NONE") {
                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+pdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();

                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);
                        if (alnLength == 0) { numDiff = pdiffs + 1000; }

                        if (m->getDebug()) { m->mothurOut("[DEBUG]: reverse " + forwardSeq.getName() + " aligned fragment=" + temp + ", primer=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }
                }

                if(minDiff > pdiffs)	{	success[2] = minDiff;  success[3] = MOTHURMAX;	}	//no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = minRPos[0];
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        forwardSeq.setUnaligned(rawFSequence.substr(fStart));
                        reverseSeq.setUnaligned(rawRSequence.substr(rStart));
                        forwardQual.trimQScores(fStart, -1);
                        reverseQual.trimQScores(rStart, -1);
                        success[1] = 0; success[2] = minDiff; success[3] = 0;
                    }else { minDiff = MOTHURMAX; success[1] = pdiffs + 10000; success[2] = minDiff; success[3] = pdiffs + 10000;	} //too many matches

                }
            }

            delete alignment;
        }

        return success;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripIForward");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripForward(Sequence& forwardSeq, Sequence& reverseSeq, int& group){
    try {
        //look for forward barcode
        string rawFSequence = forwardSeq.getUnaligned();
        string rawRSequence = reverseSeq.getUnaligned();

        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);
        success.push_back(pdiffs + 1000);
        success.push_back(MOTHURMAX);

        if ((ifprimers.size() == 0) || (irprimers.size() == 0)) { return success; }

        //can you find the forward barcode
        for(map<int,oligosPair>::iterator it=ipprimers.begin();it!=ipprimers.end();it++){
            string foligo = it->second.forward;
            string roligo = it->second.reverse;

            if(rawFSequence.length() < foligo.length()){	//let's just assume that the barcodes are the same length
                success[0] = rawFSequence.length();
                success[1] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                break;
            }
            if(rawRSequence.length() < roligo.length()){	//let's just assume that the barcodes are the same length
                success[2] = rawRSequence.length();
                success[3] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                break;
            }

            if (foligo == "NONE") {
                if (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length()))) {
                    group = it->first;
                    reverseSeq.setUnaligned(rawRSequence.substr(roligo.length())); //trim reverse
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if (roligo == "NONE") {
                if (compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) {
                    group = it->first;
                    forwardSeq.setUnaligned(rawFSequence.substr(foligo.length())); //trim forward
                    success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                    break;
                }
            }else if((compareDNASeq(foligo, rawFSequence.substr(0,foligo.length()))) && (compareDNASeq(roligo, rawRSequence.substr(0,roligo.length())))) {
                group = it->first;
                forwardSeq.setUnaligned(rawFSequence.substr(foligo.length()));
                reverseSeq.setUnaligned(rawRSequence.substr(roligo.length()));
                success[0] = 0; success[1] = 0; success[2] = 0; success[3] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }
        else { //try aligning and see if you can find it
            Alignment* alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFPrimerLength+pdiffs+1));

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            vector< vector<int> > minFGroup;
            vector<int> minFPos;

            //the pair can have duplicates, but we only want to search for the unique forward and reverses, example
            /*
             1 Sarah Westcott
             2 John Westcott
             3 Anna Westcott
             4 Sarah Schloss
             5 Pat Schloss
             6 Gail Brown
             7 Pat Moore
             only want to look for forward = Sarah, John, Anna, Pat, Gail
             reverse = Westcott, Schloss, Brown, Moore
             but if best match forward = 4, and reverse = 1, we want to count as a valid match because forward 1 and forward 4 are the same. so both barcodes map to same group.
             */

            for(map<string, vector<int> >::iterator it=ifprimers.begin();it!=ifprimers.end();it++){
                string oligo = it->first;

                if(rawFSequence.length() < maxFPrimerLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawFSequence.length();
                    success[1] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                    break;
                }

                if (oligo != "NONE") {
                    //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                    alignment->alignPrimer(oligo, rawFSequence.substr(0,oligo.length()+pdiffs));
                    oligo = alignment->getSeqAAln();
                    string temp = alignment->getSeqBAln();

                    int alnLength = oligo.length();

                    for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                    oligo = oligo.substr(0,alnLength);
                    temp = temp.substr(0,alnLength);
                    int numDiff = countDiffs(oligo, temp);

                    if (m->getDebug()) { m->mothurOut("[DEBUG]: forward " + forwardSeq.getName() + " aligned fragment=" + temp + ", primer=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                    if (alnLength == 0) { numDiff = pdiffs + 1000; }


                    if(numDiff < minDiff){
                        minDiff = numDiff;
                        minCount = 1;
                        minFGroup.clear();
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        minFPos.clear();
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }else if(numDiff == minDiff){
                        minFGroup.push_back(it->second);
                        int tempminFPos = 0;
                        for(int i=0;i<alnLength;i++){
                            if(temp[i] != '-'){
                                tempminFPos++;
                            }
                        }
                        minFPos.push_back(tempminFPos);
                    }
                }else { //is a match
                    minFGroup.push_back(it->second);
                    minFPos.push_back(0);
                    minDiff = 0;
                }
            }


            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else{
                success[0] = minDiff; //set forward primer diffs

                //check for reverse match
                delete alignment;
                alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRPrimerLength+pdiffs+1));

                //can you find the barcode
                minDiff = MOTHURMAX;
                minCount = 1;
                vector< vector<int> > minRGroup;
                vector<int> minRPos;

                for(map<string, vector<int> >::iterator it=irprimers.begin();it!=irprimers.end();it++){
                    string oligo = it->first;

                    if(rawRSequence.length() < maxRPrimerLength){	//let's just assume that the barcodes are the same length
                        success[2] = rawRSequence.length();
                        success[3] = pdiffs + 1000;	//if the sequence is shorter than the primer then bail out
                        break;
                    }

                    if (oligo != "NONE") {
                        //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                        alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+pdiffs));
                        oligo = alignment->getSeqAAln();
                        string temp = alignment->getSeqBAln();

                        int alnLength = oligo.length();
                        for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }
                        oligo = oligo.substr(0,alnLength);
                        temp = temp.substr(0,alnLength);
                        int numDiff = countDiffs(oligo, temp);

                        if (m->getDebug()) { m->mothurOut("[DEBUG]: reverse " + forwardSeq.getName() + " aligned fragment=" + temp + ", primer=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                        if (alnLength == 0) { numDiff = pdiffs + 1000; }


                        if(numDiff < minDiff){
                            minDiff = numDiff;
                            minCount = 1;
                            minRGroup.clear();
                            minRGroup.push_back(it->second);
                            int tempminRPos = 0;
                            minRPos.clear();
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                        }else if(numDiff == minDiff){
                            int tempminRPos = 0;
                            for(int i=0;i<alnLength;i++){
                                if(temp[i] != '-'){
                                    tempminRPos++;
                                }
                            }
                            minRPos.push_back(tempminRPos);
                            minRGroup.push_back(it->second);
                        }
                    }else { //is a match
                        minRPos.push_back(0);
                        minRGroup.push_back(it->second);
                        minDiff = 0;
                    }

                }

                if(minDiff > pdiffs)	{	success[2] = minDiff;  success[3] = MOTHURMAX;	}	//no good matches
                else {
                    bool foundMatch = false;
                    vector<int> matches;
                    for (int i = 0; i < minFGroup.size(); i++) {
                        for (int j = 0; j < minFGroup[i].size(); j++) {
                            for (int k = 0; k < minRGroup.size(); k++) {
                                if (util.inUsersGroups(minFGroup[i][j], minRGroup[k])) { matches.push_back(minFGroup[i][j]); k+= minRGroup.size(); }
                            }
                        }
                    }

                    int fStart = 0;
                    int rStart = 0;
                    if (matches.size() == 1) {
                        foundMatch = true;
                        group = matches[0];
                        fStart = minFPos[0];
                        rStart = minRPos[0];
                    }

                    //we have an acceptable match for the forward and reverse, but do they match?
                    if (foundMatch) {
                        forwardSeq.setUnaligned(rawFSequence.substr(fStart));
                        reverseSeq.setUnaligned(rawRSequence.substr(rStart));
                        success[1] = 0; success[2] = minDiff; success[3] = 0;
                    }else { minDiff = MOTHURMAX; success[1] = pdiffs + 10000; success[2] = minDiff; success[3] = pdiffs + 10000;	} //too many matches
                }
            }

            delete alignment;
        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripIForward");
        exit(1);
    }

}
//*******************************************************************/
vector<int> TrimOligos::stripBarcode(Sequence& seq, int& group){
    try {

        string rawSequence = seq.getUnaligned();

        vector<int> success;
        success.push_back(bdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);

        //can you find the barcode
        for(map<string,int>::iterator it=barcodes.begin();it!=barcodes.end();it++){
            string oligo = it->first;
            if(rawSequence.length() < oligo.length()){	//let's just assume that the barcodes are the same length
                success[0] = rawSequence.length();
                success[1] = bdiffs + 1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                group = it->second;
                seq.setUnaligned(rawSequence.substr(oligo.length()));
                success[0] = 0; success[1] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((bdiffs == 0) || (success[0] == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (barcodes.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFBarcodeLength+bdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minGroup = -1;
            int minPos = 0;

            for(map<string,int>::iterator it=barcodes.begin();it!=barcodes.end();it++){
                string oligo = it->first;
                // int length = oligo.length();

                if(rawSequence.length() < maxFBarcodeLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawSequence.length();
                    success[1] = bdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+bdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if (m->getDebug()) { m->mothurOut("[DEBUG]: " + seq.getName() + " aligned fragment =" + temp + ", barcode =" + oligo + ", numDiffs = " + toString(numDiff) + "\n"); }

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minGroup = it->second;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > bdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else if(minCount > 1)	{	success[0] = MOTHURMAX; success[1] = bdiffs + 10000;	}	//can't tell the difference between multiple barcodes
            else{	//use the best match
                group = minGroup;
                seq.setUnaligned(rawSequence.substr(minPos));
                success[0] = minDiff; success[1] = 0;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripBarcode");
        exit(1);
    }

}

/********************************************************************/
vector<int> TrimOligos::stripForward(Sequence& seq, int& group){
    try {
        string rawSequence = seq.getUnaligned();
        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);

        //can you find the primer
        for(map<string,int>::iterator it=primers.begin();it!=primers.end();it++){
            string oligo = it->first;
            if(rawSequence.length() < oligo.length()){	//let's just assume that the primers are the same length
                success[0] = rawSequence.length();
                success[1] = pdiffs + 1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                group = it->second;
                seq.setUnaligned(rawSequence.substr(oligo.length()));
                success[0] = 0; success[1] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) {	return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (primers.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFPrimerLength+pdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minGroup = -1;
            int minPos = 0;

            for(map<string,int>::iterator it=primers.begin();it!=primers.end();it++){
                string oligo = it->first;
                // int length = oligo.length();

                if(rawSequence.length() < maxFPrimerLength){
                    success[0] = rawSequence.length();
                    success[1] = pdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+pdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if (m->getDebug()) { m->mothurOut("[DEBUG]: " + seq.getName() + " aligned fragment=" + temp + ", primer=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minGroup = it->second;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else if(minCount > 1)	{	minDiff = MOTHURMAX; success[0] = minDiff; success[1] = pdiffs + 10000;	}	//can't tell the difference between multiple primers
            else{	//use the best match
                group = minGroup;
                seq.setUnaligned(rawSequence.substr(minPos));
                success[0] = minDiff; success[1] = 0;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripForward");
        exit(1);
    }
}
//*******************************************************************/
vector<int> TrimOligos::stripForward(Sequence& seq, QualityScores& qual, int& group, bool keepForward){
    try {

        vector<int> success;

        if (paired) { success = stripPairedPrimers(seq, qual, group, keepForward); return success; }

        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);

        string rawSequence = seq.getUnaligned();

        //can you find the primer
        for(map<string,int>::iterator it=primers.begin();it!=primers.end();it++){
            string oligo = it->first;
            if(rawSequence.length() < oligo.length()){	//let's just assume that the primers are the same length
                success[0] = rawSequence.length();
                success[1] = pdiffs + 1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                group = it->second;
                if (!keepForward) { seq.setUnaligned(rawSequence.substr(oligo.length())); }
                if(qual.getName() != ""){
                    if (!keepForward) { qual.trimQScores(oligo.length(), -1); }
                }
                success[0] = 0; success[1] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (primers.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxFPrimerLength+pdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minGroup = -1;
            int minPos = 0;

            for(map<string,int>::iterator it=primers.begin();it!=primers.end();it++){
                string oligo = it->first;
                // int length = oligo.length();

                if(rawSequence.length() < maxFPrimerLength){
                    success[0] = rawSequence.length();
                    success[1] = pdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+pdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if (m->getDebug()) { m->mothurOut("[DEBUG]: " + seq.getName() + " aligned fragment=" + temp + ", primer=" + oligo + ", numDiffs=" + toString(numDiff) + ".\n");  }

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minGroup = it->second;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else if(minCount > 1)	{	minDiff = MOTHURMAX; success[0] = minDiff; success[1] = pdiffs + 10000;	}//no good matches
            else{	//use the best match
                group = minGroup;
                if (!keepForward) { seq.setUnaligned(rawSequence.substr(minPos)); }
                if(qual.getName() != ""){
                    if (!keepForward) { qual.trimQScores(minPos, -1); }
                }
                success[0] = minDiff; success[1] = 0;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripForward");
        exit(1);
    }
}
//******************************************************************/
vector<int> TrimOligos::stripReverse(Sequence& seq, QualityScores& qual){
    try {
        string rawSequence = seq.getUnaligned();

        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);

        int maxRevPrimerLength = revPrimer[0].length();

        for(int i=0;i<revPrimer.size();i++){
            string oligo = revPrimer[i];
            if (oligo.length() > maxRevPrimerLength) { maxRevPrimerLength = oligo.length(); }

            if(rawSequence.length() < oligo.length()){
                success[0] = rawSequence.length();
                success[1] = pdiffs + 1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(rawSequence.length()-oligo.length(),oligo.length()))){
                seq.setUnaligned(rawSequence.substr(0,rawSequence.length()-oligo.length()));
                if(qual.getName() != ""){
                    qual.trimQScores(-1, rawSequence.length()-oligo.length());
                }
                success[0] = 0; success[1] = 0;
                break;
            }
        }
        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (revPrimer.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRevPrimerLength+pdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the revPrimer
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minGroup = -1;
            int minPos = 0;

            string rawRSequence = reverseOligo(seq.getUnaligned());

            for(int i=0;i<revPrimer.size();i++){
                string oligo = reverseOligo(revPrimer[i]);

                if(rawRSequence.length() < maxRevPrimerLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawRSequence.length();
                    success[1] = pdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawRSequence.substr(0,oligo.length()+pdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();


                int alnLength = oligo.length();
                for(int j=oligo.length()-1;j>=0;j--){ if(oligo[j] != '-'){	alnLength = j+1;	break;	} }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);
                int numDiff = countDiffs(oligo, temp);
                if (alnLength == 0) { numDiff = pdiffs + 1000; }


                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minGroup = i;
                    for(int j=0;j<alnLength;j++){
                        if(temp[j] != '-'){
                            minPos++;
                        }
                    }
                }else if(numDiff == minDiff){
                    minCount++;
                }
            }

            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else if(minCount > 1)	{	minDiff = MOTHURMAX; success[0] = minDiff; success[1] = pdiffs + 10000;	}	//can't tell the difference between multiple primers
            else{	//use the best match
                seq.setUnaligned(rawSequence.substr(0, (rawSequence.length() - minPos)));
                if(qual.getName() != ""){
                    qual.trimQScores(-1, (rawSequence.length() - minPos));
                }
                success[0] = minDiff; success[1] = 0;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripReverse");
        exit(1);
    }
}
//******************************************************************/
vector<int> TrimOligos::stripReverse(Sequence& seq){
    try {

        string rawSequence = seq.getUnaligned();

        vector<int> success;
        success.push_back(pdiffs + 1000);	//guilty until proven innocent
        success.push_back(MOTHURMAX);

        int maxRevPrimerLength = revPrimer[0].length();

        for(int i=0;i<revPrimer.size();i++){
            string oligo = revPrimer[i];

            if (oligo.length() > maxRevPrimerLength) { maxRevPrimerLength = oligo.length(); }

            if(rawSequence.length() < oligo.length()){
                success[0] = rawSequence.length();
                success[1] = pdiffs + 1000;
                break;
            }

            string rawSeqFragment = rawSequence.substr(rawSequence.length()-oligo.length(),oligo.length());
            if(compareDNASeq(oligo, rawSeqFragment)){
                seq.setUnaligned(rawSequence.substr(0,rawSequence.length()-oligo.length()));
                success[0] = 0; success[1] = 0;
                break;
            }
        }

        //if you found the barcode or if you don't want to allow for diffs
        if ((pdiffs == 0) || (success[0] == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (revPrimer.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxRevPrimerLength+pdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the revPrimer
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minGroup = -1;
            int minPos = 0;

            string rawRSequence = reverseOligo(seq.getUnaligned());

            for(int i=0;i<revPrimer.size();i++){
                string oligo = reverseOligo(revPrimer[i]);

                if(rawRSequence.length() < maxRevPrimerLength){	//let's just assume that the barcodes are the same length
                    success[0] = rawRSequence.length();
                    success[1] = pdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawRSequence.substr(0, oligo.length()+pdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();
                for(int j=oligo.length()-1;j>=0;j--){ if(oligo[j] != '-'){	alnLength = j+1;	break;	} }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);
                int numDiff = countDiffs(oligo, temp);
                if (alnLength == 0) { numDiff = pdiffs + 1000; }

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minGroup = i;
                    for(int j=0;j<alnLength;j++){
                        if(temp[j] != '-'){
                            minPos++;
                        }
                    }
                }else if(numDiff == minDiff){
                    minCount++;
                }
            }

            if(minDiff > pdiffs)	{	success[0] = minDiff;  success[1] = MOTHURMAX;	}	//no good matches
            else if(minCount > 1)	{	minDiff = MOTHURMAX; success[0] = minDiff; success[1] = pdiffs + 10000;	}	//can't tell the difference between multiple primers
            else{	//use the best match
                seq.setUnaligned(rawSequence.substr(0, (rawSequence.length() - minPos)));
                success[0] = minDiff; success[1] = 0;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;


    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripReverse");
        exit(1);
    }
}
//******************************************************************/
int TrimOligos::stripLinker(Sequence& seq, QualityScores& qual){
    try {
        string rawSequence = seq.getUnaligned();
        int success = ldiffs + 1;	//guilty until proven innocent

        for(int i=0;i<linker.size();i++){
            string oligo = linker[i];

            if(rawSequence.length() < oligo.length()){
                success = ldiffs + 1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                seq.setUnaligned(rawSequence.substr(oligo.length()));
                if(qual.getName() != ""){ qual.trimQScores(oligo.length(), -1);  }
                success = 0;
                break;
            }
        }

        //if you found the linker or if you don't want to allow for diffs
        if ((ldiffs == 0) || (success == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (linker.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxLinkerLength+ldiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minPos = 0;

            for(int i = 0; i < linker.size(); i++){
                string oligo = linker[i];
                // int length = oligo.length();

                if(rawSequence.length() < maxLinkerLength){	//let's just assume that the barcodes are the same length
                    success = ldiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+ldiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > ldiffs)	{	success = minDiff;	}	//no good matches
            else if(minCount > 1)	{	success = ldiffs + 10000;	}	//can't tell the difference between multiple barcodes
            else{	//use the best match
                seq.setUnaligned(rawSequence.substr(minPos));

                if(qual.getName() != ""){
                    qual.trimQScores(minPos, -1);
                }
                success = minDiff;
            }

            if (alignment != nullptr) { delete alignment; }

        }


        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripLinker");
        exit(1);
    }
}
//******************************************************************/
int TrimOligos::stripLinker(Sequence& seq){
    try {

        string rawSequence = seq.getUnaligned();
        int success = ldiffs +1;	//guilty until proven innocent

        for(int i=0;i<linker.size();i++){
            string oligo = linker[i];

            if(rawSequence.length() < oligo.length()){
                success = ldiffs +1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                seq.setUnaligned(rawSequence.substr(oligo.length()));
                success = 0;
                break;
            }
        }

        //if you found the linker or if you don't want to allow for diffs
        if ((ldiffs == 0) || (success == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (linker.size() > 0) {alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxLinkerLength+ldiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minPos = 0;

            for(int i = 0; i < linker.size(); i++){
                string oligo = linker[i];
                // int length = oligo.length();

                if(rawSequence.length() < maxLinkerLength){ success = ldiffs + 1000; break; }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                string temp = rawSequence.substr(0,oligo.length()+ldiffs);
                alignment->alignPrimer(oligo, temp);
                oligo = alignment->getSeqAAln();
                temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){ if(oligo[i] != '-'){	alnLength = i+1;	break;	} }

                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > ldiffs)	{	success = minDiff;	}	//no good matches
            else if(minCount > 1)	{	success = ldiffs + 10000;	}	//can't tell the difference between multiple barcodes
            else{	//use the best match
                seq.setUnaligned(rawSequence.substr(minPos));
                success = minDiff;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripLinker");
        exit(1);
    }
}

//******************************************************************/
int TrimOligos::stripSpacer(Sequence& seq, QualityScores& qual){
    try {
        string rawSequence = seq.getUnaligned();
        int success = sdiffs+1;	//guilty until proven innocent

        for(int i=0;i<spacer.size();i++){
            string oligo = spacer[i];

            if(rawSequence.length() < oligo.length()){
                success = sdiffs+1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                seq.setUnaligned(rawSequence.substr(oligo.length()));
                if(qual.getName() != ""){
                    qual.trimQScores(oligo.length(), -1);
                }
                success = 0;
                break;
            }
        }

        //if you found the spacer or if you don't want to allow for diffs
        if ((sdiffs == 0) || (success == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (spacer.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxSpacerLength+sdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minPos = 0;

            for(int i = 0; i < spacer.size(); i++){
                string oligo = spacer[i];
                // int length = oligo.length();

                if(rawSequence.length() < maxSpacerLength){	//let's just assume that the barcodes are the same length
                    success = sdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+sdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > sdiffs)	{	success = minDiff;	}	//no good matches
            else if(minCount > 1)	{	success = sdiffs + 10000;	}	//can't tell the difference between multiple barcodes
            else{	//use the best match
                seq.setUnaligned(rawSequence.substr(minPos));

                if(qual.getName() != ""){
                    qual.trimQScores(minPos, -1);
                }
                success = minDiff;
            }

            if (alignment != nullptr) { delete alignment; }

        }


        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripSpacer");
        exit(1);
    }
}
//******************************************************************/
int TrimOligos::stripSpacer(Sequence& seq){
    try {

        string rawSequence = seq.getUnaligned();
        int success = sdiffs+1;	//guilty until proven innocent

        for(int i=0;i<spacer.size();i++){
            string oligo = spacer[i];

            if(rawSequence.length() < oligo.length()){
                success = sdiffs+1000;
                break;
            }

            if(compareDNASeq(oligo, rawSequence.substr(0,oligo.length()))){
                seq.setUnaligned(rawSequence.substr(oligo.length()));
                success = 0;
                break;
            }
        }

        //if you found the spacer or if you don't want to allow for diffs
        if ((sdiffs == 0) || (success == 0)) { return success; }

        else { //try aligning and see if you can find it
            Alignment* alignment;
            if (spacer.size() > 0) { alignment = new NeedlemanOverlap(-1.0, 1.0, -1.0, (maxSpacerLength+sdiffs+1)); }
            else{ alignment = nullptr; }

            //can you find the barcode
            int minDiff = MOTHURMAX;
            int minCount = 1;
            int minPos = 0;

            for(int i = 0; i < spacer.size(); i++){
                string oligo = spacer[i];
                // int length = oligo.length();

                if(rawSequence.length() < maxSpacerLength){	//let's just assume that the barcodes are the same length
                    success = sdiffs + 1000;
                    break;
                }

                //use needleman to align first barcode.length()+numdiffs of sequence to each barcode
                alignment->alignPrimer(oligo, rawSequence.substr(0,oligo.length()+sdiffs));
                oligo = alignment->getSeqAAln();
                string temp = alignment->getSeqBAln();

                int alnLength = oligo.length();

                for(int i=oligo.length()-1;i>=0;i--){
                    if(oligo[i] != '-'){	alnLength = i+1;	break;	}
                }
                oligo = oligo.substr(0,alnLength);
                temp = temp.substr(0,alnLength);

                int numDiff = countDiffs(oligo, temp);

                if(numDiff < minDiff){
                    minDiff = numDiff;
                    minCount = 1;
                    minPos = 0;
                    for(int i=0;i<alnLength;i++){
                        if(temp[i] != '-'){
                            minPos++;
                        }
                    }
                }
                else if(numDiff == minDiff){
                    minCount++;
                }

            }

            if(minDiff > sdiffs)	{	success = minDiff;	}	//no good matches
            else if(minCount > 1)	{	success = sdiffs + 10000;	}	//can't tell the difference between multiple barcodes
            else{	//use the best match
                seq.setUnaligned(rawSequence.substr(minPos));
                success = minDiff;
            }

            if (alignment != nullptr) { delete alignment; }

        }

        return success;

    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "stripSpacer");
        exit(1);
    }
}

//******************************************************************/
bool TrimOligos::compareDNASeq(string oligo, string seq){
    try {
        bool success = true;
        int length = oligo.length();

        for(int i=0;i<length;i++){

            if(oligo[i] != seq[i]){
                if(oligo[i] == 'A' || oligo[i] == 'T' || oligo[i] == 'G' || oligo[i] == 'C')	{	success = false; }
                else if((oligo[i] == 'N' || oligo[i] == 'I') && (seq[i] == 'N'))	{	success = false;	}
                else if(oligo[i] == 'R' && (seq[i] != 'A' && seq[i] != 'G'))	{	success = false;	}
                else if(oligo[i] == 'Y' && (seq[i] != 'C' && seq[i] != 'T'))	{	success = false;	}
                else if(oligo[i] == 'M' && (seq[i] != 'C' && seq[i] != 'A'))	{	success = false;	}
                else if(oligo[i] == 'K' && (seq[i] != 'T' && seq[i] != 'G'))	{	success = false;	}
                else if(oligo[i] == 'W' && (seq[i] != 'T' && seq[i] != 'A'))	{	success = false;	}
                else if(oligo[i] == 'S' && (seq[i] != 'C' && seq[i] != 'G'))	{	success = false;	}
                else if(oligo[i] == 'B' && (seq[i] != 'C' && seq[i] != 'T' && seq[i] != 'G'))	{	success = false;	}
                else if(oligo[i] == 'D' && (seq[i] != 'A' && seq[i] != 'T' && seq[i] != 'G'))	{	success = false;	}
                else if(oligo[i] == 'H' && (seq[i] != 'A' && seq[i] != 'T' && seq[i] != 'C'))	{	success = false;	}
                else if(oligo[i] == 'V' && (seq[i] != 'A' && seq[i] != 'C' && seq[i] != 'G'))	{	success = false;	}

                if(success == false)	{	break;	}
            }
            else{ success = true; }
        }

        return success;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "compareDNASeq");
        exit(1);
    }

}
//********************************************************************/
int TrimOligos::countDiffs(string oligo, string seq){
    try {

        int length = oligo.length();
        int countDiffs = 0;

        for(int i=0;i<length;i++){

            if(oligo[i] != seq[i]){
                if(oligo[i] == 'A' || oligo[i] == 'T' || oligo[i] == 'G' || oligo[i] == 'C' || oligo[i] == '-' || oligo[i] == '.')	{	countDiffs++; }
                else if((oligo[i] == 'N' || oligo[i] == 'I') && (seq[i] == 'N'))	{	countDiffs++;	}
                else if(oligo[i] == 'R' && (seq[i] != 'A' && seq[i] != 'G'))	{	countDiffs++;	}
                else if(oligo[i] == 'Y' && (seq[i] != 'C' && seq[i] != 'T'))	{	countDiffs++;	}
                else if(oligo[i] == 'M' && (seq[i] != 'C' && seq[i] != 'A'))	{	countDiffs++;	}
                else if(oligo[i] == 'K' && (seq[i] != 'T' && seq[i] != 'G'))	{	countDiffs++;	}
                else if(oligo[i] == 'W' && (seq[i] != 'T' && seq[i] != 'A'))	{	countDiffs++;	}
                else if(oligo[i] == 'S' && (seq[i] != 'C' && seq[i] != 'G'))	{	countDiffs++;	}
                else if(oligo[i] == 'B' && (seq[i] != 'C' && seq[i] != 'T' && seq[i] != 'G'))	{	countDiffs++;	}
                else if(oligo[i] == 'D' && (seq[i] != 'A' && seq[i] != 'T' && seq[i] != 'G'))	{	countDiffs++;	}
                else if(oligo[i] == 'H' && (seq[i] != 'A' && seq[i] != 'T' && seq[i] != 'C'))	{	countDiffs++;	}
                else if(oligo[i] == 'V' && (seq[i] != 'A' && seq[i] != 'C' && seq[i] != 'G'))	{	countDiffs++;	}
            }

        }

        return countDiffs;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "countDiffs");
        exit(1);
    }
}
//********************************************************************/
string TrimOligos::reverseOligo(string oligo){
    try {
        string reverse = "";

        for(int i=oligo.length()-1;i>=0;i--){

            if(oligo[i] == 'A')	{	reverse += 'T';	}
            else if(oligo[i] == 'T'){	reverse += 'A';	}
            else if(oligo[i] == 'U'){	reverse += 'A';	}

            else if(oligo[i] == 'G'){	reverse += 'C';	}
            else if(oligo[i] == 'C'){	reverse += 'G';	}

            else if(oligo[i] == 'R'){	reverse += 'Y';	}
            else if(oligo[i] == 'Y'){	reverse += 'R';	}

            else if(oligo[i] == 'M'){	reverse += 'K';	}
            else if(oligo[i] == 'K'){	reverse += 'M';	}

            else if(oligo[i] == 'W'){	reverse += 'W';	}
            else if(oligo[i] == 'S'){	reverse += 'S';	}

            else if(oligo[i] == 'B'){	reverse += 'V';	}
            else if(oligo[i] == 'V'){	reverse += 'B';	}

            else if(oligo[i] == 'D'){	reverse += 'H';	}
            else if(oligo[i] == 'H'){	reverse += 'D';	}

            else if(oligo[i] == '-'){	reverse += '-';	}
            else	{	reverse += 'N';	}
        }


        return reverse;
    }
    catch(exception& e) {
        m->errorOut(e, "TrimOligos", "reverseOligo");
        exit(1);
    }
}

/********************************************************************/



