/*---------------------------------------------------------------------\
|                                                                      |  
|                      __   __    ____ _____ ____                      |  
|                      \ \ / /_ _/ ___|_   _|___ \                     |  
|                       \ V / _` \___ \ | |   __) |                    |  
|                        | | (_| |___) || |  / __/                     |  
|                        |_|\__,_|____/ |_| |_____|                    |  
|                                                                      |  
|                               core system                            | 
|                                                        (C) SuSE GmbH |  
\----------------------------------------------------------------------/ 

   File:       runqt.cc

   Author:     Klaus Kaempf (kkaempf@suse.de)
   Maintainer: Klaus Kaempf (kkaempf@suse.de)

/-*/

#include <stdio.h>

#include "YUIQt.h"

#include <ycp/YCPParser.h>
#include <yui/YUIInterpreter.h>
#include <ycp/y2log.h>

extern int yydebug;

int
main (int argc, char *argv[])
{
    const char *fname = 0;
    FILE *infile = stdin;

    if (argc > 1) {
	int argp = 1;
	while (argp < argc) {
	    if ((argv[argp][0] == '-')
	        && (argv[argp][1] == 'l')
	        && (argp+1 < argc)) {
		argp++;
		y2setLogfileName (argv[argp]);
	    }
	    else if (fname == 0) {
		fname = argv[argp];
	    } else {
		fprintf (stderr, "Bad argument '%s'\nUsage: runqt [ name.ycp ]\n", argv[argp]);
	    }
	    argp++;
	}
    }

    YCPParser *parser;
    parser = new YCPParser ();

    if (!parser) {
	fprintf (stderr, "Failed to create YCPParser\n");
	return 1;
    }

    YUIInterpreter *interpreter;
    interpreter = new YUIQt (argc, argv, true, NULL);

    if (!interpreter) {
	fprintf (stderr, "Failed to create YUIQt\n");
	delete parser;
	return 1;
    }

    if (fname != 0) {
	infile = fopen (fname, "r");
	if (infile == 0) {
	    fprintf (stderr, "Failed to open '%s'\n", fname);
	    return 1;
	}
    }
    else {
	fname = "stdin";
    }

    parser->setInput (infile, fname);
    parser->setBuffered();

    YCPValue value = YCPVoid();

    for (;;) {
	value = parser->parse();
	if (value.isNull())
	    break;

	value = interpreter->evaluate(value);

	printf ("(%s)\n", value->toString().c_str());
    }

    delete interpreter;
    delete parser;

    if (infile != stdin)
	fclose (infile);

    return 0;
}
