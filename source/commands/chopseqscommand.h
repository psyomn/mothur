#ifndef CHOPSEQSCOMMAND_H
#define CHOPSEQSCOMMAND_H

/*
 *  chopseqscommand.h
 *  Mothur
 *
 *  Created by westcott on 5/10/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"
#include "datastructures/fastqread.h"

class ChopSeqsCommand : public Command {

	public:

		ChopSeqsCommand(string);
		~ChopSeqsCommand(){};

		vector<string> setParameters();
		string getCommandName()			{ return "chop.seqs";		}
		string getCommandCategory()		{ return "Sequence Processing"; }

        string getHelpString();
        string getOutputPattern(string);
		string getCitation() { return "http://www.mothur.org/wiki/Chops.seqs"; }
		string getDescription()		{ return "trim sequence length"; }

		int execute();
		void help() { m->mothurOut(getHelpString()); }

	private:
		string fastafile, fastqfile, keep, namefile, groupfile, countfile, qualfile, format;
		bool abort, countGaps, Short, keepN;
		int numbases, processors;
		vector<string> outputNames;

        bool runChopFasta(string&);
        bool runChopFastq(string&);
        bool getFastqChopped(FastqRead&);
        bool createProcesses(string, string, string, string);
        int processQual(string, string);
};


#endif


