/*
 *  pairwiseseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 10/20/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "pairwiseseqscommand.h"

#include "calculators/eachgapdist.h"
#include "calculators/eachgapignore.h"
#include "calculators/ignoregaps.h"
#include "calculators/kmerdist.hpp"
#include "calculators/onegapdist.h"
#include "calculators/onegapignore.h"
#include "datastructures/alignment.hpp"
#include "datastructures/sequencedb.h"
#include "gotohoverlap.hpp"
#include "needlemanoverlap.hpp"
#include "noalign.hpp"
#include "validcalculator.h"

//**********************************************************************************************************************
vector<string> PairwiseSeqsCommand::setParameters(){	
	try {
        CommandParameter pcolumn("column", "InputTypes", "", "", "none", "none", "OldFastaColumn","column",false,false); parameters.push_back(pcolumn);
        CommandParameter poldfasta("oldfasta", "InputTypes", "", "", "none", "none", "OldFastaColumn","",false,false); parameters.push_back(poldfasta);
        CommandParameter pfitcalc("fitcalc", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pfitcalc);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","phylip-column",false,true,true); parameters.push_back(pfasta);
		CommandParameter palign("align", "Multiple", "needleman-gotoh-noalign", "needleman", "", "", "","",false,false); parameters.push_back(palign);
		CommandParameter pmatch("match", "Number", "", "1.0", "", "", "","",false,false); parameters.push_back(pmatch);
		CommandParameter pmismatch("mismatch", "Number", "", "-1.0", "", "", "","",false,false); parameters.push_back(pmismatch);
		CommandParameter pgapopen("gapopen", "Number", "", "-2.0", "", "", "","",false,false); parameters.push_back(pgapopen);
		CommandParameter pgapextend("gapextend", "Number", "", "-1.0", "", "", "","",false,false); parameters.push_back(pgapextend);
        CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
		CommandParameter poutput("output", "Multiple", "column-lt-square-phylip", "column", "", "", "","phylip-column",false,false,true); parameters.push_back(poutput);
		CommandParameter pcalc("calc", "Multiple", "nogaps-eachgap-onegap", "onegap", "", "", "","",false,false); parameters.push_back(pcalc);
		CommandParameter pcountends("countends", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pcountends);
        CommandParameter pcompress("compress", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pcompress);
		CommandParameter pcutoff("cutoff", "Number", "", "1.0", "", "", "","",false,false,true); parameters.push_back(pcutoff);
        CommandParameter pkcutoff("kmercutoff", "Number", "", "-1.0", "", "", "","",false,false,true);
        parameters.push_back(pkcutoff);
        CommandParameter pksize("ksize", "Number", "", "8", "", "", "","",false,false); parameters.push_back(pksize);
        CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["phylip"] = tempOutNames;
        outputTypes["column"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "PairwiseSeqsCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string PairwiseSeqsCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The pairwise.seqs command reads a fasta file and creates distance matrix.\n";
		helpString += "The pairwise.seqs command parameters are fasta, align, match, mismatch, gapopen, gapextend, calc, output, cutoff, oldfasta, column, processors.\n";
		helpString += "The fasta parameter is required.\n";
		helpString += "The align parameter allows you to specify the alignment method to use.  Your options are: gotoh, needleman and noalign. The default is needleman.\n";
		helpString += "The match parameter allows you to specify the bonus for having the same base. The default is 1.0.\n";
		helpString += "The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0.\n";
		helpString += "The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -2.0.\n";
		helpString += "The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -1.0.\n";
		helpString += "The calc parameter allows you to specify the method of calculating the distances.  Your options are: nogaps, onegap or eachgap. The default is onegap.\n";
		helpString += "The countends parameter allows you to specify whether to include terminal gaps in distance.  Your options are: T or F. The default is T.\n";
		helpString += "The cutoff parameter allows you to specify maximum distance to keep. The default is 1.0.\n";
        helpString += "The kmercutoff parameter allows you to specify maximum kmer distance. The kmercutoff is used to reduce the processing time by avoiding the aligning and distance calculations for sequences with a kmer distance above the cutoff. Kmer distance are calculated using methods described here, Edgar, R. C. (2004). Muscle: a multiple sequence alignment method with reduced time and space complexity. BMC Bioinformatics, 5:113. The defaults vary based on the cutoff selected. Cutoff <= 0.05 -> kmerCutoff = -1.0, cutoff 0.05 - 0.15 -> kmerCutoff = -0.50, cutoff 0.15-0.25 -> kmerCutoff = -0.25, cutoff > 0.25 -> kmerCutoff = -0.10.\n";
        helpString += "The ksize parameter allows you to specify the kmer size for calculating the kmer distance.  The default is 7.\n";
		helpString += "The output parameter allows you to specify format of your distance matrix. Options are column, lt, and square. The default is column.\n";
        helpString += "The oldfasta and column parameters allow you to append the distances calculated to the column file.\n";
		helpString += "The compress parameter allows you to indicate that you want the resulting distance file compressed.  The default is false.\n";
		helpString += "The pairwise.seqs command should be in the following format: \n";
		helpString += "pairwise.seqs(fasta=yourfastaFile, align=yourAlignmentMethod) \n";
		helpString += "Example pairwise.seqs(fasta=candidate.fasta, align=gotoh)\n";
		
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "PairwiseSeqsCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string PairwiseSeqsCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "phylip") {  pattern = "[filename],[outputtag],dist"; } 
        else if (type == "column") { pattern = "[filename],dist"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "PairwiseSeqsCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
PairwiseSeqsCommand::PairwiseSeqsCommand(string option) : Command()  {
	try {
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string, string> parameters = parser.getParameters(); 
			
			ValidParameters validParameter;
			fastaFileName = validParameter.validFile(parameters, "fasta");
			if (fastaFileName == "not found") {
				fastaFileName = current->getFastaFile();
				if (fastaFileName != "") {  m->mothurOut("Using " + fastaFileName + " as input file for the fasta parameter.\n");  }
				else { 	m->mothurOut("[ERROR]: You have no current fastafile and the fasta parameter is required.\n");  abort = true; }
            }else if (fastaFileName == "not open") { abort = true; }
            else{ current->setFastaFile(fastaFileName); }
            
            if (outputdir == "") {  outputdir += util.hasPath(fastaFileName); }
            
            oldfastafile = validParameter.validFile(parameters, "oldfasta");
            if (oldfastafile == "not found") { oldfastafile = ""; }
            else if (oldfastafile == "not open") { abort = true; }
            
            column = validParameter.validFile(parameters, "column");
            if (column == "not found") { column = ""; }
            else if (column == "not open") { abort = true; }
            else { current->setColumnFile(column); }
		
			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			string temp;
			temp = validParameter.valid(parameters, "match");		if (temp == "not found"){	temp = "1.0";			}
			util.mothurConvert(temp, match);  
			
			temp = validParameter.valid(parameters, "mismatch");		if (temp == "not found"){	temp = "-1.0";			}
			util.mothurConvert(temp, misMatch);  
            if (misMatch > 0) { m->mothurOut("[ERROR]: mismatch must be negative.\n"); abort=true; }
			
			temp = validParameter.valid(parameters, "gapopen");		if (temp == "not found"){	temp = "-2.0";			}
			util.mothurConvert(temp, gapOpen);  
            if (gapOpen > 0) { m->mothurOut("[ERROR]: gapopen must be negative.\n"); abort=true; }
			
			temp = validParameter.valid(parameters, "gapextend");	if (temp == "not found"){	temp = "-1.0";			}
			util.mothurConvert(temp, gapExtend); 
            if (gapExtend > 0) { m->mothurOut("[ERROR]: gapextend must be negative.\n"); abort=true; }
			
			temp = validParameter.valid(parameters, "processors");	if (temp == "not found"){	temp = current->getProcessors();	}
			processors = current->setProcessors(temp);
            
			temp = validParameter.valid(parameters, "cutoff");		if(temp == "not found"){	temp = "1.0"; }
			util.mothurConvert(temp, cutoff);
            
            temp = validParameter.valid(parameters, "kmercutoff");
            if(temp == "not found"){
                if (cutoff <= 0.05) { kmerCutoff = -1.0; }
                else if ((cutoff > 0.05) && (cutoff <= 0.15)) { kmerCutoff = -0.50; }
                else if ((cutoff > 0.15) && (cutoff <= 0.25)) { kmerCutoff = -0.25;  }
                else { kmerCutoff = -0.1; }
                
            }else { util.mothurConvert(temp, kmerCutoff); }
            
            temp = validParameter.valid(parameters, "ksize");        if (temp == "not found"){ temp = "7"; }
            util.mothurConvert(temp, kmerSize);
			
			temp = validParameter.valid(parameters, "countends");	if(temp == "not found"){	temp = "T";	}
			countends = util.isTrue(temp);
			
			temp = validParameter.valid(parameters, "compress");		if(temp == "not found"){  temp = "F"; }
			compress = util.isTrue(temp); 
			
			align = validParameter.valid(parameters, "align");		if (align == "not found"){	align = "needleman";	}
            
            temp = validParameter.valid(parameters, "fitcalc");	if(temp == "not found"){	temp = "F";	}
            fitCalc = util.isTrue(temp);
			
			output = validParameter.valid(parameters, "output");		if(output == "not found"){	output = "column"; }
            if (output=="phylip") { output = "lt"; }
			if ((output != "column") && (output != "lt") && (output != "square")) { m->mothurOut(output + " is not a valid output form. Options are column, lt and square. I will use column.\n");  output = "column"; }
			
			calc = validParameter.valid(parameters, "calc");			
			if (calc == "not found") { calc = "onegap";  }
			else { 
				 if (calc == "default")  {  calc = "onegap";  }
			}
        
            if ((calc != "nogaps") && (calc != "eachgap") && (calc != "onegap")) { m->mothurOut(calc + " is not a valid calculator for pairwise.seqs. Options are onegap, eachgap and nogaps. I will use onegap.\n");  calc = "onegap"; }
            
		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "PairwiseSeqsCommand", "PairwiseSeqsCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
PairwiseSeqsCommand::PairwiseSeqsCommand(StorageDatabase*& storageDB, vector< vector< int > > kDB, vector< int > lths,  string outputFileRoot, double cut, string outputformat, int proc) {
    try {
        abort = false; calledHelp = false;
        vector<string> tempOutNames;
        outputTypes["phylip"] = tempOutNames;
        outputTypes["column"] = tempOutNames;
        
        //defaults
        calc = "onegap";
        countends = true;
        fitCalc = false;
        cutoff = cut;
        processors = proc;
        compress = false;
        output = outputformat;
        match = 1.0;
        misMatch = -1.0;
        gapOpen = -2.0;
        gapExtend = -1.0;
        align = "needleman";
        kmerSize = 7;
        kmerDB = kDB;
        lengths = lths;
        if (cutoff <= 0.05) { kmerCutoff = -1.0; }
        else if ((cutoff > 0.05) && (cutoff <= 0.15)) { kmerCutoff = -0.50; }
        else if ((cutoff > 0.15) && (cutoff <= 0.25)) { kmerCutoff = -0.25;  }
        else { kmerCutoff = -0.1; }
            
        longestBase = 2000; //will need to update this in driver if we find sequences with more bases.  hardcoded so we don't have the pre-read user fasta file.
        numDistsBelowCutoff = 0;

        alignDB = storageDB;
        long long numSeqs = alignDB->getNumSeqs();

        if (numSeqs < 2) {  m->mothurOut("[ERROR]: you must have at least 2 sequences to calculate the distances, aborting.\n");  return; }
        
        string outputFile;
        map<string, string> variables;
        variables["[filename]"] = outputFileRoot;
        
        if (output == "lt") { //does the user want lower triangle phylip formatted file
            variables["[outputtag]"] = "phylip";
            outputFile = getOutputFileName("phylip", variables);
            util.mothurRemove(outputFile); outputTypes["phylip"].push_back(outputFile);
        }else if (output == "column") { //user wants column format
            if (fitCalc) {  variables["[outputtag]"] = "fit";  }
            outputFile = getOutputFileName("column", variables);
            outputTypes["column"].push_back(outputFile);
            util.mothurRemove(outputFile);
        }
       
        time_t start, end; time(&start);
        
        m->mothurOut("\nSequence\tTime\tNum_Dists_Below_Cutoff\n");
                     
        createProcesses(outputFile);
        
        time(&end);
        m->mothurOut("\nIt took " + toString(difftime(end, start)) + " secs to find distances for " + toString(numSeqs) + " sequences. " + toString(numDistsBelowCutoff) + " distances below cutoff " + toString(cutoff) + ".\n\n");
     
        m->mothurOut("\nOutput File Names:\n"); m->mothurOut(outputFile+"\n\n");
        
    }
    catch(exception& e) {
        m->errorOut(e, "PairwiseSeqsCommand", "PairwiseSeqsCommand");
        exit(1);
    }
}
//**********************************************************************************************************************

int PairwiseSeqsCommand::execute(){
	try {
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
		
        time_t start, end; time(&start);

		longestBase = 2000; //will need to update this in driver if we find sequences with more bases.  hardcoded so we don't have the pre-read user fasta file.
        numDistsBelowCutoff = 0;

        ifstream inFASTA; util.openInputFile(fastaFileName, inFASTA);
        alignDB = new SequenceDB(inFASTA, kmerSize, kmerDB, lengths);
        inFASTA.close();
        
        //sanity check the oldfasta and column file as well as add oldfasta sequences to alignDB
        if ((oldfastafile != "") && (column != ""))  {	if (!(sanityCheck())) { return 0; }  }
        
        if (m->getControl_pressed()) { delete alignDB; return 0; }
        
        long long numSeqs = alignDB->getNumSeqs();

        string outputFile = "";
        
        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(fastaFileName));
        if ((oldfastafile != "") && (column != ""))  {  variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(oldfastafile));  }
        
        if (output == "lt") { //does the user want lower triangle phylip formatted file
            variables["[outputtag]"] = "phylip";
            outputFile = getOutputFileName("phylip", variables);
            util.mothurRemove(outputFile); outputTypes["phylip"].push_back(outputFile);
        }else if (output == "column") { //user wants column format
            if (fitCalc) {  variables["[outputtag]"] = "fit";  }
            outputFile = getOutputFileName("column", variables);
            outputTypes["column"].push_back(outputFile);
            util.mothurRemove(outputFile);
        }else { //assume square
            variables["[outputtag]"] = "square";
            outputFile = getOutputFileName("phylip", variables);
            util.mothurRemove(outputFile);
            outputTypes["phylip"].push_back(outputFile);
        }
        
        m->mothurOut("\nSequence\tTime\tNum_Dists_Below_Cutoff\n");
        
        createProcesses(outputFile);
        
        delete alignDB;
        
        if (m->getControl_pressed()) { outputTypes.clear();   util.mothurRemove(outputFile); return 0; }
        
        if(util.isBlank(outputFile)) { m->mothurOut(outputFile + " is blank. This can result if there are no distances below your cutoff.\n"); }
        
        //append the old column file to the new one
        if ((oldfastafile != "") && (column != ""))  {
            //we had to rename the column file so we didnt overwrite above, but we want to keep old name
            if (outputFile == column) {
                string tempcolumn = column + ".old";
                util.appendFiles(tempcolumn, outputFile);
                util.mothurRemove(tempcolumn);
            }else{
                util.appendFiles(outputFile, column);
                util.mothurRemove(outputFile);
                outputFile = column;
            }
            outputTypes["column"].clear(); outputTypes["column"].push_back(outputFile);
        }
        
        if (compress) {
            m->mothurOut("Compressing...\n");
            m->mothurOut("(Replacing " + outputFile + " with " + outputFile + ".gz)\n");
            system(("gzip -v " + outputFile).c_str());
            outputNames.push_back(outputFile + ".gz");
        }else { outputNames.push_back(outputFile); }
        
        time(&end);
        m->mothurOut("\nIt took " + toString(difftime(end, start)) + " secs to find distances for " + toString(numSeqs) + " sequences. " + toString(numDistsBelowCutoff) + " distances below cutoff " + toString(cutoff) + ".\n\n");
        
        if (m->getControl_pressed()) { outputTypes.clear(); util.mothurRemove(outputFile); return 0; }
        
		//set phylip file as new current phylipfile
		string currentName = "";
		itTypes = outputTypes.find("phylip");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setPhylipFile(currentName); }
		}
		
		//set column file as new current columnfile
		itTypes = outputTypes.find("column");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setColumnFile(currentName); }
		}
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
		

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "PairwiseSeqsCommand", "execute");
		exit(1);
	}
}

/**************************************************************************************************/
struct pairwiseData {
    string align, distcalcType, outputFileName;
    unsigned long long start, end;
    long long count;
    float match, misMatch, gapOpen, gapExtend, cutoff, kmerCutoff;
    int longestBase, kmerSize;
    bool countends;
    
