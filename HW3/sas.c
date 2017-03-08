/**
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "instruction.h"

// This defines the maximum size of the SCRAM's memory
#define MAX_BYTES 16

// This defines the maximum allowed line length
#define MAX_LINE 128

// This defines the number of (pseudo-)opcodes
#define NUM_CODES 10

// This defines the maximum allowed label length
#define MAX_LABEL 32

// This defines the maximum allowed opcode length
#define MAX_OPCODE 3

// This struct will act like a dictionary mapping
//  each opcode-address pair
struct instruction {
    char opcode[3];
    int address[4];
};

// This struct will act like a dictionary mapping
//  a label to its address value
struct labeladdress {
    char label[MAX_LABEL];
    int byte_num;
    int not_empty;
};

struct labeladdress label_addresses[MAX_BYTES];
struct instruction valid_instructions[NUM_CODES];
char valid_labels[MAX_BYTES][MAX_LINE] = {};

static void initinstructions()
{
    // Set LDA 0001
    valid_instructions[0].opcode[0] = 'L';
    valid_instructions[0].opcode[1] = 'D';
    valid_instructions[0].opcode[2] = 'A';
    valid_instructions[0].address[0] = 0;
    valid_instructions[0].address[1] = 0;
    valid_instructions[0].address[2] = 0;
    valid_instructions[0].address[3] = 1;
    
    // Set LDI 0010
    valid_instructions[1].opcode[0] = 'L';
    valid_instructions[1].opcode[1] = 'D';
    valid_instructions[1].opcode[2] = 'I';
    valid_instructions[1].address[0] = 0;
    valid_instructions[1].address[1] = 0;
    valid_instructions[1].address[2] = 1;
    valid_instructions[1].address[3] = 0;
    
    // Set STA 0011
    valid_instructions[2].opcode[0] = 'S';
    valid_instructions[2].opcode[1] = 'T';
    valid_instructions[2].opcode[2] = 'A';
    valid_instructions[2].address[0] = 0;
    valid_instructions[2].address[1] = 0;
    valid_instructions[2].address[2] = 1;
    valid_instructions[2].address[3] = 1;
    
    // Set STI 0100
    valid_instructions[3].opcode[0] = 'S';
    valid_instructions[3].opcode[1] = 'T';
    valid_instructions[3].opcode[2] = 'I';
    valid_instructions[3].address[0] = 0;
    valid_instructions[3].address[1] = 1;
    valid_instructions[3].address[2] = 0;
    valid_instructions[3].address[3] = 0;
    
    // Set ADD 0101
    valid_instructions[4].opcode[0] = 'A';
    valid_instructions[4].opcode[1] = 'D';
    valid_instructions[4].opcode[2] = 'D';
    valid_instructions[4].address[0] = 0;
    valid_instructions[4].address[1] = 1;
    valid_instructions[4].address[2] = 0;
    valid_instructions[4].address[3] = 1;
    
    // Set SUB 0110
    valid_instructions[5].opcode[0] = 'S';
    valid_instructions[5].opcode[1] = 'U';
    valid_instructions[5].opcode[2] = 'B';
    valid_instructions[5].address[0] = 0;
    valid_instructions[5].address[1] = 1;
    valid_instructions[5].address[2] = 1;
    valid_instructions[5].address[3] = 0;
    
    // Set JMP 0111
    valid_instructions[6].opcode[0] = 'J';
    valid_instructions[6].opcode[1] = 'M';
    valid_instructions[6].opcode[2] = 'P';
    valid_instructions[6].address[0] = 0;
    valid_instructions[6].address[1] = 1;
    valid_instructions[6].address[2] = 1;
    valid_instructions[6].address[3] = 1;
    
    // Set JMZ 1000
    valid_instructions[7].opcode[0] = 'J';
    valid_instructions[7].opcode[1] = 'M';
    valid_instructions[7].opcode[2] = 'Z';
    valid_instructions[7].address[0] = 1;
    valid_instructions[7].address[1] = 0;
    valid_instructions[7].address[2] = 0;
    valid_instructions[7].address[3] = 0;
    
    // Set HLT 1111
    valid_instructions[8].opcode[0] = 'H';
    valid_instructions[8].opcode[1] = 'L';
    valid_instructions[8].opcode[2] = 'T';
    valid_instructions[8].address[0] = 1;
    valid_instructions[8].address[1] = 1;
    valid_instructions[8].address[2] = 1;
    valid_instructions[8].address[3] = 1;
    
    // Set DAT 0000
    valid_instructions[9].opcode[0] = 'D';
    valid_instructions[9].opcode[1] = 'A';
    valid_instructions[9].opcode[2] = 'T';
    valid_instructions[9].address[0] = 0;
    valid_instructions[9].address[1] = 0;
    valid_instructions[9].address[2] = 0;
    valid_instructions[9].address[3] = 0;
}

/*
 * Read the file, store its contents in line_input, and return
 *  the number of lines that were read.
 */
