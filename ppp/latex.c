/* Copyright (c) 2007, Thomas Fors
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ppp.h"
#include "mpi.h"

#include "print.h"
#include "latex.h"
#include "cmdline.h"

static const char intro[] =
	"\\documentclass[11pt,twocolumn,a4paper]{article}\n"
	"\\usepackage{fullpage}\n"
	"\\pagestyle{empty}\n"
	"\\begin{document}\n";

static const char blockStart[] =
	"\\begin{verbatim}\n";

static const char blockStop[] =
	"\\end{verbatim}\n"
	"\\newpage";

static const char outro[] =
	"\\end{document}\n";

void latexInit() {
}   

void latexCleanup() {
}

static void latexSingleCard(mp_int *nCard) {
	char groupChar = ',';
	mp_int start;
	mp_init(&start);
	calculatePasscodeNumberFromCardColRow(nCard, 0, 0, &start);

	char buf[70*4];
	getPasscodeBlock(&start, 70, buf);
	mp_clear(&start);
	
	char hname[39];
	strncpy(hname, hostname(), 38);
	
	mp_int n;
	mp_init(&n);
	mp_add_d(nCard, 1, &n);
	char *cardnumber = mpToDecimalString(&n, groupChar);
	char *cn = cardnumber;
	mp_clear(&n);
	
	if (strlen(hname) + strlen(cardnumber) + 3 > 38) {
		if (strlen(hname) > 27) {
			hname[27] = '\x00';
		}
		int ellipses = strlen(cardnumber) - (38 - strlen(hname) - 3);
		if (ellipses > 0) {
			cn = cardnumber+ellipses;
			cn[0] = cn[1] = cn[2] = '.';
			/* When truncating the card number, make sure we don't
			 * begin with a comma after the ellipses
			 */
			if (cn[3] == groupChar) {
				cn[3] = '.';
				cn++;
			}
		}
	}
	
	printf("%s", hname);
	int j;
	for (j=0; j<38-strlen(hname)-strlen(cn)-2; j++)
		printf(" ");
	printf("[%s]\n", cn);
	printf("    A    B    C    D    E    F    G\n");

	j = 0;
	int r, c;
	for (r=1; r<=10; r++) {
		printf("%2d: ", r);
		for (c=0; c<7; c++) {
			if (c) printf(" ");
			printf("%c%c%c%c", buf[j*4+0], buf[j*4+1], buf[j*4+2], buf[j*4+3]);  
			j++;
		}
		printf("\n");
	}
	printf("\n");

	/* zero passcodes from memory */
	memset(buf, 0, 70*4);
}

void latexCard(mp_int *fromCard) {
	int i;
	mp_int n;
	mp_init(&n);
	puts(intro);
	
	puts(blockStart);
	for (i=0; i<=2; i++) {
		mp_add_d(fromCard, i, &n);
		//latexSingleCard(&n);
		printCard(&n);
		if (i != 2) printf("\n\n");
	}
	printf("\n\n");
	puts(blockStop);


	puts(blockStart);
	for (i=3; i<=5; i++) {
		mp_add_d(fromCard, i, &n);
		//latexSingleCard(&n);
		printCard(&n);
		if (i != 5) printf("\n\n");
	}
	puts(blockStop);

	puts(outro);
}
