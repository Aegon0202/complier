#include "peephole.h"

#include "../ENUM.h"
#include "../utils/IrType.h"
#include "local_op.h"
struct LinearList* constMark;
struct LinearList* constValue;
struct LinearList* constType;
struct DequeList* prop_worklist;
struct DequeList* simp_worklist;

extern int reg_begin;
extern int current_size;
int __is_ir_value_const(IR_TYPE* ir, int* value, int* type);

int __is_op3_writable(Ir* ir) {
    switch (ir->type) {
        case K_NOT:
        case ASSIGN:
        case K_ADD:
        case K_MUL:
        case K_EQ:
        case K_NEQ:
        case K_SUB:
        case K_DIV:
        case K_MOD:
        case K_LT:
        case K_LTE:
        case K_GT:
        case K_GTE:
        case PHI:
        case CALL:
        case LOAD:
            return 1;
            break;
    }
    return 0;
}

int __is_const_op(Operand* op, const int n) {
    if (op->type == INT || op->type == FRAMEPOINT || op->type == STACKPOINT) {
        if (op->operand.v.intValue == n)
            return 2;
        else
            return 1;
    }
    return 0;
}

int caculate_const_value(Ir* ir, int* type) {
    int value;
    int num1, num2;
    int num1_type, num2_type;
    if (ir->op1->type == REGISTER) {
        num1 = *(int*)getLinearList(constValue, ir->op1->operand.reg_idx);
        num1_type = *(int*)getLinearList(constType, ir->op1->operand.reg_idx);
    } else {
        num1 = ir->op1->operand.v.intValue;
        num1_type = ir->op1->type;
    }
    if (ir->op2) {
        if (ir->op2->type == REGISTER) {
            num2 = *(int*)getLinearList(constValue, ir->op2->operand.reg_idx);
            num2_type = *(int*)getLinearList(constType, ir->op2->operand.reg_idx);
        } else {
            num2 = ir->op2->operand.v.intValue;
            num2_type = ir->op2->type;
        }
    }
    switch (ir->type) {
        case K_NOT:
            value = !num1;
            *type = num1_type;
            break;
        case ASSIGN:
            value = num1;
            *type = num1_type;
            break;
        case K_ADD:
            value = num1 + num2;
            break;
        case K_MUL:
            value = num1 * num2;
            break;
        case K_EQ:
            value = (num1 == num2);
            break;
        case K_NEQ:
            value = (num1 != num2);
            break;
        case K_SUB:
            value = (num1 - num2);
            break;
        case K_DIV:
            value = (num1 / num2);
            break;
        case K_MOD:
            value = (num1 % num2);
            break;
        case K_LT:
            value = (num1 < num2);
            break;
        case K_LTE:
            value = (num1 <= num2);
            break;
        case K_GT:
            value = (num1 > num2);
            break;
        case K_GTE:
            value = (num1 >= num2);
            break;
    }
    if (ir->type != ASSIGN && ir->type != K_NOT) {
        if (num1_type == INT && num2_type == INT)
            *type = INT;
        else {
            if (num1_type != INT)
                *type = num1_type;
            else
                *type = num2_type;
        }
    }
    return value;
}

int __is_op_value_const(OPERAND_TYPE* op) {
    int is_op_const = 0;
    if (op->type == GLOBALDATA)
        return 0;
    if (op->type != REGISTER) {
        return 1;
    }

    int* mark = getLinearList(constMark, op->operand.reg_idx);
    if (!mark) {
        struct Definition* def = get_op_definition(op);
        int value;
        int type;
        is_op_const = __is_ir_value_const(def->def_address->ir, &value, &type);
        MALLOC(i, int, 1);
        *i = is_op_const;
        setLinearList(constMark, op->operand.reg_idx, i);
        if (is_op_const) {
            pushFrontDequeList(prop_worklist, op);
            MALLOC(j, int, 1);
            *j = value;
            MALLOC(k, int, 1);
            *k = type;
            setLinearList(constValue, op->operand.reg_idx, j);
            setLinearList(constType, op->operand.reg_idx, k);
        }
        return is_op_const;
    } else
        return *mark;
}

int __is_ir_value_const(IR_TYPE* ir, int* value, int* type) {
    int is_const = 0;
    switch (ir->type) {
        case K_NOT:
        case ASSIGN:
            is_const = __is_op_value_const(ir->op1);
            break;
        case K_ADD:
        case K_MUL:
        case K_EQ:
        case K_NEQ:
        case K_SUB:
        case K_DIV:
        case K_MOD:
        case K_LT:
        case K_LTE:
        case K_GT:
        case K_GTE:
            is_const = __is_op_value_const(ir->op1) && __is_op_value_const(ir->op2);
            break;
        default:
            is_const = 0;
            break;
    }
    if (is_const) {
        *value = caculate_const_value(ir, type);
        return 1;
    }
    return 0;
}