static int readfile(char line_input[MAX_BYTES][MAX_LINE])
{
    int line_num = 0;
    
    //while (!feof(stdin) && line_num < MAX_BYTES) {
    while (!feof(stdin)) {
        if (line_num >= MAX_BYTES) {
            fprintf(stderr, "ERROR on line %d: file is too big\n", line_num);
            exit(0);
        }
        
        int line_chars = 0;
        char currChar;
        
        //while ((currChar = getchar()) != '\n' && line_chars < MAX_LINE) {
        while ((currChar = getchar()) != EOF) {
            if (line_chars >= MAX_LINE) {
                fprintf(stderr, "ERROR on line %d: line is too long\n", line_num);
                exit(0);
            }
            
            if (currChar == '\n') {
                break;
            }
            
            // <for testing>
            //printf("%c", currChar);
            
            line_input[line_num][line_chars] = currChar;
            line_chars++;
        }
        
        // <for testing>
        //printf("\n");
        
        line_num++;
    }
    
    return line_num;
}

/**
 * This compares any 3-digit char array to the opcodes in the 
 *  dictionary to test for equality between them, and returns
 *  1 if there is an equality.
 */
static int isvalidopcode(char test[3])
{
    int is_valid = 0;
    
    for (int i = 0; i < NUM_CODES; i++) {
        if (test[0] == valid_instructions[i].opcode[0] &&
            test[1] == valid_instructions[i].opcode[1] &&
            test[2] == valid_instructions[i].opcode[2]) {
            is_valid = 1;
            break;
        }
    }
    
    return is_valid;
}

/**
 * Check whether or not a given label has already been added to
 *  the array of labels, and if it has return 1;
 */
static int containslabel(char tocheck[MAX_LABEL])
{
    int found = 0;
    
    // <for testing>
    //printf("The valid labels are: %s\n", valid_labels[0]);
    
    for (int i = 0; i < MAX_BYTES; i++) {
        
        // <for testing>
        //printf("The valid label being tested is: %s\n", valid_labels[i]);
        
        for (int j = 0; j < MAX_LABEL; j++) {
            if (isalpha(tocheck[j]) &&
                    tocheck[j] != valid_labels[i][j]) {
                found = 0;
                break;
            }
            
            found = 1;
        }
        
        if (found == 1) {
            return 1;
        }
    }
    
    return 0;
}

/**
 * Add the label to the first empty slot in the array.
 */
static void addlabel(char tocheck[MAX_LABEL])
{
    for (int i = 0; i < MAX_BYTES; i++) {
        if (!isalpha(valid_labels[i][0])) {
            for (int j = 0; j < MAX_LABEL; j++) {
                valid_labels[i][j] = tocheck[j];
            }
            
            break;
        }
    }
}

/**
 * Fill an array with all of the valid labels, and throw an
 *  error if an invalid label is encountered
 */