    vector< vector< int > > kmerDB; //kmerDB[0] = vector<int> maxKmers long, contains kmer counts
    vector< vector< int > > oldkmerDB; //kmerDB[0] = vector<int> maxKmers long, contains kmer counts
    vector< int > lengths;
    vector< int > oldlengths;
    
    StorageDatabase* alignDB;
    SequenceDB oldFastaDB;
    OutputWriter* threadWriter;
    MothurOut* m;
    Utils util;
    
    pairwiseData(){}
    pairwiseData(OutputWriter* ofn) {
        threadWriter = ofn;
        m = MothurOut::getInstance();
    }
    
    pairwiseData(string ofn) {
        outputFileName = ofn;
        m = MothurOut::getInstance();
    }
    
    void setVariables(string al, string di, bool co, string op, StorageDatabase* DB, SequenceDB oldDB,  unsigned long long st, unsigned long long en, float ma, float misMa, float gapO, float gapE, int thr, float cu, float kcut, int ksz, vector< vector< int > > kdb, vector< int > le, vector< vector< int > > okdb, vector< int > ole) {
        align = al;
        distcalcType = di;
        countends = co;
        alignDB = DB;
        oldFastaDB = oldDB;
        cutoff = cu;
        start = st;
        end = en;
        match = ma;
        misMatch = misMa;
        gapOpen = gapO;
        gapExtend = gapE;
        longestBase = thr;
        kmerDB = kdb;
        oldkmerDB = okdb;
        lengths = le;
        oldlengths = ole;
        kmerSize = ksz;
        kmerCutoff = kcut;
        count = 0;
    }
};
/***********************************************************************/
vector<kmerCount> getUniqueKmers(vector<int> seqsKmers, int i){
        vector<kmerCount> uniques;
        
        for (int k = 0; k < seqsKmers.size(); k++) {
            if (seqsKmers[k] != 0) {
                kmerCount thisKmer(k, seqsKmers[k]);
                uniques.push_back(thisKmer);
            }
        }
        
        return uniques;
}

