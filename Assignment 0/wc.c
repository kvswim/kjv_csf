/*Kyle Verdeyen
* kverdey1@jhu.edu
* CSF Assignment 0 Problem 1
* wordcount: takes standard input then returns
* line, word, character counts.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int main(void) {
    int character_count = 0;
    int word_count = 0;
    int line_count = 0;
    int isin_space = 1;
    int current, previous = '\n';
//take each character from stdin until we hit an EOF
    while ((current = getchar()) != EOF) {
        previous = current;
        ++character_count;
        if (isspace(current)) {
            isin_space = 1;
            if (current == '\n') {
                ++line_count;
            }
        } else {
            word_count += isin_space;
            isin_space = 0;
        }//simple logic to count characters & words based on spaces
    }
    if (previous != '\n') {//special case for newline
        ++line_count;
    }
    printf("%3d %3d %3d \n", line_count, word_count, character_count);
}
