/*
 *  classifyseqscommand.cpp
 *  Mothur
 *
 *  Created by westcott on 11/2/09.
 *  Copyright 2009 Schloss Lab. All rights reserved.
 *
 */

#include "classifyseqscommand.h"

#include "inputdata.h"

#include "classifier/aligntree.h"
#include "classifier/bayesian.h"
#include "classifier/phylosummary.h"
#include "classifier/knn.h"
#include "classifier/kmertree.h"

#include "datastructures/groupmap.h"
#include "datastructures/counttable.h"
#include "datastructures/sharedrabundvector.hpp"

//**********************************************************************************************************************
vector<string> ClassifySeqsCommand::setParameters(){	
	try {
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(ptaxonomy);
		CommandParameter ptemplate("reference", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(ptemplate);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "none", "none","taxonomy",false,true,true); parameters.push_back(pfasta);
        CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "none","",false,false,true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "none", "none","",false,false,true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "none", "none","",false,false,true); parameters.push_back(pgroup);
        CommandParameter poutput("output", "Multiple", "simple-detail", "detail", "", "", "","",false,false, true); parameters.push_back(poutput);
		CommandParameter psearch("search", "Multiple", "kmer-suffix-distance-align", "kmer", "", "", "","",false,false); parameters.push_back(psearch);
		CommandParameter pksize("ksize", "Number", "", "8", "", "", "","",false,false); parameters.push_back(pksize);
		CommandParameter pmethod("method", "Multiple", "wang-knn-zap", "wang", "", "", "","",false,false); parameters.push_back(pmethod);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
		CommandParameter pmatch("match", "Number", "", "1.0", "", "", "","",false,false); parameters.push_back(pmatch);
        CommandParameter pprintlevel("printlevel", "Number", "", "-1", "", "", "","",false,false); parameters.push_back(pprintlevel);
		CommandParameter pmismatch("mismatch", "Number", "", "-1.0", "", "", "","",false,false); parameters.push_back(pmismatch);
		CommandParameter pgapopen("gapopen", "Number", "", "-2.0", "", "", "","",false,false); parameters.push_back(pgapopen);
		CommandParameter pgapextend("gapextend", "Number", "", "-1.0", "", "", "","",false,false); parameters.push_back(pgapextend);
		CommandParameter pcutoff("cutoff", "Number", "", "80", "", "", "","",false,true); parameters.push_back(pcutoff);
		CommandParameter pprobs("probs", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pprobs);
		CommandParameter piters("iters", "Number", "", "100", "", "", "","",false,true); parameters.push_back(piters);
        CommandParameter pshortcuts("shortcuts", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pshortcuts);
        CommandParameter prelabund("relabund", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(prelabund);
		CommandParameter pnumwanted("numwanted", "Number", "", "10", "", "", "","",false,true); parameters.push_back(pnumwanted);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["taxonomy"] = tempOutNames;
        outputTypes["accnos"] = tempOutNames;
        outputTypes["taxsummary"] = tempOutNames;
        outputTypes["matchdist"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ClassifySeqsCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifySeqsCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The classify.seqs command reads a fasta file containing sequences and creates a .taxonomy file and a .tax.summary file.\n";
		helpString += "The classify.seqs command parameters are " + getCommandParameters() + ". The reference, fasta and taxonomy parameters are required.\n";
		helpString += "The search parameter allows you to specify the method to find most similar reference sequence.  Your options are: suffix, kmer, align and distance. The default is kmer.\n";
		helpString += "The name parameter allows you add a names file with your fasta file.\n";
		helpString += "The group parameter allows you add a group file so you can have the summary totals broken up by group.\n";
        helpString += "The count parameter allows you add a count file so you can have the summary totals broken up by group.\n";
		helpString += "The method parameter allows you to specify classification method to use.  Your options are: wang, knn and zap. The default is wang.\n";
		helpString += "The ksize parameter allows you to specify the kmer size for finding most similar template to candidate.  The default is 8.\n";
		helpString += "The processors parameter allows you to specify the number of processors to use. The default is all available.\n";
		helpString += "The match parameter allows you to specify the bonus for having the same base. The default is 1.0.\n";
		helpString += "The mistmatch parameter allows you to specify the penalty for having different bases.  The default is -1.0.\n";
		helpString += "The gapopen parameter allows you to specify the penalty for opening a gap in an alignment. The default is -2.0.\n";
		helpString += "The gapextend parameter allows you to specify the penalty for extending a gap in an alignment.  The default is -1.0.\n";
		helpString += "The numwanted parameter allows you to specify the number of sequence matches you want with the knn method.  The default is 10.\n";
		helpString += "The cutoff parameter allows you to specify a bootstrap confidence threshold for your taxonomy.  The default is 80.\n";
		helpString += "The probs parameter shuts off the bootstrapping results for the wang and zap method. The default is true, meaning you want the bootstrapping to be shown.\n";
        helpString += "The relabund parameter allows you to indicate you want the summary file values to be relative abundances rather than raw abundances. Default=F. \n";
		helpString += "The iters parameter allows you to specify how many iterations to do when calculating the bootstrap confidence score for your taxonomy with the wang method.  The default is 100.\n";
		helpString += "The output parameter allows you to specify format of your summary file. Options are simple and detail. The default is detail.\n";
        helpString += "The printlevel parameter allows you to specify taxlevel of your summary file to print to. Options are 1 to the max level in the file.  The default is the max level.  If you select a level greater than the level your sequences classify to, mothur will print all possible levels. \n";
		helpString += "The classify.seqs command should be in the following format: \n";
		helpString += "classify.seqs(reference=yourReferenceFile, fasta=yourFastaFile, taxonomy=yourTaxonomyFile) \n";
		helpString += "Example classify.seqs(fasta=amazon.fasta, reference=trainset9_032012.pds.fasta, taxonomy=trainset9_032012.pds.tax)\n";
		helpString += "The .taxonomy file consists of 2 columns: 1 = your sequence name, 2 = the taxonomy for your sequence. \n";
		helpString += "The .tax.summary is a summary of the different taxonomies represented in your fasta file. \n";
        
        getCommonQuestions();
		
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ClassifySeqsCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClassifySeqsCommand::getCommonQuestions(){
    try {
        vector<string> questions, issues, qanswers, ianswers, howtos, hanswers;
        
        string question = "Does the reference need to be aligned?"; questions.push_back(question);
        string qanswer = "\tFor wang, knn and zap methods, mothur does not require an aligned reference to assign a taxonomy. Wang use k-mers to find the probabilities of the taxonomic assignment. \n"; qanswers.push_back(qanswer);
        
        question = "What reference should I use to classify?"; questions.push_back(question);
        qanswer = "\tWe provide mothur formatted references on the wiki. https://www.mothur.org/wiki/RDP_reference_files https://mothur.org/wiki/Silva_reference_files https://www.mothur.org/wiki/Greengenes-formatted_databases Alternatively, mothur allows you to create your own references as long as they are in fasta and taxonomy file format. You can find mothur's files formats here, https://www.mothur.org/wiki/File_Types. \n"; qanswers.push_back(qanswer);
        
        string issue = "Why are my sequences 'unclassifed'?"; issues.push_back(issue);
        string ianswer = "\tWhen it comes to classification there are two things main things that effect the number of unclassified results: the quality of the reads and the reference files. The bayesian classifier calculates the probabilities of reference sequences kmers being in a given genus and then uses those probabilities to classify the sequences. The quality of the query sequences affects the ability of the classifier to find enough kmers to find a good classification. A poor quality sequence is like turning up the noise in a crowded restaurant and trying to hear your date's father's name. Was that John, Tom or Ron? Uh oh... A good reference is also needed for similar reasons.\n"; ianswers.push_back(ianswer);
        
        string howto = "How do you recommend classifying to the species level?"; howtos.push_back(howto);
        string hanswer = "\tUnfortunately I do not. You will never get species level classification if you are using the RDP or Silva references. They only go to the genus level. Even the greengenes database only has 10% or so of sequences with species level names (greengenes hasn’t been updated in quite a few years). I and many others would contend that using 16S and especially a fragment to get a species name is asking too much - you need a culture and genome sequencing to do that. If someone wanted to give it a shot, they would need to add the species level names to the taxonomy strings. Also, they would need to add many more sequences that represent each species. Outside of a few groups of bacteria where the researchers have carefully selected the region (e.g. Lactobacillus or Staphylococcus), I really think this would be a lot of work for little/no benefit.\n"; hanswers.push_back(hanswer);
        
        string commonQuestions = util.getFormattedHelp(questions, qanswers, issues, ianswers, howtos, hanswers);
        
        return commonQuestions;
    }
    catch(exception& e) {
        m->errorOut(e, "ClassifySeqsCommand", "getCommonQuestions");
        exit(1);
    }
}

//**********************************************************************************************************************
string ClassifySeqsCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "taxonomy") {  pattern = "[filename],[tag],[tag2],taxonomy"; } 
        else if (type == "taxsummary") {  pattern = "[filename],[tag],[tag2],tax.summary"; } 
        else if (type == "accnos") {  pattern =  "[filename],[tag],[tag2],flip.accnos"; }
        else if (type == "matchdist") {  pattern =  "[filename],[tag],[tag2],match.dist"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ClassifySeqsCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
ClassifySeqsCommand::ClassifySeqsCommand(string option) : Command()  {
	try {
		hasName = false; hasCount=false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string, string> parameters = parser.getParameters(); 
			
			ValidParameters validParameter;
			
            fastafile = validParameter.validFile(parameters, "fasta");
            if (fastafile == "not found") {
                fastafile = current->getFastaFile();
                if (fastafile != "") { m->mothurOut("Using " + fastafile + " as input file for the fasta parameter.\n"); }
                else { 	m->mothurOut("[ERROR]: You have no current fasta file and the fasta parameter is required.\n");  abort = true; }
            }
            else if (fastafile == "not open") { abort = true; }
            else { current->setFastaFile(fastafile); }
            
            namefile = validParameter.validFile(parameters, "name");
            if (namefile == "not open") { namefile = ""; abort = true; }
            else if (namefile == "not found") {  namefile = "";  }
            else { current->setNameFile(namefile); }
            if (namefile != "") { hasName = true; }
            
            //check for required parameters
            countfile = validParameter.validFile(parameters, "count");
            if (countfile == "not open") { countfile = ""; abort = true; }
            else if (countfile == "not found") { countfile = "";  }
            else { current->setCountFile(countfile); }
            if (countfile != "") { hasCount = true; }
            
            //make sure there is at least one valid file left
            if (hasName && hasCount) { m->mothurOut("[ERROR]: You must enter ONLY ONE of the following: count or name.\n");  abort = true; }
            
            bool hasGroup = false;
            groupfile = validParameter.validFile(parameters, "group");
            if (groupfile == "not open") { abort = true; }
            else if (groupfile == "not found") {  groupfile = "";  }
            else { current->setGroupFile(groupfile); hasGroup = true; }
            
            if (hasGroup && hasCount) { m->mothurOut("[ERROR]: You must enter ONLY ONE of the following: count or group.\n");  abort = true; }
			
			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			string temp;
			temp = validParameter.valid(parameters, "processors");	if (temp == "not found"){	temp = current->getProcessors();	}
			processors = current->setProcessors(temp);

			//this has to go after save so that if the user sets save=t and provides no reference we abort
			templateFileName = validParameter.validFile(parameters, "reference");
			if (templateFileName == "not found") {
					m->mothurOut("[ERROR]: The reference parameter is a required for the classify.seqs command.\n"); abort = true;
			}else if (templateFileName == "not open") { abort = true; }
			
			
			//this has to go after save so that if the user sets save=t and provides no reference we abort
			taxonomyFileName = validParameter.validFile(parameters, "taxonomy");
			if (taxonomyFileName == "not found") {  m->mothurOut("[ERROR]: The taxonomy parameter is a required for the classify.seqs command.\n"); abort = true;
			}else if (taxonomyFileName == "not open") { abort = true; }
			
			search = validParameter.valid(parameters, "search");		if (search == "not found"){	search = "kmer";		}
			
			method = validParameter.valid(parameters, "method");		if (method == "not found"){	method = "wang";	}
            
            temp = validParameter.valid(parameters, "ksize");		if (temp == "not found"){	
                temp = "8";	
                if (method == "zap") { temp = "7"; }
            }
			util.mothurConvert(temp, kmerSize); 
			
			temp = validParameter.valid(parameters, "match");		if (temp == "not found"){	temp = "1.0";			}
			util.mothurConvert(temp, match);
            
            temp = validParameter.valid(parameters, "printlevel");		if (temp == "not found"){	temp = "-1";		}
            util.mothurConvert(temp, printlevel);
			
			temp = validParameter.valid(parameters, "mismatch");		if (temp == "not found"){	temp = "-1.0";			}
			util.mothurConvert(temp, misMatch);  
			
			temp = validParameter.valid(parameters, "gapopen");		if (temp == "not found"){	temp = "-2.0";			}
			util.mothurConvert(temp, gapOpen);  
			
			temp = validParameter.valid(parameters, "gapextend");	if (temp == "not found"){	temp = "-1.0";			}
			util.mothurConvert(temp, gapExtend); 
			
			temp = validParameter.valid(parameters, "numwanted");	if (temp == "not found"){	temp = "10";			}
			util.mothurConvert(temp, numWanted);
			
			temp = validParameter.valid(parameters, "cutoff");		if (temp == "not found"){	temp = "80";				}
			util.mothurConvert(temp, cutoff);
			
			temp = validParameter.valid(parameters, "probs");		if (temp == "not found"){	temp = "true";			}
			probs = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "relabund");		if (temp == "not found"){	temp = "false";			}
			relabund = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "shortcuts");	if (temp == "not found"){	temp = "true";			}
			writeShortcuts = util.isTrue(temp);
			
			flip = true;
			
			temp = validParameter.valid(parameters, "iters");		if (temp == "not found") { temp = "100";			}
			util.mothurConvert(temp, iters); 
            
            output = validParameter.valid(parameters, "output");		if(output == "not found"){	output = "detail"; }
			if ((output != "simple") && (output != "detail")) { m->mothurOut(output + " is not a valid output form. Options are simple and detail. I will use detail.\n"); output = "detail"; }
            
			if ((method == "wang") && (search != "kmer"))  { 
				m->mothurOut("The wang method requires the kmer search. " + search + " will be disregarded, and kmer will be used.\n" );
				search = "kmer";
			}
            
            if ((method == "zap") && ((search != "kmer") && (search != "align")))  { 
				m->mothurOut("The zap method requires the kmer or align search. " + search + " will be disregarded, and kmer will be used.\n" );
				search = "kmer";
			}
        }
	}
	catch(exception& e) {
		m->errorOut(e, "ClassifySeqsCommand", "ClassifySeqsCommand");
		exit(1);
	}
}

