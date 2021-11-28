#include<bits/stdc++.h>
#define N 10000
using namespace std;
int t_i = 0; 	//��¼t�ĸ���
int id_res = 0;	//��¼idջ��top
struct token	//��¼token��ֵ
{
	int typecode;
	char value[N];
}token[N];
int token_i ;	//��¼token�������ж��ٸ�Ԫ������
struct Symbol_table		//���ű��洢���ŵ�����
{
	char lname[20];		//��������
	char lcode[10];		//��������
	int address;		//���ŵ�ַ
	int value;			//���ŵ�ֵ
	int jug;
}symbol_table[N];
int symbol_i ;	//��¼symbol���ж��ٸ�ֵ
struct Coding_table		//��¼�������ŵı�ֵ
{
	char name[20];
	int code;
}coding_table[29] =
{ {"identifier",1},          //��ʶ��
{"integral_constant",2},    //����
{"bool_constant",3},        //������
{"string_constant",4},      //�ַ���
{"int",5},
{"if",6},
{"else",7},
{"char",8},
{"long",9},
{"short",10},
{"float",11},
{"do",12},
{"switch",13},
{"case",14},
{"default",15},
{"for",16},
{"continue",17},
{"break",18},
{"goto",19},
{",",20},
{";",21},
{"=",22},
{"+",23},
{"-",24},
{"*",25},
{"(",26},
{")",27},
{"{",28},
{"}",29}
};

struct production	//��¼����ʽ��ֵ
{
	int left_code;	//����ʽ���
	char production[N];	//����ʽ���ַ���
}pro[N];
/* =
{{1,"E'-> S'"},
{2,"S'->int ID"},
{3,"S'->ID = S"},
{4,"S->S + A"},
{5,"S->S - A"},
{6,"S->A"},
{7,"A->A * B"},
{8,"A->B"},
{9,"B->( S )"},
{10,"B->ID"},
{11,"B->INT_NUM"}
};*/

struct action_table	//��¼action�������
{
	char action;		//sΪ�ƽ���rΪ��Լ��aΪ����
	int old_state;		//��ǰ״̬
	int new_state;		//��һ��״̬
	char cause[10];		//��Ϊ�ĸ�������ת��
	int PNO;			//����ʽ����
	char left[10];		//����ʽ��
	char right[10];		//����ʽ�Ҳ�
}action[N];

struct State	//��¼action�Լ�goto���ֵ
{
	char action[10];
	char go[10];
}state[N];

struct LRstack	//LR����ջ
{
	int state[N];	//����
	int top;
}LRstack;

struct STR		//���lr����ʱ��ID��ֵ
{
	char alpb[10];
	char address[10];
	int t;				//t�Ĵ����ı��
}state_str[N];

struct ID_REM	//���ڴ洢��ʶ��id��ֵ
{
	char name[20];
}id_rem[N];

struct AST	//����ַ�洢
{
	int t;	//t�����
	char arg_1[N];	//��һ��������
	char arg_2[N];	//�ڶ���������
	int arg_l;		//���Ϊt�ķ�����
	int arg_r;		//�ұ�Ϊt�ķ�����
	char op;		//����
	char ID[N];		//t[n]������ı�ʶ��
}ast[N];

struct ADDRESS	//�м��������ʱ���õļ�¼��ַ
{
	char name[20];
}record_address[N];

char memory[N];

void code_reader();
void Lexcial_analyser();
void Grammar_analyser();
void init_production();
void init_action();
void Generate_IntermediateCode(FILE*fp,int j, int k);
void LR_1();
void WriteToAst();
void WriteToSymbol();

