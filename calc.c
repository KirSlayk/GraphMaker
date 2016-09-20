#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>



#define PI       3.14159265358979323846
#define E        2.71828182845904523536


const char* BIN_OPERATION = "!^*/+-";
const char* ALL_OPERATION = "()!^*/-+";

const double EPS = 1e-12; 

 
void printArray(const char* stringArray) {
	int i;
	for (i = 0; stringArray[i]; ++i){ 
		printf("%c", stringArray[i]);
	}
	printf("=");
	
}

void freeArray(char** a) {
	int i;
	for (i = 0; a[i]; ++i) {
		free(a[i]);
	}
	free(a);
}


int isdouble(char c) {
	return isdigit(c) || c == '.' || c==',';
}


char** lexicalAnalyzer(const char* expr, char** error) {
	const int MAX_LEX_SIZE = 20;
	char** result;
	int ind;
	int lexI = 0;
	int lexPos = 0;
	result = (char**)calloc(strlen(expr)*2 + 1, sizeof(char*));
	result[0] = (char*)malloc(MAX_LEX_SIZE);
	*error = 0;
	for (ind = 0; expr[ind] && !*error; ++ind) {
		char c = expr[ind];
		if (!isspace(c)){
			if (isdouble(c) && (lexPos > 0 && !isdouble(result[lexI][lexPos-1])) 
				|| isalpha(c) && (lexPos > 0 && !isalpha(result[lexI][lexPos-1]))
				|| lexPos>0 && strchr(ALL_OPERATION, c)) {
					result[lexI][lexPos] = 0;
					++lexI;
					lexPos = 0;
					result[lexI] = (char*)malloc(MAX_LEX_SIZE);
			}
			if (c==',')
				c='.';
		
 			if (c=='-' && (lexI == 0 || result[lexI - 1][0]=='(')){
				result[lexI][0]='0';
				result[lexI][1] = 0;
				++lexI;
				result[lexI] = (char*)malloc(MAX_LEX_SIZE);
			}
			
		
			opndbkt(expr, ind, error);
			result[lexI][lexPos++] = c; //expr[i]
			if (lexPos == MAX_LEX_SIZE - 1) {
			*error = "Too long lexeme";
			}
		}
	}
	result[lexI][lexPos] = 0;
	result[lexI+1] = NULL;
	return result;
}

int lessPriority(char op1, char op2) {
	const char* op1Ind = strchr(BIN_OPERATION, op1);
	const char* op2Ind = strchr(BIN_OPERATION, op2);
	if (op1=='^' && op2=='^' )
		return 0;
	return op2 != '(' && (op1Ind >= op2Ind ||  (op2=='/' && op1=='*') || (op2=='-' && op1=='+'));
}

int NegativeRoot (int minus, double opnd2)
{
	double opnd = 0;
	if (minus && opnd2 < 1){
		opnd = 1/opnd2;
		if (!((int) opnd & 1))
			return 0;
	}
	return 1;
}


double applyOper(char op, int minus, double opnd1, double opnd2, char** error) {
	 double result = 0;
     switch (op) {
		case '^': {if (NegativeRoot(minus, opnd2)) 
					  result = pow(opnd1, opnd2); 
				  else *error = "Impossible: root even power of a negative number "; break;}
		case '/':
			if (fabs(opnd2) < EPS)
				*error = "Division by zero";
			else 
				result = opnd1 / opnd2; 
			break;
		case '*': result = opnd1 * opnd2; break;
		case '-': result = opnd1 - opnd2; break;
		case '+': result = opnd1 + opnd2; break;
		
     }
     return result;

}

double fact(double operand)
{
	double val = 0;
	if (operand <= 1)
		return 1;
	val = fact(operand - 1)*operand;
	return val;
}



double applyFunc(char* op, double opnd, const char** error) {
	double result=0;
	if (!_stricmp("SIN", op)) {
		result = sin(opnd);
	} else if (!_stricmp("COS", op)) {
		result = cos(opnd);
	} else if (!_stricmp("TG", op)) {
		if (fabs(cos(opnd)) < EPS)
			*error = "Error evaluating TG. Argument == PI/2 + PI*n";
		else 
			result = tan(opnd);
	}
	  else if (!_stricmp("SQRT", op)) { 
		  if (opnd<0)
			  *error = "Under the root of a negative number";
		  else 
			result = sqrt(opnd);
	  }
	  else if(!_stricmp("LN", op)){
		  if(opnd<=0)
			  *error = "The negative logarithm is not extracted";
		  else
			  result = log(opnd);
	  }
	  else if(!_stricmp("abs", op)){
		  result = fabs(opnd);
	  }
	  else if(!_stricmp("CTG", op)){
		  if (fabs(sin(opnd)) < EPS)
			*error = "Error evaluating CTG. Argument == PI + PI*n";
		else
			result = 1/tan(opnd);
	  }
	  else if(!_stricmp("LG", op)){
		  if(opnd <= 0)
			  *error = "The negative logarithm is not extracted";
		  else
			  result = log10f(opnd);
	  }
	  else if( !_stricmp("!", op) ){
		  if(opnd < 0)
			  *error = "Impossible to calculate the factorial of a negative number";
		  if( opnd >= 0) {
			  double ContinOpnd = (int) ceil (opnd);
			  if ((opnd - ContinOpnd))
				  *error = "Impossible to calculate the factorial of a non-integer number";
		  }
		  if(!*error) 
			  result = fact(opnd);
	  }
	return result;
}

