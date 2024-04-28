/*
 *  getlineagecommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/24/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "getlineagecommand.h"

#include "inputdata.h"
#include "commands/getseqscommand.h"
#include "commands/getotuscommand.h"
#include "datastructures/sequence.hpp"
#include "datastructures/listvector.hpp"
#include "datastructures/counttable.h"
#include "datastructures/taxonomy.hpp"

//**********************************************************************************************************************
vector<string> GetLineageCommand::setParameters(){
	try {
		CommandParameter pfasta("fasta", "InputTypes", "", "", "none", "FNGLT", "none","fasta",false,false, true); parameters.push_back(pfasta);
        CommandParameter pname("name", "InputTypes", "", "", "NameCount", "FNGLT", "none","name",false,false, true); parameters.push_back(pname);
        CommandParameter pcount("count", "InputTypes", "", "", "NameCount-CountGroup", "FNGLT", "none","count",false,false, true); parameters.push_back(pcount);
		CommandParameter pgroup("group", "InputTypes", "", "", "CountGroup", "FNGLT", "none","group",false,false, true); parameters.push_back(pgroup);
		CommandParameter plist("list", "InputTypes", "", "", "none", "FNGLT", "none","list",false,false, true); parameters.push_back(plist);
        CommandParameter pshared("shared", "InputTypes", "", "", "none", "FNGLT", "none","shared",false,false, true); parameters.push_back(pshared);
		CommandParameter ptaxonomy("taxonomy", "InputTypes", "", "", "tax", "FNGLT", "none","taxonomy",false,false, true); parameters.push_back(ptaxonomy);
        CommandParameter pconstaxonomy("constaxonomy", "InputTypes", "", "", "tax", "FNGLT", "none","constaxonomy",false,false, true); parameters.push_back(pconstaxonomy);
		CommandParameter palignreport("alignreport", "InputTypes", "", "", "none", "FNGLT", "none","alignreport",false,false); parameters.push_back(palignreport);
        CommandParameter plabel("label", "String", "", "", "", "", "","",false,false); parameters.push_back(plabel);
		CommandParameter ptaxon("taxon", "String", "", "", "", "", "","",false,true, true); parameters.push_back(ptaxon);
		CommandParameter pdups("dups", "Boolean", "", "T", "", "", "","",false,false); parameters.push_back(pdups);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);

        abort = false; calledHelp = false;

        vector<string> tempOutNames;
        outputTypes["fasta"] = tempOutNames;
        outputTypes["taxonomy"] = tempOutNames;
        outputTypes["name"] = tempOutNames;
        outputTypes["group"] = tempOutNames;
        outputTypes["alignreport"] = tempOutNames;
        outputTypes["list"] = tempOutNames;
        outputTypes["count"] = tempOutNames;
        outputTypes["constaxonomy"] = tempOutNames;
        outputTypes["shared"] = tempOutNames;

		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "GetLineageCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetLineageCommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "The get.lineage command reads a taxonomy or constaxonomy file and any of the following file types: fasta, name, group, count, list, shared or alignreport file. The constaxonomy can only be used with a shared or list file.\n";
		helpString += "It outputs a file containing only the sequences from the taxonomy file that are from the taxon requested.\n";
		helpString += "The get.lineage command parameters are taxon, fasta, name, group, count, list, shared, taxonomy, alignreport, label and dups.  You must provide taxonomy or constaxonomy unless you have a valid current taxonomy file.\n";
		helpString += "The dups parameter allows you to add the entire line from a name file if you add any name from the line. default=false. \n";
		helpString += "The taxon parameter allows you to select the taxons you would like to get and is required.\n";
		helpString += "You may enter your taxons with confidence scores, doing so will get only those sequences that belong to the taxonomy and whose cofidence scores is above the scores you give.\n";
		helpString += "If they belong to the taxonomy and have confidences below those you provide the sequence will not be selected.\n";
         helpString += "The label parameter is used to analyze specific labels in your input. \n";
		helpString += "The get.lineage command should be in the following format: get.lineage(taxonomy=yourTaxonomyFile, taxon=yourTaxons).\n";
		helpString += "Example get.lineage(taxonomy=amazon.silva.taxonomy, taxon=Bacteria;Firmicutes;Bacilli;Lactobacillales;).\n";
		helpString += "Note: If you are running mothur in script mode you must wrap the taxon in ' characters so mothur will ignore the ; in the taxon.\n";
		helpString += "Example get.lineage(taxonomy=amazon.silva.taxonomy, taxon='Bacteria;Firmicutes;Bacilli;Lactobacillales;').\n";
		;
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "GetLineageCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetLineageCommand::getOutputPattern(string type) {
    try {
        string pattern = "";

        if (type == "fasta")                {   pattern = "[filename],pick,[extension]";    }
        else if (type == "taxonomy")        {   pattern = "[filename],pick,[extension]";    }
        else if (type == "constaxonomy")    {   pattern = "[filename],pick,[extension]";    }
        else if (type == "name")            {   pattern = "[filename],pick,[extension]";    }
        else if (type == "group")           {   pattern = "[filename],pick,[extension]";    }
        else if (type == "count")           {   pattern = "[filename],pick,[extension]";    }
        else if (type == "list")            {   pattern = "[filename],[distance],pick,[extension]";    }
        else if (type == "shared")          {   pattern = "[filename],[distance],pick,[extension]";    }
        else if (type == "alignreport")     {   pattern = "[filename],pick.[extension]";    }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }

        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "GetLineageCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
GetLineageCommand::GetLineageCommand(string option) : Command()  {
	try {
        if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }

		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();

			ValidParameters validParameter;
			fastafile = validParameter.validFile(parameters, "fasta");
			if (fastafile == "not open") { fastafile = ""; abort = true; }
			else if (fastafile == "not found") {  fastafile = "";  }
			else { current->setFastaFile(fastafile); }

			namefile = validParameter.validFile(parameters, "name");
			if (namefile == "not open") { namefile = ""; abort = true; }
			else if (namefile == "not found") {  namefile = "";  }
			else { current->setNameFile(namefile); }

			groupfile = validParameter.validFile(parameters, "group");
			if (groupfile == "not open") { abort = true; }
			else if (groupfile == "not found") {  groupfile = "";  }
			else { current->setGroupFile(groupfile); }

			alignfile = validParameter.validFile(parameters, "alignreport");
			if (alignfile == "not open") { abort = true; }
			else if (alignfile == "not found") {  alignfile = "";  }

			listfile = validParameter.validFile(parameters, "list");
			if (listfile == "not open") { abort = true; }
			else if (listfile == "not found") {  listfile = "";  }
			else { current->setListFile(listfile); }

			taxfile = validParameter.validFile(parameters, "taxonomy");
			if (taxfile == "not open") { taxfile = ""; abort = true; }
			else if (taxfile == "not found") {  taxfile = "";		}
			else { current->setTaxonomyFile(taxfile); }

            sharedfile = validParameter.validFile(parameters, "shared");
			if (sharedfile == "not open") { sharedfile = ""; abort = true; }
			else if (sharedfile == "not found") {  sharedfile = "";		}
			else { current->setSharedFile(sharedfile); }


            constaxonomy = validParameter.validFile(parameters, "constaxonomy");
			if (constaxonomy == "not open") { constaxonomy = ""; abort = true; }
			else if (constaxonomy == "not found") {  constaxonomy = "";		}

            if ((constaxonomy == "") && (taxfile == "")) {
                taxfile = current->getTaxonomyFile();
                if (taxfile != "") { m->mothurOut("Using " + taxfile + " as input file for the taxonomy parameter.\n");  }
                else {
                    m->mothurOut("You have no current taxonomy file and did not provide a constaxonomy file. The taxonomy or constaxonomy parameter is required.\n");  abort = true; }
			}

			string usedDups = "true";
			string temp = validParameter.valid(parameters, "dups");
			if (temp == "not found") {
				if (namefile != "") {  temp = "true";					}
				else				{  temp = "false"; usedDups = "";	}
			}
			dups = util.isTrue(temp);

            countfile = validParameter.validFile(parameters, "count");
            if (countfile == "not open") { countfile = ""; abort = true; }
            else if (countfile == "not found") { countfile = "";  }
            else { current->setCountFile(countfile); }

            if ((namefile != "") && (countfile != "")) {
                m->mothurOut("[ERROR]: you may only use one of the following: name or count.\n");  abort = true;
            }

            if ((groupfile != "") && (countfile != "")) {
                m->mothurOut("[ERROR]: you may only use one of the following: group or count.\n");  abort=true;
            }

			taxons = validParameter.valid(parameters, "taxon");
			if (taxons == "not found") { taxons = "";  m->mothurOut("No taxons given, please correct.\n");   abort = true;  }
			else {
				//rip off quotes
				if (taxons[0] == '\'') {  taxons = taxons.substr(1); }
				if (taxons[(taxons.length()-1)] == '\'') {  taxons = taxons.substr(0, (taxons.length()-1)); }
			}
			util.splitAtChar(taxons, listOfTaxons, '-');

			if ((fastafile == "") && (constaxonomy == "") && (namefile == "") && (groupfile == "") && (alignfile == "") && (listfile == "") && (taxfile == "") && (countfile == ""))  { m->mothurOut("You must provide one of the following: fasta, name, group, count, alignreport, taxonomy, constaxonomy, shared or listfile.\n");  abort = true; }

            if ((constaxonomy != "") && ((fastafile != "") || (namefile != "") || (groupfile != "") || (alignfile != "") || (taxfile != "") || (countfile != ""))) {
                m->mothurOut("[ERROR]: can only use constaxonomy file with a list or shared file, aborting.\n");  abort = true;
            }

            if ((constaxonomy != "") && (taxfile != "")) {
                m->mothurOut("[ERROR]: Choose only one: taxonomy or constaxonomy, aborting.\n"); abort = true;
            }

            if ((sharedfile != "") && (taxfile != "")) {
                m->mothurOut("[ERROR]: sharedfile can only be used with constaxonomy file, aborting.\n"); abort = true;
            }

            if ((sharedfile != "") || (listfile != "")) {
                label = validParameter.valid(parameters, "label");
                if (label == "not found") { label = ""; m->mothurOut("[WARNING]: You did not provide a label, I will use the first label in your inputfile.\n");  }
            }
		}

	}
	catch(exception& e) {
		m->errorOut(e, "GetLineageCommand", "GetLineageCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int GetLineageCommand::execute(){
	try {

		if (abort) { if (calledHelp) { return 0; }  return 2;	}

		if (m->getControl_pressed()) { return 0; }

        if (countfile != "") {
            if ((fastafile != "") || (listfile != "") || (taxfile != "")) {
                //m->mothurOut("\n[NOTE]: The count file should contain only unique names, so mothur assumes your fasta, list and taxonomy files also contain only uniques.\n\n");
            }
        }

		//read through the correct file and output lines you want to keep
		if (taxfile != "")			{
            string accnosFileName = readTax(); //fills the set of names to get

            if (!util.isBlank(accnosFileName)) {
                outputNames.push_back(accnosFileName); outputTypes["accnos"].push_back(accnosFileName);
                runGetSeqs(accnosFileName);
            }else { util.mothurRemove(accnosFileName); }

        }else {
            string accnosFileName = readConsTax();

            if (!util.isBlank(accnosFileName)) {
                outputNames.push_back(accnosFileName); outputTypes["accnos"].push_back(accnosFileName);
                runGetOTUs(accnosFileName);
            } else { util.mothurRemove(accnosFileName); }
        }


		if (m->getControl_pressed()) { outputTypes.clear(); for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]);  } return 0; }

		if (outputNames.size() != 0) {
			m->mothurOutEndLine();
			m->mothurOut("Output File Names:\n");
			for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
			m->mothurOutEndLine();

			//set fasta file as new current fastafile
			string currentName = "";
			itTypes = outputTypes.find("fasta");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setFastaFile(currentName); }
			}

			itTypes = outputTypes.find("name");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setNameFile(currentName); }
			}

			itTypes = outputTypes.find("group");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setGroupFile(currentName); }
			}

			itTypes = outputTypes.find("list");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setListFile(currentName); }
			}

            itTypes = outputTypes.find("shared");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setSharedFile(currentName); }
			}

			itTypes = outputTypes.find("taxonomy");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setTaxonomyFile(currentName); }
			}

            itTypes = outputTypes.find("count");
			if (itTypes != outputTypes.end()) {
				if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setCountFile(currentName); }
			}

            //set constaxonomy file as new current constaxonomyfile
            itTypes = outputTypes.find("constaxonomy");
            if (itTypes != outputTypes.end()) {
                if ((itTypes->second).size() != 0) { currentName = (itTypes->second)[0]; current->setConsTaxonomyFile(currentName); }
            }
		}

		return 0;
	}

	catch(exception& e) {
		m->errorOut(e, "GetLineageCommand", "execute");
		exit(1);
	}
}

//**********************************************************************************************************************
int GetLineageCommand::runGetSeqs(string accnosFileName){
    try {
        //use remove.seqs to create new list and shared files
        if ((namefile != "") || (fastafile != "") || (countfile != "") || (groupfile != "") || (alignfile != "") || (listfile != "")) {
            string inputString = "accnos=" + accnosFileName;

            if (namefile != "")     {  inputString += ", name=" + namefile;             }
            if (countfile != "")    {   inputString += ", count=" + countfile;          }
            if (fastafile != "")    {  inputString += ", fasta=" + fastafile;           }
            if (groupfile != "")    {   inputString += ", group=" + groupfile;          }
            if (alignfile != "")    {   inputString += ", alignreport=" + alignfile;    }
            if (listfile != "")		{	inputString += ", list=" + listfile;            }

            m->mothurOut("\n/******************************************/\n");
            m->mothurOut("Running command: get.seqs(" + inputString + ")\n");
            current->setMothurCalling(true);

            Command* getCommand = new GetSeqsCommand(inputString);
            getCommand->execute();

            map<string, vector<string> > filenames = getCommand->getOutputFiles();

            delete getCommand;
            current->setMothurCalling(false);
            m->mothurOut("/******************************************/\n");

            outputTypes.insert(filenames.begin(), filenames.end());

            if (listfile != "")         {
                vector<string> files = filenames["list"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }
            if (namefile != "")		{
                vector<string> files = filenames["name"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }

            if (countfile != "")         {
                vector<string> files = filenames["count"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }
            if (fastafile != "")		{
                vector<string> files = filenames["fasta"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }

            if (groupfile != "")         {
                vector<string> files = filenames["group"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }
            if (alignfile != "")		{
                vector<string> files = filenames["alignreport"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }
        }

        return 0;

    }
    catch(exception& e) {
        m->errorOut(e, "GetLineageCommand", "runGetSeqs");
        exit(1);
    }
}
//**********************************************************************************************************************
int GetLineageCommand::runGetOTUs(string accnosFileName){
    try {
        //use remove.otus to create new list and shared files
        if ((listfile != "") || (sharedfile != "")) {
            string inputString = "accnos=" + accnosFileName;

            if (listfile != "")         {  inputString += ", list=" + listfile;         }
            if (sharedfile != "")		{	inputString += ", shared=" + sharedfile;    }

            m->mothurOut("\n/******************************************/\n");
            m->mothurOut("Running command: get.otus(" + inputString + ")\n");
            current->setMothurCalling(true);

            Command* getCommand = new GetOtusCommand(inputString);
            getCommand->execute();

            map<string, vector<string> > filenames = getCommand->getOutputFiles();

            delete getCommand;
            current->setMothurCalling(false);
            m->mothurOut("/******************************************/\n");

            outputTypes.insert(filenames.begin(), filenames.end());

            if (listfile != "")         {
                vector<string> files = filenames["list"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }
            if (sharedfile != "")		{
                vector<string> files = filenames["shared"];
                outputNames.insert(outputNames.end(), files.begin(), files.end());
            }
        }

        return 0;
    }
    catch(exception& e) {
        m->errorOut(e, "GetLineageCommand", "runGetOTUs");
        exit(1);
    }
}
//**********************************************************************************************************************
string GetLineageCommand::readTax(){
	try {
		string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(taxfile);  }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(taxfile));
        variables["[extension]"] = util.getExtension(taxfile);
		string outputFileName = getOutputFileName("taxonomy", variables);
        string accnosFileName = outputFileName + ".accnos";

        ofstream out, outAccnos; util.openOutputFile(outputFileName, out); util.openOutputFile(accnosFileName, outAccnos);
		ifstream in; util.openInputFile(taxfile, in);
		string name, tax;

		bool wroteSomething = false;
		vector<bool> taxonsHasConfidence; taxonsHasConfidence.resize(listOfTaxons.size(), false);
		vector< vector<Taxon> > searchTaxons; searchTaxons.resize(listOfTaxons.size());

		for (int i = 0; i < listOfTaxons.size(); i++) {
            bool hasCon = false;
            searchTaxons[i] = util.getTaxons(listOfTaxons[i], hasCon);
            taxonsHasConfidence[i] = hasCon;
		}

		while(!in.eof()){

			if (m->getControl_pressed()) { break; }

            in >> name; gobble(in);
            tax = util.getline(in); gobble(in);

            Taxonomy thisSeq(name, tax);
			vector<Taxon> otuTax = thisSeq.getTaxons();
            util.removeQuotes(otuTax);

			if (util.searchTax(otuTax, taxonsHasConfidence, searchTaxons)) {
                out << name << '\t' << tax << endl;
                outAccnos << name << endl; wroteSomething = true;
            }
        }
		in.close();
		out.close();
        outAccnos.close();

		if (!wroteSomething) { m->mothurOut("Your taxonomy file does not contain any sequences from " + taxons + ".\n");  }
		outputNames.push_back(outputFileName); outputTypes["taxonomy"].push_back(outputFileName);

		return accnosFileName;

	}
	catch(exception& e) {
		m->errorOut(e, "GetLineageCommand", "readTax");
		exit(1);
	}
}
//**********************************************************************************************************************
string GetLineageCommand::readConsTax(){
	try {
		string thisOutputDir = outputdir;
		if (outputdir == "") {  thisOutputDir += util.hasPath(constaxonomy);  }
		map<string, string> variables;
		variables["[filename]"] = thisOutputDir + util.getRootName(util.getSimpleName(constaxonomy));
        variables["[extension]"] = util.getExtension(constaxonomy);
		string outputFileName = getOutputFileName("constaxonomy", variables);
        string accnosFileName = outputFileName + ".accnos";

        ofstream out, outAccnos;
		util.openOutputFile(outputFileName, out);
        util.openOutputFile(accnosFileName, outAccnos);

		ifstream in; util.openInputFile(constaxonomy, in);

        //read headers
        string headers = util.getline(in);
        out << headers << endl;

		bool wroteSomething = false;
		vector<bool> taxonsHasConfidence; taxonsHasConfidence.resize(listOfTaxons.size(), false);
		vector< vector<Taxon> > searchTaxons; searchTaxons.resize(listOfTaxons.size());

		for (int i = 0; i < listOfTaxons.size(); i++) {
            bool hasCon = false;
            searchTaxons[i] = util.getTaxons(listOfTaxons[i], hasCon);
            taxonsHasConfidence[i] = hasCon;
        }

		while(!in.eof()){

			if (m->getControl_pressed()) { break; }

            Taxonomy thisOtu(in);
            vector<Taxon> otuTax = thisOtu.getTaxons();
            util.removeQuotes(otuTax);

            if (util.searchTax(otuTax, taxonsHasConfidence, searchTaxons)) {
                wroteSomething = true; outAccnos << thisOtu.getName() << endl;
                thisOtu.printConsTax(out);
            }
        }
		in.close();
		out.close();
        outAccnos.close();

		if (!wroteSomething) { m->mothurOut("Your taxonomy file does not contain any OTUs from " + taxons + ".\n");   }
		outputNames.push_back(outputFileName); outputTypes["constaxonomy"].push_back(outputFileName);

		return accnosFileName;
	}
	catch(exception& e) {
		m->errorOut(e, "GetLineageCommand", "readConsTax");
		exit(1);
	}
}
//**********************************************************************************************************************