/**************************************************************************************************/
//the higher the kmercutoff the higher the aligned dist. As kmercutoff approaches 0, aligned dist aproaches 1.
int driverColumn(pairwiseData* params){
    try {
        int startTime = time(nullptr);
        
        Alignment* alignment;
        if(params->align == "gotoh")			{	alignment = new GotohOverlap(params->gapOpen, params->gapExtend, params->match, params->misMatch, params->longestBase);			}
        else if(params->align == "needleman")	{	alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);				}
        else if(params->align == "noalign")		{	alignment = new NoAlign();													}
        else {
            params->m->mothurOut(params->align + " is not a valid alignment option. I will run the command using needleman.\n");
            alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);
        }
        
        ValidCalculators validCalculator;
        DistCalc* distCalculator;
        if (params->countends) {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")			{	distCalculator = new ignoreGaps(params->cutoff);	}
                else if (params->distcalcType == "eachgap")	{	distCalculator = new eachGapDist(params->cutoff);	}
                else if (params->distcalcType == "onegap")		{	distCalculator = new oneGapDist(params->cutoff);	}
                //else if (params->distcalcType == "onegap")        {    distCalculator = new oneGapDist(1.0);    }
            }
        }else {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")		{	distCalculator = new ignoreGaps(params->cutoff);					}
                else if (params->distcalcType == "eachgap"){	distCalculator = new eachGapIgnoreTermGapDist(params->cutoff);	}
                else if (params->distcalcType == "onegap")	{	distCalculator = new oneGapIgnoreTermGapDist(params->cutoff);		}
            }
        }
        
        KmerDist kmerDistCalculator(params->kmerSize);
        double kmerCutoff = params->kmerCutoff;
        
        for(int i=params->start;i<params->end;i++){
            
            Sequence seq = params->alignDB->getSeq(i);
            vector<kmerCount> seqA = getUniqueKmers(params->kmerDB[i], i);
            
            for(int j=0;j<i;j++){
                
                if (params->m->getControl_pressed()) {  break;  }
            
                vector<int> seqB = params->kmerDB[j];
                
                int length = min(params->lengths[i], params->lengths[j]);
                
                vector<double> kmerDist = kmerDistCalculator.calcDist(seqA, seqB, length);
                
                if (kmerDist[0] <= kmerCutoff) {
                    Sequence seqI = seq;
                    Sequence seqJ = params->alignDB->getSeq(j);
                    
                    if (seq.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seq.getUnaligned().length()+1); }
                    if (seqJ.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seqJ.getUnaligned().length()+1); }
                    
                    alignment->align(seqI.getUnaligned(), seqJ.getUnaligned());

                    seqI.setAligned(alignment->getSeqAAln());
                    seqJ.setAligned(alignment->getSeqBAln());
                    
                    double dist = distCalculator->calcDist(seqI, seqJ);
                
                    if(dist <= params->cutoff){ params->count++; params->threadWriter->write(seqI.getName() + ' ' + seqJ.getName() + ' ' + toString(dist) + "\n"); }
                }
            }
            if((i+1) % 100 == 0){ params->m->mothurOutJustToScreen(toString(i+1) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n"); }
        }
        params->m->mothurOutJustToScreen(toString(params->end-1) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n");
        
        delete alignment; delete distCalculator;
        
        return 0;
    }
    catch(exception& e) {
        params->m->errorOut(e, "PairwiseSeqsCommand", "driver");
        exit(1);
    }
}
/**************************************************************************************************/
int driverFitCalc(pairwiseData* params){
    try {
        int startTime = time(nullptr);
        
        Alignment* alignment;
        if(params->align == "gotoh")			{	alignment = new GotohOverlap(params->gapOpen, params->gapExtend, params->match, params->misMatch, params->longestBase);			}
        else if(params->align == "needleman")	{	alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);				}
        else if(params->align == "noalign")		{	alignment = new NoAlign();													}
        else {
            params->m->mothurOut(params->align + " is not a valid alignment option. I will run the command using needleman.\n");
            alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);
        }
        
        ValidCalculators validCalculator;
        DistCalc* distCalculator;
        if (params->countends) {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")			{	distCalculator = new ignoreGaps(params->cutoff);	}
                else if (params->distcalcType == "eachgap")	{	distCalculator = new eachGapDist(params->cutoff);	}
                else if (params->distcalcType == "onegap")		{	distCalculator = new oneGapDist(params->cutoff);	}
            }
        }else {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")		{	distCalculator = new ignoreGaps(params->cutoff);					}
                else if (params->distcalcType == "eachgap"){	distCalculator = new eachGapIgnoreTermGapDist(params->cutoff);	}
                else if (params->distcalcType == "onegap")	{	distCalculator = new oneGapIgnoreTermGapDist(params->cutoff);		}
            }
        }
        
        KmerDist kmerDistCalculator(params->kmerSize);
        double kmerCutoff = params->kmerCutoff;
        
        for(int i=params->start;i<params->end;i++){ //for each oldDB fasta seq calc the distance to every new seq in alignDB
            
            Sequence seq = params->oldFastaDB.getSeq(i);
            vector<kmerCount> seqA = getUniqueKmers(params->oldkmerDB[i], i);
            
            for(int j = 0; j < params->alignDB->getNumSeqs(); j++){
                
                if (params->m->getControl_pressed()) {  break;  }
                
                vector<int> seqB = params->kmerDB[j];
                
                int length = min(params->oldlengths[i], params->lengths[j]);
                
                vector<double> kmerDist = kmerDistCalculator.calcDist(seqA, seqB, length);
                
                if (kmerDist[0] <= kmerCutoff) {
                    Sequence seqI = seq;
                    Sequence seqJ = params->alignDB->getSeq(j);
                    
                    if (seq.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seq.getUnaligned().length()+1); }
                    if (seqJ.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seqJ.getUnaligned().length()+1); }
                    
                    alignment->align(seqI.getUnaligned(), seqJ.getUnaligned());

                    seqI.setAligned(alignment->getSeqAAln());
                    seqJ.setAligned(alignment->getSeqBAln());
                    
                    double dist = distCalculator->calcDist(seqI, seqJ);
                
                    if(dist <= params->cutoff){ params->count++; params->threadWriter->write(seqI.getName() + ' ' + seqJ.getName() + ' ' + toString(dist) + "\n"); }
                }
            }
            
            if((i+1) % 100 == 0){ params->m->mothurOutJustToScreen(toString(i+1) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n"); }
        }
        params->m->mothurOutJustToScreen(toString(params->end-1) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n");
        
        delete alignment; delete distCalculator;
        
        return 0;
    }
    catch(exception& e) {
        params->m->errorOut(e, "PairwiseSeqsCommand", "driverFitCalc");
        exit(1);
    }
}
/**************************************************************************************************/
int driverLt(pairwiseData* params){
    try {
        
        int startTime = time(nullptr);
        
        Alignment* alignment;
        if(params->align == "gotoh")			{	alignment = new GotohOverlap(params->gapOpen, params->gapExtend, params->match, params->misMatch, params->longestBase);			}
        else if(params->align == "needleman")	{	alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);				}
        else if(params->align == "noalign")		{	alignment = new NoAlign();													}
        else {
            params->m->mothurOut(params->align + " is not a valid alignment option. I will run the command using needleman.\n");
            alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);
        }
        
        ValidCalculators validCalculator;
        DistCalc* distCalculator;
        double cutoff = 1.0;
        if (params->countends) {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")			{	distCalculator = new ignoreGaps(cutoff);	}
                else if (params->distcalcType == "eachgap")	{	distCalculator = new eachGapDist(cutoff);	}
                else if (params->distcalcType == "onegap")		{	distCalculator = new oneGapDist(cutoff);	}
            }
        }else {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")		{	distCalculator = new ignoreGaps(cutoff);					}
                else if (params->distcalcType == "eachgap"){	distCalculator = new eachGapIgnoreTermGapDist(cutoff);	}
                else if (params->distcalcType == "onegap")	{	distCalculator = new oneGapIgnoreTermGapDist(cutoff);		}
            }
        }
        
        //column file
        ofstream outFile;
        params->util.openOutputFile(params->outputFileName, outFile);
        outFile.setf(ios::fixed, ios::showpoint);
        outFile << setprecision(4);
        
        if(params->start == 0){	outFile << params->alignDB->getNumSeqs() << endl;	}

        for(int i=params->start;i<params->end;i++){
            
            Sequence seq = params->alignDB->getSeq(i);
            if (seq.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seq.getUnaligned().length()+1); }
            
            string name = seq.getName();
            if (name.length() < 10) {  while (name.length() < 10) {  name += " ";  } seq.setName(name); } //pad with spaces to make compatible
            outFile << name;
            
            for(int j=0;j<i;j++){
                
                if (params->m->getControl_pressed()) { break;  }
                
                Sequence seqI = seq;
                Sequence seqJ = params->alignDB->getSeq(j);
                if (seqJ.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seqJ.getUnaligned().length()+1); }
                
                alignment->align(seqI.getUnaligned(), seqJ.getUnaligned());
                seqI.setAligned(alignment->getSeqAAln());
                seqJ.setAligned(alignment->getSeqBAln());
                
                double dist = distCalculator->calcDist(seqI, seqJ);
                
                if (params->m->getDebug()) { params->m->mothurOut("[DEBUG]: " + seqI.getName() + '\t' +  alignment->getSeqAAln() + '\n' + seqJ.getName() + alignment->getSeqBAln() + '\n' + "distance = " + toString(dist) + "\n"); }
                if(dist <= params->cutoff){ params->count++; }
                outFile << '\t' << dist;
            }
            
            outFile << endl;
            
            if(i % 100 == 0){ params->m->mothurOutJustToScreen(toString(i) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n"); }
            
        }
        params->m->mothurOutJustToScreen(toString(params->end-1) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n");
        
        outFile.close();
        delete alignment;
        delete distCalculator;
        
        return 1;
    }
    catch(exception& e) {
        params->m->errorOut(e, "PairwiseSeqsCommand", "driver");
        exit(1);
    }
}
/**************************************************************************************************/
int driverSquare(pairwiseData* params){
    try {
        
        int startTime = time(nullptr);
        
        Alignment* alignment;
        if(params->align == "gotoh")			{	alignment = new GotohOverlap(params->gapOpen, params->gapExtend, params->match, params->misMatch, params->longestBase);			}
        else if(params->align == "needleman")	{	alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);				}
        else if(params->align == "noalign")		{	alignment = new NoAlign();													}
        else {
            params->m->mothurOut(params->align + " is not a valid alignment option. I will run the command using needleman.\n");
            alignment = new NeedlemanOverlap(params->gapOpen, params->match, params->misMatch, params->longestBase);
        }
        
        ValidCalculators validCalculator;
        DistCalc* distCalculator;
        double cutoff = 1.0;
        if (params->countends) {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")			{	distCalculator = new ignoreGaps(cutoff);	}
                else if (params->distcalcType == "eachgap")	{	distCalculator = new eachGapDist(cutoff);	}
                else if (params->distcalcType == "onegap")		{	distCalculator = new oneGapDist(cutoff);	}
            }
        }else {
            if (validCalculator.isValidCalculator("distance", params->distcalcType) ) {
                if (params->distcalcType == "nogaps")		{	distCalculator = new ignoreGaps(cutoff);					}
                else if (params->distcalcType == "eachgap"){	distCalculator = new eachGapIgnoreTermGapDist(cutoff);	}
                else if (params->distcalcType == "onegap")	{	distCalculator = new oneGapIgnoreTermGapDist(cutoff);		}
            }
        }
        
        //column file
        ofstream outFile;
        params->util.openOutputFile(params->outputFileName, outFile);
        outFile.setf(ios::fixed, ios::showpoint);
        outFile << setprecision(4);
        
        long long numSeqs = params->alignDB->getNumSeqs();
        if(params->start == 0){	outFile << numSeqs << endl;	}
        
        for(int i=params->start;i<params->end;i++){
            
            Sequence seq = params->alignDB->getSeq(i);
            if (seq.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seq.getUnaligned().length()+1); }
            
            string name = seq.getName();
            if (name.length() < 10) {  while (name.length() < 10) {  name += " ";  } seq.setName(name); } //pad with spaces to make compatible
            outFile << name;
            
            for(int j=0;j<numSeqs;j++){
                
                if (params->m->getControl_pressed()) { break;  }
                
                Sequence seqI = seq;
                Sequence seqJ = params->alignDB->getSeq(j);
                if (seqJ.getUnaligned().length() > alignment->getnRows()) { alignment->resize(seqJ.getUnaligned().length()+1); }

                alignment->align(seqI.getUnaligned(), seqJ.getUnaligned());
                seqI.setAligned(alignment->getSeqAAln());
                seqJ.setAligned(alignment->getSeqBAln());
                
                double dist = distCalculator->calcDist(seqI, seqJ);
                
                if(dist <= params->cutoff){ params->count++; }
                outFile << '\t' << dist;
                
                if (params->m->getDebug()) { params->m->mothurOut("[DEBUG]: " + seqI.getName() + '\t' +  alignment->getSeqAAln() + '\n' + seqJ.getName() + alignment->getSeqBAln() + '\n' + "distance = " + toString(dist) + "\n"); }
            }
            
            outFile << endl; 
            
            if(i % 100 == 0){ params->m->mothurOutJustToScreen(toString(i) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n");  }
            
        }
        params->m->mothurOutJustToScreen(toString(params->end-1) + "\t" + toString(time(nullptr) - startTime)+ "\t" + toString(params->count) +"\n"); 
        
        outFile.close();
        delete alignment;
        delete distCalculator;
        
        return 1;
    }
    catch(exception& e) {
        params->m->errorOut(e, "PairwiseSeqsCommand", "driver");
        exit(1);
    }
}