char* INCORRECT_OPERATOR = "Incorrect operator";

int applyOnTop(char** op, int* opTop, double* opnd, int* opndTop, char** error) {
	char* curOp = op[--(*opTop)];
	int minus = 0;
	double opnd1;
	double opnd2;
	double curOpnd;
	if (opTop && !_stricmp(*op, "-"))
		minus = 1;
	if (!*opndTop) {
		*error = INCORRECT_OPERATOR;
		return 0;
	}
	opnd1 = opnd[--(*opndTop)];
	if (!isFunction(curOp)) {
		if (!*opndTop) {
			*error = INCORRECT_OPERATOR;
			return 0;
		}
		opnd2 = opnd[--(*opndTop)];
		curOpnd = applyOper(curOp[0], minus, opnd2, opnd1, error);
	} else {
		curOpnd = applyFunc(curOp, opnd1, error);
	}
	opnd[(*opndTop)++] = curOpnd;
	return 1;
}

double constantValue(const char* s) {
	if (!_stricmp(s, "PI")) {
		return PI;
	} else if (!_stricmp(s, "E")) {
		return E;
	} 
	return 0;
}

int findString(const char* s, const char** a) {
	int i;
	for (i = 0; a[i]; ++i) 
		if (!_stricmp(s, a[i])) {
			return 1;
		}
	return 0;
}

int isFunction(const char* s) {
	const char* FUNC_NAMES[] = {"SIN", "COS", "TG", "LN", "ABS", "CTG", "LG", "!", "SQRT", 0};
	return findString(s, FUNC_NAMES);
}

double convertToDouble(const char* s, char** error) {
	char* endp;
	const char* realEndP = s;
	double d = strtod(s, &endp);
	while (*realEndP) 
		++realEndP;
	if (endp != realEndP) {
		*error = "Incorrect symbols in double constant";
	} else if (errno == ERANGE) {
		*error = "Too big/little double constant value";
	}
	return d;
}



void BorderAndStep (double* bordL, double* bordR, double* Step)
{
	double Left = 0, Right = 0, step=0;
	printf("Enter the data: left, right borders and step\n");
	scanf_s("%lf", &Left);
	scanf_s("%lf", &Right);
	scanf_s("%lf", &step);
	*bordL = Left;
	*bordR = Right;
	*Step = step;
}

double calculate(const char* expr, char** error, double x) {
	int ind;
	int expLen = strlen(expr)*2; 
	char** lex = lexicalAnalyzer(expr, error);
	double* opnd = (double*)calloc(expLen, sizeof(double));
	int opndTop = 0;
	char** op = (char**)calloc(expLen, sizeof(char*));
	int opTop = 0;
	/*int YSquared = 0;
	double CoefOfY = 0;
	double ValOfTwoY = 0;
	double NextAfterAfterY = 0;
	double ValNextAfterY = 0;
	*/
	double result;

	if (!correct(lex, error)) {
		return 0;
	}

	
	printArray(expr);
	
	for (ind = 0; lex[ind] && !*error; ++ind) {
		char firstChar = lex[ind][0];

		if (isdigit(firstChar)) {
			opnd[opndTop++] = convertToDouble(lex[ind], error);
		} else if (isalpha(firstChar) || firstChar == '!') {
			double c = constantValue(lex[ind]);
			if (c) {
				opnd[opndTop++] = c;
			} else {
				if (isFunction(lex[ind])) {
					op[opTop++] = lex[ind];
				} else if (!_stricmp(lex[ind], "x")){
					opnd[opndTop++] = x;
				} else {
					*error = "Unknown identifier";
				}
			} 
		} else if (firstChar == '(') {
			op[opTop++] = lex[ind];
		} else if (firstChar == ')') {
			while (opTop > 0 && op[opTop-1][0] != '(' && !*error) {
				applyOnTop(op, &opTop, opnd, &opndTop, error);
			}
			if (opTop == 0) {
				*error = "No corresponding '(' found for ')'";
			}
			opTop--;
		} else {
			char curOp = firstChar;
			while (opTop > 0 && lessPriority(curOp, op[opTop - 1][0]) && !*error) {
				applyOnTop(op, &opTop, opnd, &opndTop, error);
			}
			op[opTop++] = lex[ind];
		}
	}
	while (opTop > 0 && !*error) 
		applyOnTop(op, &opTop, opnd, &opndTop, error);
	if (opndTop != 1 && !*error) {
		*error = "Incorrect expression. Possibly expected operator is not found";
	}
	
	
	
	/*if ( NextAfterAfterY ){ 
		opnd[0] = opnd[0]*CoefOfY;
		opnd[0]+= NextAfterAfterY;
		if ( ValNextAfterY * ValNextAfterY - 4 * opnd[0] < 0 )
			*error = "NegativeSqrt";
		opnd[0] = ( -ValNextAfterY + sqrt( ValNextAfterY * ValNextAfterY - 4 * opnd[0] )) / 2;
	}
	else if (YSquared && CoefOfY){
		if (-opnd[0]*CoefOfY < 0 )
			*error = "NegativeSqrt";
		opnd[0] = sqrt(-opnd[0]*CoefOfY);
	}*/
	
	
	result = opnd[0];
	freeArray(lex);
	free(opnd);
	free(op);
	if (*error)
		printf(" Nothing\t");
	return result;
}



