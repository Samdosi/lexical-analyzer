#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

int alphatoken(char* input, int* i);
int numbertoken(char* input, int* i);
int symboltoken(char* input, int* i, int len);
int comment(char* input, int* i, int len);
int reservedcheck(char *buffer);
void printlexerror(int type);
void printtokens();

lexeme *lexanalyzer(char *input, int printFlag)
{
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;

	int length = strlen(input);

	for(int i = 0; i < length; i++){
		if(iscntrl(input[i]) != 0 || isspace(input[i]) != 0){
			continue;
		}

		if(comment(input, &i, length)){
			continue;
		}

		if(isalpha(input[i]) != 0){
			if(alphatoken(input, &i) == -1){
				printlexerror(3);
				return NULL;
			}
		}
		else if(isdigit(input[i]) != 0){
				int check = numbertoken(input, &i);
				if(check == -1){
					printlexerror(2);
					return NULL;
				}
				else if(check == -2){
					printlexerror(1);
					return NULL;
				}
			}
		else if(symboltoken(input, &i, length) == -1){
			printlexerror(4);
			return NULL;
		}
	}
	
	if (printFlag)
		printtokens();
	list[lex_index++].type = -1;
	return list;
}

int symboltoken(char* input, int* i, int len){
	char buffer[3];
	if(*i + 1 != len){
		if(input[*i] == ':' && input[*i + 1] == '='){
			list[lex_index].type = assignsym;
			lex_index++;
            *i = *i +1;
			return 1;
		}
		if(input[*i] == '=' && input[*i+ 1] == '='){
			list[lex_index].type = eqlsym;
			lex_index++;
            *i = *i +1;
			return 1;
		}
		if(input[*i] == '<' && input[*i +1] == '>'){
			list[lex_index].type = neqsym;
			lex_index++;
            *i = *i +1;
			return 1;
		}
		if(input[*i] == '<' && input[*i +1] == '='){
			list[lex_index].type = leqsym;
			lex_index++;
            *i = *i +1;
			return 1;
		}
		if(input[*i] == '>' && input[*i +1] == '='){
			list[lex_index].type = geqsym;
			lex_index++;
            *i = *i +1;
			return 1;
		}
	}


	if(input[*i] == '.'){
			list[lex_index].type = periodsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '['){
			list[lex_index].type = lbracketsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == ']'){
			list[lex_index].type = rbracketsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == ','){
			list[lex_index].type = commasym;
			lex_index++;
			return 1;
		}
	if(input[*i] == ';'){
			list[lex_index].type = semicolonsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '?'){
			list[lex_index].type = questionsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == ':'){
			list[lex_index].type = colonsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '('){
			list[lex_index].type = lparenthesissym;
			lex_index++;
			return 1;
		}
	if(input[*i] == ')'){
			list[lex_index].type = rparenthesissym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '<'){
			list[lex_index].type = lsssym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '>'){
			list[lex_index].type = gtrsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '+'){
			list[lex_index].type = addsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '-'){
			list[lex_index].type = subsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '*'){
			list[lex_index].type = multsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '/'){
			list[lex_index].type = divsym;
			lex_index++;
			return 1;
		}
	if(input[*i] == '%'){
			list[lex_index].type = modsym;
			lex_index++;
			return 1;
		}

	return -1;
}

int numbertoken(char* input, int* i){
	int count = 0;
	char buffer[MAX_NUMBER_LEN + 1];

	while(isdigit(input[*i]) != 0){
		if(count == MAX_NUMBER_LEN) return -1;

		buffer[count] = input[*i];
		count++;
        *i = *i +1;
	}	

	if(isalpha(input[*i]) != 0)
		return -2;
	*i = *i - 1;

	int num = 0;
	for(int j = 0; j < count; j++){
		num *= 10;
		 num += buffer[j] - 48;
	}
	list[lex_index].value = num;
	list[lex_index].type = numbersym;
	lex_index++;
	return 1;
	
}