static void preprocesslabels(char line_input[MAX_BYTES][MAX_LINE], int lines)
{
    int line_start = 0;
    
    for (int i = 0; i < lines - 1; i++) {
        int foundnewlabel = 0;
        
        // Find the actual starting point of the line (i.e. first alphabetical char)
        for (int j = 0; j < MAX_LABEL; j++) {
            // A label, instruction, or comment shouldn't start with a number,
            //  so throw an error
            if (isdigit(line_input[i][j])) {
                fprintf(stderr, "ERROR on line %d: no input shouldn't start"
                        "with a digit\n", i);
                exit(0);
            }
            
            // Break once the first letter has been encountered
            if (isalpha(line_input[i][j])) {
                line_start = j;
                break;
            }
            
            // If the line is just 128 chars of whitespace or if this part is
            //  a comment, make the line start at the end
            if (j == 127 || line_input[i][j] == '#') {
                line_start = 127;
            }
        }
        
        char templabel[MAX_LABEL] = {};
        
        // Find labels to add to the array
        for (int j = line_start; j < MAX_LABEL; j++) {
            int colonindex = 0;

            if (line_input[i][j] == ':') {
                colonindex = j;
                
                // If the very first char is a colon, throw an error
                if (colonindex == 0) {
                    fprintf(stderr, "ERROR on line %d: a colon shouldn't begin"
                            "a label or instruction\n", i);
                    exit(0);
                }
                
                // If the char immediately preceeding the colon is not a letter,
                //  throw an error
                if (!isalpha(line_input[i][j - 1])) {
                    fprintf(stderr, "ERROR on line %d: a label should only"
                            "contain letters\n", i);
                    exit(0);
                }
                
                foundnewlabel = 1;
                break;
            }
            
            // Start generating the (possible) label
            templabel[j] = line_input[i][j];
        }
        
        // Store the label that has been identified
        if (foundnewlabel == 1 && containslabel(templabel) == 0) {
            
            // <for testing>
            //printf("templabel is: %s\n", templabel);
            
            addlabel(templabel);
        } else if (foundnewlabel == 1 && containslabel(templabel) == 1) {
            fprintf(stderr, "ERROR on line %d: attempted to redefine a label\n", i);
            exit(0);
        }
    }
}

/**
 * Determine if a line contains a pre-processed label, and
 *  if so return the index of the colon (return 0 otherwise).
 */
static int haslabel(char tocheck[MAX_LINE], char testlabel[MAX_LABEL])
{
    int foundlabel = 0;
    char templabel[MAX_LABEL] = {};
    
    for (int i = 0; i < MAX_LINE; i++) {
        if (tocheck[i] == ':') {
            for (int j = 0; j < i; j++) {
                if (isalpha(tocheck[j])) {
                    templabel[j] = tocheck[j];
                }
            }
            
            // <for testing>
            //printf("\nTesting label: %s\n", templabel);
            
            foundlabel = containslabel(templabel);
            
            if (foundlabel == 1) {
                for (int k = 0; k < MAX_LABEL; k++) {
                    testlabel[k] = templabel[k];
                }
                
                return i;
            }
        }
    }
    
    return foundlabel;
}

/**
 * Determine if a sequence of three chars could be an opcode 
 *  based on the chars' syntax, and return 1 if so.
 */
static int couldbeopcode(char tocheck[3])
{
    int found = 1;
    
    for (int i = 0; i < 3; i++) {
        if (!isalpha(tocheck[i]) || !isupper(tocheck[i])) {
            found = 0;
        }
    }
    
    // Some form of whitespace must follow an opcode
    if (!isspace(tocheck[3])) {
        found = 0;
    }
    
    return found;
}

/**
 * Determine if the array could contain a valid address (which should
 *  be composed of either all letters or all numbers, but not a mix
 *  of both), and return 1 if so.
 */
static int couldbeaddress(char tocheck[MAX_LABEL])
{
    // At this point, the possible address should only contain
    //  letters and/or numbers
    for (int i = 0; i < MAX_LABEL - 1; i++) {
        if (isalpha(tocheck[i]) && isdigit(tocheck[i + 1])) {
            return 0;
        }
        
        if (isdigit(tocheck[i]) && isalpha(tocheck[i + 1])) {
            return 0;
        }
    }

    return 1;
}

/**
 * Determine if a line could be a valid instruction based on it
 *  having a syntactially valid opcode and address, and return 
 *  1 if so.
 */
