/*
 *  clustersplitcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 5/19/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "clustersplitcommand.h"

#include "calculators/accuracy.hpp"
#include "calculators/f1score.hpp"
#include "calculators/fdr.hpp"
#include "calculators/fn.hpp"
#include "calculators/fp.hpp"
#include "calculators/fpfn.hpp"
#include "calculators/mcc.hpp"
#include "calculators/npv.hpp"
#include "calculators/ppv.hpp"
#include "calculators/sensitivity.hpp"
#include "calculators/specificity.hpp"
#include "calculators/tn.hpp"
#include "calculators/tp.hpp"
#include "calculators/tptn.hpp"
#include "clusterclassic.h"
#include "commands/sensspeccommand.h"
#include "datastructures/nameassignment.hpp"
#include "inputdata.h"
#include "opticluster.h"
#include "read/readcolumn.h"
#include "read/readmatrix.hpp"
#include "read/splitmatrix.h"
#include "systemcommand.h"
#include "vsearchfileparser.h"

//**********************************************************************************************************************
vector<string> ClusterSplitCommand::setParameters(){	
	try {
        CommandParameter pfile("file", "InputTypes", "", "", "PhylipColumnFasta", "PhylipColumnFasta", "none","",false,false,true); parameters.push_back(pfile);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "none", "none", "FastaTaxName","",false,false,true); parameters.push_back(ptaxonomy);
		CommandParameter pfasta("fasta", "InputTypes", "", "", "PhylipColumnFasta", "PhylipColumnFasta", "FastaTaxName","list",false,false,true); parameters.push_back(pfasta);
		CommandParameter pname("name", "InputTypes", "", "", "NameCount", "none", "ColumnName-FastaTaxName","rabund-sabund",false,false,true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "NameCount", "none", "","",false,false,true); parameters.push_back(pcount);
		CommandParameter ptaxlevel("taxlevel", "Number", "", "3", "", "", "","",false,false,true); parameters.push_back(ptaxlevel);
		CommandParameter pshowabund("showabund", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pshowabund);
        CommandParameter prunspenspec("runsensspec", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(prunspenspec);
        CommandParameter pcluster("cluster", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pcluster);
		CommandParameter ptiming("timing", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(ptiming);
		CommandParameter pprocessors("processors", "Number", "", "1", "", "", "","",false,false,true); parameters.push_back(pprocessors);
		CommandParameter pcutoff("cutoff", "Number", "", "0.03", "", "", "","",false,false,true); parameters.push_back(pcutoff);
        CommandParameter pmetriccutoff("delta", "Number", "", "0.0001", "", "", "","",false,false,true); parameters.push_back(pmetriccutoff);
        CommandParameter piters("iters", "Number", "", "100", "", "", "","",false,false,true); parameters.push_back(piters);
        CommandParameter pinitialize("initialize", "Multiple", "oneotu-singleton", "singleton", "", "", "","",false,false,true); parameters.push_back(pinitialize);
        CommandParameter pprecision("precision", "Number", "", "100", "", "", "","",false,false); parameters.push_back(pprecision);
        CommandParameter pmethod("method", "Multiple", "furthest-nearest-average-weighted-agc-dgc-opti", "opti", "", "", "","",false,false,true); parameters.push_back(pmethod);
        CommandParameter pmetric("metric", "Multiple", "mcc-sens-spec-tptn-fpfn-tp-tn-fp-fn-f1score-accuracy-ppv-npv-fdr", "mcc", "", "", "","",false,false,true); parameters.push_back(pmetric);
       CommandParameter pdist("dist", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pdist);
        CommandParameter pislist("islist", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pislist);
        CommandParameter pclassic("classic", "Boolean", "", "F", "", "", "","",false,false); parameters.push_back(pclassic);
        CommandParameter pvsearchlocation("vsearch", "String", "", "", "", "", "","",false,false); parameters.push_back(pvsearchlocation);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["list"] = tempOutNames;
        outputTypes["rabund"] = tempOutNames;
        outputTypes["sabund"] = tempOutNames;
        outputTypes["column"] = tempOutNames;
        outputTypes["name"] = tempOutNames;
        outputTypes["file"] = tempOutNames;
        outputTypes["sensspec"] = tempOutNames;
			
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterSplitCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The cluster.split command parameter options are file, fasta, name, count, cutoff, precision, method, taxonomy, taxlevel, showabund, timing, cluster, iters, delta, initialize, dist, processors, runsensspec. Fasta or file are required.\n";
		helpString += "The cluster.split command splits your files by classification using a fasta file to generate distance matrices for each taxonomic group. \n";
        helpString += "The file option allows you to enter your file containing your list of column and names/count files as well as the singleton file.  This file is mothur generated, when you run cluster.split() with the cluster=f parameter.  This can be helpful when you have a large dataset that you may be able to use all your processors for the splitting step, but have to reduce them for the cluster step due to RAM constraints. For example: cluster.split(fasta=yourFasta, taxonomy=yourTax, count=yourCount, taxlevel=3, cluster=f, processors=8) then cluster.split(file=yourFile, processors=4).  This allows your to maximize your processors during the splitting step.  Also, if you are unsure if the cluster step will have RAM issue with multiple processors, you can avoid running the first part of the command multiple times.\n";
		helpString += "The fasta parameter allows you to enter your aligned fasta file. \n";
		helpString += "The name parameter allows you to enter your name file. \n";
        helpString += "The count parameter allows you to enter your count file.\n";
        helpString += "The taxonomy parameter allows you to enter the taxonomy file for your sequences. This is required unless you are running the command with the file option. \n";
        helpString += "The cluster parameter allows you to indicate whether you want to run the clustering or just split the dataset into taxanomic matrices, default=t";
        helpString += "The dist parameter allows you to indicate whether you want a column formatted distance matrix outputted along with the list file. Default=F.";
		helpString += "The cutoff parameter allow you to set the distance you want to cluster to, default is 0.03. \n";
		helpString += "The precision parameter allows you specify the precision of the precision of the distances outputted, default=100, meaning 2 decimal places. \n";
        helpString += "The iters parameter allow you to set the maxiters for the opticluster method. \n";
        helpString += "The metric parameter allows to select the metric in the opticluster method. Options are Matthews correlation coefficient (mcc), sensitivity (sens), specificity (spec), true positives + true negatives (tptn), false positives + false negatives (fpfn), true positives (tp), true negative (tn), false positive (fp), false negative (fn), f1score (f1score), accuracy (accuracy), positive predictive value (ppv), negative predictive value (npv), false discovery rate (fdr). Default=mcc.\n";
        helpString += "The delta parameter allows to set the stable value for the metric in the opticluster method. Default=0.0001\n";
        helpString += "The initialize parameter allows to select the initial randomization for the opticluster method. Options are singleton, meaning each sequence is randomly assigned to its own OTU, or oneotu meaning all sequences are assigned to one otu. Default=singleton.\n";
        helpString += "The runsensspec parameter allows to run the sens.spec command on the completed list file. Default=true.\n";
		helpString += "The method parameter allows you to enter your clustering mothod. Options are furthest, nearest, average, weighted, agc, dgc and opti. Default=opti.  The agc and dgc methods require a fasta file.";
        helpString += "The taxlevel parameter allows you to specify the taxonomy level you want to use to split the dataset, default=3.\n";
        helpString += "The classic parameter allows you to indicate that you want to run your files with cluster.classic. Default=f.\n";
        helpString += "The processors parameter allows you to specify the number of processors to use. The default is all available.\n";
         helpString += "The vsearch parameter allows you to specify the name and location of your vsearch executable if using agc or dgc clustering methods. By default mothur will look in your path, mothur's executable and mothur tools locations.  You can set the vsearch location as follows, vsearch=/usr/bin/vsearch.\n";
		helpString += "The cluster.split command should be in the following format: \n";
		helpString += "cluster.split(fasta=yourFastaFile, count=yourCountFile, method=yourMethod, cutoff=yourCutoff, taxonomy=yourTaxonomyfile, taxlevel=yourtaxlevel) \n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string ClusterSplitCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "list") {  pattern = "[filename],[clustertag],list-[filename],[clustertag],[tag2],list"; } 
        else if (type == "rabund") {  pattern = "[filename],[clustertag],rabund"; } 
        else if (type == "sabund") {  pattern = "[filename],[clustertag],sabund"; }
        else if (type == "sensspec") {  pattern = "[filename],[clustertag],sensspec"; }
        else if (type == "column") {  pattern = "[filename],dist"; }
        else if (type == "file")   {  pattern = "[filename],file"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "ClusterSplitCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
string ClusterSplitCommand::getCommonQuestions(){
    try {
        vector<string> questions, issues, qanswers, ianswers, howtos, hanswers;
        
        string issue = "Cluster.split crashes after merging individual list files. What do I do?"; issues.push_back(issue);
        string ianswer = "\tAfter merging the split list files, mothur runs the sens.spec command on the entire dataset. The entire dataset's distance matrix may be too large to fit in memory, which causes the crash. You can skip this step by setting the runsensspec parameter to false. Skipping the sens.spec analysis does not effect the OTU assignment, and you can run the sens.spec analysis separately using the sens.spec command. \n"; ianswers.push_back(ianswer);
        
        issue = "Cluster.split crashes while reading the split distance matrices. What should I do?"; issues.push_back(issue);
        ianswer = "\tThe command is crashing because the distance matrices are too large to fit into memory. Why do I have such a large distance matrix? This is most often caused by poor overlap of your reads. When reads have poor overlap, it greatly increases your error rate. Also, sequences that should cluster together don't because the errors appear to be genetic differences when in fact they are not. The quality of the data you are processing can not be overstressed. Error filled reads produce error filled results. To take a step back, if you look through our MiSeq SOP, you’ll see that we go to great pains to only work with the unique sequences to limit the number of sequences we have to align, screen for chimeras, classify, etc. We all know that 20 million reads will never make it through the pipeline without setting your computer on fire. Returning to the question at hand, you can imagine that if the reads do not fully overlap then any error in the 5’ end of the first read will be uncorrected by the 3’ end of the second read. If we assume for now that the errors are random, then every error will generate a new unique sequence. Granted, this happens less than 1% of the time, but multiply that by 20 million reads at whatever length you choose and you’ve got a big number. Viola, a bunch of unique reads and a ginormous distance matrix. \n"; ianswers.push_back(ianswer);
        
        string howto = "How do I cluster my sequences into OTUs at distance 0.03?"; howtos.push_back(howto);
        string hanswer = "\tBy default the cluster.split command will use the opti method to cluster to 0.03. To find OTUs at a different distance set the cutoff parameter. ie. cutoff=0.01 will assemble OTUs for distance 0.01.\n"; hanswers.push_back(hanswer);
        
        string commonQuestions = util.getFormattedHelp(questions, qanswers, issues, ianswers, howtos, hanswers);
        
        return commonQuestions;
    }
    catch(exception& e) {
        m->errorOut(e, "ClusterSplitCommand", "getCommonQuestions");
        exit(1);
    }
}
//**********************************************************************************************************************
//This function checks to make sure the cluster command has no errors and then clusters based on the method chosen.
ClusterSplitCommand::ClusterSplitCommand(string option) : Command()  {
	try{
		format = "";
		
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			
			//check for required parameters
			file = validParameter.validFile(parameters, "file");
			if (file == "not open") { file = ""; abort = true; }
			else if (file == "not found") { file = ""; }
            else { distfile = file; type = ""; }
			
			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { abort = true; namefile = "";}	
			else if (namefile == "not found") { namefile = "";  }
            else { current->setNameFile(namefile); type = "name"; }
            
            countfile = validParameter.validFile(parameters, "count");
			if (countfile == "not open") { abort = true; countfile = "";}	
			else if (countfile == "not found") { countfile = "";  }
            else { current->setCountFile(countfile); type = "count"; }
			
			fastafile = validParameter.validFile(parameters, "fasta");
			if (fastafile == "not open") { abort = true; }	
			else if (fastafile == "not found") { fastafile = ""; }
			else { distfile = fastafile;  current->setFastaFile(fastafile); }
			
			taxFile = validParameter.validFile(parameters, "taxonomy");
			if (taxFile == "not open") { taxFile = ""; abort = true; }	
			else if (taxFile == "not found") { taxFile = ""; }
			else { current->setTaxonomyFile(taxFile); }
			
            if ((fastafile == "") && (file == "")) {
                fastafile = current->getFastaFile();
                if (fastafile != "") {   m->mothurOut("Using " + fastafile + " as input file for the fasta parameter.\n");  }
                else {
                    m->mothurOut("[ERROR]: No valid current files. When executing a cluster.split command you must enter a file file or fastafile.\n"); abort = true;
                }
            }
			else if ((fastafile != "") && (file != "")) { m->mothurOut("[ERROR]: When executing a cluster.split command you must enter ONLY ONE of the following: file or fasta.\n");  abort = true; }
            
            if ((countfile != "") && (namefile != "")) { m->mothurOut("[ERROR]: When executing a cluster.split command you must enter ONLY ONE of the following: count or name.\n");  abort = true; }
            
            if (file != "") {
                if ((namefile == "") && (countfile == "")) {
                    m->mothurOut("\n[WARNING]: When using the file option, it is recommended you include the name or count file. Doing so will ensure the OTUs are printed by OTU size reflecting the redundant reads, instead of just the unique reads.\n");
                }
            }
			
			if (fastafile != "") {
				if (taxFile == "") { 
					taxFile = current->getTaxonomyFile(); 
					if (taxFile != "") {  m->mothurOut("Using " + taxFile + " as input file for the taxonomy parameter.\n");  }
					else { 
						m->mothurOut("[ERROR]: You need to provide a taxonomy file if you are if you are using a fasta file to generate the split.\n");
						abort = true; 
					}	
				}
				
				if ((namefile == "") && (countfile == "")) { 
					namefile = current->getNameFile(); 
                    if (namefile != "") {  m->mothurOut("Using " + namefile + " as input file for the name parameter.\n");  type = "name"; }
					else { 
						countfile = current->getCountFile();
                        if (countfile != "") {  m->mothurOut("Using " + countfile + " as input file for the count parameter.\n"); type = "count"; }
                        else {  m->mothurOut("[ERROR]: You need to provide a namefile or countfile.\n");   abort = true;
                        }	
					}	
				}
			}
					
			
			string temp;
			temp = validParameter.valid(parameters, "precision");
			if (temp == "not found") { temp = "100"; }
			//saves precision legnth for formatting below
			length = temp.length();
			util.mothurConvert(temp, precision); 
			
			temp = validParameter.valid(parameters, "processors");	if (temp == "not found"){	temp = current->getProcessors();	}
			processors = current->setProcessors(temp);
			
            temp = validParameter.valid(parameters, "classic");			if (temp == "not found") { temp = "F"; }
			classic = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "runsensspec");			if (temp == "not found") { temp = "T"; }
            runsensSpec = util.isTrue(temp);
            
			temp = validParameter.valid(parameters, "taxlevel");		if (temp == "not found")  { temp = "3"; }
			util.mothurConvert(temp, taxLevelCutoff);
            
            temp = validParameter.valid(parameters, "iters");		if (temp == "not found")  { temp = "100"; }
            util.mothurConvert(temp, maxIters);
            
            temp = validParameter.valid(parameters, "delta");		if (temp == "not found")  { temp = "0.0001"; }
            util.mothurConvert(temp, stableMetric);
			
            metricName = validParameter.valid(parameters, "metric");		if (metricName == "not found") { metricName = "mcc"; }
            
            if ((metricName == "mcc") || (metricName == "sens") || (metricName == "spec") || (metricName == "tptn") || (metricName == "tp") || (metricName == "tn") || (metricName == "fp") || (metricName == "fn") || (metricName == "f1score") || (metricName == "accuracy") || (metricName == "ppv") || (metricName == "npv") || (metricName == "fdr") || (metricName == "fpfn") ){ }
            else { m->mothurOut("[ERROR]: Not a valid metric.  Valid metrics are mcc, sens, spec, tp, tn, fp, fn, tptn, fpfn, f1score, accuracy, ppv, npv, fdr.\n");  abort = true; }
            
            initialize = validParameter.valid(parameters, "initialize");		if (initialize == "not found") { initialize = "singleton"; }
            
            if ((initialize == "singleton") || (initialize == "oneotu")){ }
            else { m->mothurOut("[ERROR]: Not a valid initialization.  Valid initializations are singleton and oneotu.\n");  abort = true; }

			method = validParameter.valid(parameters, "method");		if (method == "not found") { method = "opti";  }
            
            vector<string> versionOutputs;
            bool foundTool = false;
            string programName = "vsearch"; programName += EXECUTABLE_EXT;
            
            vsearchLocation = validParameter.validPath(parameters, "vsearch");
            if (vsearchLocation == "not found") {
                vsearchLocation = "";
                if ((method == "agc") || (method == "dgc")) {
                    foundTool = util.findTool(programName, vsearchLocation, versionOutputs, current->getLocations());
                }
            }
            else {
                if ((method == "agc") || (method == "dgc")) {
                    //test to make sure vsearch exists
                    ifstream in;
                    vsearchLocation = util.getFullPathName(vsearchLocation);
                    bool ableToOpen = util.openInputFile(vsearchLocation, in, "no error"); in.close();
                    if(!ableToOpen) {
                        m->mothurOut(vsearchLocation + " file does not exist or cannot be opened, ignoring.\n"); vsearchLocation = "";
                        programName = util.getSimpleName(vsearchLocation); vsearchLocation = "";
                        foundTool = util.findTool(programName, vsearchLocation, versionOutputs, current->getLocations());
                    }
                }
            }
			
            if ((method == "furthest") || (method == "nearest") || (method == "average") || (method == "weighted") || (method == "agc") || (method == "dgc") || (method == "opti")) { }
            else { m->mothurOut("[ERROR]: Not a valid clustering method.  Valid clustering algorithms are furthest, nearest, average, weighted, agc, dgc and opti.\n");  abort = true; }
            
            if ((method == "agc") || (method == "dgc")) {
                if (fastafile == "") { m->mothurOut("[ERROR]: You must provide a fasta file when using the agc or dgc clustering methods, aborting\n."); abort = true;}
                if (classic) { m->mothurOut("[ERROR]: You cannot use cluster.classic with the agc or dgc clustering methods, aborting\n."); abort = true; }
                if (!foundTool) { abort = true; }
            }
            
            cutoffNotSet = false;
            temp = validParameter.valid(parameters, "cutoff");
            if (temp == "not found") { cutoffNotSet = true; if ((method == "opti") || (method == "agc") || (method == "dgc")) { temp = "0.03"; }else { temp = "0.15"; } }
            util.mothurConvert(temp, cutoff);
            
			temp = validParameter.valid(parameters, "showabund");
			if (temp == "not found") { temp = "T"; }
            showabund = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "cluster");  if (temp == "not found") { temp = "T"; }
            runCluster = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "islist");  if (temp == "not found") { temp = "F"; }
            isList = util.isTrue(temp);
            
            temp = validParameter.valid(parameters, "dist");  if (temp == "not found") { temp = "F"; }
            makeDist = util.isTrue(temp);
            if (method == "opti") { makeDist = runsensSpec;  }
            
            if (classic && makeDist) { m->mothurOut("[ERROR]: You cannot use the dist parameter with the classic parameter. Mothur will ignore the dist parameter.\n"); makeDist = false; }

			timing = validParameter.valid(parameters, "timing");
			if (timing == "not found") { timing = "F"; }
		}
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "ClusterSplitCommand");
		exit(1);
	}
}

//**********************************************************************************************************************

int ClusterSplitCommand::execute(){
	try {
	
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        time_t estart;
        vector<string> listFileNames;
        vector< map<string, string> > distName;
        set<string> labels;
        string singletonName = "";
        
        double saveCutoff = cutoff;

        if (file != "") {
            deleteFiles = false; estart = time(nullptr);
            singletonName = readFile(distName);

            if (isList) {
                
                //set list file as new current listfile
                string currentName = "";
                itTypes = outputTypes.find("list");
                if (itTypes != outputTypes.end()) {
                    if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setListFile(currentName); }
                }
                
                m->mothurOut("\nOutput File Names: \n");
                for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] + "\n");	} m->mothurOutEndLine();
                
                return 0;
            }
                    
        }else {
            //splitting
            estart = time(nullptr); bool usingVsearchToCLuster = false;
            if ((method == "agc") || (method == "dgc")) { usingVsearchToCLuster = true; if (cutoffNotSet) {  m->mothurOut("\nYou did not set a cutoff, using 0.03.\n"); cutoff = 0.03; } }
            
            m->mothurOut("Splitting the file...\n");
            current->setMothurCalling(true);
            
            //split matrix into non-overlapping groups
            SplitMatrix* split = new SplitMatrix(fastafile, namefile, countfile, taxFile, taxLevelCutoff, cutoff,  processors, classic, outputdir, usingVsearchToCLuster);

            if (fastafile != "") {  current->setFastaFile(fastafile);  }

            if (m->getControl_pressed()) { delete split; return 0; }
                
            singletonName = split->getSingletonNames();
            distName = split->getDistanceFiles();  //returns map of distance files -> namefile sorted by distance file size
            delete split;
            current->setMothurCalling(false);
            
            if (m->getDebug()) { m->mothurOut("[DEBUG]: distName.size() = " + toString(distName.size()) + ".\n"); }
            
            m->mothurOut("It took " + toString(time(nullptr) - estart) + " seconds to split the distance file.\n");
            
            //output a merged distance file
            if (makeDist)		{ createMergedDistanceFile(distName); }
            
            if (m->getControl_pressed()) { return 0; }
            
            estart = time(nullptr);
            
            if (!runCluster) {
                string filename = printFile(singletonName, distName);
                
                m->mothurOutEndLine();
                m->mothurOut("Output File Names:\n\n"); m->mothurOut(filename); m->mothurOutEndLine();
                for (int i = 0; i < distName.size(); i++) {	m->mothurOut(distName[i].begin()->first); m->mothurOutEndLine(); m->mothurOut(distName[i].begin()->second); m->mothurOutEndLine();	}
                m->mothurOutEndLine();
                
                return 0;
            }
            deleteFiles = true;
        }
		//****************** break up files between processes and cluster each file set ******************************//
		
        listFileNames = createProcesses(distName, labels);
        
        if (deleteFiles) {
            //delete the temp files now that we are done
            for (int i = 0; i < distName.size(); i++) {
                string thisNamefile = distName[i].begin()->second;
                string thisDistFile = distName[i].begin()->first;
                util.mothurRemove(thisNamefile);
                util.mothurRemove(thisDistFile);
            }
        }
		
		if (m->getControl_pressed()) { for (int i = 0; i < listFileNames.size(); i++) { util.mothurRemove(listFileNames[i]); } return 0; }
		
		if (!util.isEqual(saveCutoff, cutoff)) { m->mothurOut("\nCutoff was " + toString(saveCutoff) + " changed cutoff to " + toString(cutoff)); m->mothurOutEndLine();  }
		
		m->mothurOut("It took " + toString(time(nullptr) - estart) + " seconds to cluster\n");
		
		//****************** merge list file and create rabund and sabund files ******************************//
		estart = time(nullptr);
		m->mothurOut("Merging the clustered files...\n");

		ListVector* listSingle;
		map<double, int> labelBins = completeListFile(listFileNames, singletonName, labels, listSingle); //returns map of label to numBins
		
		if (m->getControl_pressed()) { if (listSingle != nullptr) { delete listSingle; } for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); } return 0; }
		
		mergeLists(listFileNames, labelBins, listSingle);

		if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); } return 0; }
        
        //delete after all are complete incase a crash happens
        if (!deleteFiles) { for (int i = 0; i < distName.size(); i++) {	util.mothurRemove(distName[i].begin()->first); util.mothurRemove(distName[i].begin()->second); 	} }
		
		m->mothurOut("It took " + toString(time(nullptr) - estart) + " seconds to merge.\n");
        
        if ((method == "opti") && (runsensSpec)) { runSensSpec();  }
        
        if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) { util.mothurRemove(outputNames[i]); } return 0; }
        
		//set list file as new current listfile
		string currentName = "";
		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setListFile(currentName); }
		}
		
		//set rabund file as new current rabundfile
		itTypes = outputTypes.find("rabund");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setRabundFile(currentName); } }
		
		//set sabund file as new current sabundfile
		itTypes = outputTypes.find("sabund");
		if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setSabundFile(currentName); } }
        
        //set sabund file as new current sabundfile
        itTypes = outputTypes.find("column");
        if (itTypes != outputTypes.end()) { if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setColumnFile(currentName); } }
				
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();

		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************