void constFolding(BasicBlock* block, void* not_use) {
    list_entry_t* head = &(block->ir_list->ir_link);
    list_entry_t* t = list_next(head);
    int default_exit;
    while (t != head) {
        Ir* ir = le2struct(t, Ir, ir_link);
        default_exit = 0;
        t = list_next(t);
        if ((!ir->op1 || __is_const_op(ir->op1, 0)) && (!ir->op2 || __is_const_op(ir->op2, 0))) {
            switch (ir->type) {
                case K_ADD:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = ir->op1->operand.v.intValue + ir->op2->operand.v.intValue;

                    break;
                case K_SUB:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = ir->op1->operand.v.intValue - ir->op2->operand.v.intValue;

                    break;
                case K_MUL:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = ir->op1->operand.v.intValue * ir->op2->operand.v.intValue;

                    break;
                case K_DIV:
                    if (ir->op2->operand.v.intValue == 0)
                        break;
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = ir->op1->operand.v.intValue / ir->op2->operand.v.intValue;

                    break;
                case K_MOD:
                    if (ir->op2->operand.v.intValue == 0)
                        break;
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = ir->op1->operand.v.intValue % ir->op2->operand.v.intValue;

                    break;
                case K_NOT:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = !ir->op1->operand.v.intValue;

                    break;
                case K_EQ:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = (ir->op1->operand.v.intValue == ir->op2->operand.v.intValue);

                    break;
                case K_NEQ:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = (ir->op1->operand.v.intValue != ir->op2->operand.v.intValue);

                    break;
                case K_LT:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = (ir->op1->operand.v.intValue < ir->op2->operand.v.intValue);

                    break;
                case K_LTE:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = (ir->op1->operand.v.intValue <= ir->op2->operand.v.intValue);

                    break;
                case K_GT:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = (ir->op1->operand.v.intValue > ir->op2->operand.v.intValue);

                    break;
                case K_GTE:
                    ir->type = ASSIGN;
                    ir->op1->operand.v.intValue = (ir->op1->operand.v.intValue >= ir->op2->operand.v.intValue);

                    break;
                default:
                    default_exit = 1;
            }
            if (!default_exit) {
                delete_operand(ir->op2);
                ir->op2 = NULL;
            }
        }
    }
}

//代数化简
void algebraic_simplification(BasicBlock* block, void* notuse) {
    list_entry_t* head = &(block->ir_list->ir_link);
    list_entry_t* elem = list_next(head);
    while (elem != head) {
        Ir* ir = le2struct(elem, Ir, ir_link);
        elem = list_next(elem);

        if (!ir->op1 || !ir->op2)
            continue;
        if (ir->op1 && ir->op2 && (__is_const_op(ir->op1, 0) != 0 || __is_const_op(ir->op2, 0) != 0))
            switch (ir->type) {
                case K_MUL:
                    if (__is_const_op(ir->op1, 0) == 2 || __is_const_op(ir->op2, 0) == 2) {
                        ir->type = ASSIGN;
                        delete_user(ir->op1, ir);
                        delete_user(ir->op2, ir);
                        delete_operand(ir->op1);
                        delete_operand(ir->op2);
                        ir->op1 = create_new_operand(INT, -1, 0);
                        ir->op2 = NULL;
                    } else if (__is_const_op(ir->op1, 1) == 2) {
                        ir->type = ASSIGN;
                        delete_operand(ir->op1);
                        ir->op1 = ir->op2;
                        ir->op2 = NULL;
                    } else if (__is_const_op(ir->op2, 1) == 2) {
                        ir->type = ASSIGN;
                        delete_operand(ir->op2);
                        ir->op2 = NULL;
                    }
                    break;
                case K_DIV:
                    if (__is_const_op(ir->op2, 0) == 2)
                        break;
                    else if (__is_const_op(ir->op1, 0) == 2) {
                        ir->type = ASSIGN;
                        delete_user(ir->op2, ir);
                        delete_operand(ir->op2);
                        ir->op2 = NULL;
                    } else if (__is_const_op(ir->op2, 1) == 2) {
                        ir->type = ASSIGN;
                        delete_operand(ir->op2);
                        ir->op2 = NULL;
                    }
                    break;
                case K_MOD:
                    if (__is_const_op(ir->op2, 0) == 2) break;
                    if (__is_const_op(ir->op1, 0) == 2) {
                        ir->type = ASSIGN;
                        delete_user(ir->op2, ir);
                        delete_operand(ir->op2);
                        ir->op2 = NULL;
                    }
                    break;
            }
    }
}

