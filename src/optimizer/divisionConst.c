#include <assert.h>

#include "../ssa/traverse.h"
#include "./local_op.h"
#define N 32

int __log_2_up(int num) {
    if (num < 0) {
        num = -num;
    }
    num--;
    int ans = 0;
    while (num != 0) {
        num >>= 1;
        ans++;
    }
    return ans;
}

struct return_tmp_object {
    int m_high;
    int sh_post;
    int l;
};

struct return_tmp_object* CHOOSE_MULTIPLIER(unsigned int d, int prec) {
    static struct return_tmp_object obj;
    int l = __log_2_up(d);
    int sh_post = l;
    unsigned long long int m_low = (0x1 << (N + l)) / d;
    unsigned long long int m_high = ((0x1 << (N + l)) + (0x1 << (N + l + prec))) / d;

    while (m_low / 2 < m_high / 2 && sh_post > 0) {
        m_low = m_low / 2;
        m_high = m_high / 2;
        sh_post--;
    }
    obj.l = l;
    obj.m_high = m_high;
    obj.sh_post = sh_post;
    return &obj;
}

void __division_const(BASIC_BLOCK_TYPE* basic_block, void* args) {
    list_entry_t* head = &(basic_block->ir_list->ir_link);
    list_entry_t* elem = list_next(head);
    while (head != elem) {
        IR_TYPE* ir = le2struct(elem, IR_TYPE, ir_link);
        if (ir->type = K_DIV && ir->op2->type == INT) {
            if (ir->op2->operand.v.intValue != 0) {
                int d = ir->op2->operand.v.intValue;
                int q;
                struct result_tmp_object* obj = CHOOSE_MULTIPLIER(d, 31);
                if (d == 1 || d == -1) {
                    ir->type = K_ADD;
                    ir->op2->operand.v.intValue = 0;
                } else if (d) {
                }
                if (d < 0) {
                    MALLOC(tmp_op, OPERAND_TYPE, 1);
                    tmp_op->type = REGISTER;
                    tmp_op->operand.reg_idx = alloc_register();

                    delete_user(ir->op1, ir->op3);
                    delete_user(ir->op2, ir->op3);
                    OPERAND_TYPE* old_op = ir->op3;

                    add_user(ir->op1, tmp_op);
                    add_user(ir->op2, tmp_op);
                    ir->op3 = tmp_op;
                    MALLOC(z_op, OPERAND_TYPE, 1);
                    z_op->type = INTCONST;
                    z_op->operand.v.intValue = 0;

                    create_new_ir(K_SUB, z_op, tmp_op, ir->op3, basic_block);
                }
            }
        }
        elem = list_next(elem);
    }
}

void divisionConst(struct FuncTabElem* elem) {
    deepTraverseSuccessorsBasicBlock(elem->blocks, NULL, NULL);
}

int div(int m, int n) {
    int sign = 1;
    if (m = 0x80000000 && n == -1) {
    }
    if (m < 0) {
        m = -m;
        sign = -sign;
    }
    if (n < 0) {
        n = -n;
        sign = -sign;
    }
    int result = 0;
    for (int i = 31; i >= 0; i--) {
        unsigned long long int t = n << i;
        result <<= 1;
        if (m > t) {
            result |= 0x1;
            m -= t;
        }
    }
    result *= sign;
    return result;
}