/**************************************************************************************************/
void PairwiseSeqsCommand::createProcesses(string filename) {
	try {
        vector<linePair> lines;
        vector<std::thread*> workerThreads;
        vector<pairwiseData*> data;
        long long numSeqs = alignDB->getNumSeqs();
        
        long long numDists = 0;
        if (output == "square") { numDists = numSeqs * numSeqs; }
        else { for(int i=0;i<numSeqs;i++){ for(int j=0;j<i;j++){ numDists++; if (numDists > processors) { break; } } } }
        if (numDists < processors) { processors = numDists; }
       
        for (int i = 0; i < processors; i++) {
            linePair tempLine;
            lines.push_back(tempLine);
            if (output != "square") {
                lines[i].start = int (sqrt(float(i)/float(processors)) * numSeqs);
                lines[i].end = int (sqrt(float(i+1)/float(processors)) * numSeqs);
            }else{
                lines[i].start = int ((float(i)/float(processors)) * numSeqs);
                lines[i].end = int ((float(i+1)/float(processors)) * numSeqs);
            }
        }
        
        auto synchronizedOutputFile = std::make_shared<SynchronizedOutputFile>(filename);
        synchronizedOutputFile->setFixedShowPoint(); synchronizedOutputFile->setPrecision(4);
        
        SequenceDB oldFastaDB; vector< int > oldlengths; vector< vector< int > > oldkmerDB;
        if (fitCalc) {
            ifstream inFASTA; util.openInputFile(oldfastafile, inFASTA);
            oldFastaDB = SequenceDB(inFASTA, kmerSize, oldkmerDB, oldlengths);
            inFASTA.close();
            
            lines.clear();
            if (processors > oldFastaDB.getNumSeqs()) { processors = oldFastaDB.getNumSeqs(); }
            int remainingSeqs = oldFastaDB.getNumSeqs();
            int startIndex = 0;
            for (int remainingProcessors = processors; remainingProcessors > 0; remainingProcessors--) {
                int numSeqsToFit = remainingSeqs; //case for last processor
                if (remainingProcessors != 1) { numSeqsToFit = ceil(remainingSeqs / remainingProcessors); }
                lines.push_back(linePair(startIndex, (startIndex+numSeqsToFit))); //startIndex, endIndex
                startIndex = startIndex + numSeqsToFit;
                remainingSeqs -= numSeqsToFit;
            }
        }

        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            OutputWriter* threadWriter = nullptr;
            pairwiseData* dataBundle = nullptr;
            string extension = toString(i+1) + ".temp";
            if (output == "column") {
                threadWriter = new OutputWriter(synchronizedOutputFile);
                dataBundle = new pairwiseData(threadWriter);
            }else { dataBundle = new pairwiseData(filename+extension); }
            
            dataBundle->setVariables(align, calc, countends, output, alignDB, oldFastaDB, lines[i+1].start, lines[i+1].end, match, misMatch, gapOpen, gapExtend, longestBase, cutoff, kmerCutoff, kmerSize, kmerDB, lengths, oldkmerDB, oldlengths);
            data.push_back(dataBundle);
            
            std::thread* thisThread = nullptr;
            if (output == "column")     {
                if (fitCalc)    { thisThread = new std::thread(driverFitCalc, dataBundle);   }
                else            {  thisThread = new std::thread(driverColumn, dataBundle);   }
            }
            else if (output == "lt")    { thisThread = new std::thread(driverLt, dataBundle);            }
            else                        { thisThread = new std::thread(driverSquare, dataBundle);        }
            workerThreads.push_back(thisThread);
        }

        OutputWriter* threadWriter = nullptr;
        pairwiseData* dataBundle = nullptr;
        if (output == "column") {
            threadWriter = new OutputWriter(synchronizedOutputFile);
            dataBundle = new pairwiseData(threadWriter);
        }else { dataBundle = new pairwiseData(filename); }
        
        dataBundle->setVariables(align, calc, countends, output, alignDB, oldFastaDB, lines[0].start, lines[0].end, match, misMatch, gapOpen, gapExtend, longestBase, cutoff, kmerCutoff, kmerSize, kmerDB, lengths, oldkmerDB, oldlengths);
    
        if (output == "column")     {
            if (fitCalc)    { driverFitCalc(dataBundle);    }
            else            { driverColumn(dataBundle);     }
            delete threadWriter;
        }
        else if (output == "lt")    { driverLt(dataBundle);            }
        else                        { driverSquare(dataBundle);        }
        numDistsBelowCutoff = dataBundle->count;
        
        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            
            numDistsBelowCutoff += data[i]->count;
            
            if (output == "column") {  delete data[i]->threadWriter; }
            else {
                string extension = toString(i+1) + ".temp";
                util.appendFiles((filename+extension), filename);
                util.mothurRemove(filename+extension);
            }
            delete data[i];
            delete workerThreads[i];
        }
    
        delete dataBundle;
	}
	catch(exception& e) {
		m->errorOut(e, "PairwiseSeqsCommand", "createProcesses");
		exit(1);
	}
}
/**************************************************************************************************/
//its okay if the column file does not contain all the names in the fasta file, since some distance may have been above a cutoff,
//but no sequences can be in the column file that are not in oldfasta. also, if a distance is above the cutoff given then remove it.
bool PairwiseSeqsCommand::sanityCheck() {
    try{
        bool good = true;
        
        //read fasta file and save names as well as adding them to the alignDB
        set<string> namesOldFasta;
        
        ifstream inFasta; util.openInputFile(oldfastafile, inFasta);
        
        while (!inFasta.eof()) {
            if (m->getControl_pressed()) {  inFasta.close(); return good;  }
            
            Sequence temp(inFasta);  gobble(inFasta);
            
            if (temp.getName() != "") {
                namesOldFasta.insert(temp.getName());  //save name
                if (!fitCalc) { alignDB->push_back(temp);  }//add to DB
            }
        }
        inFasta.close();
        
        //read through the column file checking names and removing distances above the cutoff
        ifstream inDist; util.openInputFile(column, inDist);
        
        ofstream outDist;
        string outputFile = column + ".temp";
        util.openOutputFile(outputFile, outDist);
        
        string name1, name2;
        float dist;
        while (!inDist.eof()) {
            if (m->getControl_pressed()) {  inDist.close(); outDist.close(); util.mothurRemove(outputFile); return good;  }
            
            inDist >> name1; gobble(inDist); inDist >> name2; gobble(inDist); inDist >> dist; gobble(inDist);
            
            //both names are in fasta file and distance is below cutoff
            if ((namesOldFasta.count(name1) == 0) || (namesOldFasta.count(name2) == 0)) {  good = false; break;  }
            else{ if (dist <= cutoff) { numDistsBelowCutoff++; outDist << name1 << '\t' << name2 << '\t' << dist << endl; } }
        }
        
        inDist.close(); outDist.close();
        
        if (good) {
            util.mothurRemove(column);
            rename(outputFile.c_str(), column.c_str());
        }else{
            util.mothurRemove(outputFile); //temp file is bad because file mismatch above
        }
        
        return good;
        
    }
    catch(exception& e) {
        m->errorOut(e, "PairwiseSeqsCommand", "sanityCheck");
        exit(1);
    }
}
/**************************************************************************************************/

