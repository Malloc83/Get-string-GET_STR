## GET_STR: standardized reusable function to obtain a STRING

Version:        1.03
Date:           5/16/2026
Author:         Malloc83

**This file and associated code is licensed under the MIT License, see LICENSE.txt**

## Why is this project useful?


## Functionality of the program ##

### Flags ###
The GET_STR defined function uses bitwise **flags** used during input validation when getting a string.
They are set as unique bitwise pattern 'unsigned integers' via the bitwise left shift operator (<<).
These flags are set in the **TYPEDEFS/ENUMS/STRUCTS** definition part of the progam and not in the function itself. These are unambigeous integer identifiers by utilizing power of two (2^n), so each individual bit can be used as a separate flag without confusion. Examples of these bitwise flags are:
1u << 0 equals 1 shifted left with zero bits : 00000001 << 0 = 00000001 (decimal: 2^0 == 1)
1u << 2 equals 1 shifted left with  two bits : 00000001 << 2 = 00000100 (decimal: 2^2 == 4)

The flags have a type definition (typedef) **StrGetFlags** as unsigned integer.
Through enumeration (enum) the different flags are named and assigned an unsigned integer.

For the GET_STR function the following named flags are defined:
ALLOW_SPACES       = 1u << 0, // 1 
ALLOW_DOT          = 1u << 1, // 2
REQUIRE_NONEMPTY   = 1u << 2, // 4
NO_ONLY_WHITESPACE = 1u << 3, // 8
ALLOW_HYPHEN       = 1u << 4, // 16
ALLOW_UNDERSCORE   = 1u << 5, // 32
ALLOW_SLASH        = 1u << 6  // 64

### Presets ###
In a similar way several **presets**, i.e. combination of different flags, are defined via enumeration.
These presets use the bitwise OR (|) in the definition as these are used as input parameter in the function itself and all the flags need to be 'switched on'. In the INPUT_IDENTIFIER the flags indicate that it requires non-empy (must have at least one char) and not only white spaces to be a valid input for this type.
In the GET_STR function itself it is defined through **isalnum()** that the base case is that all inputed characters need to be alphanumeric (a-z, A-Z, 0-9). The exceptions are defined via the "ALLOW_" flags.

INPUT_IDENTIFIER =
        REQUIRE_NONEMPTY |
        NO_ONLY_WHITESPACE,

For the GET_STR function the following named presets are defined, where all presets require **REQUIRE_NONEMPTY** and:

INPUT_IDENTIFIER    NO_ONLY_WHITESPACE
INPUT_SENTENCE      NO_ONLY_WHITESPACE, ALLOW_SPACES, ALLOW_DOT
INPUT_PATH          NO_ONLY_WHITESPACE, ALLOW_SPACES, ALLOW_UNDERSCORE, ALLOW_HYPHEN, ALLOW_SLASH
INPUT_FILENAME      NO_ONLY_WHITESPACE, ALLOW_DOT, ALLOW_UNDERSCORE, ALLOW_HYPHEN
INPUT_FREE_TEXT     ALLOW_SPACES, ALLOW_DOT, ALLOW_UNDERSCORE, ALLOW_HYPHEN

### GET_STR - how the function works ###

#### obtaining string via fgets() ####
fgets() is a standard C function to safely read a line of text from either a file of from the keyboard (STDIN).

In this funcion the buffer 'str' is where the text will be stored. The maximum number of characters to be read is set via max_lenght, and the input source is here defined as the keyboard (stdin).

fgets(str, max_length, stdin)

NOTE that this function does NOT allocate memory for the text 'str' to be stored. A character array with the desired number of postions (character+ \0) needs to be declared upfront, like: char outp_str[GET_STR_MAX];

When the value of string is NULL then the function exits by returning NULL itself. This is done for all the exit cases.

#### strip new line (\n) & flush buffer when input exceeds buffer size ####

The total lenght of the inputted string is determined via variable 'len' through strlen(str).
If the last character [len-1, the terminator not included] is a newline \n then this is replaced with a terminator character \0 to indicate the new end of the string and then the lenght 'len' is updated with -1 to account for the removed character. 