//**********************************************************************************************************************
ClassifySeqsCommand::~ClassifySeqsCommand(){}
//**********************************************************************************************************************

int ClassifySeqsCommand::execute(){
	try {
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        string outputMethodTag = method;
		if(method == "wang"){	classify = new Bayesian(taxonomyFileName, templateFileName, search, kmerSize, cutoff, iters, util.getRandomNumber(), flip, writeShortcuts, current->getVersion());	}
        else if(method == "knn"){	classify = new Knn(taxonomyFileName, templateFileName, search, kmerSize, gapOpen, gapExtend, match, misMatch, numWanted, util.getRandomNumber(), current->getVersion());				}
        else if(method == "zap"){	
            outputMethodTag = search + "_" + outputMethodTag;
            if (search == "kmer") {   classify = new KmerTree(templateFileName, taxonomyFileName, kmerSize, cutoff); }
            else {  classify = new AlignTree(templateFileName, taxonomyFileName, cutoff);  }
        }
		else {
			m->mothurOut(search + " is not a valid method option. I will run the command using wang.\n");
			classify = new Bayesian(taxonomyFileName, templateFileName, search, kmerSize, cutoff, iters, util.getRandomNumber(), flip, writeShortcuts, current->getVersion());
		}
		
		if (m->getControl_pressed()) { delete classify; return 0; }
				
        m->mothurOut("Classifying sequences from " + fastafile + " ...\n" );
        
        string baseTName = util.getSimpleName(taxonomyFileName);
        
        //set rippedTaxName to
        string RippedTaxName = "";
        bool foundDot = false;
        for (int i = baseTName.length()-1; i >= 0; i--) {
            if (foundDot && (baseTName[i] != '.')) {  RippedTaxName = baseTName[i] + RippedTaxName; }
            else if (foundDot && (baseTName[i] == '.')) {  break; }
            else if (!foundDot && (baseTName[i] == '.')) {  foundDot = true; }
        }
        
        if (outputdir == "") { outputdir += util.hasPath(fastafile); }
        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(fastafile));
        variables["[tag]"] = RippedTaxName;
        variables["[tag2]"] = outputMethodTag;
        string newTaxonomyFile = getOutputFileName("taxonomy", variables);
        string newaccnosFile = getOutputFileName("accnos", variables);
        string tempTaxonomyFile = outputdir + util.getRootName(util.getSimpleName(fastafile)) + "taxonomy.temp";
        string taxSummary = getOutputFileName("taxsummary", variables);
        
        if ((method == "knn") && (search == "distance")) {
            string DistName = getOutputFileName("matchdist", variables);
            classify->setDistName(DistName);  outputNames.push_back(DistName); outputTypes["matchdist"].push_back(DistName);
        }
        
        outputNames.push_back(newTaxonomyFile); outputTypes["taxonomy"].push_back(newTaxonomyFile);
        outputNames.push_back(taxSummary);	outputTypes["taxsummary"].push_back(taxSummary);
        
        long start = time(nullptr);
        int numFastaSeqs = createProcesses(newTaxonomyFile, tempTaxonomyFile, newaccnosFile, fastafile);
        
        if (!util.isBlank(newaccnosFile)) { m->mothurOut("\n[WARNING]: mothur reversed some your sequences for a better classification.  If you would like to take a closer look, please check " + newaccnosFile + " for the list of the sequences.\n");
            outputNames.push_back(newaccnosFile); outputTypes["accnos"].push_back(newaccnosFile);
        }else { util.mothurRemove(newaccnosFile); }
        
        m->mothurOut("\nIt took " + toString(time(nullptr) - start) + " secs to classify " + toString(numFastaSeqs) + " sequences.\n\n");
        start = time(nullptr);
        
        //read namefile
        map<string, vector<string> > nameMap;
        map<string,  vector<string> >::iterator itNames;
        if(namefile != "") {
            m->mothurOut("Reading " + namefile + "..."); cout.flush();
            nameMap.clear(); //remove old names
            util.readNames(namefile, nameMap);
            m->mothurOut("  Done.\n");
        }
        
        //output taxonomy with the unclassified bins added
        ifstream inTax;
        util.openInputFile(newTaxonomyFile, inTax);
        
        ofstream outTax;
        string unclass = newTaxonomyFile + ".unclass.temp";
        util.openOutputFile(unclass, outTax);
        
        //get maxLevel from phylotree so you know how many 'unclassified's to add
        int maxLevel = classify->getMaxLevel();
        
        //read taxfile - this reading and rewriting is done to preserve the confidence scores.
        string name, taxon;
        GroupMap* groupMap = nullptr;
        CountTable* ct = nullptr;
        PhyloSummary* taxaSum;
        
        if (hasCount) {
            ct = new CountTable();
            ct->readTable(countfile, true, false);
            taxaSum = new PhyloSummary(ct, relabund, printlevel);
        }else {
            if (groupfile != "") {  groupMap = new GroupMap(groupfile); groupMap->readMap(); }
            taxaSum = new PhyloSummary(groupMap, relabund, printlevel);
        }
        
        while (!inTax.eof()) {
            if (m->getControl_pressed()) { outputTypes.clear(); if (ct != nullptr) { delete ct; }  if (groupMap != nullptr) { delete groupMap; } delete taxaSum; for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);	} delete classify; return 0; }
            
            inTax >> name; gobble(inTax);
            taxon = util.getline(inTax); gobble(inTax);
            
            string newTax = util.addUnclassifieds(taxon, maxLevel, probs);
            
            outTax << name << '\t' << newTax << endl;
            
            if (namefile != "") {
                itNames = nameMap.find(name);
                
                if (itNames == nameMap.end()) {
                    m->mothurOut(name + " is not in your name file please correct.\n");  exit(1);
                }else{
                    //add it as many times as there are identical seqs
                    for (int i = 0; i < itNames->second.size(); i++) { taxaSum->addSeqToTree(itNames->second[i], newTax); }
                    itNames->second.clear();
                    nameMap.erase(itNames->first);
                }
            }else { taxaSum->addSeqToTree(name, newTax); }
        }
        inTax.close();
        outTax.close();
        
        util.mothurRemove(newTaxonomyFile);
        util.renameFile(unclass, newTaxonomyFile);
        
        if (m->getControl_pressed()) {  outputTypes.clear(); if (ct != nullptr) { delete ct; } if (groupMap != nullptr) { delete groupMap; } for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);	} delete classify; delete taxaSum;  return 0; }
        
        //print summary file
        ofstream outTaxTree;
        util.openOutputFile(taxSummary, outTaxTree);
        taxaSum->print(outTaxTree, output);
        outTaxTree.close();
        
        if (ct != nullptr) { delete ct; }
        if (groupMap != nullptr) { delete groupMap; } delete taxaSum;
        util.mothurRemove(tempTaxonomyFile);
        delete classify;
        
        m->mothurOut("\nIt took " + toString(time(nullptr) - start) + " secs to create the summary file for " + toString(numFastaSeqs) + " sequences.\n\n");

        m->mothurOut("\nOutput File Names: \n");
        for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
        m->mothurOutEndLine();
		
		//set taxonomy file as new current taxonomyfile
		string currentName = "";
		itTypes = outputTypes.find("taxonomy");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setTaxonomyFile(currentName); } }
		
		currentName = "";
		itTypes = outputTypes.find("accnos");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setAccnosFile(currentName); } }

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ClassifySeqsCommand", "execute");
		exit(1);
	}
}
/**************************************************************************************************/
struct classifyData {
    OutputWriter* taxTWriter;
    OutputWriter* taxWriter;
    OutputWriter* accnosWriter;
    string search, taxonomyFileName, templateFileName, method, filename;
    unsigned long long start;
    unsigned long long end;
    MothurOut* m;
    Classify* classify;
    float match, misMatch, gapOpen, gapExtend;
    int count, kmerSize, threadID, cutoff, iters, numWanted;
    bool probs, flip, writeShortcuts;
    Utils util;
    