static int hasinstruction(char tocheck[MAX_LINE], int start,
                          char testop[3], char testad[MAX_LINE])
{
    int foundopcode = 0;
    int foundaddress = 0;
    int opcodestart = -1;
    int count = 0;
    char tempop[4] = {};
    char tempad[MAX_LABEL] = {};
    
    // Find where a potential opcode begins
    for (int i = start; i < MAX_LINE; i++) {
        if (tocheck[i] == '#') {
            break;
        }
        
        if (isalpha(tocheck[i])) {
            opcodestart = i;
            break;
        }
    }
    
    // Determine if the (potential) instruction contains
    //  what could be an opcode and an address
    if (opcodestart != -1) {
        tempop[0] = tocheck[opcodestart];
        tempop[1] = tocheck[opcodestart + 1];
        tempop[2] = tocheck[opcodestart + 2];
        tempop[3] = tocheck[opcodestart + 3];
        
        // Test the opcode
        foundopcode = couldbeopcode(tempop);
        int adstart = -1;
        
        // Find where a potential address begins
        for (int i = opcodestart + 4; i < MAX_LINE; i++) {
            if (tocheck[i] == '#') {
                break;
            }
            
            if(isalnum(tocheck[i])) {
                adstart = i;
                break;
            }
        }
        
        if (adstart != -1) {
            // Get the potential address
            for (int i = adstart; i < adstart + MAX_LABEL; i++) {
                if (tocheck[i] == '#') {
                    break;
                }
                
                if (i < MAX_LINE - 1 && isalnum(tocheck[i]) &&
                        isspace(tocheck[i + 1])) {
                    tempad[count] = tocheck[i];
                    break;
                } else if (isalnum(tocheck[i])) {
                    tempad[count] = tocheck[i];
                }
                
                count++;
            }
            
            // Test the address
            foundaddress = couldbeaddress(tempad);
        }
    }
    
    if (foundopcode == 1 && foundaddress == 1) {
        for (int i = 0; i < 3; i++) {
            testop[i] = tempop[i];
        }
        
        for (int i = 0; i < MAX_LABEL; i++) {
            testad[i] = tempad[i];
        }
        
        // <for testing>
        printf("Possibly valid instruction found\n");
        printf("Opcode is: %s\n", testop);
        printf("Address is: %s\n\n", testad);
        
        return 1;
    }
    
    // <for testing>
    printf("No valid instruction found: ");
    
    // <for testing>
    if (foundopcode == 1 && foundaddress == 0) {
        printf("invalid address\n\n");
    } else if (foundopcode == 0 && foundaddress == 1) {
        printf("invalid opcode\n\n");
    } else if (foundopcode == 0 && foundaddress == 0) {
        printf("invalid opcode and address\n\n");
    }
    
    return 0;
}

/**
 * Once the input array has been filled with instructions, we now
 *  want to check them to see if any are invalid.
 */
static void checkforerrors(char line_input[MAX_BYTES][MAX_LINE], int lines,
                           char final_labels[MAX_BYTES][MAX_LABEL],
                           char final_opcodes[MAX_BYTES][MAX_OPCODE],
                           char final_addresses[MAX_BYTES][MAX_LINE])
{
    for (int i = 0; i < lines - 1; i++) {
        
        // <for testing>
        printf("Line %d is: %s\n", i, line_input[i]);
        
        char testlabel[MAX_LABEL] = {};
        char testop[MAX_OPCODE] = {};
        char testad[MAX_LINE] = {};
        
        int instructionstart = 0;
        
        // Check for the presence of a label (which would be valid if present)
        //  to determine where to start looking for an instruction
        int labelpresent = haslabel(line_input[i], testlabel);
        
        if (labelpresent != 0) {
            instructionstart = labelpresent + 1;
            
            // <for testing>
            printf("This line has a label: ");
            
            // Since there are no errors in the instruction at this point,
            //  copy the opcodes and addresses for easier access later
            for (int a = 0; a < MAX_LABEL; a++) {
                final_labels[i][a] = testlabel[a];
            }
        }
        
        // Check for the presence of an instruction (opcode + address)
        if (hasinstruction(line_input[i], instructionstart,
                           testop, testad) == 1) {
            // Check for a valid opcode
            if (isvalidopcode(testop) != 1) {
                fprintf(stderr, "ERROR on line %d: instruction's opcode"
                        "is invalid\n", i);
                exit(0);
            }
            
            // Check for a defined label or a valid number (0-15 for
            //  instructions, or 0-255 for DAT)
            if (isdigit(testad[0])) {
                long address_num;
                char *end;
                
                address_num = strtol(testad, &end, 10);
                
                if (testop[0] == 'D' &&
                    testop[1] == 'A' &&
                    testop[2] == 'T') {
                    // For DAT, if its value isn't within 8 bits, throw an error
                    if (address_num < 0 || address_num > 255) {
                        fprintf(stderr, "ERROR on line %d: non-address data"
                                "must be within 8 bits\n", i);
                        exit(0);
                    }
                } else {
                    // If addresses aren't within 4 bits, throw an error
                    if (address_num < 0 || address_num > 15) {
                        fprintf(stderr, "ERROR on line %d: addresses"
                                "must be within 4 bits\n", i);
                        exit(0);
                    }
                }
            }
            
            // Since there are no errors in the instruction at this point,
            //  copy the opcodes and addresses for easier access later
            for (int a = 0; a < MAX_OPCODE; a++) {
                final_opcodes[i][a] = testop[a];
            }
            
            for (int a = 0; a < MAX_LINE; a++) {
                final_addresses[i][a] = testad[a];
            }
        }
    }
}

