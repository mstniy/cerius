#ifndef MOVESTACK_H
#define MOVESTACK_H

// Moves the stack to specified location. Changes rbp such that rbp-rsp will not change.
// Copies the first *stackMoveSize* many bytes from the old stack into the new stack.
// Since this function isn't smart enough to change stack-relative pointers that currently exist on the stack (rbp's in particular), it needs to be called directly from bootloader_64bit_stage
extern "C" __attribute((sysv_abi)) void MoveStack(void* newStackTop, uint64_t stackMoveSize);

#endif