map<double, int> ClusterSplitCommand::completeListFile(vector<string> listNames, string singleton, set<string>& userLabels, ListVector*& listSingle){
	try {
		map<double, int> labelBin;
		vector<double> orderFloat;
		int numSingleBins;
        
		//read in singletons
		if (singleton != "none") {
            
            listSingle = new ListVector();
            if (type == "count") {
                
                CountTable ct; ct.readTable(singleton, false, false);
                
                vector<string> singletonSeqNames = ct.getNamesOfSeqs();
                
                for (int i = 0; i < singletonSeqNames.size(); i++) { listSingle->push_back(singletonSeqNames[i]); }
                
            }else if (type == "name") {
                map<string, string> singletonSeqNames; util.readNames(singleton, singletonSeqNames);
                
                for (map<string, string>::iterator it = singletonSeqNames.begin(); it != singletonSeqNames.end(); it++) { listSingle->push_back(it->second); }
            }
    
			util.mothurRemove(singleton);
            
			numSingleBins = listSingle->getNumBins();
        }else{  listSingle = nullptr; numSingleBins = 0;  }
		
        //go through users set and make them floats so we can sort them
        for(set<string>::iterator it = userLabels.begin(); it != userLabels.end(); ++it) {
            double temp = -10.0;
            
            if ((*it != "unique") && (convertTestFloat(*it, temp) ))	{	util.mothurConvert(*it, temp);	}
            else if (*it == "unique")										{	temp = -1.0;		}
            
            if ((temp < cutoff) || util.isEqual(cutoff, temp)) {
                orderFloat.push_back(temp);
                labelBin[temp] = numSingleBins; //initialize numbins
            }
        }
	
		//sort order
		sort(orderFloat.begin(), orderFloat.end());
		userLabels.clear();
        
		//get the list info from each file
		for (int k = 0; k < listNames.size(); k++) {
            
			if (m->getControl_pressed()) {  
				if (listSingle != nullptr) { delete listSingle; listSingle = nullptr; util.mothurRemove(singleton);  }
				for (int i = 0; i < listNames.size(); i++) {   util.mothurRemove(listNames[i]);  }
				return labelBin;
			}
			
			InputData* input = new InputData(listNames[k], "list", nullVector);
			ListVector* list = input->getListVector();
			string lastLabel = list->getLabel();
            
			string filledInList = listNames[k] + "filledInTemp";
			ofstream outFilled;
			util.openOutputFile(filledInList, outFilled);
            bool printHeaders = true;
            
            
			//for each label needed
			for(int l = 0; l < orderFloat.size(); l++){
                
				string thisLabel;
				if (util.isEqual(orderFloat[l],-1)) { thisLabel = "unique"; }
				else { thisLabel = toString(orderFloat[l],  length-1);  } 
                
				//this file has reached the end
				if (list == nullptr) { 
					list = input->getListVector(lastLabel, true); 
				}else{	//do you have the distance, or do you need to fill in
						
					float labelFloat;
					if (list->getLabel() == "unique") {  labelFloat = -1.0;  }
					else { convert(list->getLabel(), labelFloat); }
                    
					//check for missing labels
					if (labelFloat > orderFloat[l]) { //you are missing the label, get the next smallest one
						//if its bigger get last label, otherwise keep it
						delete list;
						list = input->getListVector(lastLabel, true);  //get last list vector to use, you actually want to move back in the file
					}
					lastLabel = list->getLabel();
				}
				
				//print to new file
				list->setLabel(thisLabel);
                list->setPrintedLabels(printHeaders);
                list->print(outFilled, true); printHeaders = false;
		
				//update labelBin
				labelBin[orderFloat[l]] += list->getNumBins();
									
				delete list;
									
				list = input->getListVector();
			}
			
			if (list != nullptr) { delete list; }
			delete input;
			
			outFilled.close();
			util.mothurRemove(listNames[k]);
			rename(filledInList.c_str(), listNames[k].c_str());
		}
		
		return labelBin;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "completeListFile");
		exit(1);
	}
}
//**********************************************************************************************************************
int ClusterSplitCommand::mergeLists(vector<string> listNames, map<double, int> userLabels, ListVector* listSingle){
	try {
		if (outputdir == "") { outputdir += util.hasPath(distfile); }
		fileroot = outputdir + util.getRootName(util.getSimpleName(distfile));
		
        map<string, string> variables; 
        variables["[filename]"] = fileroot;
        variables["[clustertag]"] = tag;
        string sabundFileName = getOutputFileName("sabund", variables);
        string rabundFileName = getOutputFileName("rabund", variables);
        string listFileName = getOutputFileName("list", variables);
        
        map<string, int> counts;
        ofstream outList, outRabund, outSabund;
        if (countfile == "") {
            util.openOutputFile(sabundFileName,	outSabund);
            util.openOutputFile(rabundFileName,	outRabund);
            outputNames.push_back(sabundFileName); outputTypes["sabund"].push_back(sabundFileName);
            outputNames.push_back(rabundFileName); outputTypes["rabund"].push_back(rabundFileName);
            
        }else {
            CountTable ct;
            ct.readTable(countfile, false, false);
            counts = ct.getNameMap();
        }
        
		util.openOutputFile(listFileName,	outList);
        outputNames.push_back(listFileName); outputTypes["list"].push_back(listFileName);
        bool printHeaders = true;

		//for each label needed
		for(map<double, int>::iterator itLabel = userLabels.begin(); itLabel != userLabels.end(); itLabel++) {
			
			string thisLabel;
			if (util.isEqual(itLabel->first,-1)) { thisLabel = "unique"; }
			else { thisLabel = toString(itLabel->first,  length-1);  } 
			
			//outList << thisLabel << '\t' << itLabel->second << '\t';
            
            RAbundVector* rabund = nullptr;
            ListVector completeList;
            completeList.setLabel(thisLabel);
            
            if (countfile == "") {
                rabund = new RAbundVector();
                rabund->setLabel(thisLabel);
            }

			//add in singletons
			if (listSingle != nullptr) {
				for (int j = 0; j < listSingle->getNumBins(); j++) {
					//outList << listSingle->get(j) << '\t';
                    completeList.push_back(listSingle->get(j));
					if (countfile == "") { rabund->push_back(util.getNumNames(listSingle->get(j))); }
				}
			}
			
			//get the list info from each file
			for (int k = 0; k < listNames.size(); k++) {
	
				if (m->getControl_pressed()) {  if (listSingle != nullptr) { delete listSingle;   } for (int i = 0; i < listNames.size(); i++) { util.mothurRemove(listNames[i]);  } if (rabund != nullptr) { delete rabund; } return 0; }
				
				InputData* input = new InputData(listNames[k], "list", nullVector);
				ListVector* list = input->getListVector(thisLabel);
				
				//this file has reached the end
				if (list == nullptr) { m->mothurOut("Error merging listvectors in file " + listNames[k]); m->mothurOutEndLine();  }	
				else {		
					for (int j = 0; j < list->getNumBins(); j++) {
                        completeList.push_back(list->get(j));
						if (countfile == "") { rabund->push_back(util.getNumNames(list->get(j))); }
					}
					delete list;
				}
				delete input;
			}
			
            if (countfile == "") {
                SAbundVector sabund = rabund->getSAbundVector();
                sabund.print(outSabund);
                rabund->print(outRabund);
            }

            completeList.setPrintedLabels(printHeaders);
            if (countfile != "") { completeList.print(outList, counts);  printHeaders = false; }
            else { completeList.print(outList);  printHeaders = false; }
			
			if (rabund != nullptr) { delete rabund; }
		}
		
		outList.close();
        if (countfile == "") {
            outRabund.close();
            outSabund.close();
		}
		if (listSingle != nullptr) { delete listSingle;  }
		
		for (int i = 0; i < listNames.size(); i++) {  util.mothurRemove(listNames[i]);  }
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "mergeLists");
		exit(1);
	}
}
/**************************************************************************************************/
struct clusterData {
    MothurOut* m;
    Utils util;
    int count, precision, length, maxIters; //numSingletons,
    bool showabund, classic, useName, useCount, deleteFiles, cutoffNotSet;
    double cutoff, stableMetric;
    ofstream outList, outRabund, outSabund;
    string tag, method,  vsearchLocation, metricName, initialize, outputDir, type;
    vector< map<string, string> > distNames;
    set<string> labels;
    vector<string> listFileNames;
    
