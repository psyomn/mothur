#ifndef CHIMERACHECKCOMMAND_H
#define CHIMERACHECKCOMMAND_H

/*
 *  chimeracheckcommand.h
 *  Mothur
 *
 *  Created by westcott on 3/31/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"

/***********************************************************/

class ChimeraCheckCommand : public Command {
public:
	ChimeraCheckCommand(string);
	~ChimeraCheckCommand(){}

	vector<string> setParameters();
	string getCommandName()			{ return "chimera.check";		}
	string getCommandCategory()		{ return "Sequence Processing"; }

	string getHelpString();
    string getOutputPattern(string);
	string getCitation() { return "CHIMERA_CHECK version 2.7 written by Niels Larsen (http://wdcm.nig.ac.jp/RDP/docs/chimera_doc.html) \nhttp://www.mothur.org/wiki/Chimera.check"; }
	string getDescription()		{ return "detect chimeric sequences"; }

	int execute();
	void help() { m->mothurOut(getHelpString()); }


private:
	int checkChimeras();

	bool abort, svg, save;
	string fastafile, templatefile, namefile;
	int increment, ksize, numSeqs, templateSeqsLength;
	vector<string> outputNames;
};

/***********************************************************/

#endif


