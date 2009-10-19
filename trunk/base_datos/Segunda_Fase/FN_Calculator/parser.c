#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void print_df (char * alfa, char * beta)
{
	printf ("\tdf(set([%s]),set([%s])),\n", alfa, beta);
}


int main (void)
{
	int c = 0;
	int pos = 0;
	char alfa[500] = { 0 };
	char beta[500] = { 0 };
	bool isAlfa = true;
	
	printf ("set ([");
	c = getc(stdin);
	while (c != EOF) {
		pos = 0;
		alfa[pos] = '\'';
		pos++;
		while (c != EOF && c != '\n') {
			if (c == ' ') {
				c = getc(stdin);
				continue;
			} else if (c == ','){
				alfa[pos] = '\'';
				pos++;
				alfa[pos] = ',';
				pos++;
				alfa[pos] = '\'';
				pos++;
				c = getc(stdin);
				continue;
			}
			alfa[pos] = c;
			pos++;
			c = getc(stdin);
		}
		alfa[pos] = '\'';
		pos++;
		alfa[pos] = '\0';
		pos = 1;
		beta[0] = '\'';
		c = getc(stdin);
		while (c != EOF && c != '\n') {
			if (c == ' ') {
				c = getc(stdin);
				continue;
			}else if (c == ','){
				beta[pos] = '\'';
				pos++;
				beta[pos] = ',';
				pos++;
				beta[pos] = '\'';
				pos++;
				c = getc(stdin);
				continue;
			}
				
			beta[pos] = c;
			pos++;
			c = getc(stdin);
		}
		c = getc(stdin);
		beta[pos] = '\'';
		beta[pos+1] = '\0';
		print_df (alfa, beta);
	}
	printf ("])\n");
	return 0;
}
	
