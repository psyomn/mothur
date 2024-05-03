//
//  makelookupcommand.cpp
//  Mothur
//
//  Created by SarahsWork on 5/14/13.
//  Copyright (c) 2013 Schloss Lab. All rights reserved.
//

#include "makelookupcommand.h"

//**********************************************************************************************************************
vector<string> MakeLookupCommand::setParameters(){
	try {
		CommandParameter ptemplate("reference", "InputTypes", "", "", "none", "none", "none","",false,true,true); parameters.push_back(ptemplate);
        CommandParameter pflow("flow", "InputTypes", "", "", "none", "none", "none","lookup",false,true,true); parameters.push_back(pflow);
        CommandParameter perrors("error", "InputTypes", "", "", "none", "none", "none","none",false,true,true); parameters.push_back(perrors);
        CommandParameter pbarcode("barcode", "String", "", "AACCGTGTC", "", "", "","",false,false); parameters.push_back(pbarcode);
		CommandParameter pkey("key", "String", "", "TCAG", "", "", "","",false,false); parameters.push_back(pkey);
        CommandParameter pthreshold("threshold", "Number", "", "10000", "", "", "","",false,false); parameters.push_back(pthreshold);
        CommandParameter porder("order", "Multiple", "A-B-I", "A", "", "", "","",false,false, true); parameters.push_back(porder);
		CommandParameter pseed("seed", "Number", "", "0", "", "", "","",false,false); parameters.push_back(pseed);
        CommandParameter pinputdir("inputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "","",false,false); parameters.push_back(poutputdir);
        
        abort = false; calledHelp = false;
        
        vector<string> tempOutNames;
        outputTypes["lookup"] = tempOutNames;
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string MakeLookupCommand::getHelpString(){
	try {
		string helpString = "";
		helpString += "The make.lookup command allows you to create custom lookup files for use with shhh.flows.\n";
		helpString += "The make.lookup command parameters are: reference, flow, error, barcode, key, threshold and order.\n";
		helpString += "The reference file needs to be in the same direction as the flow data and it must start with the forward primer sequence. It is required.\n";
        helpString += "The flow parameter is used to provide the flow data. It is required.\n";
        helpString += "The error parameter is used to provide the error summary. It is required.\n";
        helpString += "The barcode parameter is used to provide the barcode sequence. Default=AACCGTGTC.\n";
        helpString += "The key parameter is used to provide the key sequence. Default=TCAG.\n";
        helpString += "The threshold parameter is ....Default=10000.\n";
        helpString += "The order parameter options are A, B or I.  Default=A. A = TACG and B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n";
		helpString += "The make.lookup should be in the following format: make.lookup(reference=HMP_MOCK.v53.fasta, flow=H3YD4Z101.mock3.flow_450.flow, error=H3YD4Z101.mock3.flow_450.error.summary, barcode=AACCTGGC)\n";
		helpString += "new(...)\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
string MakeLookupCommand::getOutputPattern(string type) {
    try {
        string pattern = "";
        
        if (type == "lookup") {  pattern = "[filename],lookup"; }
        else { m->mothurOut("[ERROR]: No definition for type " + type + " output pattern.\n"); m->setControl_pressed(true);  }
        
        return pattern;
    }
    catch(exception& e) {
        m->errorOut(e, "MakeLookupCommand", "getOutputPattern");
        exit(1);
    }
}
//**********************************************************************************************************************
MakeLookupCommand::MakeLookupCommand(string option) : Command()  {
	try {
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
        else if(option == "category") {  abort = true; calledHelp = true;  }
		
		else {
			OptionParser parser(option, setParameters());
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
            errorFileName = validParameter.validFile(parameters, "error");
			if (errorFileName == "not open") { errorFileName = ""; abort = true; }
			else if (errorFileName == "not found") { errorFileName = ""; m->mothurOut("[ERROR]: error parameter is required.\n");   abort = true; }
			
			flowFileName = validParameter.validFile(parameters, "flow");
			if (flowFileName == "not open") { flowFileName = ""; abort = true; }
			else if (flowFileName == "not found") { flowFileName = ""; m->mothurOut("[ERROR]: flow parameter is required.\n");   abort = true; }
			else {   current->setFlowFile(flowFileName);	}
			
			refFastaFileName = validParameter.validFile(parameters, "reference");
			if (refFastaFileName == "not open") { abort = true; }
			else if (refFastaFileName == "not found") { refFastaFileName = ""; m->mothurOut("[ERROR]: reference parameter is required.\n");   abort = true; }
                      
            if (outputdir == ""){
				outputdir = util.hasPath(flowFileName); 
			}
            
            string temp = validParameter.valid(parameters, "threshold");	if (temp == "not found"){	temp = "10000";	}
			util.mothurConvert(temp, thresholdCount);
            
            barcodeSequence = validParameter.valid(parameters, "barcode");	if (barcodeSequence == "not found"){	barcodeSequence = "AACCGTGTC";	}
            
            keySequence = validParameter.valid(parameters, "key");	if (keySequence == "not found"){	keySequence = "TCAG";	}
            
            temp = validParameter.valid(parameters, "order");  if (temp == "not found"){ 	temp = "A";	}
            if (temp.length() > 1) {  m->mothurOut("[ERROR]: " + temp + " is not a valid option for order. order options are A, B, or I. A = TACG, B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC, and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n");  abort=true;
            }
            else {
                if (toupper(temp[0]) == 'A') {  flowOrder = "TACG";   }
                else if(toupper(temp[0]) == 'B'){
                    flowOrder = "TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC";   }
                else if(toupper(temp[0]) == 'I'){
                    flowOrder = "TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC";   }
                else {
                    m->mothurOut("[ERROR]: " + temp + " is not a valid option for order. order options are A, B, or I. A = TACG, B = TACGTACGTACGATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATAGATCGCATGACGATCGCATATCGTCAGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGTAGTCGAGCATCATCTGACGCAGTACGTGCATGATCTCAGTCAGCAGCTATGTCAGTGCATGCATAGATCGCATGACGATCGCATATCGTCAGTGCAGTGACTGATCGTCATCAGCTAGCATCGACTGCATGATCTCAGTCAGCAGC, and I = TACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGCTACGTACGTCTGAGCATCGATCGATGTACAGC.\n");  abort=true;
                }
            }
			
		}
		
	}
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "MakeLookupCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

int MakeLookupCommand::execute(){
	try {
        
		if (abort) { if (calledHelp) { return 0; }  return 2;	}
        
        cout.setf(ios::fixed, ios::floatfield);
        cout.setf(ios::showpoint);
        
        double gapOpening = 10;
        int maxHomoP = 101;
        vector<vector<double> > penaltyMatrix; penaltyMatrix.resize(maxHomoP);
        for(int i=0;i<maxHomoP;i++){
            penaltyMatrix[i].resize(maxHomoP, 5);
            penaltyMatrix[i][i] = 0;
        }
        
        //Create flows for reference sequences...
        ifstream refFASTA;
        util.openInputFile(refFastaFileName, refFASTA);                        //  *   open reference sequence file
        map<string, vector<double> > refFlowgrams;
        
        while(!refFASTA.eof()){
            if (m->getControl_pressed()) { refFASTA.close(); return 0; }
            
            Sequence seq(refFASTA);  gobble(refFASTA);
            
            if (m->getDebug()) { m->mothurOut("[DEBUG]: seq = " + seq.getName() + ".\n"); }
            
            string fullSequence = keySequence + barcodeSequence + seq.getAligned(); //  *   concatenate the keySequence, barcodeSequence, and
            //      referenceSequences
            refFlowgrams[seq.getName()] = convertSeqToFlow(fullSequence, flowOrder); //  *   translate concatenated sequences into flowgram
        }
        refFASTA.close();
        
        vector<vector<double> > lookupTable; lookupTable.resize(1000);
        for(int i=0;i<1000;i++){
            lookupTable[i].resize(11, 0);
        }
        
        if (m->getDebug()) { m->mothurOut("[DEBUG]: here .\n"); }
        //Loop through each sequence in the flow file and the error summary file.
        ifstream flowFile;
        util.openInputFile(flowFileName, flowFile);
        int numFlows;
        flowFile >> numFlows;
        
        if (m->getDebug()) { m->mothurOut("[DEBUG]: numflows = " + toString(numFlows) +  ".\n"); }
        
        ifstream errorFile;
        util.openInputFile(errorFileName, errorFile);
        util.getline(errorFile); //grab headers
        
        string errorQuery, flowQuery, referenceName, dummy;
        string chimera;
        float intensity;
        
        vector<double> std;  std.resize(11, 0);
        
        while(errorFile && flowFile){
            
            if (m->getControl_pressed()) { errorFile.close(); flowFile.close(); return 0; }
            
            //  * if it's chimeric, chuck it
            errorFile >> errorQuery >> referenceName;
            for(int i=2;i<40;i++){
                errorFile >> dummy;
            }
            errorFile >> chimera;
           
            
            if(chimera == "2"){
                util.getline(flowFile);
            }
            else{
                
                flowFile >> flowQuery >> dummy;
                if(flowQuery != errorQuery){    m->mothurOut("[ERROR]: " + flowQuery + " != " + errorQuery + "\n");  }
                
                map<string, vector<double> >::iterator it = refFlowgrams.find(referenceName);       //  * compare sequence to its closest reference
                if (it == refFlowgrams.end()) {
                    m->mothurOut("[WARNING]: missing reference flow " + referenceName + ", ignoring flow " + flowQuery + ".\n");
                    util.getline(flowFile); gobble(flowFile);
                }else {
                    vector<double> refFlow = it->second;
                    vector<double> flowgram; flowgram.resize(numFlows);
                    
                    if (m->getDebug()) { m->mothurOut("[DEBUG]: flowQuery = " + flowQuery +  ".\t" + "refName " + referenceName+  ".\n"); }
                    
                    for(int i=0;i<numFlows;i++){
                        flowFile >> intensity;
                        flowgram[i] = intensity;// (int)round(100 * intensity);
                    }
                    gobble(flowFile);
                    
                    if (m->getDebug()) { m->mothurOut("[DEBUG]: before align.\n"); }
                    
                    alignFlowGrams(flowgram, refFlow, gapOpening, penaltyMatrix, flowOrder);
                    
                    if (m->getDebug()) { m->mothurOut("[DEBUG]: after align.\n"); }
                    
                    if (m->getControl_pressed()) { errorFile.close(); flowFile.close(); return 0; }
                    
                    for(int i=0;i<flowgram.size();i++){
                        int count = (int)round(100*flowgram[i]);
                        if(count > 1000){count = 999;}
                        if(abs(flowgram[i]-refFlow[i])<=0.50){
                            lookupTable[count][int(refFlow[i])]++;               //  * build table
                            std[int(refFlow[i])] += (100*refFlow[i]-count)*(100*refFlow[i]-count);
                        }
                    }
                }
                
            }
            gobble(errorFile);
            gobble(flowFile);
        }
        errorFile.close(); flowFile.close();
        
        
        //get probabilities
        vector<int> counts; counts.resize(11, 0);
        int totalCount = 0;
        for(int i=0;i<1000;i++){
            for(int j=0;j<11;j++){
                counts[j] += lookupTable[i][j];
                totalCount += lookupTable[i][j];
            }
        }
        
        int N = 11;
        for(int i=0;i<11;i++){
            if(counts[i] < thresholdCount){ N = i; break; }  //bring back
            std[i] = sqrt(std[i]/(double)(counts[i]));  //bring back
        }
        
        regress(std, N);  //bring back
        
        if (m->getControl_pressed()) { return 0; }
        
        double minProbability = 0.1 / (double)totalCount;
        
        //calculate the negative log probabilities of each intensity given the actual homopolymer length; impute with a guassian when counts are too low
        double sqrtTwoPi = 2.50662827463;//pow(2.0 * 3.14159, 0.5);
        
        for(int i=0;i<1000;i++){
            if (m->getControl_pressed()) { return 0; }
            
            for(int j=0;j<N;j++){
                if(util.isEqual(lookupTable[i][j], 0)){
                    lookupTable[i][j] = 1;  //bring back
                }
                lookupTable[i][j] = -log(lookupTable[i][j]/double(counts[j]));  //bring back
            }
            
            for(int j=N;j<11;j++){  //bring back
                double normalProbability = 1.0/((double)std[j] * sqrtTwoPi) * exp(-double(i - j*100)* double(i - j*100)/ double(2*std[j]*std[j]));
                if(normalProbability > minProbability){
                    lookupTable[i][j] = -log(normalProbability);
                }
                else{
                    lookupTable[i][j] = -log(minProbability);
                }
            }
        }
        
        
        //calculate the probability of each homopolymer length
        vector<double> negLogHomoProb; negLogHomoProb.resize(11, 0.00);  //bring back
        for(int i=0;i<N;i++){
            negLogHomoProb[i] = -log(counts[i] / (double)totalCount);
        }
        regress(negLogHomoProb, N);
        
        if (m->getControl_pressed()) { return 0; }
        
        //output data table.  column one is the probability of each homopolymer length
        map<string, string> variables;
        variables["[filename]"] = outputdir + util.getRootName(util.getSimpleName(flowFileName));
		string outputFile = getOutputFileName("lookup",variables);
		outputNames.push_back(outputFile); outputTypes["lookup"].push_back(outputFile);
        
        ofstream lookupFile;
        util.openOutputFile(outputFile, lookupFile);
        lookupFile.precision(8);
        
        for(int j=0;j<11;j++){
            //        lookupFile << counts[j];
            lookupFile << showpoint << negLogHomoProb[j]; //bring back
            for(int i=0;i<1000;i++){
                lookupFile << '\t' << lookupTable[i][j];
            }
            lookupFile << endl;
        }
        lookupFile.close();
        
        m->mothurOut("\nData for homopolymer lengths of " + toString(N) + " and longer were imputed for this analysis\n\n");
         
        if (m->getControl_pressed()) { for (int i = 0; i < outputNames.size(); i++) {	util.mothurRemove(outputNames[i]); } return 0; }
		
		m->mothurOut("\nOutput File Names: \n"); 
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i] +"\n"); 	} m->mothurOutEndLine();
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "execute");
		exit(1);
	}
}
//******************************************************************************************************************************

vector<double> MakeLookupCommand::convertSeqToFlow(string sequence, string order){
    try {
        int seqLength = (int)sequence.length();
        int numFlows = (int)order.length();
        vector<double> flowgram;
        
        int orderIndex = 0;
        int sequenceIndex = 0;
        
        while(orderIndex < numFlows && sequenceIndex < seqLength){
            
            if (m->getControl_pressed()) { return flowgram; }
            
            int homopolymerLength = 1;
            
            char base = sequence[sequenceIndex];
            
            while(base == sequence[sequenceIndex+1] && sequenceIndex < seqLength){
                homopolymerLength++;
                sequenceIndex++;
            }
            
            sequenceIndex++;
            
            for(int i=orderIndex; i<orderIndex+numFlows;i++){
                if(order[i%numFlows] == base){
                    //flowgram[i] = homopolymerLength;
                    orderIndex = i%numFlows;
                    break;
                }else {  flowgram.push_back(0); }
            }
            
            //flowgram[orderIndex] = homopolymerLength;
            flowgram.push_back(homopolymerLength);
            
            orderIndex++;
            orderIndex = orderIndex % numFlows;
        }
        
        return flowgram;
    }
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "convertSeqToFlow");
		exit(1);
	}
}
//******************************************************************************************************************************