When the string inputted via STDIN exceeds the max_buffer size the buffer needs to be flushed. This is done via the else statement with a while loop which represents: continue UNTIL newline OR EOF (equivalent to continue WHILE not newline AND not EOF as used here).

/* strip newline */
    size_t len = strlen(str);

    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
        len--;
    } else { /* input via stdin exceeds buffer - need to flush the buffer */
        int c;

        while ((c = getchar()) != '\n' && c != EOF);
    }

#### for loop with bitwise comparison of flags and presets ####
The validation check if the provided string matches the required format as per **preset** is done via a 'for' loop which checks for each inputted char if it is allowed. 
An variable 'ch' of type unsigned char is declared for this purpose. The for loop counter i is used to get the value of i-th position of the string and is typecasted as unsigned char to match type of ch. 

for (size_t i = 0; i < len; i++) {

        unsigned char ch = (unsigned char)str[i];
...
}

For the validation check the basecase is that all inputted charactes are alphanumeric (isalnum(ch)), and then with logival OR(||) the exceptions are checked in case the character would not be alphanumric to maintain the 'valid' variable TRUE for the 5 defined exceptions. 

For this exception validation the **flags** and the **presets** are used in a bitwise AND (&) comparison. This means that the bit corresponding to a specific flag needs to be present in the preset as per function input variable **flag**.

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

An example of the bitewise AND (&) comparison is provided for case that flags == INPUT_SENTENCE and if spaces are allowed (ALLOW_SPACES). In this case the INPUT_SENTENCE flags contains the 'ALLOW_SPACES' flag identifier as well and hence the statement (INPUT_SENTENCE & ALLOW_SPACES) is TRUE as in both the most left (2^0) bit is switched on as TRUE (1).

ALLOW_SPACES       = 1u << 0                                    --> bit sequence: 0000000**1**

INPUT_SENTENCE      NO_ONLY_WHITESPACE, ALLOW_SPACES, ALLOW_DOT --> bit sequence: 0000101**1**
  bit sequence                00001000      00000001   00000010

When at least one condition is TRUE in the overall validation then the int variable **valid** is set to TRUE.
This means that this character in the string is valid. In this program it is selected for integer approach and not the true BOOLEAN appproach for compatibility reasons and also avoiding the stdbool.h library. Funcitons like isalnum() already return an integer, also another reason to stick to the integer approach. Valid returns 1 when TRUE and zero (0) when FALSE.

Within the same loop a check is done to see if there are not only whitespace characters (for NO_ONLY_WHITESPACE) by setting the variable 'has_non_space' to 1 when a non-white space character is detected in the string. The variable has_non_space is initiated and set to zero (0) outside of the for loop.
 
 if (!isspace(ch)) {
            has_non_space = 1;
        }

This code utilizes the isspace() funtion to determine if ch is whitespace ( " ", "\t", "\n", "\r", "\f", "\v").
As it is used as **!isspace(ch)** it becomes true when it is NOT a whitespace character. 

The GET_STR function is uncessesfull in getting returing a valid string when 'NO_ONLY_WHITESPACE' is set and there is no (!has_non_space) other than whitespace characters are detected.

 if ((flags & NO_ONLY_WHITESPACE) && !has_non_space) {
        return NULL;
    }

If all the inputted characters are valid then the string (str) is retured by the function. 

## How to get started with this project.

The GET_STR function has three (3) input parameters, the string which it returns (str), the maximum number of characters which it can store it the buffer max_lenght and the flags (either as preset as combination of flags or indivual flags).

char* GET_STR(char *str, int max_length, StrGetFlags flags);

An typical usage example within the main() function is given below: 

enum {GET_STR_MAX  = 21 }; // --> len+1 for \0, this will allow for up to [GET_STR_MAX -1] characters

char outp_str[GET_STR_MAX]; /* allocate GET_STR_MAX characters memory for string to store results */

printf("Provide string via STDIN (max %d char): ", GET_STR_MAX-1);
if (GET_STR(outp_str, sizeof(outp_str),INPUT_IDENTIFIER) == NULL) { // validation, string as IDENTIFIER
        printf("Input error\n");
        return 1;
}
printf("You entered: %s\n", outp_str);

## Who maintains this project 
This project is maintained by Malloc83 and last revised on 16th May 2026.
