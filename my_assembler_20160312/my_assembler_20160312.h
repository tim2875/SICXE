/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/*
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
    char *inst_name;
    char *inst_type;
    char *inst_opcode;
    int inst_operandnum;
};

// instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

struct opcode_unit
{   
    char* opcode;
    int type;       //type 이면서 byte수 ex) 2형식 2byte, 3형식 3byte
    int csect;
};
//object program에 담길 것들
typedef struct opcode_unit opcode;
opcode* opcode_table[MAX_LINES];

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;

static int opcode_index;
/*
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit
{
    char *label;                //명령어 라인 중 label
    char* operator;             //명령어 라인 중 operator
    char *operand[MAX_OPERAND]; //명령어 라인 중 operand
    char *comment;              //명령어 라인 중 comment
    char nixbpe;                //하위 6bit 사용 : _ _ n i x b p e
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;
static int token_usedline;      //할당된 라인

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 프로젝트에서 사용된다.
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
* 리터럴을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
* 추후 프로젝트에서 사용된다.
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

/* 추후 프로젝트에서 사용하게 되는 함수*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