    clusterData(){}
    clusterData(bool showab, bool cla, bool df, vector< map<string, string> > dN, bool cns, double cu, int prec, int len, string meth, string opd, string vl, string ty) {
        showabund = showab;
        distNames = dN;
        cutoff = cu;
        classic = cla;
        method = meth;
        precision = prec;
        length = len;
        outputDir = opd;
        vsearchLocation = vl;
        deleteFiles = df;
        cutoffNotSet = cns;
        m = MothurOut::getInstance();
        count = 0;
        type = ty;
        useName = false;
        useCount = false;
        //numSingletons = 0;
    }
    void setOptiOptions(string metn, double stabMet, string init, int mxi ) {
        metricName = metn;
        stableMetric = stabMet;
        maxIters = mxi;
        initialize = init;
    }
    void setNamesCount(string cnf) {
        useName = false;
        useCount = false;
        if (type == "name") { useName = true;  }
        if (type == "count") { useCount = true; }
    }
};
//**********************************************************************************************************************
int createRabund(CountTable*& ct, ListVector*& list, RAbundVector*& rabund, clusterData* params){
    try {
        rabund->setLabel(list->getLabel());
        for(int i = 0; i < list->getNumBins(); i++) {
            if (params->m->getControl_pressed()) { break; }
            vector<string> binNames;
            string bin = list->get(i);
            params->util.splitAtComma(bin, binNames);
            int total = 0;
            for (int j = 0; j < binNames.size(); j++) { total += ct->getNumSeqs(binNames[j]);  }
            rabund->push_back(total);
        }
        return 0;
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "createRabund");
        exit(1);
    }
    
}
//**********************************************************************************************************************
string clusterClassicFile(string thisDistFile, string thisNamefile, double& smallestCutoff, clusterData* params){
    try {
        string listFileName = "";
        
        ListVector* list = nullptr;
        ListVector oldList;
        RAbundVector* rabund = nullptr;
        
        params->m->mothurOut("\nReading " + thisDistFile + "\n");
        
        //reads phylip file storing data in 2D vector, also fills list and rabund
        bool sim = false;
        ClusterClassic cluster(params->cutoff, params->method, sim);
        
        NameAssignment* nameMap = nullptr;
        CountTable* ct = nullptr;
        if(params->useName){
            nameMap = new NameAssignment(thisNamefile);
            nameMap->readMap();
            cluster.readPhylipFile(thisDistFile, nameMap);
        }else if (params->useCount) {
            ct = new CountTable();
            ct->readTable(thisNamefile, false, false);
            cluster.readPhylipFile(thisDistFile, ct);
        }
        params->tag = cluster.getTag();
        
        if (params->m->getControl_pressed()) { if(params->useName){	delete nameMap; }
            else if (params->useCount) { delete ct; } return listFileName; }
        
        list = cluster.getListVector();
        rabund = cluster.getRAbundVector();
        
        string thisOutputDir = params->outputDir;
        if (params->outputDir == "") { thisOutputDir += params->util.hasPath(thisDistFile); }
        string fileroot = thisOutputDir + params->util.getRootName(params->util.getSimpleName(thisDistFile));
        listFileName = fileroot+ params->tag + ".list";
        
        ofstream listFile;
        params->util.openOutputFile(fileroot+ params->tag + ".list",	listFile);
        
        float previousDist = 0.00000;
        float rndPreviousDist = 0.00000;
        bool printHeaders = true;
        oldList = *list;
        
        params->m->mothurOut("\nClustering " + thisDistFile + "\n");
        
        while ((cluster.getSmallDist() < params->cutoff) && (cluster.getNSeqs() > 1)){
            if (params->m->getControl_pressed()) {  delete list; delete rabund; listFile.close();  if(params->useName){	delete nameMap; }
                else if (params->useCount) { delete ct; } return listFileName;  }
            
            cluster.update(params->cutoff);
            
            float dist = cluster.getSmallDist();
            float rndDist = params->util.ceilDist(dist, params->precision);
            
            if(previousDist <= 0.0000 && !params->util.isEqual(dist, previousDist)){
                oldList.setLabel("unique");
                oldList.setPrintedLabels(printHeaders);
                oldList.print(listFile); printHeaders = false;
                if (params->labels.count("unique") == 0) {  params->labels.insert("unique");  }
            }
            else if(!params->util.isEqual(rndDist, rndPreviousDist)){
                oldList.setLabel(toString(rndPreviousDist,  params->length-1));
                oldList.setPrintedLabels(printHeaders);
                oldList.print(listFile); printHeaders = false;
                if (params->labels.count(toString(rndPreviousDist,  params->length-1)) == 0) { params->labels.insert(toString(rndPreviousDist,  params->length-1)); }
            }
            
            
            previousDist = dist;
            rndPreviousDist = rndDist;
            oldList = *list;
        }
        
        if(previousDist <= 0.0000){
            oldList.setLabel("unique");
            oldList.setPrintedLabels(printHeaders);
            oldList.print(listFile); printHeaders = false;
            if (params->labels.count("unique") == 0) { params->labels.insert("unique"); }
        }
        else if(rndPreviousDist<params->cutoff){
            oldList.setLabel(toString(rndPreviousDist,  params->length-1));
            oldList.setPrintedLabels(printHeaders);
            oldList.print(listFile); printHeaders = false;
            if (params->labels.count(toString(rndPreviousDist,  params->length-1)) == 0) { params->labels.insert(toString(rndPreviousDist,  params->length-1)); }
        }
        listFile.close();
        
        delete list; delete rabund;
        if(params->useName)         {	delete nameMap; }
        else if (params->useCount)  {   delete ct;      }
        
        if (params->deleteFiles) {
            params->util.mothurRemove(thisDistFile);
            params->util.mothurRemove(thisNamefile);
        }
        return listFileName;
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "clusterClassicFile");
        exit(1);
    }
}
//**********************************************************************************************************************
string runOptiCluster(string thisDistFile, string thisNamefile, double& smallestCutoff, clusterData* params){
    try {
        if (params->cutoffNotSet) {  params->m->mothurOut("\nYou did not set a cutoff, using 0.03.\n"); params->cutoff = 0.03;  }
        
        string nameOrCount = params->type;
        
        OptiMatrix matrix(thisDistFile, thisNamefile, nameOrCount, "column", params->cutoff, false);
        
        ClusterMetric* metric = nullptr;
        if (params->metricName == "mcc")             { metric = new MCC();              }
        else if (params->metricName == "sens")       { metric = new Sensitivity();      }
        else if (params->metricName == "spec")       { metric = new Specificity();      }
        else if (params->metricName == "tptn")       { metric = new TPTN();             }
        else if (params->metricName == "tp")         { metric = new TP();               }
        else if (params->metricName == "tn")         { metric = new TN();               }
        else if (params->metricName == "fp")         { metric = new FP();               }
        else if (params->metricName == "fn")         { metric = new FN();               }
        else if (params->metricName == "f1score")    { metric = new F1Score();          }
        else if (params->metricName == "accuracy")   { metric = new Accuracy();         }
        else if (params->metricName == "ppv")        { metric = new PPV();              }
        else if (params->metricName == "npv")        { metric = new NPV();              }
        else if (params->metricName == "fdr")        { metric = new FDR();              }
        else if (params->metricName == "fpfn")       { metric = new FPFN();             }
        
        OptiCluster cluster(&matrix, metric, 0);
        params->tag = cluster.getTag();
        
        params->m->mothurOut("\nClustering " + thisDistFile + "\n");
        
        string thisOutputDir = params->outputDir;
        if (params->outputDir == "") { thisOutputDir += params->util.hasPath(thisDistFile); }
        string fileroot = thisOutputDir + params->util.getRootName(params->util.getSimpleName(thisDistFile));
        string listFileName = fileroot+ params->tag + ".list";

        int iters = 0;
        double listVectorMetric = 0; //worst state
        double delta = 1;
        
        cluster.initialize(listVectorMetric, true, params->initialize);
        
        while ((delta > params->stableMetric) && (iters < params->maxIters)) {
            
            if (params->m->getControl_pressed()) { if (params->deleteFiles) { params->util.mothurRemove(thisDistFile);  params->util.mothurRemove(thisNamefile); } return listFileName; }
            double oldMetric = listVectorMetric;
            cluster.update(listVectorMetric);
            
            delta = abs(oldMetric - listVectorMetric);
            iters++;
        }
        
        if (params->m->getControl_pressed()) { delete metric; metric = nullptr; return 0; }
        
        ListVector* list = cluster.getList();
        list->setLabel(toString(smallestCutoff));
        //params->cutoff = params->util.ceilDist(params->cutoff, params->precision);
        params->labels.insert(toString(smallestCutoff));
        
        ofstream listFile;
        params->util.openOutputFile(listFileName,	listFile);
        list->print(listFile);
        listFile.close();
        
        if (params->deleteFiles) {
            params->util.mothurRemove(thisDistFile);
            params->util.mothurRemove(thisNamefile);
        }
        
        double tp, tn, fp, fn;
        params->m->mothurOut("\ntp\ttn\tfp\tfn\tsensitivity\tspecificity\tppv\tnpv\tfdr\taccuracy\tmcc\tf1score\n");
        vector<double> results = cluster.getStats(tp, tn, fp, fn);
        params->m->mothurOut(toString(tp) + "\t" + toString(tn) + "\t" + toString(fp) + "\t" + toString(fn) + "\t");
        for (int i = 0; i < results.size(); i++) { params->m->mothurOut(toString(results[i]) + "\t");  }
        params->m->mothurOut("\n\n");
        return listFileName;
        
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "runOptiCluster");
        exit(1);
    }
}
//**********************************************************************************************************************

