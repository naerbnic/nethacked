/*	SCCS Id: @(#)dgn_main.c 3.4	1994/09/23	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet	*/
/*	Copyright (c) 1990 by M. Stephenson		*/
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the main function for the parser
 * and some useful functions needed by yacc
 */

#include <string>

using std::string;

#include <string.h>

#include "config.h"
#include "dlb.h"

#define MAX_ERRORS	25

extern int  yyparse();
extern int line_number;
string fname = "(stdin)";
int fatal_error = 0;

int  main(int,char **);
void yyerror(const char *);
void yywarning(const char *);
extern "C" int  yywrap();
void init_yyin(FILE *);
void init_yyout(FILE *);

#define Fprintf (void)fprintf

int main(int argc, char **argv) {
	string infile, outfile, basename;
	FILE *fin, *fout;
	int i, len;
	bool errors_encountered = FALSE;

	infile = "(stdin)";
	fin = stdin;
	outfile = "(stdout)";
	fout = stdout;

	if (argc == 1) { /* Read standard input */
		init_yyin(fin);
		init_yyout(fout);
		(void) yyparse();
		if (fatal_error > 0)
			errors_encountered = TRUE;
	} else { /* Otherwise every argument is a filename */
		for (i = 1; i < argc; i++) {
			fname = infile = argv[i];
			/* the input file had better be a .pdf file */
			len = fname.size() - 4; /* length excluding suffix */
			if (len < 0
					|| fname.substr(fname.size() - 4, string::npos) != ".pdf") {
				Fprintf (stderr, "Error - file name \"%s\" in wrong format.\n",
						fname.c_str());
				errors_encountered = TRUE;
				continue;
			}

			/* build output file name */
			/* Use the whole name - strip off the last 3 or 4 chars. */

			basename = infile.substr(0, len);

			outfile.clear();
			outfile.append(basename);
			fin = freopen(infile.c_str(), "r", stdin);
			if (!fin) {
				Fprintf (stderr, "Can't open %s for input.\n", infile.c_str());
				perror(infile.c_str());
				errors_encountered = TRUE;
				continue;
			}
			fout = freopen(outfile.c_str(), WRBMODE, stdout);
			if (!fout) {
				Fprintf (stderr, "Can't open %s for output.\n",
						outfile.c_str());
				perror(outfile.c_str());
				errors_encountered = TRUE;
				continue;
			}
			init_yyin(fin);
			init_yyout(fout);
			(void) yyparse();
			line_number = 1;
			if (fatal_error > 0) {
				errors_encountered = TRUE;
				fatal_error = 0;
			}
		}
	}
	if (fout && fclose(fout) < 0) {
		Fprintf (stderr, "Can't finish output file.");
		perror(outfile.c_str());
		errors_encountered = TRUE;
	}
	exit(errors_encountered ? EXIT_FAILURE : EXIT_SUCCESS);
	/*NOTREACHED*/
	return 0;
}

/*
 * Each time the parser detects an error, it uses this function.
 * Here we take count of the errors. To continue farther than
 * MAX_ERRORS wouldn't be reasonable.
 */

void yyerror(const char* s) {
	(void) fprintf(stderr,"%s : line %d : %s\n",fname.c_str(),line_number, s);
	if (++fatal_error > MAX_ERRORS) {
		(void) fprintf(stderr,"Too many errors, good bye!\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Just display a warning (that is : a non fatal error)
 */

void yywarning(char const* s) {
	(void) fprintf(stderr,"%s : line %d : WARNING : %s\n",fname.c_str(),line_number,s);
}

extern "C" int yywrap()
{
       return 1;
}

/*dgn_main.c*/