int main()
{
	cout << "==========Start Compiler==========" <<endl;
	cout << "Initing compiler......" << endl;
	init_production();
	init_action();
	cout << "Init success!" << endl;
	cout << "Scanning input code......" << endl;
	code_reader();
	cout << "Code scanning success!" << endl;
	cout << "Starting lexcial analysis......" << endl;
	Lexcial_analyser();
	cout << "Lexcial analysis success!" << endl;
	cout << "Starting syntax analysis by LR(1) alogrithm......" << endl;
	Grammar_analyser();
	cout << "Syntax analysis success!" << endl;
	cout << "AST codes are as follow" << endl;
	WriteToAst();
	cout << "==========Compiling success!==========" << endl;
	return 0;
}
void code_reader()
{
	int i = 0;
	FILE* fp = NULL;
	fp = fopen("prefiles/input_code.txt", "r");
	if ((fp = fopen("prefiles/input_code.txt", "r")) == NULL)
	{
		printf("file cannot open!");
		exit(1);
	}
	//��ʼ���ļ��������ļ�input�����ݴ��memory������
	char str = fgetc(fp);
	while (str != EOF)
	{
		memory[i] = str;
		str = fgetc(fp);
		i++;
	}
	memory[i] = '\0';
	fclose(fp);
}
void Lexcial_analyser()
{
	int symbol_jug = 0;
	int value_i;
	int i = 0;
	//�����Ǵʷ�����������
	while (memory[i] != '\0')
	{
		if (isalpha(memory[i]) != 0)   //������ĸ
		{
			value_i = 0;
			while (isalpha(memory[i]) != 0 || isdigit(memory[i]) != 0)
			{
				token[token_i].value[value_i] = memory[i];
				i++;
				value_i++;
			}
			//�ؼ���
			if (token[token_i].value[0] == 'i' && token[token_i].value[1] == 'n' && token[token_i].value[2] == 't') { token[token_i].typecode = 5; token_i++; }		//int->5
			else if (token[token_i].value[0] == 'i' && token[token_i].value[1] == 'f') { token[token_i].typecode = 6; token_i++; }		//if->6
			else if (token[token_i].value[0] == 'e' && token[token_i].value[1] == 'l' && token[token_i].value[2] == 's' && token[token_i].value[3] == 'e') { token[token_i].typecode = 7; token_i++; }	//else->7
			else if (token[token_i].value[0] == 'c' && token[token_i].value[1] == 'h' && token[token_i].value[2] == 'a' && token[token_i].value[3] == 'r') { token[token_i].typecode = 8; token_i++; }	//char->8
			else if (token[token_i].value[0] == 'l' && token[token_i].value[1] == 'o' && token[token_i].value[2] == 'n' && token[token_i].value[3] == 'g') { token[token_i].typecode = 9; token_i++; }	//long->9
			else if (token[token_i].value[0] == 's' && token[token_i].value[1] == 'h' && token[token_i].value[2] == 'o' && token[token_i].value[3] == 'r' && token[token_i].value[4] == 't') { token[token_i].typecode = 10; token_i++; }	//short->10
			else if (token[token_i].value[0] == 'f' && token[token_i].value[1] == 'l' && token[token_i].value[2] == 'o' && token[token_i].value[3] == 'a' && token[token_i].value[4] == 't') { token[token_i].typecode = 11; token_i++; }	//float->11
			else if (token[token_i].value[0] == 'd' && token[token_i].value[1] == 'o') { token[token_i].typecode = 12; token_i++; }	//do->12
			else if (token[token_i].value[0] == 's' && token[token_i].value[1] == 'w' && token[token_i].value[2] == 'i' && token[token_i].value[3] == 't' && token[token_i].value[4] == 'c' && token[token_i].value[5] == 'h') { token[token_i].typecode = 13; token_i++; }	//switch->13
			else if (token[token_i].value[0] == 'c' && token[token_i].value[1] == 'a' && token[token_i].value[2] == 's' && token[token_i].value[3] == 'e') { token[token_i].typecode = 14; token_i++; }	//case->14
			else if (token[token_i].value[0] == 'd' && token[token_i].value[1] == 'e' && token[token_i].value[2] == 'f' && token[token_i].value[3] == 'a' && token[token_i].value[4] == 'u' && token[token_i].value[5] == 'l' && token[token_i].value[6] == 't') { token[token_i].typecode = 15; token_i++; }	//default->15
			else if (token[token_i].value[0] == 'f' && token[token_i].value[1] == 'o' && token[token_i].value[2] == 'r') { token[token_i].typecode = 16; token_i++; }	//for->16
			else if (token[token_i].value[0] == 'c' && token[token_i].value[1] == 'o' && token[token_i].value[2] == 'n' && token[token_i].value[3] == 't' && token[token_i].value[4] == 'i' && token[token_i].value[5] == 'n' && token[token_i].value[6] == 'u' && token[token_i].value[7] == 'e') { token[token_i].typecode = 17; token_i++; }	//continue->17
			else if (token[token_i].value[0] == 'b' && token[token_i].value[1] == 'r' && token[token_i].value[2] == 'e' && token[token_i].value[3] == 'a' && token[token_i].value[4] == 'k') { token[token_i].typecode = 18; token_i++; }	//break->18
			else if (token[token_i].value[0] == 'g' && token[token_i].value[1] == 'o' && token[token_i].value[2] == 't' && token[token_i].value[3] == 'o') { token[token_i].typecode = 19; token_i++; }	//goto->19
			else
			{
				token[token_i].typecode = 1;
				token_i++;
			}
		}

		else if (isdigit(memory[i]) != 0)	//����
		{
			value_i = 0;
			while (isdigit(memory[i]) != 0)
			{
				token[token_i].value[value_i] = memory[i];
				i++;
				value_i++;
			}
			token[token_i].typecode = 2;
			token_i++;
		}

		else if (memory[i] == '\"')		//�ַ���
		{
			token[token_i].value[0] = '\"';
			value_i = 1;
			i++;
			while (memory[i] != '\"')
			{
				token[token_i].value[value_i] = memory[i];
				i++;
				value_i++;
			}
			token[token_i].value[value_i] = '\"';
			token[token_i].typecode = 4;
			token_i++;
			i++;
		}

		else if (isspace(memory[i]) != 0)	//�ո�����
		{
			i++;
		}

		else	//���� 
		{
			if (memory[i] == ',') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 20; i++; token_i++; }
			else if (memory[i] == ';') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 21; i++; token_i++; }
			else if (memory[i] == '=') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 22; i++; token_i++; }
			else if (memory[i] == '+') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 23; i++; token_i++; }
			else if (memory[i] == '-') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 24; i++; token_i++; }
			else if (memory[i] == '*') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 25; i++; token_i++; }
			else if (memory[i] == '(') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 26; i++; token_i++; }
			else if (memory[i] == ')') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 27; i++; token_i++; }
			else if (memory[i] == '{') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 28; i++; token_i++; }
			else if (memory[i] == '}') { token[token_i].value[0] = memory[i]; token[token_i].typecode = 29; i++; token_i++; }
		}
	}
	//fclose(fp);

	//�ѵõ�������symbol������ű���
	for (int i = 0; i < token_i; i++)
	{
		if (token[i].typecode == 1)
		{
			for (int j = 0; j < i; j++)
			{
				if (!strcmp(symbol_table[j].lname, token[i].value))
				{
					symbol_jug = 1;
				}
			}
			if (symbol_jug == 0)		//���뵽���ű���
			{
				strcpy(symbol_table[symbol_i].lname, token[i].value);
				symbol_table[symbol_i].jug = 1;
				symbol_i++;
				symbol_jug = 0;
			}
		}
	}

	//���token��
	FILE* fpwrite = fopen("outputfiles/token.txt", "w");
	if (fpwrite == NULL)
	{
		printf("failed to write file!");
	}
	for (i = 0; i < token_i; i++)
	{
		fprintf(fpwrite, "(%d,%s)\n", token[i].typecode, token[i].value);
	}
	fclose(fpwrite);
}
void init_production()
{
	char ch = '\0';
	FILE* fp1 = NULL;
	//fp1 = fopen("prefiles/production.txt", "r");
	if ((fp1 = fopen("prefiles/production.txt", "r")) == NULL)
	{
		printf("file production cannot open!");
		exit(1);
	}
	int pro_i;
	for (pro_i = 1; ch != EOF; pro_i++)
	{
		int j = 0;
		pro[pro_i].left_code = pro_i;
		ch = fgetc(fp1);
		while (ch != '\n' && ch != EOF)
		{
			pro[pro_i].production[j++] = ch;
			ch = fgetc(fp1);
		}
	}
	fclose(fp1);
}
void init_action()
{
	int i = 0;
	//���Ƚ���lr��Ķ�ȡ
	FILE* fp = NULL;
	fp = fopen("prefiles/LR1.csv", "r");
	if ((fp = fopen("prefiles/LR1.csv", "r")) == NULL)
	{
		printf("file LR1.csv cannot open!");
		exit(1);
	}
	char ch = fgetc(fp);
	while (ch != '\n')
		ch = fgetc(fp);
	ch = fgetc(fp);
	int count = 0;
	int rem_count = 0;
	int jug = 0;
	while (ch != '\n')	//һ��һ�ж�ȡlr��
	{
		if (ch == ',')
		{
			count++;
			i = 0;
			ch = fgetc(fp);
		}
		else if (ch == '$')
		{
			state[count].action[0] = '$';
			jug = 1;
			ch = fgetc(fp);
			rem_count = count;
			count = 0;
		}
		else if (jug == 0)		//action��
		{
			while (ch != ',')
			{
				state[count].action[i++] = ch;
				ch = fgetc(fp);
			}
		}
		else if (jug == 1)		//goto��
		{
			while (ch != ',' && ch != '\n')
			{
				state[count].go[i++] = ch;
				ch = fgetc(fp);
			}
		}
	}
	int goto_count = count;
	int act_i = 0;
	int old_state;
	int num[2];
	int j = 0;
	while (ch != EOF)
	{
		ch = fgetc(fp);
		j = 0;
		num[0] = 0;
		num[1] = -1;
		if (ch == EOF)
			break;
		while (ch != ',')
		{
			num[j] = ch - '0';
			j++;
			ch = fgetc(fp);
		}
		if (num[1] == -1)
			old_state = num[0];
		else if (num[1] != -1)
			old_state = num[0] * 10 + num[1];
		count = 0;
		while (ch != '\n' && ch != EOF)
		{
			if (ch == ',')
			{
				count++;
				ch = fgetc(fp);
			}
			else
			{
				if (ch == 's')		//����shift
				{
					action[act_i].old_state = old_state;
					action[act_i].action = ch;
					while (ch != ' ')
					{
						ch = fgetc(fp);
					}
					ch = fgetc(fp);
					j = 0;
					num[0] = 0;
					num[1] = -1;
					while (ch != ',')
					{
						num[j] = ch - '0';
						j++;
						ch = fgetc(fp);
					}
					if (num[1] == -1)
						i = num[0];
					else if (num[1] != -1)
						i = num[0] * 10 + num[1];
					action[act_i].new_state = i;
					i = 0;
					while (state[count].action[i] != '\0')
					{
						action[act_i].cause[i] = state[count].action[i];
						i++;
					}
					action[act_i++].PNO = 0;
					count++;
					ch = fgetc(fp);
				}
				else if (ch == 'r')	//����reduce
				{
					action[act_i].old_state = old_state;
					action[act_i].action = ch;
					while (ch != ' ')
					{
						ch = fgetc(fp);
					}
					ch = fgetc(fp);
					j = 0;
					num[0] = 0;
					num[1] = -1;
					while (ch != ',')
					{
						num[j] = ch - '0';
						j++;
						ch = fgetc(fp);
					}
					if (num[1] == -1)
						i = num[0];
					else if (num[1] != -1)
						i = num[0] * 10 + num[1];
					action[act_i].PNO = i;
					action[act_i].new_state = -1;
					i = 0;
					while (pro[action[act_i].PNO].production[i] != '-' && pro[action[act_i].PNO].production[i] != ' ')
					{
						action[act_i].left[i] = pro[action[act_i].PNO].production[i];
						i++;
					}
					j = 0;
					while (pro[action[act_i].PNO].production[i] == ' ' || pro[action[act_i].PNO].production[i] == '-' || pro[action[act_i].PNO].production[i] == '>')
						i++;
					while (pro[action[act_i].PNO].production[i] != '\0')
					{
						action[act_i].right[j] = pro[action[act_i].PNO].production[i];
						i++;
						j++;
					}
					i = 0;
					while (state[count].action[i] != '\0')
					{
						action[act_i].cause[i] = state[count].action[i];
						i++;
					}
					act_i++;
					count++;
					ch = fgetc(fp);
				}
				else if (ch == 'a')	//����accept
				{
					action[act_i].action = ch;
					while (ch != ',')
					{
						ch = fgetc(fp);
					}
					action[act_i].old_state = old_state;
					action[act_i].new_state = -2;
					i = 0;
					while (state[count].action[i] != '\0')
					{
						action[act_i].cause[i] = state[count].action[i];
						i++;
					}
					count++;
					act_i++;
				}
				else if (isdigit(ch))
				{
					j = 0;
					num[0] = 0;
					num[1] = -1;
					while (ch != ',' && ch != '\n')
					{
						num[j] = ch - '0';
						j++;
						ch = fgetc(fp);
					}
					if (num[1] == -1)
						i = num[0];
					else if (num[1] != -1)
						i = num[0] * 10 + num[1];
					action[act_i].old_state = old_state;
					action[act_i].new_state = i;
					i = 0;
					while (state[count - rem_count].go[i] != '\0')
					{
						action[act_i].cause[i] = state[count - rem_count].go[i];
						i++;
					}
					count++;
					action[act_i++].PNO = -1;
					if (ch != '\n')
						ch = fgetc(fp);
				}
				else//����lr����
				{
					printf("lr error!");
					exit(0);
				}
			}
		}
	}
}
void Generate_IntermediateCode(FILE *fp,int j,int k)
{
	//��������﷨�Ƶ������Լ��м�������ɻ��ڣ�pno�ǲ���ʽ�ı��
	if (action[j].PNO == 3) // ID = S
	{
		ast[t_i].t = t_i;
		strcpy(ast[t_i].ID, state_str[LRstack.top].address);
		t_i++;
		id_res++;
	}
	else if (action[j].PNO == 4)	//S ->S+A
	{
		ast[t_i].t = t_i;
		if (state_str[LRstack.top].t != 0)
		{
			ast[t_i].arg_l = state_str[LRstack.top].t;
			state_str[LRstack.top].t = t_i;
		}
		else
		{
			strcpy(ast[t_i].arg_1, state_str[LRstack.top].address);
			if (t_i == 0)
				state_str[LRstack.top].t = -1;
			else
				state_str[LRstack.top].t = t_i;
		}
		ast[t_i].op = '+';
		if (state_str[LRstack.top + 2].t != 0)
		{
			ast[t_i].arg_r = state_str[LRstack.top + 2].t;
			state_str[LRstack.top + 2].t = 0;
		}
		else
			strcpy(ast[t_i].arg_2, state_str[LRstack.top + 2].address);
		t_i++;
	}
	else if (action[j].PNO == 5)	//S ->S-A
	{
		ast[t_i].t = t_i;
		if (state_str[LRstack.top].t != 0)
		{
			ast[t_i].arg_l = state_str[LRstack.top].t;
			state_str[LRstack.top].t = t_i;
		}
		else
		{
			strcpy(ast[t_i].arg_1, state_str[LRstack.top].address);
			if (t_i == 0)
				state_str[LRstack.top].t = -1;
			else
				state_str[LRstack.top].t = t_i;
		}
		ast[t_i].op = '-';
		if (state_str[LRstack.top + 2].t != 0)
		{
			ast[t_i].arg_r = state_str[LRstack.top + 2].t;
			state_str[LRstack.top + 2].t = 0;
		}
		else
			strcpy(ast[t_i].arg_2, state_str[LRstack.top + 2].address);
		t_i++;
	}
	else if (action[j].PNO == 7)	//A ->A*B
	{
		ast[t_i].t = t_i;
		if (state_str[LRstack.top].t != 0)
		{
			ast[t_i].arg_l = state_str[LRstack.top].t;
			state_str[LRstack.top].t = t_i;
		}
		else
		{
			strcpy(ast[t_i].arg_1, state_str[LRstack.top].address);
			if (t_i == 0)
				state_str[LRstack.top].t = -1;
			else
				state_str[LRstack.top].t = t_i;
		}
		ast[t_i].op = '*';
		if (state_str[LRstack.top + 2].t != 0)
		{
			ast[t_i].arg_r = state_str[LRstack.top + 2].t;
			state_str[LRstack.top + 2].t = 0;
		}
		else
			strcpy(ast[t_i].arg_2, state_str[LRstack.top + 2].address);
		t_i++;
	}
	else if (action[j].PNO == 9)	//B->(S)
	{
		state_str[LRstack.top].t = state_str[LRstack.top + 1].t;
		state_str[LRstack.top + 1].t = 0;
	}
	else if (action[j].PNO == 11)	//B->INT_NUM
	{
		ast[t_i].t = t_i;
		ast[t_i].arg_r = atoi(state_str[LRstack.top].address);
	}
	strcpy(state_str[LRstack.top].alpb, action[j].left);
	LRstack.top++;
	LRstack.state[LRstack.top] = action[k].new_state;
	fprintf(fp, "%s->%s\n", action[j].left, action[j].right);
}
void LR_1()
{
	int i = 0;
	int count = 1;
	char str_rem[10];
	char sym_rem[10];
	memset(str_rem, 0, sizeof(str_rem));
	memset(sym_rem, 0, sizeof(sym_rem));
	int z;
	int address = -4;
	int id_i = 0;	//��¼һ��������id�ĸ���
	FILE* fp = fopen("outputfiles/syntax_analysis.txt", "w");
	if (fp == NULL)
		printf("failed to write syntax_analysis!");
	while (i < token_i)
	{
		LRstack.top = 0;
		LRstack.state[LRstack.top] = 0;
		int j = 0;
		for (z = 0; state_str[z].address != "\0" && z < 100; z++)	//��ʼ��
			strcpy(state_str[z].address, "\0");
		while (action[j].action != 'a')
		{
			if (token[i].typecode == 5)	//int��
			{
				strcpy(str_rem, token[i].value);
				strcpy(sym_rem, token[i + 1].value);
				for (z = 0; symbol_table[z].jug == 1; z++)
				{
					if (!strcmp(sym_rem, symbol_table[z].lname))
					{
						address = address + sizeof(int);
						symbol_table[z].address = address;
						strcpy(symbol_table[z].lcode, token[i].value);
						break;
					}
				}
			}
			else if (token[i].typecode > 21 && token[i].typecode < 28)		//����
				strcpy(str_rem, token[i].value);
			else if (token[i].typecode == 1)	//��ʶ��
			{
				str_rem[0] = 'I';
				str_rem[1] = 'D';
				str_rem[2] = '\0';
				strcpy(id_rem[id_i].name, token[i].value);
				strcpy(state_str[LRstack.top].address, token[i].value);
				id_i++;
				if (token[i + 1].typecode == 22 && token[i + 2].typecode == 2)		//��ʶ����ֵ��䣬��¼��valueֵ
				{
					strcpy(sym_rem, token[i].value);
					for (z = 0; symbol_table[z].jug == 1; z++)
					{
						if (!strcmp(sym_rem, symbol_table[z].lname))
						{
							symbol_table[z].value = atoi(token[i + 2].value);
							break;
						}
					}
				}
			}
			else if (token[i].typecode == 2)	//����
			{
				str_rem[0] = 'I';
				str_rem[1] = 'N';
				str_rem[2] = 'T';
				str_rem[3] = '_';
				str_rem[4] = 'N';
				str_rem[5] = 'U';
				str_rem[6] = 'M';
				str_rem[7] = '\0';
				strcpy(state_str[LRstack.top].address, token[i].value);
			}
			else if (token[i].typecode == 21)	//�ֺ�
			{
				str_rem[0] = '$';
				str_rem[1] = '\0';
			}
			j = 0;
			while (action[j].old_state != LRstack.state[LRstack.top] || strcmp(str_rem, action[j].cause))
			{
				j++;
				if (j > 1000)
				{
					fprintf(fp, "cant receive a sentense!!!");
					printf("cant receive a sentense!!!");
					exit(1);
				}
			}
			if (action[j].action == 's')		//�ƽ�
			{
				strcpy(state_str[LRstack.top].alpb, str_rem);
				LRstack.top++;
				LRstack.state[LRstack.top] = action[j].new_state;
				i++;
			}
			else if (action[j].action == 'r')	//��Լ
			{
				int k = 0;
				count = 1;
				for (k = 0; action[j].right[k] != '\0'; k++)
				{
					if (action[j].right[k] == '=' || action[j].right[k] == '+' || action[j].right[k] == '-' || action[j].right[k] == '*' || action[j].right[k] == '(' || action[j].right[k] == ' ')
						count = count + 2;
				}
				LRstack.top = LRstack.top - count;
				k = 0;
				while (action[k].old_state != LRstack.state[LRstack.top] || strcmp(action[k].cause, action[j].left))
				{
					k++;
					if (k > 1000)
					{
						fprintf(fp, "cant receive a sentense!!!");
						printf("cant receive a sentense!!!");
						exit(1);
					}
				}
				Generate_IntermediateCode(fp, j, k);
			}
			else if (action[j].action == 'a')	//����Ϊaccept�����һ�����ӡ�
			{
				i++;
				fprintf(fp, "accept a sentense!\n");
				id_i = 0;
				id_res = 0;
				int p;
				for (p = 0; p < 100; p++)
					strcpy(id_rem[p].name, "\0");
			}
		}
	}
}
void Grammar_analyser()
{
	LR_1();
	WriteToSymbol();
}
void WriteToAst()
{
	FILE* fp= fopen("outputfiles/ast.txt", "w");
	if (fp == NULL)
	{
		printf("failed to write ast!");
	}
	int num_jug[32] = { 0 };
	int ram[32];
	for (int i = 0; i < 32; i++)
	{
		ram[i] = i;
	}
	int j = 0;
	for (int i = 0; i < t_i; i++)
	{
		if (ast[i].op == 0 && !strcmp(ast[i].arg_1, "\0") && !strcmp(ast[i].arg_2, "\0") && ast[i].arg_l == 0 && ast[i].arg_r != 0)	//��ʾID = INT_NUM
		{
			fprintf(fp, "%s = %d\n", ast[i].ID, ast[i].arg_r);
			fprintf(fp, "t%d = %s\n", i, ast[i].ID);
			printf("MOV\t%s,%d\n", ast[i].ID, ast[i].arg_r);
			num_jug[ast[i].t] = j;
			for (int k = ast[i].t; k < 32; k++)
				num_jug[k] = j;
			j++;
		}
		else
		{
			if (ast[i].op != 0 && ast[i].arg_l == 0 && ast[i].arg_r == 0)	//t = a op b
			{
				fprintf(fp, "t%d = %s %c %s\n", i, ast[i].arg_1, ast[i].op, ast[i].arg_2);
				printf("MOV \tR%d,%s\n", i - num_jug[i], ast[i].arg_1);
				if (ast[i].op == '+')
					printf("ADD \tR%d,%s\n", i - num_jug[i], ast[i].arg_2);
				else if (ast[i].op == '-')
					printf("SUB \tR%d,%s\n", i - num_jug[i], ast[i].arg_2);
				else if (ast[i].op == '*')
					printf("MUL \tR%d,%s\n", i - num_jug[i], ast[i].arg_2);
				else if (ast[i].op == '/')
					printf("DIV \tR%d,%s\n", i - num_jug[i], ast[i].arg_2);
			}
			else if (ast[i].arg_l != 0 && ast[i].arg_r != 0)	//for t0
			{
				if (ast[i].arg_l == -1)
				{
					fprintf(fp, "t%d = t0 %c t%d\n", i, ast[i].op, ast[i].arg_r);
					if (ast[i].op == '+')
						printf("ADD \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					else if (ast[i].op == '-')
						printf("SUB \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					else if (ast[i].op == '*')
						printf("MUL \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					else if (ast[i].op == '/')
						printf("DIV \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					ram[ast[i].arg_r - num_jug[i]] = 0;
					j++;
					for (int k = ast[i].t; k < 32; k++)
						num_jug[k] = j;
				}
				else if (ast[i].arg_r == -1)
					fprintf(fp, "t%d = t%d %c t0\n", i, ast[i].arg_l, ast[i].op);
				else
				{
					fprintf(fp, "t%d = t%d %c t%d\n", i, ast[i].arg_l, ast[i].op, ast[i].arg_r);
					if (ast[i].op == '+')
						printf("ADD \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					else if (ast[i].op == '-')
						printf("SUB \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					else if (ast[i].op == '*')
						printf("MUL \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					else if (ast[i].op == '/')
						printf("DIV \tR%d,R%d\n", ram[ast[i].arg_l - num_jug[i]], ram[ast[i].arg_r - num_jug[i]]);
					ram[ast[i].arg_r - num_jug[i]] = ast[i].arg_l - num_jug[i];
					j++;
					for (int k = ast[i].t; k < 32; k++)
						num_jug[k] = j;
				}
			}
			else if (ast[i].arg_l == 0 && ast[i].arg_r != 0)	//t = a op t
			{
				fprintf(fp, "t%d = %s %c t%d\n", i, ast[i].arg_1, ast[i].op, ast[i].arg_r);
				if (ast[i].op == '+')
					printf("ADD \tR%d,R%d\n", ast[i].arg_l - num_jug[i], ast[i].arg_r - num_jug[i]);
				else if (ast[i].op == '-')
					printf("SUB \tR%d,R%d\n", ast[i].arg_l - num_jug[i], ast[i].arg_r - num_jug[i]);
				else if (ast[i].op == '*')
					printf("MUL \tR%d,R%d\n", ast[i].arg_l - num_jug[i], ast[i].arg_r - num_jug[i]);
				else if (ast[i].op == '/')
					printf("DIV \tR%d,R%d\n", ast[i].arg_l - num_jug[i], ast[i].arg_r - num_jug[i]);
			}
			else if (ast[i].arg_l != 0 && ast[i].arg_r == 0)	//t = t op a
			{
				fprintf(fp, "t%d = t%d %c %s\n", i, ast[i].arg_l, ast[i].op, ast[i].arg_2);
				if (ast[i].op == '+')
					printf("ADD \tR%d,%s\n", ast[i].arg_l - num_jug[i], ast[i].arg_2);
				else if (ast[i].op == '-')
					printf("SUB \tR%d,%s\n", ast[i].arg_l - num_jug[i], ast[i].arg_2);
				else if (ast[i].op == '*')
					printf("MUL \tR%d,%s\n", ast[i].arg_l - num_jug[i], ast[i].arg_2);
				else if (ast[i].op == '/')
					printf("DIV \tR%d,%s\n", ast[i].arg_l - num_jug[i], ast[i].arg_2);
				j++;
				for (int k = ast[i].t; k < 32; k++)
					num_jug[k] = j;
			}
			if (!strcmp(ast[i].arg_1, "\0") && !strcmp(ast[i].arg_2, "\0") && ast[i].arg_l == 0 && ast[i].arg_r == 0)
			{
				fprintf(fp, "%s = t%d\n", ast[i].ID, i - 1);
				int u = ram[i - 1 - num_jug[i]];
				while (ram[u] != u)
					u = ram[u];
				printf("MOV \t%s,R%d\n", ast[i].ID, u);
			}
		}
	}
	fclose(fp);
}
void WriteToSymbol()
{
	//symbol������
	FILE* fp = fopen("outputfiles/symbol.txt", "w");
	if (fp == NULL)
	{
		printf("failed to write symbol!");
	}
	for (int i = 0; i < token_i; i++)
	{
		if (symbol_table[i].jug == 1)
		{
			fprintf(fp, "(%s,%s,%d)\n", symbol_table[i].lname, symbol_table[i].lcode, symbol_table[i].address);
		}
	}
	fclose(fp);
}