int vsearchDriver(string inputFile, string ucClusteredFile, string logfile, double cutoff, clusterData* params){
    try {
        
        //vsearch --maxaccepts 16 --usersort --id 0.97 --minseqlength 30 --wordlength 8 --uc $ROOT.clustered.uc --cluster_smallmem $ROOT.sorted.fna --maxrejects 64 --strand both --log $ROOT.clustered.log --sizeorder
        
        
        ucClusteredFile = params->util.getFullPathName(ucClusteredFile);
        inputFile = params->util.getFullPathName(inputFile);
        logfile = params->util.getFullPathName(logfile);
        
        //to allow for spaces in the path
        ucClusteredFile = "\"" + ucClusteredFile + "\"";
        inputFile = "\"" + inputFile + "\"";
        logfile = "\"" + logfile + "\"";
        
        vector<char*> cPara;
        
        string vsearchCommand = params->vsearchLocation;
        vsearchCommand = "\"" + vsearchCommand + "\" ";
        
        vector<char*> vsearchParameters;
        vsearchParameters.push_back(params->util.mothurConvert(vsearchCommand));
        
        //--maxaccepts=16
        vsearchParameters.push_back(params->util.mothurConvert("--maxaccepts=16"));
        
        //--threads=1
        string processorsString = "--threads=1";
        vsearchParameters.push_back(params->util.mothurConvert(processorsString));
        
        //--usersort
        vsearchParameters.push_back(params->util.mothurConvert("--usersort"));
        
        //--id=0.97
        cutoff = abs(1.0 - cutoff); string cutoffString = toString(cutoff);
        if (cutoffString.length() > 4) {  cutoffString = cutoffString.substr(0, 4);  }
        else if (cutoffString.length() < 4)  {  for (int i = cutoffString.length(); i < 4; i++)  { cutoffString += "0";  } }
        
        cutoffString = "--id=" +  cutoffString;
        vsearchParameters.push_back(params->util.mothurConvert(cutoffString));
        
        //--minseqlength=30
        vsearchParameters.push_back(params->util.mothurConvert("--minseqlength=30"));
        
        //--wordlength=8
        vsearchParameters.push_back(params->util.mothurConvert("--wordlength=8"));

        //--uc=$ROOT.clustered.uc
        string tempIn = "--uc=" + ucClusteredFile;
        vsearchParameters.push_back(params->util.mothurConvert(tempIn));

        //--cluster_smallmem $ROOT.sorted.fna
        string tempSorted = "--cluster_smallmem=" + inputFile;
        vsearchParameters.push_back(params->util.mothurConvert(tempSorted));
     
        //--maxrejects=64
        vsearchParameters.push_back(params->util.mothurConvert("--maxrejects=64"));
        
        //--strand=both
        vsearchParameters.push_back(params->util.mothurConvert("--strand=both"));
        
        //--log=$ROOT.clustered.log
        string tempLog = "--log=" + logfile;
        vsearchParameters.push_back(params->util.mothurConvert(tempLog));

        if (params->method == "agc") { //--sizeorder
            vsearchParameters.push_back(params->util.mothurConvert("--sizeorder"));
         }
          
        if (params->m->getDebug()) {  params->m->mothurOut("[DEBUG]: "); for(int i = 0; i < vsearchParameters.size(); i++)  { params->m->mothurOut(toString(vsearchParameters[i]) + "\t"); } params->m->mothurOut("\n");  }
        
        string commandString = "";
        for (int i = 0; i < vsearchParameters.size(); i++) {    commandString += toString(vsearchParameters[i]) + " "; }
        
#if defined NON_WINDOWS
#else
        commandString = "\"" + commandString + "\"";
#endif
        if (params->m->getDebug()) {  params->m->mothurOut("[DEBUG]: vsearch cluster command = " + commandString + ".\n"); }
        
        system(commandString.c_str());
        
        //free memory
        for(int i = 0; i < vsearchParameters.size(); i++)  {  delete vsearchParameters[i];  }
        
        //remove "" from filenames
        ucClusteredFile = ucClusteredFile.substr(1, ucClusteredFile.length()-2);
        inputFile = inputFile.substr(1, inputFile.length()-2);
        logfile = logfile.substr(1, logfile.length()-2);
        
        return 0;
        
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "vsearchDriver");
        exit(1);
    }
}
//**********************************************************************************************************************
string runVsearchCluster(string thisDistFile, string thisNamefile, double& smallestCutoff, clusterData* params){
    try {
        
        params->m->mothurOut("\nClustering " + thisDistFile + "\n");
        
        string vsearchFastafile = ""; VsearchFileParser* vParse;
        if (params->useName)                    { vParse = new VsearchFileParser(thisDistFile, thisNamefile, "name");       }
        else if (params->useCount)              { vParse = new VsearchFileParser(thisDistFile, thisNamefile, "count");      }
        else                                    { params->m->mothurOut("[ERROR]: Opps, should never get here. ClusterSplitCommand::runVsearchCluster() \n"); params->m->setControl_pressed(true);  return ""; }
        
        if (params->m->getControl_pressed()) {  delete vParse; return ""; }
        
        vsearchFastafile = vParse->getVsearchFile();
        
        if (params->cutoff > 1.0) {  params->m->mothurOut("You did not set a cutoff, using 0.03.\n"); params->cutoff = 0.03; }
        
        //Run vsearch
        string ucVsearchFile = params->util.getSimpleName(vsearchFastafile) + ".clustered.uc";
        string logfile = params->util.getSimpleName(vsearchFastafile) + ".clustered.log";
        vsearchDriver(vsearchFastafile, ucVsearchFile, logfile, smallestCutoff, params);
        
        if (params->m->getControl_pressed()) { params->util.mothurRemove(ucVsearchFile); params->util.mothurRemove(logfile);  params->util.mothurRemove(vsearchFastafile); return ""; }
        
        string thisOutputDir = params->outputDir;
        if (params->outputDir == "") { thisOutputDir += params->util.hasPath(thisDistFile); }
        params->tag = params->method;
        string listFileName = thisOutputDir + params->util.getRootName(params->util.getSimpleName(thisDistFile)) + params->tag + ".list";
        
        //Convert outputted *.uc file into a list file
        map<string, int> counts;
        ListVector list = vParse->createListFile(ucVsearchFile, vParse->getNumBins(logfile), toString(params->cutoff), counts);
        
        ofstream out;
        params->util.openOutputFile(listFileName,	out);

        list.DataVector::printHeaders(out);
        
        if (params->useCount) { list.print(out, counts); }
        else { list.print(out); } delete vParse;

        //remove temp files
        params->util.mothurRemove(ucVsearchFile); params->util.mothurRemove(logfile);  params->util.mothurRemove(vsearchFastafile);
        
        if (params->deleteFiles) {
            params->util.mothurRemove(thisDistFile);
            params->util.mothurRemove(thisNamefile);
        }
        params->labels.insert(toString(params->cutoff));
        
        return listFileName;
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "runVsearchCluster");
        exit(1);
    }
}
//**********************************************************************************************************************
string clusterFile(string thisDistFile, string thisNamefile, double& smallestCutoff, clusterData* params){
    try {
        string listFileName = "";
        
        if ((params->method == "agc") || (params->method == "dgc")) {  listFileName = runVsearchCluster(thisDistFile, thisNamefile, smallestCutoff, params);  }
        else if (params->method == "opti")                          {  listFileName = runOptiCluster(thisDistFile, thisNamefile, smallestCutoff, params);     }
        else {
            
            Cluster* cluster = nullptr;
            SparseDistanceMatrix* matrix = nullptr;
            ListVector* list = nullptr;
            ListVector oldList;
            RAbundVector* rabund = nullptr;
            
            if (params->m->getControl_pressed()) { return listFileName; }
            
            params->m->mothurOut("\nReading " + thisDistFile + "\n");
            
            ReadMatrix* read = new ReadColumnMatrix(thisDistFile);
            read->setCutoff(params->cutoff);
            
            NameAssignment* nameMap = nullptr;
            CountTable* ct = nullptr;
            if(params->useName){
                nameMap = new NameAssignment(thisNamefile);
                nameMap->readMap();
                read->read(nameMap);
            }else if (params->useCount) {
                ct = new CountTable();
                ct->readTable(thisNamefile, false, false);
                read->read(ct);
            }else { read->read(nameMap); }
            
            list = read->getListVector();
            matrix = read->getDMatrix();
            
            if(params->useCount) {
                rabund = new RAbundVector();
                createRabund(ct, list, rabund, params); //creates an rabund that includes the counts for the unique list
                delete ct;
            }else { rabund = new RAbundVector(list->getRAbundVector()); }
            
            delete read;  read = nullptr;
            if (params->useName) { delete nameMap; nameMap = nullptr; }
            
            params->m->mothurOut("\nClustering " + thisDistFile + "\n");
            
            //create cluster
            float adjust = -1.0;
            if (params->method == "furthest")	{	cluster = new CompleteLinkage(rabund, list, matrix, params->cutoff, params->method, adjust); }
            else if(params->method == "nearest"){	cluster = new SingleLinkage(rabund, list, matrix, params->cutoff, params->method, adjust); }
            else if(params->method == "average"){	cluster = new AverageLinkage(rabund, list, matrix, params->cutoff, params->method, adjust);	}
            params->tag = cluster->getTag();
            
            string thisOutputDir = params->outputDir;
            if (params->outputDir == "") { thisOutputDir += params->util.hasPath(thisDistFile); }
            string fileroot = thisOutputDir + params->util.getRootName(params->util.getSimpleName(thisDistFile));
            listFileName = fileroot+ params->tag + ".list";

            ofstream listFile;
            params->util.openOutputFile(listFileName,	listFile);

            float previousDist = 0.00000;
            float rndPreviousDist = 0.00000;
            bool printHeaders = true;
            
            oldList = *list;
            
            double saveCutoff = params->cutoff;
            
            while (matrix->getSmallDist() < params->cutoff && matrix->getNNodes() > 0){
                
                if (params->m->getControl_pressed()) { //clean up
                    delete matrix; delete list;	delete cluster; delete rabund;
                    listFile.close();
                    params->util.mothurRemove(listFileName);
                    return listFileName;
                }
                
                cluster->update(saveCutoff);
                
                float dist = matrix->getSmallDist();
                float rndDist = params->util.ceilDist(dist, params->precision);
                
                if(previousDist <= 0.0000 && !params->util.isEqual(dist, previousDist)){
                    oldList.setLabel("unique");
                    oldList.setPrintedLabels(printHeaders);
                    oldList.print(listFile); printHeaders = false;
                    if (params->labels.count("unique") == 0) {  params->labels.insert("unique");  }
                }
                else if(!params->util.isEqual(rndDist, rndPreviousDist)){
                    oldList.setPrintedLabels(printHeaders);
                    oldList.setLabel(toString(rndPreviousDist,  params->length-1));
                    oldList.setPrintedLabels(printHeaders);
                    oldList.print(listFile); printHeaders = false;
                    if (params->labels.count(toString(rndPreviousDist,  params->length-1)) == 0) { params->labels.insert(toString(rndPreviousDist,  params->length-1)); }
                }
                
                previousDist = dist;
                rndPreviousDist = rndDist;
                oldList = *list;
            }
            
            
            if(previousDist <= 0.0000){
                oldList.setLabel("unique");
                oldList.setPrintedLabels(printHeaders);
                oldList.print(listFile); printHeaders = false;
                if (params->labels.count("unique") == 0) { params->labels.insert("unique"); }
            }
            else if(rndPreviousDist<params->cutoff){
                oldList.setLabel(toString(rndPreviousDist,  params->length-1));
                oldList.setPrintedLabels(printHeaders);
                oldList.print(listFile); printHeaders = false;
                if (params->labels.count(toString(rndPreviousDist,  params->length-1)) == 0) { params->labels.insert(toString(rndPreviousDist,  params->length-1)); }
            }
            
            delete matrix; delete list;	delete cluster; delete rabund;
            matrix = nullptr; list = nullptr; cluster = nullptr; rabund = nullptr;
            listFile.close();
            
            if (params->m->getControl_pressed()) { //clean up
                params->util.mothurRemove(listFileName);
                return listFileName;
            }
            
            if (params->deleteFiles) {
                params->util.mothurRemove(thisDistFile);
                params->util.mothurRemove(thisNamefile);
            }
            
            if (!params->util.isEqual(saveCutoff, params->cutoff)) {
                saveCutoff = params->util.ceilDist(saveCutoff, params->precision);
                params->m->mothurOut("Cutoff was " + toString(params->cutoff) + " changed cutoff to " + toString(saveCutoff) + "\n");
            }
            
            if (saveCutoff < smallestCutoff) { smallestCutoff = saveCutoff;  }
        }
        return listFileName;
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "clusterFile");
        exit(1);
    }
}
//**********************************************************************************************************************
void cluster(clusterData* params){
    try {
        vector<string> listFileNames;
        double smallestCutoff = params->cutoff;
        
        //cluster each distance file
        for (int i = 0; i < params->distNames.size(); i++) {
            
            string thisNamefile = params->distNames[i].begin()->second;
            string thisDistFile = params->distNames[i].begin()->first;
            
            params->setNamesCount(thisNamefile);
            
            string listFileName = "";
            if (params->classic)    {  listFileName = clusterClassicFile(thisDistFile, thisNamefile, smallestCutoff, params);   }
            else                    {  listFileName = clusterFile(thisDistFile, thisNamefile, smallestCutoff, params);          }
            
            if (params->m->getControl_pressed()) { //clean up
                for (int i = 0; i < listFileNames.size(); i++) {	params->util.mothurRemove(listFileNames[i]); 	}
                params->listFileNames.clear(); break;
            }
            params->listFileNames.push_back(listFileName);
        }
        params->cutoff = smallestCutoff;
    }
    catch(exception& e) {
        params->m->errorOut(e, "ClusterSplitCommand", "cluster");
        exit(1);
    }
    
    
}
//**********************************************************************************************************************
void printData(ListVector* oldList, clusterData* params){
	try {
		string label = oldList->getLabel();
		RAbundVector oldRAbund = oldList->getRAbundVector();
		
		oldRAbund.setLabel(label);
		if (params->showabund) {
			oldRAbund.getSAbundVector().print(cout);
		}
		oldRAbund.print(params->outRabund);
		oldRAbund.getSAbundVector().print(params->outSabund);
	
		oldList->print(params->outList, true);
	}
	catch(exception& e) {
		params->m->errorOut(e, "ClusterSplitCommand", "printData");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> ClusterSplitCommand::createProcesses(vector< map<string, string> > distName, set<string>& labels){
	try {
        //sanity check
        if (processors > distName.size()) { processors = distName.size(); }
        deleteFiles = false; //so if we need to recalc the processors the files are still there
        vector<string> listFiles;
        vector < vector < map<string, string> > > dividedNames; //distNames[1] = vector of filenames for process 1...
        dividedNames.resize(processors);
        
        //for each file group figure out which process will complete it
        //want to divide the load intelligently so the big files are spread between processes
        for (int i = 0; i < distName.size(); i++) {
            int processToAssign = (i+1) % processors; 
            if (processToAssign == 0) { processToAssign = processors; }
            
            dividedNames[(processToAssign-1)].push_back(distName[i]);
            if ((processToAssign-1) == 1) { m->mothurOut(distName[i].begin()->first + "\n"); }
        }
        
        //now lets reverse the order of ever other process, so we balance big files running with little ones
        for (int i = 0; i < processors; i++) {
            int remainder = ((i+1) % processors);
            if (remainder) {  reverse(dividedNames[i].begin(), dividedNames[i].end());  }
        }
        
        if (m->getControl_pressed()) { return listFiles; }
        
        
        //create array of worker threads
        vector<std::thread*> workerThreads;
        vector<clusterData*> data;
        
        //Lauch worker threads
        for (int i = 0; i < processors-1; i++) {
            clusterData* dataBundle = new clusterData(showabund, classic, deleteFiles, dividedNames[i+1], cutoffNotSet, cutoff, precision, length, method, outputdir, vsearchLocation, type);
            dataBundle->setOptiOptions(metricName, stableMetric, initialize, maxIters);
            data.push_back(dataBundle);
            
            workerThreads.push_back(new std::thread(cluster, dataBundle));
        }
        
        
        clusterData* dataBundle = new clusterData(showabund, classic, deleteFiles, dividedNames[0], cutoffNotSet, cutoff, precision, length, method, outputdir, vsearchLocation, type);
        dataBundle->setOptiOptions(metricName, stableMetric, initialize, maxIters);
        cluster(dataBundle);
        listFiles = dataBundle->listFileNames;
        tag = dataBundle->tag;
        cutoff = dataBundle->cutoff;
        labels = dataBundle->labels;
        
        
        for (int i = 0; i < processors-1; i++) {
            workerThreads[i]->join();
            
            listFiles.insert(listFiles.end(), data[i]->listFileNames.begin(), data[i]->listFileNames.end());
            labels.insert(data[i]->labels.begin(), data[i]->labels.end());
            if (data[i]->cutoff < cutoff) { cutoff = data[i]->cutoff; }
            
            delete data[i];
            delete workerThreads[i];
        }
        delete dataBundle;
        deleteFiles = true;
        
        return listFiles;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "createProcesses");
		exit(1);
	}
}
//**********************************************************************************************************************

int ClusterSplitCommand::createMergedDistanceFile(vector< map<string, string> > distNames) {
	try{
		string thisOutputDir = outputdir;
		if (outputdir == "") { thisOutputDir = util.hasPath(fastafile); }
        map<string, string> variables; 
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(fastafile));
		string outputFileName = getOutputFileName("column", variables);
		util.mothurRemove(outputFileName);
		
		
		for (int i = 0; i < distNames.size(); i++) {
			if (m->getControl_pressed()) {  return 0; }
			
			string thisDistFile = distNames[i].begin()->first;
			
			util.appendFiles(thisDistFile, outputFileName);
		}	
			
		outputTypes["column"].push_back(outputFileName); outputNames.push_back(outputFileName);
			
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "createMergedDistanceFile");
		exit(1);
	}
}
//**********************************************************************************************************************

