/*
 * ȭ�ϸ� : my_assembler_20160312.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "my_assembler_20160312.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	
	make_symtab_output("symtab_20160312");
    make_literaltab_output("literaltab_20160312");

	if(assem_pass2() < 0 ){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_20160312") ;
	
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
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
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
        printf("����\n");
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
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
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
        printf("����\n");
        errno = -1;
    }
    else {
        while (!feof(file)) {                                   //1���� �߰��� �� �����
            fgets(input_data[line_num], 100, file);
            line_num++;
        }
        if (line_num != 0) {
            //line_num--;
            k = line_num;
        }
        for (; k < MAX_LINES; k++) {                            //������ ������ ������ input_data �迭�� NULL�� ä��
            input_data[k] = NULL;
        }
        errno = 0;
    }
    fclose(file);
 
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�.  
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
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
        
        if (strncmp("\t", str, 1) == 0) {                   //label�� ���� ��
            token_table[token_line]->label = NULL;          //label = NULL
           /*------------------------------------------------------------------------------*/
            if(strncmp("\t\t", str, 2)   )
            
            temp_str = strtok(str, "\t\n");                   //operator
            if (temp_str == NULL)
                token_table[token_line]->operator = NULL;
            else
                token_table[token_line]->operator = temp_str;
           /*------------------------------------------------------------------------------*/
            temp_str2 = strtok(NULL, "\n");                 //operator ������ ����
            if (temp_str2 == NULL) {
                temp = temp_str2;                           //comment�� NULL�� ����� ����
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else if (strncmp(temp_str2, "\t", 1) == 0) {       // operand���� comment�� ���� �� input.txt���� \t�� �ι� ��
                temp_str3 = strtok(temp_str2,"\t\n");
                temp = temp_str3;
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else {              //operand�� ���� ��
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
        else if (strncmp(".", str, 1) == 0) {               //.���� �����ϴ� �ּ�
            token_table[token_line]->label = NULL;
            token_table[token_line]->operator = NULL;
            for (int k = 0; k < MAX_OPERAND; k++) {
                token_table[token_line]->operand[k] = NULL;
            }
            token_table[token_line]->comment = strtok(str, "\n");
        }
        else {                                              //label���� ���� ��
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
            temp_str2 = strtok(NULL, "\n");                      //operator ������ ����
            if (temp_str2 == NULL) {
                temp = temp_str2;
                for (int k = 0; k < MAX_OPERAND; k++) {
                    token_table[token_line]->operand[k] = NULL;
                }
            }
            else if (strncmp(temp_str2, "\t", 1) == 0) {       // operand���� comment�� ���� �� input.txt���� \t�� �ι� ��
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
                if(strncmp(inst_table[index]->inst_type, "2", 1) == 0){         //2����
                    token_table[token_line]->nixbpe = 0;
                }
                else {      //3,4����
                    if (strncmp(token_table[token_line]->operator, "+", 1) == 0) {  //4����
                        if (token_table[token_line]->operand[1] != NULL) {
                            if (strcmp(token_table[token_line]->operand[1], "X") == 0) {    //���� ��� O
                                token_table[token_line]->nixbpe |= 57;
                            }
                        }
                        else {
                            token_table[token_line]->nixbpe |= 49;
                        }
                    }
                    else if (token_table[token_line]->operand[0] != NULL && strncmp(token_table[token_line]->operand[0], "@", 1) == 0) { // 3���� n=1,i=0
                        token_table[token_line]->nixbpe |= 34;
                    }
                    else if (token_table[token_line]->operand[0] != NULL && strncmp(token_table[token_line]->operand[0], "#", 1) == 0) { // 3���� n=0,i=1
                        token_table[token_line]->nixbpe |= 16;
                    }
                    else {          //3���� n,i = 1
                        if (strcmp(token_table[token_line]->operator, "RSUB")== 0 && token_table[token_line]->operand[0] == NULL) {
                            token_table[token_line]->nixbpe |= 48;
                        }
                        else if (token_table[token_line]->operand[1] != NULL) {
                            if (strcmp(token_table[token_line]->operand[1], "X") == 0) {    //���� ��� O
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
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : ��ū ������ ���е� ���ڿ� 
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)                                        //���ڿ��� ������ inst_table�� ���° �ּҿ� �����ִ����� ���� ����
{
    opcode_index = 0;
    for (int i = 0; i < inst_index; i++) {
        if (strcmp(strtok(str,"+"), inst_table[i]->inst_name) == 0) {
            opcode_index = i;
            break;
        }
    }
    if (opcode_index == 0) {                                        //���� �ڵ尡 �ƴҶ�
        opcode_index = -1;
    }
    return opcode_index;
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
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

void make_symtab(){     //���ͷ� ���̺�� �ɺ� ���̺��� ����� �Լ�
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
            if (strcmp(token_table[i]->operator,"CSECT") == 0) {      //CSECT = control section ���� ����
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
                else if (isright == 1) {        //symbol�� ��� �� ��
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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

int make_opcode(int a) {   //a = �迭 ��ȣ
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
            csect_count++;          //���� section
       

        index = search_opcode(token_table[a]->operator);    //opcode ��ġ

        if (index == -1 && token_table[a]->nixbpe == 0) {    // �ƹ��͵� �ƴ� ���
            if (token_table[a]->operator != NULL) {
                if (strcmp(token_table[a]->operator, "WORD") == 0) {
                    if (token_table[a]->operand[0] != NULL) {
                        opcode_table[a]->csect = csect_count;
                        for (int b = 0; b < 3; b++) {
                            if (extref_table[csect_count - 1]->extref[b] != NULL) {
                                if (strcmp(token_table[a]->operand[0], extref_table[csect_count - 1]->extref[b]) == 0) {    //�ܺ������ϴ� ���

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
                else if (strcmp(token_table[a]->operator, "LTORG") == 0) {              //opcode_table�� ���� ����;;
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
        else if (strncmp(inst_table[index]->inst_type, "2", 1) == 0 && token_table[a]->nixbpe == 0) {   // 2����
            opcode_table[a]->type = atoi(inst_table[index]->inst_type);
            opcode_table[a]->csect = csect_count;

            if (token_table[a]->operand[0] != NULL) {               //operand[0]�� 0�� �ƴ� ��
                strcpy(opcode_table[a]->opcode, inst_table[index]->inst_opcode);
                if (strcmp(token_table[a]->operand[0], "X") == 0) { // X�������� = 1
                    strcat(opcode_table[a]->opcode, "1");
                }
                else if (strcmp(token_table[a]->operand[0], "A") == 0) { // A�������� = 0
                    strcat(opcode_table[a]->opcode, "0");
                }
                else if (strcmp(token_table[a]->operand[0], "S") == 0) { // S�������� = 4
                    strcat(opcode_table[a]->opcode, "4");
                }
                else if (strcmp(token_table[a]->operand[0], "T") == 0) { // T�������� = 5
                    strcat(opcode_table[a]->opcode, "5");
                }
                
                if(token_table[a]->operand[1] == NULL)       //�ι� ° operand�� ���� ��
                    strcat(opcode_table[a]->opcode, "0");
                else {
                    if (strcmp(token_table[a]->operand[1], "X") == 0) { //�ι� ° operand�� X��������
                        strcat(opcode_table[a]->opcode, "1");
                    }
                    else if (strcmp(token_table[a]->operand[1], "A") == 0) { //�ι� ° operand�� A��������
                        strcat(opcode_table[a]->opcode, "0");
                    }
                    else if (strcmp(token_table[a]->operand[1], "S") == 0) { //�ι� ° operand�� S��������
                        strcat(opcode_table[a]->opcode, "4");
                    }
                    else if (strcmp(token_table[a]->operand[1], "T") == 0) { //�ι� ° operand�� T��������
                        strcat(opcode_table[a]->opcode, "5");
                    }
                }
          
            }
        }
        else if ((token_table[a]->nixbpe & 1) == 1 && token_table[a]->nixbpe != 0) {          //e = 1 4����
            opcode_table[a]->type = 4;
            opcode_table[a]->csect = csect_count;
            if (token_table[a]->operand[0] != NULL) {
                for (int b = 0; b < 3; b++) {
                    if (extref_table[csect_count - 1]->extref[b] != NULL) {
                        if (strcmp(token_table[a]->operand[0], extref_table[csect_count - 1]->extref[b]) == 0 ) {    //�ܺ������ϴ� ���
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
        else{          //3����
        opcode_table[a]->type = 3;
        opcode_table[a]->csect = csect_count;
            if (strcmp(token_table[a]->operator, "RSUB") == 0) {      //RSUB �� ��
          
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
            else if (strncmp(token_table[a]->operand[0], "=X'", 3) == 0 || strncmp(token_table[a]->operand[0], "=C'", 3) == 0) {//���ͷ��� ������ ��
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
                temp_addr = TA - pc_addr[a];                //TA - PC��
                
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

            else if (strncmp(token_table[a]->operand[0],"#",1) == 0) {//#�� ���� ��
                strcpy(temp_str2, token_table[a]->operand[0]);
                temp_str = strtok(temp_str2, "#\n");
            //    temp_str = strtok(NULL, "\n");
                
                temp_addr = atoi(temp_str);             //TA - PC��

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
            else {//indirect�� ��
                if (strncmp(token_table[a]->operand[0], "@", 1) == 0) { //indirect �� ��
                    strcpy(temp_str2, token_table[a]->operand[0]);
                    temp_str = strtok(temp_str2, "@\n");            //@�� �� ��
                }
                else {     //�Ϲ� 3����
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
                temp_addr = TA - pc_addr[a];                //TA - PC��

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
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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

    if (file_name == NULL) {        //NULL�� ��
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

                    for (int y = 0; y < token_usedline; y++) {      //section�� ������ �ּҰ�
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

                    for (int y = 0; y < token_usedline; y++) {      //section�� ������ �ּҰ�
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