int MakeLookupCommand::alignFlowGrams(vector<double>& flowgram, vector<double>& refFlow, double gapOpening, vector<vector<double> > penaltyMatrix, string flowOrder){
    try {
        int numQueryFlows = (int)flowgram.size();
        int numRefFlows = (int)refFlow.size();
        
        vector<vector<double> > scoreMatrix; scoreMatrix.resize(numQueryFlows+1);
        vector<vector<char> > directMatrix; directMatrix.resize(numQueryFlows+1);
        
        for(int i=0;i<=numQueryFlows;i++){
            if (m->getControl_pressed()) { return 0; }
            scoreMatrix[i].resize(numRefFlows+1, 0.00);
            directMatrix[i].resize(numRefFlows+1, 'x');
            
            scoreMatrix[i][0] = i * gapOpening;
            directMatrix[i][0] = 'u';
        }        
        
        for(int i=0;i<=numRefFlows;i++){
            scoreMatrix[0][i] = i * gapOpening;
            directMatrix[0][i] = 'l';
        }
        
        
        for(int i=1;i<=numQueryFlows;i++){
            for(int j=1;j<=numRefFlows;j++){
                if (m->getControl_pressed()) { return 0; }
                double diagonal = 1000000000;
                if(flowOrder[i%flowOrder.length()] == flowOrder[j%flowOrder.length()]){
                    diagonal = scoreMatrix[i-1][j-1] + penaltyMatrix[round(flowgram[i-1])][refFlow[j-1]];
                }
                double up = scoreMatrix[i-1][j] + gapOpening;
                double left = scoreMatrix[i][j-1] + gapOpening;
                
                double minScore = diagonal;
                char direction = 'd';
                
                if(left < diagonal && left < up){
                    minScore = left;
                    direction = 'l';
                }
                else if(up < diagonal && up < left){
                    minScore = up;
                    direction = 'u';
                }
                
                scoreMatrix[i][j] = minScore;
                directMatrix[i][j] = direction;
                
            }
        }
        
        int minRowIndex = numQueryFlows;
        double minRowScore = scoreMatrix[numQueryFlows][numRefFlows];
        for(int i=0;i<numQueryFlows;i++){
            if (m->getControl_pressed()) { return 0; }
            if(scoreMatrix[i][numRefFlows] < minRowScore){
                minRowScore = scoreMatrix[i][numRefFlows];
                minRowIndex = i;
            }
        }
        
        int minColumnIndex = numRefFlows;
        double minColumnScore = scoreMatrix[numQueryFlows][numRefFlows];
        for(int i=0;i<numRefFlows;i++){
            if (m->getControl_pressed()) { return 0; }
            if(scoreMatrix[numQueryFlows][i] < minColumnScore){
                minColumnScore = scoreMatrix[numQueryFlows][i];
                minColumnIndex = i;
            }
        }
        
        
        int i=minRowIndex;
        int j= minColumnIndex;
        
        vector<double> newFlowgram;
        vector<double> newRefFlowgram;
       
        while(i > 0 && j > 0){
            if (m->getControl_pressed()) { return 0; }
            if(directMatrix[i][j] == 'd'){
                newFlowgram.push_back(flowgram[i-1]);
                newRefFlowgram.push_back(refFlow[j-1]);
                
                i--;
                j--;
            }
            else if(directMatrix[i][j] == 'l'){
                newFlowgram.push_back(0);
                newRefFlowgram.push_back(refFlow[j-1]);
                
                j--;
            }
            else if(directMatrix[i][j] == 'u'){
                newFlowgram.push_back(flowgram[i-1]);
                newRefFlowgram.push_back(0);
                
                i--;
            }
        }
        
        flowgram = newFlowgram;
        refFlow = newRefFlowgram;
        
        return 0;
    
    }
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "alignFlowGrams");
		exit(1);
	}
}

//******************************************************************************************************************************

int MakeLookupCommand::regress(vector<double>& data, int N){
    try {
        //fit data for larger values of N
        double xMean = 0;
        double yMean = 0;
        
        for(int i=1;i<N;i++){
            if (m->getControl_pressed()) { return 0; }
            xMean += i;
            yMean += data[i];
        }
        xMean /= (N-1);
        yMean /= (N-1);
        
        double numerator = 0;
        double denomenator = 0;
        for(int i=1;i<N;i++){
            if (m->getControl_pressed()) { return 0; }
            numerator += (i-xMean)*(data[i] - yMean);
            denomenator += (i-xMean) * (i-xMean);
        }
        double slope = numerator / denomenator;
        double intercept = yMean - slope * xMean;
        
        for(int i=N;i<11;i++){
            data[i] = intercept + i * slope;
        }
        
        return 0;
    }
	catch(exception& e) {
		m->errorOut(e, "MakeLookupCommand", "regress");
		exit(1);
	}
}

//******************************************************************************************************************************

//**********************************************************************************************************************