int __is_operand_equal(Operand* op, int reg) {
    return op && op->type == REGISTER && op->operand.reg_idx == reg;
}

void getAssignStm_local(BasicBlock* block, void* args) {
    struct DequeList* workList = (struct DequeList*)args;
    list_entry_t* ir_head = &(block->ir_list->ir_link);
    list_entry_t* ir_elem = list_next(ir_head);
    while (ir_head != ir_elem) {
        Ir* ir_value = le2struct(ir_elem, Ir, ir_link);
        if (ir_value->type == ASSIGN)
            pushBackDequeList(workList, ir_value);

        ir_elem = list_next(ir_elem);
    }
}

void getAssignStm_global(BasicBlock* start, void* args) {
    deepTraverseSuccessorsBasicBlock(start, getAssignStm_local, args);
}

void copy_prop_read_op(Ir* ir_value, int reg, Operand* suber, Operand* subor) {
    if (__is_operand_equal(suber, reg)) {
        delete_user(suber, ir_value);
        suber->type = subor->type;
        suber->operand.reg_idx = subor->operand.reg_idx;
        suber->operand.v.intValue = suber->operand.v.intValue;
        add_user(subor, ir_value);
    }
}

void copy_propgation(BasicBlock* start) {
    struct DequeList* workList = newDequeList();
    getAssignStm_global(start, workList);

    while (!isEmptyDequeList(workList)) {
        Ir* process_elem = popFrontDequeList(workList);
        list_entry_t* ir_head = &(get_op_definition(process_elem->op3)->chain->DU_chain);
        list_entry_t* ir_elem = list_next(ir_head);

        Operand* subor = process_elem->op1;
        int reg = process_elem->op3->operand.reg_idx;
        while (!list_empty(ir_head)) {
            ir_elem = list_next(ir_head);
            Ir* ir_value = le2struct(ir_elem, def_use_chain, DU_chain)->user;

#define READ_OP(substituter) copy_prop_read_op(ir_value, reg, substituter, subor);
#define WRITE_OP(substituter)
            IR_OP_READ_WRITE(ir_value, READ_OP, WRITE_OP, PrintErrExit(" "););
#undef READ_OP
#undef WRITE_O
        }
        printf("copy prop\n");
        delete_ir(process_elem, process_elem->block);
    }
}

//substituter 在ir_value中出现
void const_prop_read_op(Ir* ir_value, Operand* substituter, Operand* substitutor, int const_value, int const_type) {
    if (__is_operand_equal(substituter, substitutor->operand.reg_idx)) {
        delete_user(substituter, ir_value);
        substituter->type = const_type;
        substituter->operand.v.intValue = const_value;
    }
}

void const_propgation(Operand* op) {
    Ir* ir = get_op_definition(op)->def_address->ir;
    list_entry_t* du_head = &(get_op_definition(op)->chain->DU_chain);
    list_entry_t* du_elem = list_next(du_head);
    int const_value = *(int*)(getLinearList(constValue, op->operand.reg_idx));
    int const_type = *(int*)(getLinearList(constType, op->operand.reg_idx));
    while (!list_empty(du_head)) {
        du_elem = list_next(du_head);
        Ir* ir_value = le2struct(du_elem, def_use_chain, DU_chain)->user;

#define READ_OP(substituter) const_prop_read_op(ir_value, substituter, op, const_value, const_type);
#define WRITE_OP(substituter)
        IR_OP_READ_WRITE(ir_value, READ_OP, WRITE_OP, PrintErrExit(" "););
#undef READ_OP
#undef WRITE_OP
    }
    printf("const prop\n");
    delete_ir(ir, ir->block);
}

void __mark_const(BasicBlock* block, void* args) {
    list_entry_t* ir_head = &(block->ir_list->ir_link);
    list_entry_t* ir_elem = list_next(ir_head);

    while (ir_head != ir_elem) {
        Ir* ir_value = le2struct(ir_elem, Ir, ir_link);
        if (__is_op3_writable(ir_value))
            __is_op_value_const(ir_value->op3);
        ir_elem = list_next(ir_elem);
    }
}

void alSimplifyAndConstProp(BasicBlock* start) {
    deepTraverseSuccessorsBasicBlock(start, constFolding, NULL);
    deepTraverseSuccessorsBasicBlock(start, algebraic_simplification, NULL);

    deepTraverseSuccessorsBasicBlock(start, __mark_const, NULL);
    while (!isEmptyDequeList(prop_worklist)) {
        Operand* op = popBackDequeList(prop_worklist);
        const_propgation(op);
    }
    deepTraverseSuccessorsBasicBlock(start, algebraic_simplification, NULL);
    deepTraverseSuccessorsBasicBlock(start, constFolding, NULL);
    copy_propgation(start);
}
