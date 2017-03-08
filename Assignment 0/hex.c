/**Kyle Verdeyen
 * kverdey1@jhu.edu
 * Assignment 0, Problem 2
 *
 * Produce a hexdump of standard input to standard output until EOF.
 *
 * Display offset, data bytes, and character equivalents as follows:
 *
 * ooooooo: dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd dd  abcdefghABCDEFGH
 *
 * Offset and bytes are in lowercase hexadecimal, character equivalents use
 * "." for non-printable characters. Partial lines look like this:
 *
 * ooooooo: dd dd dd                                         abc
 *
 * NOTE: We only use 7 characters for the offset to be compatible with xxd
 * which simplifies testing; use "xxd -g 1" to run against ours.
 * ^nope jk lol ¯\_(ツ)_/¯
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_BYTES 16

/*
 * Read up to NUM_BYTES characters from standard in and fill them into the
 * given buffer in order. Return how many bytes were actually read, 0 means
 * end-of-file.
 */
static int getchunk(char buffer[NUM_BYTES])
{
    int temp = 0;
    while (!feof(stdin) && temp < NUM_BYTES) {
        buffer[temp] = getchar();
        ++temp;
    }//read 16 characters into the buffer array unless we're at EOF
    return temp;
}

/*
 * Write a line of the hexdump.
 */
static void putchunk(char buffer[NUM_BYTES], int length, int position)
{
    //tracking if input buffer is less than 16 (numbytes)
    int diff = NUM_BYTES - length;

    for (int i = 0; i < length; ++i) {
        if ((i % NUM_BYTES) == 0) {
            printf("%08x: ", (unsigned char) position);
        } //for printing of chunk starting offset
        if (buffer[i] != EOF) {
            printf("%02x ", (unsigned char) buffer[i]);
        } else {
            buffer[i] = 0;
            ++diff;
            --length;
        } /*for printing of hex. need to check for EOF
        so we don't print it*/
    }
    if (diff > 0) {
        for (int i = 0; i < diff; ++i) {
            printf("   ");
        }/*if the difference 16-size there are blanks
        (eg difference 16-x is >0)
        so we need to print spaces for them*/
    }
    for (int i = 0; i < length; ++i) {
        if (isprint(buffer[i])) {
            printf("%c", buffer[i]);
        } else {
            printf(".");
        }
     }//print the string

    printf("\n");//newline
    memset(buffer, 0, 16); //do i need to clear buffer?
}

int main(void)
{
	int position = 0;

	char buffer[NUM_BYTES];
	int length;

	while ((length = getchunk(buffer)) != 0) {
		putchunk(buffer, length, position);
		position += length;
	}

	return EXIT_SUCCESS;
}