    classifyData(){}
    classifyData(OutputWriter* acc, bool p, OutputWriter* a, OutputWriter* r, string f, unsigned long long st, unsigned long long en, bool fli, Classify* c) {
        accnosWriter = acc;
        taxWriter = a;
        taxTWriter = r;
        filename = f;
        m = MothurOut::getInstance();
        start = st;
        end = en;
        probs = p;
        flip = fli;
        count = 0;
        classify = c;
    }
};
//**********************************************************************************************************************
void driverClassifier(classifyData* params){
    try {
        ifstream inFASTA; params->util.openInputFile(params->filename, inFASTA); inFASTA.seekg(params->start);
        
        string taxonomy;
        bool done = false;
        string taxBuffer = ""; string taxTBuffer = ""; string accnosBuffer = "";
        while (!done) {
            if (params->m->getControl_pressed()) { break; }
            
            Sequence* candidateSeq = new Sequence(inFASTA); gobble(inFASTA);
            
            if (candidateSeq->getName() != "") {
                
                string simpleTax = ""; bool flipped = false;
                taxonomy = params->classify->getTaxonomy(candidateSeq, simpleTax, flipped);
                
                if (params->m->getControl_pressed()) { delete candidateSeq; break; }
                
                if (taxonomy == "unknown;") { params->m->mothurOut("[WARNING]: " + candidateSeq->getName() + " could not be classified. You can use the remove.lineage command with taxon=unknown; to remove such sequences.\n");  }
                
                //output confidence scores or not
                if (params->probs)  { taxBuffer += candidateSeq->getName() + '\t' + taxonomy + '\n';    }
                else                { taxBuffer += candidateSeq->getName() + '\t' + simpleTax + '\n';   }
                
                if (flipped) { accnosBuffer += candidateSeq->getName() + '\n'; }
                
                taxTBuffer = candidateSeq->getName() + '\t' + simpleTax + '\n';
                
                params->count++;
            }
            delete candidateSeq;
            
            //report progress
            if((params->count) % 100 == 0){
                params->m->mothurOutJustToScreen(toString(params->count) +"\n");
                params->taxTWriter->write(taxTBuffer); taxTBuffer = "";
                params->taxWriter->write(taxBuffer); taxBuffer = "";
                if (accnosBuffer != "") { params->accnosWriter->write(accnosBuffer); accnosBuffer = ""; }
            }
            
#if defined NON_WINDOWS
            unsigned long long pos = inFASTA.tellg();
            if ((pos == -1) || (pos >= params->end)) { break; }
#else
            if (params->count == params->end) { break; }
#endif
            
    }
        //report progress
        if((params->count) % 100 != 0){
            params->m->mothurOutJustToScreen(toString(params->count)+"\n");
            params->taxTWriter->write(taxTBuffer); taxTBuffer = "";
            params->taxWriter->write(taxBuffer); taxBuffer = "";
            if (accnosBuffer != "") { params->accnosWriter->write(accnosBuffer); accnosBuffer = ""; }
        }
        
        inFASTA.close();
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClassifySeqsCommand", "driver");
        exit(1);
    }
}
/**************************************************************************************************/