int ClusterSplitCommand::runSensSpec() {
    try{
        string listFile = "";
        itTypes = outputTypes.find("list");
        if (itTypes != outputTypes.end()) {
            if ((itTypes->second).size() != 0) { listFile = (itTypes->second)[0];  }
        }
        
        string columnFile = "";
        if (makeDist) {
            itTypes = outputTypes.find("column");
            if (itTypes != outputTypes.end()) {
                if ((itTypes->second).size() != 0) { columnFile = (itTypes->second)[0];  }
            }
        }
    
        string inputString = "cutoff=" + toString(cutoff) + ", list=" + listFile;
        if (columnFile != "") { inputString += ", column=" + columnFile;  }
        else { m->mothurOut("[WARNING]: Cannot run sens.spec analysis without a column file, skipping."); return 0;  }

        if (namefile != "")         {  inputString += ", name=" + namefile; }
        else if (countfile != "")   { inputString += ", count=" + countfile; }
        else { m->mothurOut("[WARNING]: Cannot run sens.spec analysis without a name or count file, skipping."); return 0;  }
        
        m->mothurOut("/******************************************/\n");
        m->mothurOut("Running command: sens.spec(" + inputString + ")\n");
        current->setMothurCalling(true);
        
        Command* sensspecCommand = new SensSpecCommand(inputString);
        sensspecCommand->execute();
        
        map<string, vector<string> > filenames = sensspecCommand->getOutputFiles();
        
        delete sensspecCommand;
         current->setMothurCalling(false);
        
        string outputFileName = filenames["sensspec"][0];
        
        outputTypes["sensspec"].push_back(outputFileName);  outputNames.push_back(outputFileName);
        
        m->mothurOut("/******************************************/\n"); 
        m->mothurOut("Done.\n\n\n"); 
        
        ifstream in; util.openInputFile(outputFileName, in);
        
        while(!in.eof()){
            if (m->getControl_pressed()) { break; }
            
            m->mothurOut(util.getline(in)+"\n"); gobble(in);
        }
        in.close();
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "ClusterSplitCommand", "runSensSpec");
        exit(1);
    }
}
//**********************************************************************************************************************
string ClusterSplitCommand::printFile(string singleton, vector< map<string, string> >& distName){
    try {
        ofstream out;
        map<string, string> variables;
        string thisOutputDir = outputdir;
		if (outputdir == "") { thisOutputDir = util.hasPath(distfile); }
        variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(distfile));
		string outputFileName = getOutputFileName("file", variables);
        util.openOutputFile(outputFileName, out);
        outputTypes["file"].push_back(outputFileName); outputNames.push_back(outputFileName);
        current->setFileFile(outputFileName);
        
        out << singleton << endl;
        if (namefile != "") { out << "name" << endl; }
        else if (countfile != "") { out << "count" << endl; }
        else { out << "unknown" << endl; }
        
        for (int i = 0; i < distName.size(); i++) {    out << distName[i].begin()->first << '\t' << distName[i].begin()->second << endl;	}
        out.close();
        
        return outputFileName;
    }
    catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "printFile");
		exit(1);
	}
    
}
//**********************************************************************************************************************
string ClusterSplitCommand::readFile(vector< map<string, string> >& distName){
    try {
        
        string singleton, thiscolumn, thisname;
        
        ifstream in; util.openInputFile(file, in);
        
        in >> singleton; gobble(in);
        
        string path = util.hasPath(singleton);
        if (path == "") {  singleton = inputDir + singleton;  }
        
        in >> type; gobble(in);
        
        if (type == "name") { }
        else if (type == "count") { }
        else {  m->mothurOut("[ERROR]: unknown file type. Are the files in column 2 of the file name files or count files? Please change unknown to name or count.\n"); m->setControl_pressed(true); }
        
        if (isList) {

            vector<string> listFileNames;
            string thisListFileName = "";
            set<string> listLabels;
            
            while(!in.eof()) {
                if (m->getControl_pressed()) { break; }
                
                in >> thisListFileName; gobble(in);
                
                string path = util.hasPath(thisListFileName);
                if (path == "") {  thisListFileName = inputDir + thisListFileName;  }
                
                getLabels(thisListFileName, listLabels);
                listFileNames.push_back(thisListFileName);
            }
            
            ListVector* listSingle;
            map<double, int> labelBins = completeListFile(listFileNames, singleton, listLabels, listSingle);
            
            mergeLists(listFileNames, labelBins, listSingle);
        
        }else {
            
            while(!in.eof()) {
                if (m->getControl_pressed()) { break; }
                
                in >> thiscolumn; gobble(in);
                in >> thisname; gobble(in);
                
                map<string, string> temp;
                temp[thiscolumn] = thisname;
                distName.push_back(temp);
            }
        }
        
        in.close();
        
        return singleton;
    }
    catch(exception& e) {
		m->errorOut(e, "ClusterSplitCommand", "readFile");
		exit(1);
	}
    
}
//**********************************************************************************************************************
int ClusterSplitCommand::getLabels(string file, set<string>& listLabels){
    try {
        ifstream in; util.openInputFile(file, in);

        //read headers
        util.getline(in); gobble(in);
        
        string label;
        while(!in.eof()) {
            if (m->getControl_pressed()) { break; }
            
            in >> label; util.getline(in); gobble(in);
            
            listLabels.insert(label);
        }
        
        in.close();
        
        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "ClusterSplitCommand", "getLabels");
        exit(1);
    }
    
}
//**********************************************************************************************************************
