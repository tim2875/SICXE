/*
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit
{
    char *inst_name;
    char *inst_type;
    char *inst_opcode;
    int inst_operandnum;
};

// instruction�� ������ ���� ����ü�� �����ϴ� ���̺� ����
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

struct opcode_unit
{   
    char* opcode;
    int type;       //type �̸鼭 byte�� ex) 2���� 2byte, 3���� 3byte
    int csect;
};
//object program�� ��� �͵�
typedef struct opcode_unit opcode;
opcode* opcode_table[MAX_LINES];

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;

static int opcode_index;
/*
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit
{
    char *label;                //��ɾ� ���� �� label
    char* operator;             //��ɾ� ���� �� operator
    char *operand[MAX_OPERAND]; //��ɾ� ���� �� operand
    char *comment;              //��ɾ� ���� �� comment
    char nixbpe;                //���� 6bit ��� : _ _ n i x b p e
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;
static int token_usedline;      //�Ҵ�� ����

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������Ʈ���� ���ȴ�.
 */
struct symbol_unit
{
    char symbol[10];
    int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
int sym_csect[MAX_LINES];

static int sym_index;

/*
* ���ͷ��� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ��� �̸�, ���ͷ��� ��ġ�� �����ȴ�.
* ���� ������Ʈ���� ���ȴ�.
*/
struct literal_unit
{
    char literal[10];
    int addr;
};

typedef struct literal_unit literal;
literal literal_table[MAX_LINES];
static int literal_index;
static int ltorg_index;

struct extdef_unit
{
    char *extdef[3];
};

typedef struct extdef_unit extdef;
extdef* extdef_table[MAX_LINES];

struct extref_unit
{
    char* extref[3];
};
typedef struct extref_unit extref;
extref* extref_table[MAX_LINES];

int pc_addr[MAX_LINES];
int locctr_addr[MAX_LINES];

static int extref_index;
static int extdef_index;
int csect_count;
int sym_csect_count;
static int all_csect;
static int locctr;
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_symtab();
int make_opcode(int a);
int alphabet_to_num(char* str);

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