int ClassifySeqsCommand::createProcesses(string taxFileName, string tempTaxFile, string accnos, string filename) {
	try {
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<classifyData*> data;
        
        long long num = 0;

        vector<double> positions;
        vector<linePair> lines;
#if defined NON_WINDOWS
        positions = util.divideFile(filename, processors);
        for (int i = 0; i < (positions.size()-1); i++) {	lines.push_back(linePair(positions[i], positions[(i+1)]));	}
#else
        positions = util.setFilePosFasta(filename, num);
        if (num < processors) { processors = num; }
            
        //figure out how many sequences you have to process
        int numSeqsPerProcessor = num / processors;
        for (int i = 0; i < processors; i++) {
            int startIndex =  i * numSeqsPerProcessor;
            if(i == (processors - 1)){	numSeqsPerProcessor = num - i * numSeqsPerProcessor; 	}
            lines.push_back(linePair(positions[startIndex], numSeqsPerProcessor));
        }
#endif
        auto synchronizedAccnosFile = std::make_shared<SynchronizedOutputFile>(accnos);
        auto synchronizedTaxFile = std::make_shared<SynchronizedOutputFile>(taxFileName);
        auto synchronizedTaxTFile = std::make_shared<SynchronizedOutputFile>(tempTaxFile);

        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            OutputWriter* threadTaxWriter = new OutputWriter(synchronizedTaxFile);
            OutputWriter* threadTaxTWriter = new OutputWriter(synchronizedTaxTFile);
            OutputWriter* threadAccnosWriter = new OutputWriter(synchronizedAccnosFile);
            
            classifyData* dataBundle = new classifyData(threadAccnosWriter, probs, threadTaxWriter, threadTaxTWriter, filename, lines[i+1].start, lines[i+1].end, flip, classify);
            data.push_back(dataBundle);
            
            workerThreads.push_back(new std::thread(driverClassifier, dataBundle));
        }
        
        OutputWriter* threadTaxWriter = new OutputWriter(synchronizedTaxFile);
        OutputWriter* threadTaxTWriter = new OutputWriter(synchronizedTaxTFile);
        OutputWriter* threadAccnosWriter = new OutputWriter(synchronizedAccnosFile);
        
        classifyData* dataBundle = new classifyData(threadAccnosWriter, probs, threadTaxWriter, threadTaxTWriter, filename, lines[0].start, lines[0].end, flip, classify);
        driverClassifier(dataBundle);
        num = dataBundle->count;
        
        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            num += data[i]->count;
            
            delete data[i]->taxTWriter;
            delete data[i]->taxWriter;
            delete data[i]->accnosWriter;

            delete data[i];
            delete workerThreads[i];
        }
        synchronizedTaxTFile->close(); synchronizedTaxFile->close(); synchronizedAccnosFile->close();
        delete threadTaxWriter; delete threadTaxTWriter; delete threadAccnosWriter;
        delete dataBundle;
        
        return num;
	}
	catch(exception& e) {
		m->errorOut(e, "ClassifySeqsCommand", "createProcesses");
		exit(1);
	}
}
/**************************************************************************************************/