/**
 * This concatenates two ints into a single number.
 */
static int concatints(int first, int second)
{
    int concatenated_int;
    concatenated_int = first * pow(10, (int)log10(second) + 1) + second;
    
    return concatenated_int;
}

/**
 * Convert a 4-bit decimal int to its binary representation.
 */
static void tobinary(int num, int binaddress[4])
{
    int b;
    int j = 0;
    
    for (int i = 3; i >= 0; i--) {
        b = num >> i;
        
        if (b & 1) {
            binaddress[j] = 1;
        } else {
            binaddress[j] = 0;
        }
        
        j++;
    }
}

/**
 * Get the binary representation of a valid opcode.
 */
static void getbinaryopcode(char code[3], int binopcode[4])
{
    for (int i = 0; i < NUM_CODES; i++) {
        if (code[0] == valid_instructions[i].opcode[0] &&
            code[1] == valid_instructions[i].opcode[1] &&
            code[2] == valid_instructions[i].opcode[2]) {
            binopcode[0] = valid_instructions[i].address[0];
            binopcode[1] = valid_instructions[i].address[1];
            binopcode[2] = valid_instructions[i].address[2];
            binopcode[3] = valid_instructions[i].address[3];
            break;
        }
    }
}

/**
 * Add a new label-byte pair to the label_addresses array.
 */
static void addlabelbyte(char labeltoadd[MAX_LABEL], int count)
{
    for (int i = 0; i < MAX_BYTES; i++) {
        if (label_addresses[i].not_empty != 1) {
            for (int j = 0; j < MAX_LABEL; j++) {
                label_addresses[i].label[j] = labeltoadd[j];
            }
            
            label_addresses[i].byte_num = count;
            label_addresses[i].not_empty = 1;
            break;
        }
    }
}

/**
 * Determine if a label exists within the label_addresses array,
 *  and return the corresponding byte_num if so (-1 otherwise).
 */
static int getbytenum(char tocheck[MAX_LABEL])
{
    int found;
    
    for (int i = 0; i < MAX_BYTES; i++) {
        for (int j = 0; j < MAX_LABEL; j++) {
            if (label_addresses[i].label[j] != tocheck[j]) {
                found = 0;
                break;
            }
            
            found = 1;
        }
        
        if (found == 1) {
            return label_addresses[i].byte_num;
        }
    }
    
    return -1;
}

/**
 * Convert an address to a label.
 */
static void convertlabel(char address[MAX_LINE], char labeltoconvert[MAX_LABEL])
{
    for (int i = 0; i < MAX_LABEL; i++) {
        labeltoconvert[i] = address[i];
    }
}

/**
 * Output the binary representation of the input to stdout.
 */