int correct(char** lex, char** error){
	int ind;
	for (ind = 0; lex[ind] && !*error; ++ind) {
		if (isFunction(lex[ind])) {
			if (!(lex[ind+1] && lex[ind+1][0] == '(')) {
				*error = "'(' is expected after function name";
				return 0;
			}
		}
	}
	return 1;	
}

void chart(const char* expr, char** error, double x1, double x2, double step) {
	double x;
	double ans = 0;
	for (x = x1; x < x2; x+= step) {
		ans = calculate(expr, error, x);
		if (!*error)
			printf("%lf\t", ans);
		*error = NULL;
	}
}



void printCalculate(char* expr) {
	char* error = 0;
	double x = 0;
	char** lex = lexicalAnalyzer(expr, &error);
	if (findString("x", lex)) {
		double x1, x2, step;
		BorderAndStep(&x1, &x2, &step);
		chart(expr, &error, x1, x2, step);
		x = getchar();
	} else {
		double result = calculate(expr, &error, x);
		if (error) {
			printf("ERROR: %s\n", error);
		} else {
			printf("%f\n", result);
		}
	}
	freeArray(lex);
}


void ensureCapacity(char** s, int* size, int newSize) {
	if (newSize > *size) {
		*s = (char*)realloc(*s, newSize);
		*size = newSize;
	}
}

int opndbkt(const char* line, int lineI, char** error){
	if (lineI>1 && line[lineI-1]=='(' && isdigit(line[lineI-2]) || line[lineI-1]==')' && isdigit(line[lineI] )){
		*error="No operation is necessary";
		return 0;

	}
	return 1;
}


int repeat()
{
	printf("Do you want repeat? yes/no\n");
	do{
		char answer [100] = {0};
		gets (answer);
		if (!_stricmp(answer, "yes")){
			answer[4] = 0;  
			return 1;
		}
		else if (!_stricmp(answer, "no")){
			answer [3] = 0;
			return 0;
		}
		printf("ERROR: Not correct query operations. Please retry\n");
		printf("\n");
		
	}while (1);
}


/*

int main(int argc, char* argv[])
{
	if (argc > 1) {
		int index;
		for (index = 1; index < argc; ++index)
			printCalculate(argv[index]);
	} 
	else {
		int size = 1;
		char *line = (char*)malloc(size);
		int rep=1;
		int lineI;
		int c = 0;
		
		
		while (c != EOF) {
			lineI = 0;
			while (1) {
				
				c = getchar();
				if (c == EOF || c == '\n')
					break;
				ensureCapacity(&line, &size, lineI+2);
				line[lineI++] = c;
				
			}
			line[lineI] = 0;
			if (lineI > 0)
				printCalculate(line);
			else printf("ERROR: Not correct\n");
			if (!repeat())
				goto end;
				
		}
		
		free(line);
	}
	end:
	return 0;  
}
 
 */


/*if ( firstChar == 'y'){
				int j = ind;
				opnd[opndTop++] = 0;
				YSquared = 1;
				if ( ind && lex[ind - 1][0] == '(' ){
					char NextAfterY = lex[ind + 1][0];
					NextAfterAfterY = atof( &lex[ind + 2][0] );
					if ( NextAfterY == '-' )
						NextAfterAfterY = -NextAfterAfterY;
					ValNextAfterY = NextAfterAfterY * 2;				
					NextAfterAfterY = pow( NextAfterAfterY, 2 );
					lex[ind + 2][0] = '0'; 
				}
				for ( ; lex[j]; j++ ){
					if ( lex[j][0] == '/')
						CoefOfY = convertToDouble(lex[j+1], error);
					if ( CoefOfY )
						break;
				}
				if (ind && lex[ind-1][0] == '-' )
					CoefOfY = -CoefOfY;
				else if (ind-1 && ind!=0 && lex[ind-2][0] == '-')
					CoefOfY = -CoefOfY;
				if (!CoefOfY)
					CoefOfY = 1;
			}*/