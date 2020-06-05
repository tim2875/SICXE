/*
 * 화일명 : my_assembler_20160312.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "my_assembler_20160312.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}

	
	make_symtab_output("symtab_20160312");
    make_literaltab_output("literaltab_20160312");

	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_20160312") ;
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
        
	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE *file = NULL;
	int errno;
    inst_index = 0;
 
    for (int i = 0; i < MAX_INST; i++) {
        inst_table[i] = malloc(sizeof(inst));
        inst_table[i]->inst_name = malloc(sizeof(char));
        inst_table[i]->inst_type = malloc(sizeof(char));
        inst_table[i]->inst_opcode = malloc(sizeof(char));
        inst_table[i]->inst_operandnum = 0;
    }
  

    file = fopen(inst_file, "rt");
    if (file == NULL) {
        printf("오류\n");
        errno = -1;
    }
    else {
        while (!feof(file)) {
            
            fscanf(file, "%s", inst_table[inst_index]->inst_name);
            fscanf(file, "%s", inst_table[inst_index]->inst_type);
            fscanf(file, "%s", inst_table[inst_index]->inst_opcode);
            fscanf(file, "%d", &(inst_table[inst_index]->inst_operandnum));
            inst_index++;
        }
        errno = 0;
        if(inst_index != 0)
        inst_index--;
    }  

    fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE *file = NULL;
	int errno;
    line_num = 0;
    int k = 0;

    for (int i = 0; i < MAX_LINES; i++)
        input_data[i] = malloc(sizeof(char));

    file = fopen(input_file, "rt");
    if (file == NULL) {
        printf("오류\n");
        errno = -1;
    }
    else {
        while (!feof(file)) {                                   //1개가 추가로 더 저장됨
            fgets(input_data[line_num], 100, file);
            line_num++;
        }
        if (line_num != 0) {
            //line_num--;
            k = line_num;
        }
        for (; k < MAX_LINES; k++) {                            //문장이 끝나면 나머지 input_data 배열을 NULL로 채움
            input_data[k] = NULL;
        }
        errno = 0;
    }
    fclose(file);
 
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다.  
 * 매계 : 파싱을 원하는 문자열  
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str)
{
    int errno;
    int i = 0, count = 0, index = 0;
    char* temp_str, *temp_str2, *temp, *temp_str3, *temp_str4;
    temp_str = malloc(sizeof(char));
    temp_str2 = malloc(sizeof(char));
    temp_str3 = malloc(sizeof(char));
    temp_str4 = malloc(sizeof(char));
    temp = malloc(sizeof(char));
    token_usedline++;

    if (str == NULL) 
        errno = -1;
    else {
        token_table[token_line] = malloc(sizeof(token));
        token_table[token_line]->nixbpe = 0;
        token_table[token_line]->label = malloc(sizeof(char));
        token_table[token_line]->operator = malloc(sizeof(char));
        token_table[token_line]->comment = malloc(sizeof(char));
        for (int k = 0; k < MAX_OPERAND; k++) {
            token_table[token_line]->operand[k] = malloc(sizeof(char));
        }
        
        if (strncmp("\t", str, 1) == 0) {                   //label이 없을 때
            token_table[token_line]->label = NULL;          //label = NULL
           /*------------------------------------------------------------------------------*/
            if(strncmp("\t\t", str, 2)   )
            
            temp_str = strtok(str, "\t\n");                   //operator
            if (temp_str == NULL)
                token_table[token_line]->operator = NULL;
            else
                token_table[token_line]->operator = temp_str;
           /*------------------------------------------------------------------------------*/
            temp_str2 = strtok(NULL, "\n");                 //operator 이후의 전부
            if (temp_str2 == NULL) {
                temp = temp_str2;                           //comment를 NULL로 만들기 위함
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else if (strncmp(temp_str2, "\t", 1) == 0) {       // operand없이 comment만 있을 때 input.txt에서 \t을 두번 씀
                temp_str3 = strtok(temp_str2,"\t\n");
                temp = temp_str3;
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else {              //operand가 있을 때
                temp_str3 = strtok(temp_str2, "\t\n");                  //operand
                temp = strtok(NULL, "\t\n");
                if (temp_str3 == NULL) {
                    for (int k = 0; k < MAX_OPERAND; k++) {
                        token_table[token_line]->operand[k] = NULL;
                    }
                }
                else {
                    int m = 0;
       
                    temp_str4 = strtok(temp_str3, "-,");
                    while (m < MAX_OPERAND) {
                        if (temp_str4 == NULL) {
                            token_table[token_line]->operand[m] = NULL;
                        }
                        else {
                            token_table[token_line]->operand[m] = temp_str4;
                        }
                        temp_str4 = strtok(NULL, "-,");
                        m++;
                    }
                }
            }
      /*------------------------------------------------------------------------*/
           //comment
           temp_str = temp;                                           
            if (temp_str == NULL)
                token_table[token_line]->comment = NULL;
            else
                token_table[token_line]->comment = temp_str;
        }
        else if (strncmp(".", str, 1) == 0) {               //.으로 시작하는 주석
            token_table[token_line]->label = NULL;
            token_table[token_line]->operator = NULL;
            for (int k = 0; k < MAX_OPERAND; k++) {
                token_table[token_line]->operand[k] = NULL;
            }
            token_table[token_line]->comment = strtok(str, "\n");
        }
        else {                                              //label부터 있을 때
            temp_str = strtok(str, "\t");                   //label
            if (temp_str == NULL)
                token_table[token_line]->label = NULL;
            else
                token_table[token_line]->label = temp_str;
           
            /*-----------------------------------------------------------------*/
            //operator
            temp_str = strtok(NULL, "\t\n");                  
            if (temp_str == NULL) 
                token_table[token_line]->operator = NULL;
            else
                token_table[token_line]->operator = temp_str;
            
            /*----------------------------------------------------------------*/
            //operand
            temp_str2 = strtok(NULL, "\n");                      //operator 이후의 전부
            if (temp_str2 == NULL) {
                temp = temp_str2;
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else if (strncmp(temp_str2, "\t", 1) == 0) {       // operand없이 comment만 있을 때 input.txt에서 \t을 두번 씀
                temp_str3 = strtok(temp_str2, "\t\n");
                temp = temp_str3;
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else {
                temp_str3 = strtok(temp_str2, "\t\n");                  
                temp = strtok(NULL, "\t\n");
                if (temp_str3 == NULL) {
                    for (int k = 0; k < MAX_OPERAND; k++) {
                        token_table[token_line]->operand[k] = NULL;
                    }
                }
                else {
                    int n = 0;
                    temp_str4 = strtok(temp_str3, "-,");
                    while (n < MAX_OPERAND) {
                        if (temp_str4 == NULL) {
                            token_table[token_line]->operand[n] = NULL;
                        }
                        else {
                            token_table[token_line]->operand[n] = temp_str4;
                        }
                        temp_str4 = strtok(NULL, "-,\n");
                        n++;
                    }
                }
            }

            temp_str = temp;
            if(temp_str == NULL)
                token_table[token_line]->comment = NULL;
            else
                token_table[token_line]->comment = temp_str;
        }
        errno = 0;
        
    }
    if (token_table[token_line]->operator != NULL) {
        index = search_opcode(token_table[token_line]->operator);
        if (index == -1)
            token_table[token_line]->nixbpe = 0;
        else {
            if (token_table[token_line]->operator != NULL) {
                if(strncmp(inst_table[index]->inst_type, "2", 1) == 0){         //2형식
                    token_table[token_line]->nixbpe = 0;
                }
                else {      //3,4형식
                    if (strncmp(token_table[token_line]->operator, "+", 1) == 0) {  //4형식
                        if (token_table[token_line]->operand[1] != NULL) {
                            if (strcmp(token_table[token_line]->operand[1], "X") == 0) {    //루프 사용 O
                                token_table[token_line]->nixbpe |= 57;
                            }
                        }
                        else {
                            token_table[token_line]->nixbpe |= 49;
                        }
                    }
                    else if (token_table[token_line]->operand[0] != NULL && strncmp(token_table[token_line]->operand[0], "@", 1) == 0) { // 3형식 n=1,i=0
                        token_table[token_line]->nixbpe |= 34;
                    }
                    else if (token_table[token_line]->operand[0] != NULL && strncmp(token_table[token_line]->operand[0], "#", 1) == 0) { // 3형식 n=0,i=1
                        token_table[token_line]->nixbpe |= 16;
                    }
                    else {          //3형식 n,i = 1
                        if (strcmp(token_table[token_line]->operator, "RSUB")== 0 && token_table[token_line]->operand[0] == NULL) {
                            token_table[token_line]->nixbpe |= 48;
                        }
                        else if (token_table[token_line]->operand[1] != NULL) {
                            if (strcmp(token_table[token_line]->operand[1], "X") == 0) {    //루프 사용 O
                                token_table[token_line]->nixbpe |= 58;
                            }
                        }
                        else {
                            token_table[token_line]->nixbpe |= 50;  //110010
                        }
                    }
                }
            }
        }
    }


    return errno;
 }

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)                                        //문자열을 넣으면 inst_table의 몇번째 주소에 속해있는지의 값을 리턴
{
    opcode_index = 0;
    for (int i = 0; i < inst_index; i++) {
        if (strcmp(strtok(str,"+"), inst_table[i]->inst_name) == 0) {
            opcode_index = i;
            break;
        }
    }
    if (opcode_index == 0) {                                        //기계어 코드가 아닐때
        opcode_index = -1;
    }
    return opcode_index;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{  
    int errno;
    sym_csect_count = 1;
    token_usedline = 0;
        for (token_line = 0; token_line < MAX_LINES; token_line++) {
            if(input_data[token_line] != NULL)
                errno = token_parsing(input_data[token_line]);
        }
        make_symtab();

    return errno;
}

void make_symtab(){     //리터럴 테이블과 심볼 테이블을 만드는 함수
    extdef_index = 0; extref_index = 0;
    ltorg_index = 0; sym_index = 0; literal_index = 0;
    
    int k = 0, literal_check = 1, literal_count = 0;
    int index = 0;
    int isright = 1;
    int temp_locctr = 0, temp = 0;
    char* temp_str = malloc(sizeof(char));
    char* temp_str2 = malloc(sizeof(char));

    for (int i = 0; i < token_usedline; i++) {
        pc_addr[i] = -1;
        locctr_addr[i] = -1;
        sym_table[i].addr = -1;
        extdef_table[i] = malloc(sizeof(extdef));
        extref_table[i] = malloc(sizeof(extref));
        for (int a = 0; a < MAX_OPERAND; a++) {
            extdef_table[i]->extdef[a] = malloc(sizeof(char));
            extref_table[i]->extref[a] = malloc(sizeof(char));
            extdef_table[i]->extdef[a] = NULL;
            extref_table[i]->extref[a] = NULL;
        }

        if (token_table[i]->operator != NULL) {
            if (strcmp(token_table[i]->operator, "EXTDEF") == 0) {
                k = 0;
                while (k < 3 || token_table[i]->operand[k] != NULL) {
                    extdef_table[extdef_index]->extdef[k] = token_table[i]->operand[k];
                    k++;
                }
            }
            else if (strcmp(token_table[i]->operator, "EXTREF") == 0) {
                k = 0;
                while (k < 3 || token_table[i]->operand[k] != NULL) {
                    extref_table[extref_index]->extref[k] = token_table[i]->operand[k];
                    k++;
                }
            }

            if (strcmp(token_table[i]->operator, "START") == 0) {
                
                locctr_addr[i] = locctr;
                locctr = atoi(token_table[i]->operand[0]);
                pc_addr[i] = locctr;
            }
            if (strcmp(token_table[i]->operator,"CSECT") == 0) {      //CSECT = control section 새로 시작
                sym_csect_count++;
                locctr_addr[i] = locctr;
                locctr = 0;
                pc_addr[i] = locctr;

                extdef_index++;
                extref_index++;
            }
            if (token_table[i]->label == NULL && strcmp(token_table[i]->operator, "EXTDEF") == 0)
                continue;
            if (token_table[i]->label == NULL && strcmp(token_table[i]->operator, "EXTREF") == 0)
                continue;
            if (token_table[i]->label == NULL && ( strcmp(token_table[i]->operator, "LTORG") == 0 || strcmp(token_table[i]->operator, "END") == 0)) {
                for (int x = literal_count; x < literal_index; x++) {
                    locctr_addr[i] = locctr;
                    temp = literal_table[x].addr;
                    literal_table[x].addr = locctr;
                    locctr += temp; 
                    pc_addr[i] = locctr;
                    literal_count++;
                    if(ltorg_index == 0)
                        ltorg_index = literal_index;
                }
                continue;
            }
          
            if (token_table[i]->label != NULL) {
                strcpy(sym_table[sym_index].symbol, token_table[i]->label);
                sym_table[sym_index].addr = locctr;
                sym_csect[sym_index] = sym_csect_count;
                sym_index++;
            }

            if (token_table[i]->operand[0] != NULL) {
                if (strcmp(token_table[i]->operand[0], "*") == 0) {
                    locctr_addr[i] = locctr;
                }
                else if (strncmp(token_table[i]->operand[0], "=X'", 3) == 0) {
                    strcpy(temp_str2, token_table[i]->operand[0]);
                    temp_str = strtok(temp_str2, "'");
                    temp_str = strtok(NULL, "'");

                    if (literal_index == 0) {
                        strcpy(literal_table[literal_index].literal, temp_str);
                        literal_table[literal_index].addr = strlen(temp_str)/2;
                        literal_index++;
                    }
                    else {
                        for (int p = 0; p < literal_index; p++) {
                            if (strcmp(literal_table[p].literal, temp_str) == 0) {
                                literal_check = 0;
                                break;
                            }
                        }
                        if(literal_check == 1) {
                            strcpy(literal_table[literal_index].literal, temp_str);
                            literal_table[literal_index].addr = strlen(temp_str)/2;
                            literal_index++;
                        }
                    }
                }
                else if (strncmp(token_table[i]->operand[0], "=C'", 3) == 0) {
                    strcpy(temp_str2, token_table[i]->operand[0]);
                    temp_str = strtok(temp_str2, "'");
                    temp_str = strtok(NULL, "'");
                    if (literal_index == 0) {
                        strcpy(literal_table[literal_index].literal, temp_str);
                        literal_table[literal_index].addr = strlen(temp_str);
                        literal_index++;
                    }
                    else {
                        for (int p = 0; p < literal_index; p++) {
                            if (strcmp(literal_table[p].literal, temp_str) == 0) {
                                literal_check = 0;
                                break;
                            }
                        }
                        if (literal_check == 1) {
                            strcpy(literal_table[literal_index].literal, temp_str);
                            literal_table[literal_index].addr = strlen(temp_str);
                            literal_index++;
                        }
                    }
                }
            }

            index = search_opcode(token_table[i]->operator);
            if (strncmp(token_table[i]->operator, "+", 1) == 0) {
                locctr_addr[i] = locctr;
                locctr += 4;
                pc_addr[i] = locctr;
            }
            else if (strncmp(token_table[i]->operator, "=C", 2) == 0) {
                locctr_addr[i] = locctr;
               locctr += 3 * (strlen(token_table[i]->operator) - 4);
                pc_addr[i] = locctr;
            }
            else if (strncmp(token_table[i]->operator, "=X", 2) == 0) {
                locctr_addr[i] = locctr;
                locctr += (strlen(token_table[i]->operator) - 4);
                pc_addr[i] = locctr;
            }
            else if (strncmp(token_table[i]->operator, "BYTE", 4) == 0) {           //operator = RESB
                if (strncmp(token_table[i]->operand[0], "X'", 2) == 0) {
                    locctr_addr[i] = locctr;
                    locctr += (strlen(token_table[i]->operand[0]) - 3) / 2;
                    pc_addr[i] = locctr;
                }
                else if (strncmp(token_table[i]->operand[0], "C'", 2) == 0) {       //operator = RESW
                    locctr_addr[i] = locctr;
                    locctr += 3 * (strlen(token_table[i]->operand[0]) - 4);
                    pc_addr[i] = locctr;
                }
            }
            else if ((token_table[i]->label != NULL && (strncmp(token_table[i]->label, "MAXLEN", 6) == 0)) || (strncmp(token_table[i]->operator, "WORD", 4) == 0)) {
                isright = 1;
                for (int check = 0; check < 3; check++) {
                    if (token_table[i]->operand[check] == NULL && isright == 1) {
                        break;
                    }
                    for (int a = 0; a < 3; a++) {
                        if (token_table[i]->operand[check] != NULL && extref_table[extref_index]->extref[a] != NULL) {
                            if (strcmp(token_table[i]->operand[check], extref_table[extdef_index]->extref[a]) == 0) {
                                isright = 0; 
                                break;
                            }
                        }
                    }
                    if (isright == 0)
                        break;
                }
                if (isright == 0) {
                    locctr_addr[i] = locctr;
                    locctr += 3;
                    pc_addr[i] = locctr;
                }
                else if (isright == 1) {        //symbol을 사용 할 때
                    for (int a = 0; a < 3; a++) {
                        for (int b = 0; b < sym_index - 1; b++) {
                            if (token_table[i]->operand[a] != NULL && sym_table[b].symbol != NULL) {
                                if (strcmp(token_table[i]->operand[a], sym_table[b].symbol) == 0) {
                                    if (a == 0)
                                        temp_locctr = sym_table[b].addr;
                                    else
                                        temp_locctr -= sym_table[b].addr;
                                    break;
                                }
                            }
                            else
                                break;
                        }
                    }
                    temp = locctr;
                    if (sym_index > 0) {
                        sym_table[sym_index - 1].addr = temp_locctr;

                    }
                    locctr = temp;
                }
            }
            else if (strncmp(token_table[i]->operator, "RESW", 4) == 0) {
                locctr_addr[i] = locctr;
                locctr += 3 * atoi(token_table[i]->operand[0]);
                pc_addr[i] = locctr;
            }
            else if (strncmp(token_table[i]->operator, "RESB", 4) == 0) {
                locctr_addr[i] = locctr;
                locctr += atoi(token_table[i]->operand[0]);
                pc_addr[i] = locctr;
            }
            else {
                if (inst_table[index] != NULL && inst_table[index]->inst_type != NULL) {
                    if (strncmp(inst_table[index]->inst_type, "1", 1) == 0) {
                        locctr_addr[i] = locctr;
                        locctr += 1;
                        pc_addr[i] = locctr;

                    }
                    else if (strncmp(inst_table[index]->inst_type, "2", 1) == 0) {
                        locctr_addr[i] = locctr;
                        locctr += 2;
                        pc_addr[i] = locctr;

                    }
                    else if (strncmp(inst_table[index]->inst_type, "3", 1) == 0) {
                        locctr_addr[i] = locctr;
                        locctr += 3;
                        pc_addr[i] = locctr;
                    }
                }
            }
        }
    }
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
    if (file_name == NULL) {
        if (sym_index != 0) {
            for (int i = 0; i < sym_index ; i++) {
                printf(sym_table[i].symbol);
           
                printf("\t");
                printf("%X\n", sym_table[i].addr);
                if (strcmp(sym_table[i].symbol, "MAXLEN") == 0)
                    printf("\n");
            }
        }
        printf("\n");
    }
    else {
        FILE* file = NULL;
        file = fopen(file_name, "wt");
        for (int i = 0; i < sym_index; i++) {
            fprintf(file, sym_table[i].symbol);
            fprintf(file, "\t");
            fprintf(file, "%X\n", sym_table[i].addr);
            if (strcmp(sym_table[i].symbol, "MAXLEN") == 0)
                fprintf(file, "\n");
        }
        fclose(file);
    }
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *file_name)
{
    if (file_name == NULL) {
        if (literal_index != 0) {
            for (int i = 0; i < literal_index; i++) {
                printf(literal_table[i].literal);

                printf("\t");
                printf("%X\n", literal_table[i].addr);
                if (strcmp(literal_table[i].literal, "EOF") == 0)
                    printf("\n");
            }
        }
    }
    else {
        FILE* file = NULL;
        file = fopen(file_name, "wt");
        for (int i = 0; i < sym_index; i++) {
            if (literal_table[i].addr == 0)
                break;
            fprintf(file, literal_table[i].literal);
            fprintf(file, "\t");
            fprintf(file, "%X\n", literal_table[i].addr);
            if (strcmp(literal_table[i].literal, "EOF") == 0)
                fprintf(file, "\n");
            
        }
        fclose(file);
    }
}

int alphabet_to_num(char *str) {
    int num;
    if (strcmp(str, "A") == 0) {
        num = 10;
    }
    else if (strcmp(str, "B") == 0) {
        num = 11;
    }
    else if (strcmp(str, "C") == 0) {
        num = 12;
    }
    else if (strcmp(str, "D") == 0) {
        num = 13;
    }
    else if (strcmp(str, "E") == 0) {
        num = 14;
    }
    else if (strcmp(str, "F") == 0) {
        num = 15;
    }
    else {
        num = atoi(str);
    }
    return num;
}

int make_opcode(int a) {   //a = 배열 번호
    int index;
    int temp;
    int TA = 0, PC = 0, temp_addr = 0;
    int literal_count = 0;
    char* buffer = malloc(sizeof(char));
    char* temp_str = malloc(sizeof(char));
    char* temp_str2 = malloc(sizeof(char));
    opcode_table[a] = malloc(sizeof(opcode));
    opcode_table[a]->opcode = malloc(sizeof(char));

    if (token_table[a]->label == NULL && token_table[a]->operator == NULL) {
        opcode_table[a]->csect = csect_count;
        opcode_table[a]->type = 0;
        opcode_table[a]->opcode = 0;
    }
    else if (token_table[a]->operator != NULL){
        if (strcmp(token_table[a]->operator, "CSECT") == 0)
            csect_count++;          //다음 section
       

        index = search_opcode(token_table[a]->operator);    //opcode 위치

        if (index == -1 && token_table[a]->nixbpe == 0) {    // 아무것도 아닌 경우
            if (token_table[a]->operator != NULL) {
                if (strcmp(token_table[a]->operator, "WORD") == 0) {
                    if (token_table[a]->operand[0] != NULL) {
                        opcode_table[a]->csect = csect_count;
                        for (int b = 0; b < 3; b++) {
                            if (extref_table[csect_count - 1]->extref[b] != NULL) {
                                if (strcmp(token_table[a]->operand[0], extref_table[csect_count - 1]->extref[b]) == 0) {    //외부참조하는 경우

                                    strcpy(opcode_table[a]->opcode, "000000");
                                    opcode_table[a]->type = 3;
                                  
                                }
                            }
                        }
                    }
                }
                else if (strcmp(token_table[a]->operator, "BYTE") == 0) {
                    if (token_table[a]->operand[0] != NULL) {
                        opcode_table[a]->csect = csect_count;
                        buffer = strtok(token_table[a]->operand[0], "'");
                        buffer = strtok(NULL, "'");
                        opcode_table[a]->type = strlen(buffer)/2;
                        strcpy(opcode_table[a]->opcode, buffer);
                       
                    }
                }
                else if (strcmp(token_table[a]->operator, "LTORG") == 0) {              //opcode_table에 넣을 차례;;
                    opcode_table[a]->csect = csect_count;
                    opcode_table[a]->type = 3;
                    for (int i = 0; i < ltorg_index; i++) {
                        for (int j = 0; j < 3; j++) {
                            if (literal_table[i].literal[j] != 0) {
                                temp = literal_table[i].literal[j];
                                sprintf(buffer, "%X", temp);
                                if (j == 0)
                                    strcpy(opcode_table[a]->opcode, buffer);
                                else
                                    strcat(opcode_table[a]->opcode, buffer);
                            }
                            else
                                break;
                        }
                 
                    }
                }
                else if (strcmp(token_table[a]->operator, "END") == 0) {
                    if (ltorg_index <= literal_index) {
                        opcode_table[a]->csect = csect_count;
                        opcode_table[a]->type = 1;
                        for (int i = ltorg_index; i < literal_index; i++) {
                            strcpy(opcode_table[a]->opcode, literal_table[i].literal);
                           
                        }
                    }
                }
                else {
                    opcode_table[a]->opcode = 0;
                    opcode_table[a]->type = 0;
                    opcode_table[a]->csect = csect_count;
                }
            }
           

        }
        else if (strncmp(inst_table[index]->inst_type, "2", 1) == 0 && token_table[a]->nixbpe == 0) {   // 2형식
            opcode_table[a]->type = atoi(inst_table[index]->inst_type);
            opcode_table[a]->csect = csect_count;

            if (token_table[a]->operand[0] != NULL) {               //operand[0]가 0이 아닐 때
                strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                if (strcmp(token_table[a]->operand[0], "X") == 0) { // X레지스터 = 1
                    strcat(opcode_table[a]->opcode, "1");
                }
                else if (strcmp(token_table[a]->operand[0], "A") == 0) { // A레지스터 = 0
                    strcat(opcode_table[a]->opcode, "0");
                }
                else if (strcmp(token_table[a]->operand[0], "S") == 0) { // S레지스터 = 4
                    strcat(opcode_table[a]->opcode, "4");
                }
                else if (strcmp(token_table[a]->operand[0], "T") == 0) { // T레지스터 = 5
                    strcat(opcode_table[a]->opcode, "5");
                }
                
                if(token_table[a]->operand[1] == NULL)       //두번 째 operand가 없을 때
                    strcat(opcode_table[a]->opcode, "0");
                else {
                    if (strcmp(token_table[a]->operand[1], "X") == 0) { //두번 째 operand가 X레지스터
                        strcat(opcode_table[a]->opcode, "1");
                    }
                    else if (strcmp(token_table[a]->operand[1], "A") == 0) { //두번 째 operand가 A레지스터
                        strcat(opcode_table[a]->opcode, "0");
                    }
                    else if (strcmp(token_table[a]->operand[1], "S") == 0) { //두번 째 operand가 S레지스터
                        strcat(opcode_table[a]->opcode, "4");
                    }
                    else if (strcmp(token_table[a]->operand[1], "T") == 0) { //두번 째 operand가 T레지스터
                        strcat(opcode_table[a]->opcode, "5");
                    }
                }
          
            }
        }
        else if ((token_table[a]->nixbpe & 1) == 1 && token_table[a]->nixbpe != 0) {          //e = 1 4형식
            opcode_table[a]->type = 4;
            opcode_table[a]->csect = csect_count;
            if (token_table[a]->operand[0] != NULL) {
                for (int b = 0; b < 3; b++) {
                    if (extref_table[csect_count - 1]->extref[b] != NULL) {
                        if (strcmp(token_table[a]->operand[0], extref_table[csect_count - 1]->extref[b]) == 0 ) {    //외부참조하는 경우
                            strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                             temp = alphabet_to_num(&inst_table[index]->inst_opcode[1]);
                             temp >>= 2;
                             temp <<= 6;
                             sprintf(buffer, "%X", (token_table[a]->nixbpe | temp));
                             
                             memcpy(opcode_table[a]->opcode +1, buffer, 3);
                             strcat(opcode_table[a]->opcode, "00000");
                       
                        }
                    }
                }

            }
        }
        else{          //3형식
        opcode_table[a]->type = 3;
        opcode_table[a]->csect = csect_count;
            if (strcmp(token_table[a]->operator, "RSUB") == 0) {      //RSUB 일 때
          
                if (token_table[a]->operand[0] == NULL) {
                    strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                    temp = alphabet_to_num(&inst_table[index]->inst_opcode[1]);
                    temp >>= 2;
                    temp <<= 6;

                    sprintf(buffer, "%X", (token_table[a]->nixbpe | temp));
                    memcpy(opcode_table[a]->opcode + 1, buffer, 3);
                    strcat(opcode_table[a]->opcode, "000");
                  
                }
            }
            else if (strncmp(token_table[a]->operand[0], "=X'", 3) == 0 || strncmp(token_table[a]->operand[0], "=C'", 3) == 0) {//리터럴을 만났을 때
                strcpy(temp_str2, token_table[a]->operand[0]);
                temp_str = strtok(temp_str2, "'");
                temp_str = strtok(NULL, "'");
                for (int i = 0; i < literal_index; i++) {
                    if (literal_table[i].literal != NULL) {
                        if (strcmp(literal_table[i].literal, temp_str) == 0) {
                            TA = literal_table[i].addr;
                            break;
                        }
                    }
                }
                temp_addr = TA - pc_addr[a];                //TA - PC값
                
                strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                temp = alphabet_to_num(&inst_table[index]->inst_opcode[1]);
                temp >>= 2;
                temp <<= 6;
                sprintf(buffer, "%X", (token_table[a]->nixbpe | temp));
                memcpy(opcode_table[a]->opcode + 1, buffer, 3);
                sprintf(buffer, "%X", temp_addr);
                if (temp_addr < 256 && temp_addr >= 16) {
                    strcat(opcode_table[a]->opcode, "0");
                    strcat(opcode_table[a]->opcode, buffer);
                }
                else if (temp_addr < 16){
                    strcat(opcode_table[a]->opcode, "00");
                    strcat(opcode_table[a]->opcode, buffer);
                }
                else {
                    strcat(opcode_table[a]->opcode, buffer);
                }
                
                
        
            }

            else if (strncmp(token_table[a]->operand[0],"#",1) == 0) {//#이 있을 때
                strcpy(temp_str2, token_table[a]->operand[0]);
                temp_str = strtok(temp_str2, "#\n");
            //    temp_str = strtok(NULL, "\n");
                
                temp_addr = atoi(temp_str);             //TA - PC값

                strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                temp = alphabet_to_num(&inst_table[index]->inst_opcode[1]);
                temp >>= 2;
                temp <<= 6;
                sprintf(buffer, "%X", (token_table[a]->nixbpe | temp));
                memcpy(opcode_table[a]->opcode + 1, buffer, 3);
                sprintf(buffer, "%X", temp_addr);
                if (temp_addr < 256 && temp_addr >= 16) {
                    strcat(opcode_table[a]->opcode, "0");
                    strcat(opcode_table[a]->opcode, buffer);
                }
                else if (temp_addr < 16) {
                    strcat(opcode_table[a]->opcode, "00");
                    strcat(opcode_table[a]->opcode, buffer);
                }
                else {
                    strcat(opcode_table[a]->opcode, buffer);
                }


            

            }
            else {//indirect일 때
                if (strncmp(token_table[a]->operand[0], "@", 1) == 0) { //indirect 할 때
                    strcpy(temp_str2, token_table[a]->operand[0]);
                    temp_str = strtok(temp_str2, "@\n");            //@을 뗀 값
                }
                else {     //일반 3형식
                    strcpy(temp_str, token_table[a]->operand[0]);
                }
                for (int i = 0; i < sym_index; i++) {
                    if (sym_table[i].symbol != NULL) {
                        if ((opcode_table[a]->csect == sym_csect[i])&& strcmp(sym_table[i].symbol, temp_str) == 0) {
                            TA = sym_table[i].addr;
                            break;
                        }
                    }
                }
                temp_addr = TA - pc_addr[a];                //TA - PC값

                strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                temp = alphabet_to_num(&inst_table[index]->inst_opcode[1]);
                temp >>= 2;
                temp <<= 6;
                sprintf(buffer, "%X", (token_table[a]->nixbpe | temp));
                memcpy(opcode_table[a]->opcode + 1, buffer, 3);
                sprintf(buffer, "%X", temp_addr);
                if (temp_addr < 256 && temp_addr >= 16) {
                    strcat(opcode_table[a]->opcode, "0");
                    strcat(opcode_table[a]->opcode, buffer);
                }
                else if (temp_addr < 16 && temp_addr >= 0) {
                    strcat(opcode_table[a]->opcode, "00");
                    strcat(opcode_table[a]->opcode, buffer);
                }
                else if (temp_addr < 0) {
                    sprintf(buffer, "%X", temp_addr);
                    strcat(opcode_table[a]->opcode, &buffer[5]);
                }
                else {
                    strcat(opcode_table[a]->opcode, buffer);
                }

            
            }
        }
    }
    all_csect = csect_count;
    return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
    int errno;
    csect_count = 1;
    for (int a = 0; a < token_usedline; a++) {
           errno = make_opcode(a);
    }
    return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
    int temp_arr = 0, next = 0, next_sym = 0, next_num = 0, next_addr = 0;
    int len, csect = 0, m_count = 0, count = 0, byte_count = 8, check = 0, isfirst = 0;
    int m_byte[10], m_size[10], m_index = 0;
    char* m_name[10] = {0};
    char* addr = malloc(sizeof(char));
    char* buffer = malloc(sizeof(char)*80);
    char* buffer2 = malloc(sizeof(char)*80);

    if (file_name == NULL) {        //NULL일 떄
        printf("\n");
        for (int a = 0; a < all_csect; a++) {
            for (int q = 0; q < 10; q++) {
                m_byte[q] = -1;
                m_size[q] = -1;
                m_name[q] = 0;
            }
            count = 0; isfirst = 0; m_index = 0;
            for (int j = 0; j < token_usedline; j++) {
                check = 0;
                if (token_table[j]->label != NULL && token_table[j]->operator != NULL && (strcmp(token_table[j]->operator, "CSECT") == 0 || 
                    strcmp(token_table[j]->label, "COPY") == 0)) {
                    temp_arr = -1;
                    for (int i = 0; i < sym_index; i++) {
                        
                            if (sym_csect[next_sym + i] != csect && sym_table[next_sym + i].addr == 0) {     //H record
                                printf("H%s 000000", sym_table[next_sym + i].symbol);
                                csect = sym_csect[next_sym + i];
                                check = 1;
                                next_sym += i;
                                break;
                            }
                        
                    }

                    for (int y = 0; y < token_usedline; y++) {      //section의 마지막 주소값
                        if (opcode_table[next + y] != NULL && opcode_table[next+y]->csect == csect) {
                            if (temp_arr < pc_addr[next+y]) {
                                temp_arr = pc_addr[next+y];
                            }
                        }
                        else if(opcode_table[next + y] == NULL || opcode_table[next + y]->csect != csect){
                            next += y; 
                            sprintf(buffer, "%X", temp_arr);
                            len = strlen(buffer);
                            for (int z = 0; z < 6 - len; z++) {
                                printf("0");
                            }
                            printf(buffer);
                            printf("\n");
                            break;
                        }
                    }
                }
                if (check == 1)
                    break;
            }
                             //EXTDEF Record
            for (int k = 0; k < 3; k++) {
                if (extdef_table[a]->extdef[k] == NULL)
                    break;
                if (k == 0)
                    printf("D");
                if (extdef_table[a]->extdef[k] != NULL) {
                    printf("%s", extdef_table[a]->extdef[k]);
                    for (int n = 0; n < token_usedline; n++) {
                        if (token_table[n]->label != NULL && strcmp(token_table[n]->label, extdef_table[a]->extdef[k]) == 0) {
                            sprintf(buffer, "%X", locctr_addr[n]);
                            len = strlen(buffer);
                            for (int z = 0; z < 6 - len; z++) {
                                printf("0");
                            }
                            printf(buffer);
                            break;
                        }
                    }
                    if (k == 2) {
                        printf("\n");
                    }
                }
                else {
                    printf("\n");
                    break;
                }
            }
            
            for (int k = 0; k < 3; k++) {
                if (extref_table[a]->extref[k] == NULL) {
                    printf("\n");
                    break;
                }
                    
                if(k == 0) printf("R");
                if (extref_table[a]->extref[k] != NULL) {
                    printf("%s ", extref_table[a]->extref[k]);
                }
                if (k == 2)
                    printf("\n");

            }
            
            
            for (int n = 0; n < token_usedline; n++) {
                if (opcode_table[next_num + n] == NULL) {
                    if (opcode_table[next_num + n - 1] != NULL && opcode_table[next_num + n - 1]->csect == 3) {
                        strcpy(buffer, "T000000");
                        sprintf(addr, "%X", (byte_count  - 8) / 2);
                        next_addr = (byte_count- 8) / 2;
                        strcat(buffer, addr);
                        strcat(buffer, buffer2);
                        printf(buffer);
                        break;
                    }
                    else
                        break;
                }
                if (opcode_table[next_num+n] != NULL && opcode_table[next_num + n]->opcode == NULL) {
                    
                    if (a != 0 && opcode_table[next_num + n]->csect > a + 1) {
                        strcpy(buffer, "\nT");
                        sprintf(addr, "%X", next_addr);
                        len = strlen(addr);
                        for (int m = 0; m < 6 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        sprintf(addr, "%X", (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2);
                        len = strlen(addr);
                        for (int m = 0; m < 2 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        strcat(buffer, buffer2);
                        printf(buffer);
                        byte_count = 8;
                        next_num += n;
                        break;
                    }
                    else if (opcode_table[next_num + n]->csect > a + 1) {
                        next_num = n;
                        break;
                    }
                    continue;
                }
                if (byte_count > 69) {
                    isfirst = 0;
                    byte_count = 8;
                }
                if(opcode_table[next_num + n]!= NULL && byte_count != 8)
                byte_count += opcode_table[next_num + n]->type * 2;

                if (opcode_table[next_num + n]->opcode != NULL && token_table[next_num + n]->operator != NULL) {
                    if ((strcmp(opcode_table[next_num+n]->opcode, "000000")==0 || opcode_table[next_num + n]->type == 4) && token_table[next_num + n]->operand[0] != NULL) {
                        if(count == 0)
                             m_byte[m_index] =  (byte_count - 8) / 2 - 3;
                        else if(count == 1)
                            m_byte[m_index] = next_addr+(byte_count - 8) / 2 - 3;
                        if (token_table[next_num + n]->operand[1] != NULL && strcmp(opcode_table[next_num + n]->opcode, "000000") == 0) {
                            m_name[m_index] = token_table[next_num + n]->operand[0];
                            m_size[m_index] = 6;
                            m_index++;
                            m_name[m_index] = token_table[next_num + n]->operand[1];
                            m_byte[m_index] = m_byte[m_index - 1];
                            m_size[m_index] = 6;
                            m_index++;
                        }
                        else {
                            m_name[m_index] = token_table[next_num + n]->operand[0];
                            m_size[m_index] = 5;
                            m_index++;

                        }
                     
                    }
                  if (strcmp(token_table[next_num + n]->operator,"LTORG") == 0 ) {
                        strcpy(buffer, "\nT");
                        sprintf(addr, "%X", next_addr);
                        len = strlen(addr);
                        for (int m = 0; m < 6 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        sprintf(addr, "%X", (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2);
                        len = strlen(addr);
                        for (int m = 0; m < 2 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        strcat(buffer, buffer2);
                        printf(buffer);
                       
                        if (strcmp(token_table[next_num + n]->operator,"LTORG") == 0) {
                            sprintf(addr, "%X", literal_table[0].addr);
                            strcpy(buffer, "\nT");
                            len = strlen(addr);
                            for (int m = 0; m < 6 - len; m++) {
                                strcat(buffer, "0");
                            }
                            strcat(buffer, addr);
                            sprintf(addr, "%X", strlen(literal_table[0].literal));
                            len = strlen(addr);
                            for (int m = 0; m < 2 - len; m++) {
                                strcat(buffer, "0");
                            }
                            strcat(buffer, addr);
                            strcat(buffer, opcode_table[next_num + n]->opcode);
                          printf(buffer);
                         }
                        
                        byte_count = 8;
                        continue;
                    }
                  
                     if (byte_count > 69) {
                        if (count == 0) {
                            strcpy(buffer, "T000000");
                            sprintf(addr, "%X", (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2);
                            next_addr = (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2;
                            strcat(buffer, addr);
                            strcat(buffer, buffer2);
                            printf(buffer);
                            count = 1;
                            n--;
                        }
                        continue;
                    }
                    if (opcode_table[next_num + n]->opcode != NULL) {
                         if (isfirst == 0) {
                             strcpy(buffer2, opcode_table[next_num + n]->opcode);
                             byte_count += opcode_table[next_num + n]->type * 2;
                             isfirst = 1;
                         }
                         else if (isfirst == 1 && buffer2 != NULL) {
                             strcat(buffer2, opcode_table[next_num + n]->opcode);
                         }
                    }
                }
            }
            for (int x = 0; x < m_index; x++) {
                if (m_name[x] != NULL && x == 3) {
                    sprintf(addr, "%X", m_byte[x]);
                    strcpy(buffer, "\nM");
                    len = strlen(addr);
                    for (int c = 0; c < 6 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);

                    sprintf(addr, "%X", m_size[x]);
                    len = strlen(addr);
                    for (int c = 0; c < 2 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);
                    strcat(buffer, "-");
                    strcat(buffer, m_name[x]);

                   printf(buffer);
                }
                else if(m_name[x] != NULL) {
                    sprintf(addr, "%X", m_byte[x]);
                    strcpy(buffer, "\nM");
                    len = strlen(addr);
                    for (int c = 0; c < 6 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);

                    sprintf(addr, "%X", m_size[x]);
                    len = strlen(addr);
                    for (int c = 0; c < 2 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);
                    strcat(buffer, "+");
                    strcat(buffer, m_name[x]);

                    printf(buffer);
                }
            }
            if (a == 0)
                printf("\nE000000\n");      //End record
            else
                printf("\nE\n");
            printf("\n");
        }
    }
    else {
        FILE* file = NULL;
        file = fopen(file_name, "wt");
        fprintf(file, "\n");
        for (int a = 0; a < all_csect; a++) {
            for (int q = 0; q < 10; q++) {
                m_byte[q] = -1;
                m_size[q] = -1;
                m_name[q] = 0;
            }
            count = 0; isfirst = 0; m_index = 0;
            for (int j = 0; j < token_usedline; j++) {
                check = 0;
                if (token_table[j]->label != NULL && token_table[j]->operator != NULL && (strcmp(token_table[j]->operator, "CSECT") == 0 ||
                    strcmp(token_table[j]->label, "COPY") == 0)) {
                    temp_arr = -1;
                    for (int i = 0; i < sym_index; i++) {

                        if (sym_csect[next_sym + i] != csect && sym_table[next_sym + i].addr == 0) {     //H record
                            fprintf(file, "H%s 000000", sym_table[next_sym + i].symbol);
                            csect = sym_csect[next_sym + i];
                            check = 1;
                            next_sym += i;
                            break;
                        }

                    }

                    for (int y = 0; y < token_usedline; y++) {      //section의 마지막 주소값
                        if (opcode_table[next + y] != NULL && opcode_table[next + y]->csect == csect) {
                            if (temp_arr < pc_addr[next + y]) {
                                temp_arr = pc_addr[next + y];
                            }
                        }
                        else if (opcode_table[next + y] == NULL || opcode_table[next + y]->csect != csect) {
                            next += y;
                            sprintf(buffer, "%X", temp_arr);
                            len = strlen(buffer);
                            for (int z = 0; z < 6 - len; z++) {
                                fprintf(file,"0");
                            }
                            fprintf(file,buffer);
                            fprintf(file,"\n");
                            break;
                        }
                    }
                }
                if (check == 1)
                    break;
            }
            //EXTDEF Record
            for (int k = 0; k < 3; k++) {
                if (extdef_table[a]->extdef[k] == NULL)
                    break;
                if (k == 0)
                    fprintf(file,"D");
                if (extdef_table[a]->extdef[k] != NULL) {
                    fprintf(file,"%s", extdef_table[a]->extdef[k]);
                    for (int n = 0; n < token_usedline; n++) {
                        if (token_table[n]->label != NULL && strcmp(token_table[n]->label, extdef_table[a]->extdef[k]) == 0) {
                            sprintf(buffer, "%X", locctr_addr[n]);
                            len = strlen(buffer);
                            for (int z = 0; z < 6 - len; z++) {
                                fprintf(file,"0");
                            }
                            fprintf(file,buffer);
                            break;
                        }
                    }
                    if (k == 2) {
                        fprintf(file,"\n");
                    }
                }
                else {
                    fprintf(file, "\n");
                    break;
                }
            }

            for (int k = 0; k < 3; k++) {
                if (extref_table[a]->extref[k] == NULL) {
                    fprintf(file, "\n");
                    break;
                }

                if (k == 0) fprintf(file,"R");
                if (extref_table[a]->extref[k] != NULL) {
                    fprintf(file, "%s ", extref_table[a]->extref[k]);
                }
                if (k == 2)
                    fprintf(file, "\n");

            }


            for (int n = 0; n < token_usedline; n++) {
                if (opcode_table[next_num + n] == NULL) {
                    if (opcode_table[next_num + n - 1] != NULL && opcode_table[next_num + n - 1]->csect == 3) {
                        strcpy(buffer, "T000000");
                        sprintf(addr, "%X", (byte_count - 8) / 2);
                        next_addr = (byte_count - 8) / 2;
                        strcat(buffer, addr);
                        strcat(buffer, buffer2);
                        fprintf(file, buffer);
                        break;
                    }
                    else
                        break;
                }
                if (opcode_table[next_num + n] != NULL && opcode_table[next_num + n]->opcode == NULL) {

                    if (a != 0 && opcode_table[next_num + n]->csect > a + 1) {
                        strcpy(buffer, "\nT");
                        sprintf(addr, "%X", next_addr);
                        len = strlen(addr);
                        for (int m = 0; m < 6 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        sprintf(addr, "%X", (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2);
                        len = strlen(addr);
                        for (int m = 0; m < 2 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        strcat(buffer, buffer2);
                        fprintf(file, buffer);
                        byte_count = 8;
                        next_num += n;
                        break;
                    }
                    else if (opcode_table[next_num + n]->csect > a + 1) {
                        next_num = n;
                        break;
                    }
                    continue;
                }
                if (byte_count > 69) {
                    isfirst = 0;
                    byte_count = 8;
                }
                if (opcode_table[next_num + n] != NULL && byte_count != 8)
                    byte_count += opcode_table[next_num + n]->type * 2;

                if (opcode_table[next_num + n]->opcode != NULL && token_table[next_num + n]->operator != NULL) {
                    if ((strcmp(opcode_table[next_num + n]->opcode, "000000") == 0 || opcode_table[next_num + n]->type == 4) && token_table[next_num + n]->operand[0] != NULL) {
                        if (count == 0)
                            m_byte[m_index] = (byte_count - 8) / 2 - 3;
                        else if (count == 1)
                            m_byte[m_index] = next_addr + (byte_count - 8) / 2 - 3;
                        if (token_table[next_num + n]->operand[1] != NULL && strcmp(opcode_table[next_num + n]->opcode, "000000") == 0) {
                            m_name[m_index] = token_table[next_num + n]->operand[0];
                            m_size[m_index] = 6;
                            m_index++;
                            m_name[m_index] = token_table[next_num + n]->operand[1];
                            m_byte[m_index] = m_byte[m_index - 1];
                            m_size[m_index] = 6;
                            m_index++;
                        }
                        else {
                            m_name[m_index] = token_table[next_num + n]->operand[0];
                            m_size[m_index] = 5;
                            m_index++;

                        }

                    }
                    if (strcmp(token_table[next_num + n]->operator,"LTORG") == 0) {
                        strcpy(buffer, "\nT");
                        sprintf(addr, "%X", next_addr);
                        len = strlen(addr);
                        for (int m = 0; m < 6 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        sprintf(addr, "%X", (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2);
                        len = strlen(addr);
                        for (int m = 0; m < 2 - len; m++) {
                            strcat(buffer, "0");
                        }
                        strcat(buffer, addr);
                        strcat(buffer, buffer2);
                        fprintf(file, buffer);

                        if (strcmp(token_table[next_num + n]->operator,"LTORG") == 0) {
                            sprintf(addr, "%X", literal_table[0].addr);
                            strcpy(buffer, "\nT");
                            len = strlen(addr);
                            for (int m = 0; m < 6 - len; m++) {
                                strcat(buffer, "0");
                            }
                            strcat(buffer, addr);
                            sprintf(addr, "%X", strlen(literal_table[0].literal));
                            len = strlen(addr);
                            for (int m = 0; m < 2 - len; m++) {
                                strcat(buffer, "0");
                            }
                            strcat(buffer, addr);
                            strcat(buffer, opcode_table[next_num + n]->opcode);
                            fprintf(file,buffer);
                        }

                        byte_count = 8;
                        continue;
                    }

                    if (byte_count > 69) {
                        if (count == 0) {
                            strcpy(buffer, "T000000");
                            sprintf(addr, "%X", (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2);
                            next_addr = (byte_count - opcode_table[next_num + n]->type * 2 - 8) / 2;
                            strcat(buffer, addr);
                            strcat(buffer, buffer2);
                            fprintf(file, buffer);
                            count = 1;
                            n--;
                        }
                        continue;
                    }
                    if (opcode_table[next_num + n]->opcode != NULL) {
                        if (isfirst == 0) {
                            strcpy(buffer2, opcode_table[next_num + n]->opcode);
                            byte_count += opcode_table[next_num + n]->type * 2;
                            isfirst = 1;
                        }
                        else if (isfirst == 1 && buffer2 != NULL) {
                            strcat(buffer2, opcode_table[next_num + n]->opcode);
                        }
                    }
                }
            }
            for (int x = 0; x < m_index; x++) {
                if (m_name[x] != NULL && x == 3) {
                    sprintf(addr, "%X", m_byte[x]);
                    strcpy(buffer, "\nM");
                    len = strlen(addr);
                    for (int c = 0; c < 6 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);

                    sprintf(addr, "%X", m_size[x]);
                    len = strlen(addr);
                    for (int c = 0; c < 2 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);
                    strcat(buffer, "-");
                    strcat(buffer, m_name[x]);

                    fprintf(file, buffer);
                }
                else if (m_name[x] != NULL) {
                    sprintf(addr, "%X", m_byte[x]);
                    strcpy(buffer, "\nM");
                    len = strlen(addr);
                    for (int c = 0; c < 6 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);

                    sprintf(addr, "%X", m_size[x]);
                    len = strlen(addr);
                    for (int c = 0; c < 2 - len; c++) {
                        strcat(buffer, "0");
                    }
                    strcat(buffer, addr);
                    strcat(buffer, "+");
                    strcat(buffer, m_name[x]);

                    fprintf(file, buffer);
                }
            }
            if (a == 0)
                fprintf(file,"\nE000000\n");      //End record
            else
                fprintf(file,"\nE\n");
            fprintf(file, "\n");
        }
        fclose(file);
    }
    
}
