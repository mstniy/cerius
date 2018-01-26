#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "../memory/MemoryManager/MemoryManager.h"

struct IDTDescr {
   uint16_t offset_1; // offset bits 0..15
   uint16_t selector; // a code segment selector in GDT or LDT
   uint8_t ist:3;       // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t zero_1:5;
   uint8_t type:4;
   uint8_t zero_2:1;
   uint8_t dpl:2;
   uint8_t p:1;
   uint16_t offset_2; // offset bits 16..31
   uint32_t offset_3; // offset bits 32..63
   uint32_t zero_3;     // reserved
};

struct SystemSegmentDescriptor
{
    uint16_t limit1;
    uint16_t base1;
    uint8_t  base2;
	uint8_t type : 4;
	uint8_t system : 1; // Must be 0
	uint8_t dpl : 2;
	uint8_t p : 1;
	uint8_t limit2 : 4;
    uint8_t avl : 1;
    uint8_t l : 1; // Must be 0
    uint8_t db : 1; // Must be 0
    uint8_t g : 1;
	uint8_t base3;
	uint32_t base4;
	uint32_t res; // Must be 0
};

struct InterruptFrame
{
    uint64_t rip;
    uint16_t cs;
    uint16_t dummy4;
    uint32_t dummy3;
    uint64_t rflags;
    uint64_t rsp;
    uint16_t ss;
    uint16_t dummy2;
    uint32_t dummy1;
};

// Memory manager must have been initialized!
void SetISRs();

#endif
