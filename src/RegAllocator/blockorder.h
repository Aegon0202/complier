#ifndef REGALLOCATOR_BLOCK_ORDER_H
#define REGALLOCATOR_BLOCK_ORDER_H

#include "../ssa/ssa.h"
#include "../utils/DequeList.h"

typedef struct BlockBegin {
    BasicBlock* block;
    int loop_index;
    int loop_depth;
} BlockBegin;

typedef struct BlockBeginNode {
    BlockBegin* value;
    list_entry_t link;
} BlockBeginNode;

//nimen
void computeLoopInfo(BlockBegin* func_entry_block);
//successor 是一个blockbegin*
struct DequeList* getSuccessors(BlockBegin* block);

//wo
struct LinearList* computeBlockOrder(BlockBegin* func_entry_block);
void insertBlock(BlockBegin* block, list_entry_t* work_list_head);
void numberLirOp(struct LinearList* blocks);
#endif