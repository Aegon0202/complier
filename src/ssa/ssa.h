#include"link.h"
#include"defs.h"
#include"../SysY.type/SysY.type.def.h"
#include"../SysY.type/SysY.type.symtab.h"
#include"../utils/Malloc.h"

#define MAX_CAPACITY 1000000
#define DEF_TABLE_SIZE 1000
typedef void* ID;

typedef struct BasicBlockNode BasicBlockNode;
typedef struct Ir Ir;

typedef struct BasicBlock{
    int is_sealed;          //前驱节点是否已经确定
    int is_full;            //
//    int begin_reg_idx;  
//    int end_reg_idx;
    Ir* ir_list;             // 基本块内包含的ir
    int predecessor_num;    //前驱节点个数
    BasicBlockNode* predecessors; //前驱节点
    int successor_num;      //后代个数
    BasicBlockNode* successors; //后代节点
} BasicBlock;

//基本块链表
struct BasicBlockNode{
    BasicBlock* value;
    list_entry_t block_link;
};

typedef union{
    int intValue;
    char* str;
    void* funcID;
    BasicBlock* b;
    Ir* complex_value;
} Value;


//放在ir中的操作数，可以是Value也可以是寄存器，type表示的是操作数的类型,如果是value则只能是address或者int
typedef struct{
    enum Operand_type{PHI,INT,ConstSTRING,FUNCID,REGISTER,ADDRESS,BASIC_BLOCK} type;
    union{
        Value v;
        int reg_idx;
    } operand;
} Operand;

//IR
struct Ir{
    enum IrType type;
    Operand *op1,*op2,*op3;
    list_entry_t ir_link;
};

typedef struct{
    BasicBlock start_block;
    ID* id_list;
    Value* value_list;
} Program;

#define le2struct(le,type,member)    \
        to_struct((le),type,member)




void * fp;                         //函数调用,栈顶指针

int current_size;

int max_capacity = MAX_CAPACITY;

Value* reg_list[MAX_CAPACITY];     //寄存器堆，在这个阶段，寄存器的数量为无限大

ID id_list[MAX_CAPACITY];         //这个数组为ast和IR之间的桥梁，表示在每个寄存器中存的value在ast中是属于哪个变量的

enum IrType {HEAD,Add,Sub,Mul,Div,Minus,And,Or,Not,Branch,Jump,Call,Return,Function,Param};

Ir* create_new_ir(enum IrType op_type, Operand *op1,Operand*,Operand*);
BasicBlock* create_new_block();
Value* new_Value();

//建立祖先后代关系
void connect_block(BasicBlock* pre, BasicBlock* suc);
void add_user(int i, BasicBlock* block, Ir* ir);
void delete_user(int , Ir* );
int read_variable(ID id, BasicBlock* block);
void write_variable(ID operand, BasicBlock* block, Ir* ir);
void seal_block();
int alloc_register();

#define BASIC_BLOCK_TYPE BasicBlock
#define IR_LIST_TYPE Ir
#define OPERAND_TYPE Operand
#define IR_TYPE Ir
//需要在SSA中完成的函数
/**
 * 生成一个新的基本块
 * 
 * :param (BASIC_BLOCK_TYPE*) predecessor 一个前驱，可能为NULL
 * 
 * :return (BASIC_BLOCK_TYPE*) 一个新生成的基本块
 */
BASIC_BLOCK_TYPE* newBasicBlock(BASIC_BLOCK_TYPE* predecessor);

/**
 * 设置一个基本块已封闭
 * 会在这个基本块的所有前驱都设置完的情况下调用
 * 
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本快
 * 
 * :return (void) 没有返回值
 */
void setBasicBlockSealed(BASIC_BLOCK_TYPE* basic_block);

/**
 * 将一个（struct IntConst*）转化成一个合法的操作符
 * 
 * :param (struct IntConst*) int_const 一个输入，代表一个常数
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSAIntConst(struct IntConst* int_const, BASIC_BLOCK_TYPE* basic_block);


/**
 * 将一个（struct String*）转化成一个合法的操作符
 * 
 * :param (struct String*) str 一个输入，代表一个字符串
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSAString(struct String* str, BASIC_BLOCK_TYPE* basic_block);

/**
 * 将一个（struct VarSymEntry*）转化成一个合法的操作符
 * 可能是作为输入，也可能是作为输出
 * 
 * :param (struct VarSymEntry*) vse 一个输入，代表一个int变量
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSAVarSymEntry(struct VarSymEntry* vse, BASIC_BLOCK_TYPE* basic_block);

/**
 * 将一个（BASIC_BLOCK_TYPE*）转化成一个合法的操作符
 * 
 * :param (BASIC_BLOCK_TYPE*) target_block 一个输入，代表一个基本块，即跳转地址
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSABasicBlock(BASIC_BLOCK_TYPE* target_block, BASIC_BLOCK_TYPE* basic_block);

/**
 * 将一个（struct FuncSymEntry*）转化成一个合法的操作符，作为函数名称。
 * 可用于 PARAM和CALL
 * 
 * :param (struct FuncSymEntry*) fse 一个输入，函数名。
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSAFuncName(struct FuncSymEntry* fse, BASIC_BLOCK_TYPE* basic_block);

/**
 * 将一个（struct VarSymEntry*）转化成一个合法的操作符，作为函数中形参的名称。
 * 可用于 PARAM
 * vse 可能为NULL，作为可变参数用。按照从左向右传参。
 * 
 * :param (struct VarSymEntry*) vse 一个输入，函数形参名。
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSAParamName(struct VarSymEntry* vse, BASIC_BLOCK_TYPE* basic_block);

/**
 * 返回一个可用于存放并读取数据的操作数。
 * 
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将需要的ir或其他东西放到此基本块中
 * 
 * :return (OPERAND_TYPE*) ssa中的操作符
*/
OPERAND_TYPE* toSSATempVariable(BASIC_BLOCK_TYPE* basic_block);

/**
 * 创建一条ir
 * 
 * :param (int) op ir的操作指令
 * :param (OPERAND_TYPE*) op1,op2,op3 操作符
 * :param (BASIC_BLOCK_TYPE*) basic_block 一个基本块，将新创建的ir放到此基本块中
 * 
 * :return (IR_LIST_TYPE*) 一条ir指令
*/
IR_TYPE* newIR(int op, OPERAND_TYPE* op1, OPERAND_TYPE* op2, OPERAND_TYPE* op3, BASIC_BLOCK_TYPE* basic_block);

/**
 * 添加一条由predecessor指向successor的边
 * 
 * :param (BASIC_BLOCK_TYPE*) predecessor   前驱
 * :param (BASIC_BLOCK_TYPE*) successor     后继
*/
void addBasicBlockEdge(BASIC_BLOCK_TYPE* predecessor, BASIC_BLOCK_TYPE* successor);