int alphatoken(char* input, int* i){
	int count = 0;
	char buffer[MAX_IDENT_LEN + 1];

	while(isalpha(input[*i]) != 0 || isdigit(input[*i]) != 0){
		if(count == MAX_IDENT_LEN) return -1;

		buffer[count] = input[*i];
		count++;
        *i = *i +1;
	}
    buffer[count] = '\0';
	*i = *i -1;
	int flag = reservedcheck(buffer);

	switch(flag){
			case varsym:
                strcpy(list[lex_index].name, "var");
				list[lex_index].type = varsym;
				lex_index++;
				return 1;
			case procsym:
                strcpy(list[lex_index].name, "procedure");
				list[lex_index].type = procsym;
				lex_index++;
				return 1;
			case callsym:
                strcpy(list[lex_index].name, "call");
				list[lex_index].type = callsym;
				lex_index++;
				return 1;
			case beginsym:
                strcpy(list[lex_index].name, "begin");
				list[lex_index].type = beginsym;
				lex_index++;
				return 1;
			case endsym:
                strcpy(list[lex_index].name, "end");
				list[lex_index].type = endsym;
				lex_index++;
				return 1;
			case ifsym:
                strcpy(list[lex_index].name, "if");
				list[lex_index].type = ifsym;
				lex_index++;
				return 1;
			case dosym:
                strcpy(list[lex_index].name, "do");
				list[lex_index].type = dosym;
				lex_index++;
				return 1;
			case readsym:
                strcpy(list[lex_index].name, "read");
				list[lex_index].type = readsym;
				lex_index++;
				return 1;
			case writesym:
                strcpy(list[lex_index].name, "write");
				list[lex_index].type = writesym;
				lex_index++;
				return 1;
			case whilesym:
                strcpy(list[lex_index].name, "while");
				list[lex_index].type = whilesym;
				lex_index++;
				return 1;
			default:
                strcpy(list[lex_index].name, buffer);
				list[lex_index].type = identsym;
				lex_index++;
				return 1;
	}
}

int reservedcheck(char* buffer){
	if(strcmp(buffer, "var") == 0)
		return 2;
	if(strcmp(buffer, "procedure") == 0)
		return 4;
	if(strcmp(buffer, "call") == 0)
		return 6;
	if(strcmp(buffer, "begin") == 0)
		return 8;
	if(strcmp(buffer, "end") == 0)
		return 10;
	if(strcmp(buffer, "if") == 0)
		return 12;
	if(strcmp(buffer, "do") == 0)
		return 14;
	if(strcmp(buffer, "while") == 0)
		return 33;
	if(strcmp(buffer, "read") == 0)
        return 16;
	if(strcmp(buffer, "write") == 0)
		return 18;
	return 20;
	
}

int comment(char* input, int *i, int len){
	if(*i + 1 == len) return 0;
	
	if(input[*i] == '/' && input[*i + 1] == '/'){
		while((*i + 1 != len) &&( input[*i] != '\0' && input[*i] != '\n' && (input[*i] != '\r' || input[*i +1] != '\n'))){
            *i = *i +1;
		}
		return 1;
	}
	return 0;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case lbracketsym:
				printf("%11s\t%d", "[", lbracketsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case rbracketsym:
				printf("%11s\t%d", "]", rbracketsym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case questionsym:
				printf("%11s\t%d", "?", questionsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case colonsym:
				printf("%11s\t%d", ":", colonsym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case lparenthesissym:
				printf("%11s\t%d", "(", lparenthesissym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case rparenthesissym:
				printf("%11s\t%d", ")", rparenthesissym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
			case neqsym:
				printf("%11s\t%d", "<>", neqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			default:
				printf("%11s\t%s", "err", "err");
				break;
		}
		printf("\n");
	}
	printf("\n");
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Number Length\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Identifier Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}
