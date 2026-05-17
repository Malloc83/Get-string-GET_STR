/*
File:           get_str.c | version: 1.03
Last updated:   05/16/2026 
By:             Tom Burm
Purpose:        GET_STR: standardized reusable function to obtain STRING via USER INPUT */   

/*----------------*/
/*--- INCLUDES ---*/
/*----------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*------------------------------------------*/
/* --- PREPROCESOR MACROS AND CONSTANTS --- */
/*------------------------------------------*/



/* ----------------------------*/
/* -- TYPEDEFS/ENUMS/STRUCTS --*/
/* ----------------------------*/

/*  StrGetFlags - set the different bitwise flags for the function GET_STR
The flags are defined by typdef StrGetFlag as unsigned integers.
They are set as unique bitwise pattern 'integers' via the bitwise left shift operator (<<).
A descriptive name is given via enumeration (enum) for each of the flags.
Examples:
1u << 0 equals 1 shifted left with zero bits : 00000001 << 0 = 00000001 (decimal: 1)
1u << 2 equals 1 shifted left with  two bits : 00000001 << 2 = 00000100 (decimal: 4) */

typedef unsigned int StrGetFlags;
enum {
    ALLOW_SPACES       = 1u << 0, // 1 (unsigned integers through 1u)
    ALLOW_DOT          = 1u << 1, // 2
    REQUIRE_NONEMPTY   = 1u << 2, // 4
    NO_ONLY_WHITESPACE = 1u << 3, // 8
    ALLOW_HYPHEN       = 1u << 4, // 16
    ALLOW_UNDERSCORE   = 1u << 5, // 32
    ALLOW_SLASH        = 1u << 6  // 64
};

/* GET_STR | Presets for validating input */
enum {
    INPUT_IDENTIFIER =
        REQUIRE_NONEMPTY |
        NO_ONLY_WHITESPACE,

    INPUT_SENTENCE =
        REQUIRE_NONEMPTY |
        NO_ONLY_WHITESPACE |
        ALLOW_SPACES |
        ALLOW_DOT,

    INPUT_FILENAME =
        REQUIRE_NONEMPTY |
        NO_ONLY_WHITESPACE |
        ALLOW_DOT |
        ALLOW_UNDERSCORE |
        ALLOW_HYPHEN,

    INPUT_PATH =
        REQUIRE_NONEMPTY |
        NO_ONLY_WHITESPACE |
        ALLOW_SPACES |
        ALLOW_SLASH |
        ALLOW_UNDERSCORE |
        ALLOW_HYPHEN,

    INPUT_FREE_TEXT =
        REQUIRE_NONEMPTY |
        ALLOW_SPACES |
        ALLOW_DOT |
        ALLOW_HYPHEN |
        ALLOW_UNDERSCORE
};

/* Enum GET_STR_MAX for max size (CHAR) to define aray size  */
enum {GET_STR_MAX  = 21 }; // --> len+1 for \0, this will allow for up to [GET_STR_MAX -1] characters

/*------------------------------------------*/
/*--- FUNCTION DECLARATIONS (PROTOTYPES) ---*/
/*------------------------------------------*/

char* GET_STR(char *str, int max_length, StrGetFlags flags);

/* ----------------------------*/
/* --- MAIN BODY OF PRGRAM --- */
/* ----------------------------*/

int main(int argc, char* argv[]){

    /* --- OUTPUT --- */
    char outp_str[GET_STR_MAX]; /* allocate GET_STR_MAX characters memory for string to store results */
    printf("Provide string via STDIN (max %d char): ", GET_STR_MAX-1);
    if (GET_STR(outp_str, sizeof(outp_str),INPUT_IDENTIFIER) == NULL) { // validation, string as IDENTIFIER
        printf("Input error\n");
        return 1;
    }
    printf("You entered: %s\n", outp_str);

    return 0; // return 0 when successfully completed

} // end main function

/*------------------------*/
/*--- HELPER FUNCTIONS ---*/
/*------------------------*/

/* FUNCTION: GET_STR | Obtains validated string based on flags */

char* GET_STR(char *str, int max_length, StrGetFlags flags)
{
    if (fgets(str, max_length, stdin) == NULL) {
        return NULL;
    }

    /* strip newline & flush buffer when overflowed for max_lenght */
    size_t len = strlen(str);

    if (len > 0 && str[len - 1] == '\n') { /* checks if the last character is a newline */
        str[len - 1] = '\0'; /* replace the newline with terminator \0 */
        len--; /* update the lenght of the string with -1 as \n is removed */
    } else { /* input via stdin exceeds buffer - need to flush the buffer */
        int c;

        while ((c = getchar()) != '\n' && c != EOF);
    }

    if ((flags & REQUIRE_NONEMPTY) && len == 0) {
        return NULL;
    }

    int has_non_space = 0;

    for (size_t i = 0; i < len; i++) {

        unsigned char ch = (unsigned char)str[i];

        int valid =
            isalnum(ch) || /* this is the base case, i.e. all chars need to be alphanumeric OR */
            ((flags & ALLOW_SPACES)     && ch == ' ') ||
            ((flags & ALLOW_DOT)        && ch == '.') ||
            ((flags & ALLOW_HYPHEN)     && ch == '-') ||
            ((flags & ALLOW_UNDERSCORE) && ch == '_') ||
            ((flags & ALLOW_SLASH)      && ch == '/');

        if (!valid) {
            return NULL;
        }

        if (!isspace(ch)) {
            has_non_space = 1;
        }
    }

    if ((flags & NO_ONLY_WHITESPACE) && !has_non_space) {
        return NULL;
    }

    return str;
}