static void processforoutput(char line_input[MAX_BYTES][MAX_LINE], int lines,
                             char final_labels[MAX_BYTES][MAX_LABEL],
                             char final_opcodes[MAX_BYTES][MAX_OPCODE],
                             char final_addresses[MAX_BYTES][MAX_LINE])
{
    int label_bytes[MAX_BYTES] = {};
    int byte_count = 0;
    
    // Get the value for each label
    for (int i = 0; i < MAX_BYTES; i++) {
        if (isalpha(final_opcodes[i][0]) && isalnum(final_addresses[i][0])) {
            if (isalpha(final_labels[i][0])) {
                
                
                // <for testing>
                /*printf("The current label being checked is: %s; ", final_labels[i]);
                printf("Byte count is: %d; ", byte_count);
                printf("i is: %d.\n\n", i);*/
                
                
                addlabelbyte(final_labels[i], byte_count);
            }
            
            byte_count++;
        }
    }
    
    
    // <for testing>
    /*for (int a = 0; a < MAX_BYTES; a++) {
        if (isalpha(label_addresses[a].label[0])) {
            printf("Label %d in label_addresses is %s, and its bytes are %d\n", a,
                   label_addresses[a].label, label_addresses[a].byte_num);
        }
        
    }
    printf("\n");*/
    
    
    
    for (int i = 0; i < MAX_BYTES; i++) {
        if (isalpha(final_opcodes[i][0]) && isalnum(final_addresses[i][0])) {
            int address;
            int binaddress[4] = {};
            int binopcode[4] = {};
            int uses_label = 0;
            char labeltoconvert[MAX_LABEL] = {};
            
            // Get the binary representation of each opcode
            getbinaryopcode(final_opcodes[i], binopcode);
            
            // Convert the decimal int to its binary representation
            if (isdigit(final_addresses[i][0]) && !isdigit(final_addresses[i][1])) {
                address = final_addresses[i][0] - '0';
            } else if (isdigit(final_addresses[i][0]) &&
                       isdigit(final_addresses[i][1]) &&
                       !isdigit(final_addresses[i][2])) {
                address = concatints(final_addresses[i][0] - '0',
                                     final_addresses[i][1] - '0');
            } else if (isdigit(final_addresses[i][0]) &&
                       isdigit(final_addresses[i][1]) &&
                       isdigit(final_addresses[i][2])) {
                // This is only the case for DAT
                address = concatints(final_addresses[i][0] - '0',
                                     final_addresses[i][1] - '0');
                address = concatints(address, final_addresses[i][2] - '0');
            }
            
            if (!isdigit(final_addresses[i][0])) {
                uses_label = 1;
            }
            
            if (uses_label == 0) {
                tobinary(address, binaddress);
            } else {
                convertlabel(final_addresses[i], labeltoconvert);
                
                int bytes = getbytenum(labeltoconvert);
                tobinary(bytes, binaddress);
            }
            
            
            // <for testing>
            /*printf("Opcode is: %s; ", final_opcodes[i]);
            printf("Address is: %s; ", final_addresses[i]);
            if (uses_label == 1) {
                printf("Uses label: true; ");
                printf("The current label being checked is: %s\n", labeltoconvert);
            } else {
                printf("Uses label: false\n");
            }
            printf("i is: %d.\n", i);*/
            
            
            printf("%d%d%d%d%d%d%d%d ", binopcode[0],
                   binopcode[1],
                   binopcode[2],
                   binopcode[3],
                   binaddress[0],
                   binaddress[1],
                   binaddress[2],
                   binaddress[3]);
            
            // <for testing>
            //printf("\n");
        }
    }
    
    //printf("  .U4p..");
    printf("\n");
}

int main(void)
{
    char line_input[MAX_BYTES][MAX_LINE];
    int lines = 0;
    char final_labels[MAX_BYTES][MAX_LABEL] = {};
    char final_opcodes[MAX_BYTES][MAX_OPCODE] = {};
    char final_addresses[MAX_BYTES][MAX_LINE] = {};
    
    // Initialize the "dictionary" of valid opcode-address pairs
    initinstructions();

    // Read in the SCRAM instructions from standard input
    lines = readfile(line_input);

    // Pre-process the input to get all of the valid labels
    preprocesslabels(line_input, lines);
    
    // Parse the inputted instructions for errors
    checkforerrors(line_input, lines, final_labels,
                   final_opcodes, final_addresses);
    
    // At this point, all the input must be valid so get each
    //  opcode and address's binary representations and output
    //  them to stdout
    
    // <for testing>
    printf("\nPRINTING THE FINAL BINARY REPRESENTATIONS:\n");
    
    
    processforoutput(line_input, lines, final_labels,
                     final_opcodes, final_addresses);
    
    return EXIT_SUCCESS;
}