#include "PrintHelper.h"

#include "../ENUM.h"
#include "../parser/SysY.tab.h"
//#include "../ssa/ssa.h"
const char *EnumTypeToString(int type) {
    switch (type) {
        case INTCONST:
            return "INTCONST";
        case MULOP:
            return "MULOP";
        case EQOP:
            return "EQOP";
        case RELOP:
            return "RELOP";
        case STRING:
            return "STRING";
        case K_CURLY_L:
            return "K_CURLY_L";
        case K_CURLY_R:
            return "K_CURLY_R";
        case K_SQUARE_L:
            return "K_SQUARE_L";
        case K_SQUARE_R:
            return "K_SQUARE_R";
        case K_PARENTHESES_L:
            return "K_PARENTHESES_L";
        case K_PARENTHESES_R:
            return "K_PARENTHESES_R";
        case K_COMMA:
            return "K_COMMA";
        case K_SEMICOLON:
            return "K_SEMICOLON";
        case K_ASSIGNOP:
            return "K_ASSIGNOP";
        case K_ADD:
            return "K_ADD";
        case K_SUB:
            return "K_SUB";
        case K_MUL:
            return "K_MUL";
        case K_DIV:
            return "K_DIV";
        case K_MOD:
            return "K_MOD";
        case K_GT:
            return "K_GT";
        case K_GTE:
            return "K_GTE";
        case K_LT:
            return "K_LT";
        case K_LTE:
            return "K_LTE";
        case K_EQ:
            return "K_EQ";
        case K_NEQ:
            return "K_NEQ";
        case K_AND:
            return "K_AND";
        case K_OR:
            return "K_OR";
        case K_NOT:
            return "K_NOT";
        case KEYWORD:
            return "KEYWORD";
        case K_CONST:
            return "K_CONST";
        case K_VOID:
            return "K_VOID";
        case K_INT:
            return "K_INT";
        case K_IF:
            return "K_IF";
        case K_ELSE:
            return "K_ELSE";
        case K_WHILE:
            return "K_WHILE";
        case K_BREAK:
            return "K_BREAK";
        case K_CONTINUE:
            return "K_CONTINUE";
        case K_RETURN:
            return "K_RETURN";
        case IDENT:
            return "IDENT";
        case COMMENT:
            return "COMMENT";
        case FILEEND:
            return "FILEEND";
        case K_LOW_THAN_ELSE:
            return "K_LOW_THAN_ELSE";
        case COMPUNIT:
            return "COMPUNIT";
        case DECL:
            return "DECL";
        case CONSTDECL:
            return "CONSTDECL";
        case BTYPE:
            return "BTYPE";
        case CONSTDEFS:
            return "CONSTDEFS";
        case CONSTDEF:
            return "CONSTDEF";
        case CONSTARRAYDEFS:
            return "CONSTARRAYDEFS";
        case CONSTARRAYDEF:
            return "CONSTARRAYDEF";
        case CONSTINITVAL:
            return "CONSTINITVAL";
        case CONSTINITVALS:
            return "CONSTINITVALS";
        case VARDECL:
            return "VARDECL";
        case VARDEFS:
            return "VARDEFS";
        case VARDEF:
            return "VARDEF";
        case INITVAL:
            return "INITVAL";
        case INITVALS:
            return "INITVALS";
        case FUNCDEF:
            return "FUNCDEF";
        case FUNCTYPE:
            return "FUNCTYPE";
        case FUNCFPARAMS:
            return "FUNCFPARAMS";
        case FUNCFPARAM:
            return "FUNCFPARAM";
        case EXPARRAYDEFS:
            return "EXPARRAYDEFS";
        case EXPARRAYDEF:
            return "EXPARRAYDEF";
        case BLOCK:
            return "BLOCK";
        case BLOCKITEMS:
            return "BLOCKITEMS";
        case BLOCKITEM:
            return "BLOCKITEM";
        case STMT:
            return "STMT";
        case ASSIGN:
            return "ASSIGN";
        case IFSTMT:
            return "IFSTMT";
        case WHILESTMT:
            return "WHILESTMT";
        case RETURNSTMT:
            return "RETURNSTMT";
        case BREAKSTMT:
            return "BREAKSTMT";
        case CONTINUESTMT:
            return "CONTINUESTMT";
        case EXP:
            return "EXP";
        case COND:
            return "COND";
        case LVAL:
            return "LVAL";
        case ARRAYIMPL:
            return "ARRAYIMPL";
        case PRIMARYEXP:
            return "PRIMARYEXP";
        case NUMBER:
            return "NUMBER";
        case UNARYEXP:
            return "UNARYEXP";
        case FUNCIMPL:
            return "FUNCIMPL";
        case UNARYEXPS:
            return "UNARYEXPS";
        case UNARYOP:
            return "UNARYOP";
        case FUNCRPARAMS:
            return "FUNCRPARAMS";
        case FUNCRPARAM:
            return "FUNCRPARAM";
        case MULEXP:
            return "MULEXP";
        case ADDEXP:
            return "ADDEXP";
        case ADDOP:
            return "ADDOP";
        case RELEXP:
            return "RELEXP";
        case EQEXP:
            return "EQEXP";
        case LANDEXP:
            return "LANDEXP";
        case LOREXP:
            return "LOREXP";
        case CONSTEXP:
            return "CONSTEXP";
        case VARSYMENTRY:
            return "VARSYMENTRY";
        case FUNCSYMENTRY:
            return "FUNCSYMENTRY";
        case FUNCSYMTABLE:
            return "FUNCSYMTABLE";
        case VARSYMTABLE:
            return "VARSYMTABLE";
        case LABELENTRY:
            return "LABELENTRY";
        case FUNCIMPLAST:
            return "FUNCIMPLAST";
        case ARRAYIMPLAST:
            return "ARRAYIMPLAST";
        case EXPAST:
            return "EXPAST";
        case OPREAND:
            return "OPREAND";
        case PARAM:
            return "PARAM";
        case CALL:
            return "CALL";
        case JUMP:
            return "JUMP";
        case BRANCH:
            return "BRANCH";
        case LOAD:
            return "LOAD";
        case STORE:
            return "STORE";
        case NOP:
            return "NOP";
        case ARRAY:
            return "ARRAY";
        case FRAMEPOINT:
            return "FRAMEPOINT";
        case STACKPOINT:
            return "STACKPOINT";
        case GLOBALDATA:
            return "GLOBALDATA";
        case PHI:
            return "PHI";
        case INT:
            return "INT";
        case ConstSTRING:
            return "ConstSTRING";
        case FUNCID:
            return "FUNCID";
        case REGISTER:
            return "REGISTER";
        case ADDRESS:
            return "ADDRESS";
        case BASIC_BLOCK:
            return "BASIC_BLOCK";
        case 0:
            return "NULL";
        default:
            return "UNKNOWN";
    }
}
