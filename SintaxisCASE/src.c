#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <conio.h>
#include <ctype.h>

char CONST_TOKENS[5][5] = { "CASE", "OF", "ELSE", "END" };
char SIGN_TOKENS[12][3] = { "+", "-", "*", "/" }; // : ; := ( )
char NUMBER_TOKENS[10][2] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
//char arrayToken[100][20] = { "\0" };
int next = 0;

struct StructTokens
{
	char token[50];
	char *tokenType;
};

struct StructTokens arrayToken[100] = { "\0" };

char *getTokenType(char *token);
int E();
int E1();
int E2();
int term(char *tok);
int EXPR();
int CONST();
int VARIABLE();
int BODY();
int SELECTION();

void main()
{
	FILE *myFile;
	char *fileName = "input.txt";
	char myString[255] = { "\0" };
	int quantityToken = 0, lenToken = 0;
	char flag = 1;
	myFile = fopen(fileName, "r");

	char *pointerString;
	int len = 0;
	char delim[] = "();:=+-*,/";
	while (!feof(myFile))
	{
		fscanf(myFile, "%s", myString);
		for (int i = 0; i < strlen(myString); i++)
			myString[i] = towupper(myString[i]);
		for (pointerString = myString; *pointerString != '\0'; (len > 0) ? pointerString += len : pointerString++)
		{
			len = strcspn(pointerString, delim);
			if (len == 0) 
			{
				if ((*pointerString == ':') && (*(pointerString + 1) == '='))
				{
					arrayToken[quantityToken].token[0] = *pointerString;
					arrayToken[quantityToken].token[1] = *(pointerString + 1);
					quantityToken++;
					pointerString++;
				}
				else
				{
					arrayToken[quantityToken].token[0] = *pointerString;
					quantityToken++;
				}
				continue;
			}
			for (int i = 0; i < len; i++)
			{
				arrayToken[quantityToken].token[i] = *(pointerString + i);
			}
			quantityToken++;
		}
	}
	for (int i = 0; i < quantityToken; i++)
	{
		arrayToken[i].tokenType = getTokenType(arrayToken[i].token);
		printf("<%s>[%s]\n", arrayToken[i].token, arrayToken[i].tokenType);
	}

	if (E() == 0)
	{
		printf("\nERROR:\n   Token number: [%d]\n   Token name: <%s>\n", next, arrayToken[next-1].token);
	}
	else
		printf("\nCompilation passed successful!\n");
	system("pause");
}

char *getTokenType(char *token)
{
	char flag = 1;
	for (int i = 0; i < 4; i++)
	{
		if (strcmp(token, CONST_TOKENS[i]) == 0)
		{
			flag = 0;
			break;
		}
	} 
	if (flag == 0)
		return token;
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (strcmp(token, SIGN_TOKENS[i]) == 0)
			{
				flag = 0;
				break;
			}
		}
		if (flag == 0)
			return "SIGN";
		else
			if (strcmp(token, "(") == 0)
				return "OPEN";
			else
				if (strcmp(token, ")") == 0)
					return "CLOSE";
				else
					if (strcmp(token, ";") == 0)
						return "ENDEXPR";
					else
						if (strcmp(token, ",") == 0)
							return "COMMA";
						else
							if (strcmp(token, ":=") == 0)
								return "ASSIGN";
							else
								if (strcmp(token, ":") == 0)
									return "COLON";
		else
		{
			for (int i = 0; i < 10; i++)
			{
				if (strncmp(token, NUMBER_TOKENS[i], 1) == 0)
				{
					flag = 0;
					break;
				}
			}
			if (flag == 0)
				return "CONST";
			else
				return "VAR";
		}
	}
}

int term(char *tok)	
{
	return !strcmp(arrayToken[next++].tokenType, tok);
}

int CONST()
{
	return term("CONST");
}

int VARIABLE()
{
	int save = next;
	return (next = save, term("VAR")) 
		|| (next = save, term("CONST"));
}

int EXPR()
{
	int save = next;
	return (next = save, term("OPEN") && EXPR() && term("CLOSE") && term("SIGN") && EXPR())
		|| (next = save, term("OPEN") && EXPR() && term("CLOSE"))
		|| (next = save, VARIABLE() && term("SIGN") && EXPR())
		|| (next = save, term("SIGN") && VARIABLE() && term("SIGN") && EXPR())
		|| (next = save, term("SIGN") && VARIABLE())
		|| (next = save, VARIABLE());
}

int E2()
{
	int save = next;
	return term("CASE") && EXPR() && term("OF");
}

int E1()
{
	return term("CASE") && term("OPEN") && EXPR() && term("CLOSE") && term("OF");
}

int SELECTION()
{
	int save = next;
	return (next = save, term("CONST") && term("COMMA") && SELECTION()) 
		|| (next = save, term("CONST") && term("COLON"));
}

int BODY2()
{
	int save = next;
	return (next = save, term("ELSE") && term("VAR") && term("ASSIGN") && EXPR() && term("ENDEXPR"));
}

int BODY1()
{
	int save = next;
	return (next = save, SELECTION() && term("VAR") && term("ASSIGN") && EXPR() && term("ENDEXPR") && BODY1())
		|| (next = save, SELECTION() && term("VAR") && term("ASSIGN") && EXPR() && term("ENDEXPR"));
}

int BODY()
{
	int save = next;
	return (next = save, BODY1() && BODY2()) || (next = save, BODY1());
	/*return (next = save, SELECTION() && term("VAR") && term("ASSIGN") && EXPR() && term("ENDEXPR") && BODY()) 
		|| (next = save, SELECTION() && term("VAR") && term("ASSIGN") && EXPR() && term("ENDEXPR")) 
		|| (next = save, term("ELSE") && term("VAR") && term("ASSIGN") && EXPR() && term("ENDEXPR"));*/
}

int E()
{
	int save = next;
	return (next = save, E1() && BODY() && term("END") && term("ENDEXPR"))
		|| (next = save, E2() && BODY() && term("END") && term("ENDEXPR"));
}