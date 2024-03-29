#include <stdio.h>
#include <ctype.h>

/* Global declarations */ 

/* Variables */
int charClass;
char lexeme [100];
char nextChar;
char errormsg[10000];
int i = 0;
int lexLen;
int token;
int nextToken;
char *line = NULL;
size_t len = 0;
ssize_t read;
int errorbool = 0;
char errorchar;
char errorchar1;

FILE *in_fp, *fopen();
FILE *out_fp, *fopen();
FILE *temp, *fopen();

/* Function declarations */ 
void addChar();
void getChar();
void getNonBlank();
int lex();
void term();
void expr();
void factor();


/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26

/******************************************************/
/* main driver */


int main(int argc, char *argv[]) {
    /* Open the input data file and process its contents */ 
    if (argc == 2){
        printf("The file to be parsed is %s\n\n", argv[1]);
        if ((in_fp = fopen(argv[1], "r")) == NULL)
            printf("ERROR - cannot open %s\n", argv[1]); 
        else {
            while ((read = getline(&line, &len, in_fp)) != -1) {
                if ((out_fp = fopen("front.out", "w")) == NULL){
                    printf("ERROR - cannot open front.out \n"); 
                }
                else{
                    fputs(line, out_fp);
                    fputs("\0", out_fp);
                    fclose(out_fp);
                    if ((temp = fopen("front.out", "r")) == NULL){
                        printf("ERROR - cannot open front.out \n"); 
                    }
                    else{
                        printf("Parsing new line %s\n", line);
                        printf("*****************\n");
                        i = 0;
                        errorbool = 0;
                        errorchar = ' ';
                        errorchar1 = ' ';
                        printf("  \n");
                        getChar(); 
                        do {
                            lex();
                            expr();
                        } while (nextToken != EOF);
                        printf(" \n");
                        printf("Parsing current line finished\n");
                        printf("*****************\n");
                        printf(" \n");
                        fclose(temp);
                        if ((temp = fopen("front.out", "w")) == NULL){
                        printf("ERROR - cannot open front.out \n"); 
                        }
                        else{  
                            fclose(temp);
                        }
                    }
                } 
            } 
            printf("Parsing completed. \n");
            printf("*****************\n");
        }
    }
    else if (argc > 2){
        printf("Too many file names supplied. Only one required.\n");
    }
    else {
        printf("One filename expected.\n");
    }
    fclose(in_fp);
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */ 
int lookup(char ch) {
    switch (ch) { 
        case '(':
            addChar();
            nextToken = LEFT_PAREN; 
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN; 
            break;
        case '+':
            addChar(); 
            nextToken = ADD_OP; 
            break;
        case '-':
            addChar(); 
            nextToken = SUB_OP; 
            break;
        case '*':
            addChar();
            nextToken = MULT_OP; 
            break;
        case '/':
            addChar(); 
            nextToken = DIV_OP; 
            break;
        default: 
            addChar(); 
            nextToken = EOF; 
            break;
    }
    return nextToken; 
}


/*****************************************************/ 
/* addChar - a function to add nextChar to lexeme */ 
void addChar() {
    if (lexLen <= 98) { 
        lexeme[lexLen++] = nextChar; 
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}


/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */ 
void getChar() {
    if ((nextChar = getc(temp)) != EOF) {
        if (errorbool == 0){
            errormsg[i] = nextChar;
            if (nextChar != '\n')
                errorchar1 = nextChar;
            i = i + 1;
            errormsg[i] = '\0';
        }
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else 
            charClass = UNKNOWN;
    }
    else
        charClass = EOF;
}


/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */ 
void getNonBlank() {
    while (isspace(nextChar)) 
        getChar();
} 

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic
expressions */ 
int lex() {
    lexLen = 0; 
    getNonBlank();
    errorchar = nextChar;
    switch (charClass) {
    /* Parse identifiers */ 
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar(); 
            }
            nextToken = IDENT; 
            break;
    /* Parse integer literals */ 
        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar(); 
            }
            nextToken = INT_LIT; 
            break;
    /* Parentheses and operators */ 
        case UNKNOWN:
            lookup(nextChar); 
            getChar();
            break;
    /* EOF */
        case EOF:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    } /* End of switch */
    printf("Next token is: %d, Next lexeme is %s\n", nextToken, lexeme); 
    return nextToken;
} /* End of function lex */


/* expr
Parses strings in the language generated by the rule:
<expr> -> <term> {(+ | -) <term>}
*/
void expr() {
	printf("Enter <expr>\n");
    /* Parse the first term */
    term();
    /* As long as the next token is + or -, get
    the next token and parse the next term */
	while (nextToken == ADD_OP || nextToken == SUB_OP) {
		lex();
		term(); 
	}
    printf("Exit <expr>\n");
 }  /* End of function expr */


/* term
   Parses strings in the language generated by the rule:
   <term> -> <factor> {(* | /) <factor>)
*/
void term() { 
    printf("Enter <term>\n");
    /* Parse the first factor */
    factor();
    /* As long as the next token is * or /, get the
    next token and parse the next factor */
    while (nextToken == MULT_OP || nextToken == DIV_OP) { 
        lex();
        factor(); 
    }
    printf("Exit <term>\n");
}  /* End of function term */



void error() {
    errorbool = 1;
    printf("%s\n",errormsg);
    if ((lexeme[0] == 'E') && (lexeme[1] == 'O') && (lexeme[2] == 'F'))
        printf("********   There is an error at '%c' .   ************\n", errorchar1);
    else
        printf("********   There is an error at '%c' .  ************\n", errorchar);
}

/* factor
   Parses strings in the language generated by the rule:
   <factor> -> id | int_constant | ( <expr )
   */

void factor() {
	printf("Enter <factor>\n");
    /* Determine which RHS */
	if (nextToken == IDENT || nextToken == INT_LIT)
        /* Get the next token */
	   lex();
    /* If the RHS is ( <expr>), call lex to pass over the
    left parenthesis, call expr, and check for the right
    parenthesis */
	else {
		if (nextToken == LEFT_PAREN) {
			lex();
			expr();
			if (nextToken == RIGHT_PAREN)
				lex();
			else
                error();
        }  
        /* End of if (nextToken == ... */
        /* It was not an id, an integer literal, or a left
        parenthesis */
		else
            error();
    }  /* End of else */
    
    printf("Exit <factor>\n");;
}  /* End of function factor */
