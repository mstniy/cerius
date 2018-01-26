#include "interrupt.h"

#include "kout.h"

extern "C" void irq_pre_handler_0(); // We must have a seperate handler for each interrupt because the processor doesn't pass which interrupt caused the function to be called.
extern "C" void irq_pre_handler_1();
extern "C" void irq_pre_handler_2();
extern "C" void irq_pre_handler_3();
extern "C" void irq_pre_handler_4();
extern "C" void irq_pre_handler_5();
extern "C" void irq_pre_handler_6();
extern "C" void irq_pre_handler_7();
extern "C" void irq_pre_handler_8();
extern "C" void irq_pre_handler_9();
extern "C" void irq_pre_handler_10();
extern "C" void irq_pre_handler_11();
extern "C" void irq_pre_handler_12();
extern "C" void irq_pre_handler_13();
extern "C" void irq_pre_handler_14();
extern "C" void irq_pre_handler_15();
extern "C" void irq_pre_handler_16();
extern "C" void irq_pre_handler_17();
extern "C" void irq_pre_handler_18();
extern "C" void irq_pre_handler_19();
extern "C" void irq_pre_handler_20();
extern "C" void irq_pre_handler_21();
extern "C" void irq_pre_handler_22();
extern "C" void irq_pre_handler_23();
extern "C" void irq_pre_handler_24();
extern "C" void irq_pre_handler_25();
extern "C" void irq_pre_handler_26();
extern "C" void irq_pre_handler_27();
extern "C" void irq_pre_handler_28();
extern "C" void irq_pre_handler_29();
extern "C" void irq_pre_handler_30();
extern "C" void irq_pre_handler_31();
extern "C" void irq_pre_handler_32();
extern "C" void irq_pre_handler_33();
extern "C" void irq_pre_handler_34();
extern "C" void irq_pre_handler_35();
extern "C" void irq_pre_handler_36();
extern "C" void irq_pre_handler_37();
extern "C" void irq_pre_handler_38();
extern "C" void irq_pre_handler_39();
extern "C" void irq_pre_handler_40();
extern "C" void irq_pre_handler_41();
extern "C" void irq_pre_handler_42();
extern "C" void irq_pre_handler_43();
extern "C" void irq_pre_handler_44();
extern "C" void irq_pre_handler_45();
extern "C" void irq_pre_handler_46();
extern "C" void irq_pre_handler_47();
extern "C" void irq_pre_handler_48();
extern "C" void irq_pre_handler_49();
extern "C" void irq_pre_handler_50();
extern "C" void irq_pre_handler_51();
extern "C" void irq_pre_handler_52();
extern "C" void irq_pre_handler_53();
extern "C" void irq_pre_handler_54();
extern "C" void irq_pre_handler_55();
extern "C" void irq_pre_handler_56();
extern "C" void irq_pre_handler_57();
extern "C" void irq_pre_handler_58();
extern "C" void irq_pre_handler_59();
extern "C" void irq_pre_handler_60();
extern "C" void irq_pre_handler_61();
extern "C" void irq_pre_handler_62();
extern "C" void irq_pre_handler_63();
extern "C" void irq_pre_handler_64();
extern "C" void irq_pre_handler_65();
extern "C" void irq_pre_handler_66();
extern "C" void irq_pre_handler_67();
extern "C" void irq_pre_handler_68();
extern "C" void irq_pre_handler_69();
extern "C" void irq_pre_handler_70();
extern "C" void irq_pre_handler_71();
extern "C" void irq_pre_handler_72();
extern "C" void irq_pre_handler_73();
extern "C" void irq_pre_handler_74();
extern "C" void irq_pre_handler_75();
extern "C" void irq_pre_handler_76();
extern "C" void irq_pre_handler_77();
extern "C" void irq_pre_handler_78();
extern "C" void irq_pre_handler_79();
extern "C" void irq_pre_handler_80();
extern "C" void irq_pre_handler_81();
extern "C" void irq_pre_handler_82();
extern "C" void irq_pre_handler_83();
extern "C" void irq_pre_handler_84();
extern "C" void irq_pre_handler_85();
extern "C" void irq_pre_handler_86();
extern "C" void irq_pre_handler_87();
extern "C" void irq_pre_handler_88();
extern "C" void irq_pre_handler_89();
extern "C" void irq_pre_handler_90();
extern "C" void irq_pre_handler_91();
extern "C" void irq_pre_handler_92();
extern "C" void irq_pre_handler_93();
extern "C" void irq_pre_handler_94();
extern "C" void irq_pre_handler_95();
extern "C" void irq_pre_handler_96();
extern "C" void irq_pre_handler_97();
extern "C" void irq_pre_handler_98();
extern "C" void irq_pre_handler_99();
extern "C" void irq_pre_handler_100();
extern "C" void irq_pre_handler_101();
extern "C" void irq_pre_handler_102();
extern "C" void irq_pre_handler_103();
extern "C" void irq_pre_handler_104();
extern "C" void irq_pre_handler_105();
extern "C" void irq_pre_handler_106();
extern "C" void irq_pre_handler_107();
extern "C" void irq_pre_handler_108();
extern "C" void irq_pre_handler_109();
extern "C" void irq_pre_handler_110();
extern "C" void irq_pre_handler_111();
extern "C" void irq_pre_handler_112();
extern "C" void irq_pre_handler_113();
extern "C" void irq_pre_handler_114();
extern "C" void irq_pre_handler_115();
extern "C" void irq_pre_handler_116();
extern "C" void irq_pre_handler_117();
extern "C" void irq_pre_handler_118();
extern "C" void irq_pre_handler_119();
extern "C" void irq_pre_handler_120();
extern "C" void irq_pre_handler_121();
extern "C" void irq_pre_handler_122();
extern "C" void irq_pre_handler_123();
extern "C" void irq_pre_handler_124();
extern "C" void irq_pre_handler_125();
extern "C" void irq_pre_handler_126();
extern "C" void irq_pre_handler_127();
extern "C" void irq_pre_handler_128();
extern "C" void irq_pre_handler_129();
extern "C" void irq_pre_handler_130();
extern "C" void irq_pre_handler_131();
extern "C" void irq_pre_handler_132();
extern "C" void irq_pre_handler_133();
extern "C" void irq_pre_handler_134();
extern "C" void irq_pre_handler_135();
extern "C" void irq_pre_handler_136();
extern "C" void irq_pre_handler_137();
extern "C" void irq_pre_handler_138();
extern "C" void irq_pre_handler_139();
extern "C" void irq_pre_handler_140();
extern "C" void irq_pre_handler_141();
extern "C" void irq_pre_handler_142();
extern "C" void irq_pre_handler_143();
extern "C" void irq_pre_handler_144();
extern "C" void irq_pre_handler_145();
extern "C" void irq_pre_handler_146();
extern "C" void irq_pre_handler_147();
extern "C" void irq_pre_handler_148();
extern "C" void irq_pre_handler_149();
extern "C" void irq_pre_handler_150();
extern "C" void irq_pre_handler_151();
extern "C" void irq_pre_handler_152();
extern "C" void irq_pre_handler_153();
extern "C" void irq_pre_handler_154();
extern "C" void irq_pre_handler_155();
extern "C" void irq_pre_handler_156();
extern "C" void irq_pre_handler_157();
extern "C" void irq_pre_handler_158();
extern "C" void irq_pre_handler_159();
extern "C" void irq_pre_handler_160();
extern "C" void irq_pre_handler_161();
extern "C" void irq_pre_handler_162();
extern "C" void irq_pre_handler_163();
extern "C" void irq_pre_handler_164();
extern "C" void irq_pre_handler_165();
extern "C" void irq_pre_handler_166();
extern "C" void irq_pre_handler_167();
extern "C" void irq_pre_handler_168();
extern "C" void irq_pre_handler_169();
extern "C" void irq_pre_handler_170();
extern "C" void irq_pre_handler_171();
extern "C" void irq_pre_handler_172();
extern "C" void irq_pre_handler_173();
extern "C" void irq_pre_handler_174();
extern "C" void irq_pre_handler_175();
extern "C" void irq_pre_handler_176();
extern "C" void irq_pre_handler_177();
extern "C" void irq_pre_handler_178();
extern "C" void irq_pre_handler_179();
extern "C" void irq_pre_handler_180();
extern "C" void irq_pre_handler_181();
extern "C" void irq_pre_handler_182();
extern "C" void irq_pre_handler_183();
extern "C" void irq_pre_handler_184();
extern "C" void irq_pre_handler_185();
extern "C" void irq_pre_handler_186();
extern "C" void irq_pre_handler_187();
extern "C" void irq_pre_handler_188();
extern "C" void irq_pre_handler_189();
extern "C" void irq_pre_handler_190();
extern "C" void irq_pre_handler_191();
extern "C" void irq_pre_handler_192();
extern "C" void irq_pre_handler_193();
extern "C" void irq_pre_handler_194();
extern "C" void irq_pre_handler_195();
extern "C" void irq_pre_handler_196();
extern "C" void irq_pre_handler_197();
extern "C" void irq_pre_handler_198();
extern "C" void irq_pre_handler_199();
extern "C" void irq_pre_handler_200();
extern "C" void irq_pre_handler_201();
extern "C" void irq_pre_handler_202();
extern "C" void irq_pre_handler_203();
extern "C" void irq_pre_handler_204();
extern "C" void irq_pre_handler_205();
extern "C" void irq_pre_handler_206();
extern "C" void irq_pre_handler_207();
extern "C" void irq_pre_handler_208();
extern "C" void irq_pre_handler_209();
extern "C" void irq_pre_handler_210();
extern "C" void irq_pre_handler_211();
extern "C" void irq_pre_handler_212();
extern "C" void irq_pre_handler_213();
extern "C" void irq_pre_handler_214();
extern "C" void irq_pre_handler_215();
extern "C" void irq_pre_handler_216();
extern "C" void irq_pre_handler_217();
extern "C" void irq_pre_handler_218();
extern "C" void irq_pre_handler_219();
extern "C" void irq_pre_handler_220();
extern "C" void irq_pre_handler_221();
extern "C" void irq_pre_handler_222();
extern "C" void irq_pre_handler_223();
extern "C" void irq_pre_handler_224();
extern "C" void irq_pre_handler_225();
extern "C" void irq_pre_handler_226();
extern "C" void irq_pre_handler_227();
extern "C" void irq_pre_handler_228();
extern "C" void irq_pre_handler_229();
extern "C" void irq_pre_handler_230();
extern "C" void irq_pre_handler_231();
extern "C" void irq_pre_handler_232();
extern "C" void irq_pre_handler_233();
extern "C" void irq_pre_handler_234();
extern "C" void irq_pre_handler_235();
extern "C" void irq_pre_handler_236();
extern "C" void irq_pre_handler_237();
extern "C" void irq_pre_handler_238();
extern "C" void irq_pre_handler_239();
extern "C" void irq_pre_handler_240();
extern "C" void irq_pre_handler_241();
extern "C" void irq_pre_handler_242();
extern "C" void irq_pre_handler_243();
extern "C" void irq_pre_handler_244();
extern "C" void irq_pre_handler_245();
extern "C" void irq_pre_handler_246();
extern "C" void irq_pre_handler_247();
extern "C" void irq_pre_handler_248();
extern "C" void irq_pre_handler_249();
extern "C" void irq_pre_handler_250();
extern "C" void irq_pre_handler_251();
extern "C" void irq_pre_handler_252();
extern "C" void irq_pre_handler_253();
extern "C" void irq_pre_handler_254();
extern "C" void irq_pre_handler_255();

void PrintInterruptFrame(InterruptFrame* frame)
{
	kout << "rip: " << (void*)frame->rip << " cs: " << frame->cs << "\n";
	kout << "rsp: " << (void*)frame->rsp << " ss: " << frame->ss << "\n";
	kout << "rflags: " << (void*)frame->rflags << "\n";
}

void __attribute((sysv_abi)) divide_by_zero(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Divide by zero";
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) invalid_opcode(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Invalid opcode";
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) double_fault(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Double fault";
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) invalid_tss(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Invalid tss";
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) segment_not_present(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Segment not present";
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) stack_exception(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Stack exception";
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) general_protection(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: General protection\n";
	kout << "Top (to bottom) of the previous stack: ";
	for (int i=0;i<8;i++)
        kout << (void*)*(uint64_t*)(frame->rsp+8*i) << " ";
	PrintInterruptFrame(frame);
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) page_fault(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Page fault\n";
	PrintInterruptFrame(frame);
	kout << "Ecode: " << errorCode << "\n";
	void* cr2=nullptr;
	asm volatile("mov %%cr2, %0"
		:"=rm"(cr2));
	kout << "cr2: " << cr2;
	asm volatile("cli\n\thlt");
}

void __attribute((sysv_abi)) simd_exception(InterruptFrame* frame, int irq, uint64_t errorCode)
{
	kout << "ISR: Simd exception";
	asm volatile("cli\n\thlt");
}

using ISRoutine = void(__attribute((sysv_abi)) *)(InterruptFrame* frame, int irq, uint64_t errorCode);

ISRoutine IRQHandlers[21]={
	divide_by_zero, nullptr, nullptr, nullptr, nullptr, nullptr,
	invalid_opcode, nullptr, double_fault, nullptr, invalid_tss,
	segment_not_present, stack_exception, general_protection, page_fault,
	nullptr, nullptr, nullptr, nullptr, simd_exception, nullptr
};

extern "C" void __attribute((sysv_abi)) ExternalIrqHandler(InterruptFrame* frame, int irq)
{
	kout << "IST: External irq (" << irq << ")\n";
}

uint32_t tss[26];

void SetISRs()
{
	void(*irq_pre_handlers[256])()={
		irq_pre_handler_0,irq_pre_handler_1,irq_pre_handler_2,
		irq_pre_handler_3,irq_pre_handler_4,irq_pre_handler_5,
		irq_pre_handler_6,irq_pre_handler_7,irq_pre_handler_8,
		irq_pre_handler_9,irq_pre_handler_10,irq_pre_handler_11,
		irq_pre_handler_12,irq_pre_handler_13,irq_pre_handler_14,
		irq_pre_handler_15,irq_pre_handler_16,irq_pre_handler_17,
		irq_pre_handler_18,irq_pre_handler_19,irq_pre_handler_20,
		irq_pre_handler_21,irq_pre_handler_22,irq_pre_handler_23,
		irq_pre_handler_24,irq_pre_handler_25,irq_pre_handler_26,
		irq_pre_handler_27,irq_pre_handler_28,irq_pre_handler_29,
		irq_pre_handler_30,irq_pre_handler_31,irq_pre_handler_32,
		irq_pre_handler_33,irq_pre_handler_34,irq_pre_handler_35,
		irq_pre_handler_36,irq_pre_handler_37,irq_pre_handler_38,
		irq_pre_handler_39,irq_pre_handler_40,irq_pre_handler_41,
		irq_pre_handler_42,irq_pre_handler_43,irq_pre_handler_44,
		irq_pre_handler_45,irq_pre_handler_46,irq_pre_handler_47,
		irq_pre_handler_48,irq_pre_handler_49,irq_pre_handler_50,
		irq_pre_handler_51,irq_pre_handler_52,irq_pre_handler_53,
		irq_pre_handler_54,irq_pre_handler_55,irq_pre_handler_56,
		irq_pre_handler_57,irq_pre_handler_58,irq_pre_handler_59,
		irq_pre_handler_60,irq_pre_handler_61,irq_pre_handler_62,
		irq_pre_handler_63,irq_pre_handler_64,irq_pre_handler_65,
		irq_pre_handler_66,irq_pre_handler_67,irq_pre_handler_68,
		irq_pre_handler_69,irq_pre_handler_70,irq_pre_handler_71,
		irq_pre_handler_72,irq_pre_handler_73,irq_pre_handler_74,
		irq_pre_handler_75,irq_pre_handler_76,irq_pre_handler_77,
		irq_pre_handler_78,irq_pre_handler_79,irq_pre_handler_80,
		irq_pre_handler_81,irq_pre_handler_82,irq_pre_handler_83,
		irq_pre_handler_84,irq_pre_handler_85,irq_pre_handler_86,
		irq_pre_handler_87,irq_pre_handler_88,irq_pre_handler_89,
		irq_pre_handler_90,irq_pre_handler_91,irq_pre_handler_92,
		irq_pre_handler_93,irq_pre_handler_94,irq_pre_handler_95,
		irq_pre_handler_96,irq_pre_handler_97,irq_pre_handler_98,
		irq_pre_handler_99,irq_pre_handler_100,irq_pre_handler_101,
		irq_pre_handler_102,irq_pre_handler_103,irq_pre_handler_104,
		irq_pre_handler_105,irq_pre_handler_106,irq_pre_handler_107,
		irq_pre_handler_108,irq_pre_handler_109,irq_pre_handler_110,
		irq_pre_handler_111,irq_pre_handler_112,irq_pre_handler_113,
		irq_pre_handler_114,irq_pre_handler_115,irq_pre_handler_116,
		irq_pre_handler_117,irq_pre_handler_118,irq_pre_handler_119,
		irq_pre_handler_120,irq_pre_handler_121,irq_pre_handler_122,
		irq_pre_handler_123,irq_pre_handler_124,irq_pre_handler_125,
		irq_pre_handler_126,irq_pre_handler_127,irq_pre_handler_128,
		irq_pre_handler_129,irq_pre_handler_130,irq_pre_handler_131,
		irq_pre_handler_132,irq_pre_handler_133,irq_pre_handler_134,
		irq_pre_handler_135,irq_pre_handler_136,irq_pre_handler_137,
		irq_pre_handler_138,irq_pre_handler_139,irq_pre_handler_140,
		irq_pre_handler_141,irq_pre_handler_142,irq_pre_handler_143,
		irq_pre_handler_144,irq_pre_handler_145,irq_pre_handler_146,
		irq_pre_handler_147,irq_pre_handler_148,irq_pre_handler_149,
		irq_pre_handler_150,irq_pre_handler_151,irq_pre_handler_152,
		irq_pre_handler_153,irq_pre_handler_154,irq_pre_handler_155,
		irq_pre_handler_156,irq_pre_handler_157,irq_pre_handler_158,
		irq_pre_handler_159,irq_pre_handler_160,irq_pre_handler_161,
		irq_pre_handler_162,irq_pre_handler_163,irq_pre_handler_164,
		irq_pre_handler_165,irq_pre_handler_166,irq_pre_handler_167,
		irq_pre_handler_168,irq_pre_handler_169,irq_pre_handler_170,
		irq_pre_handler_171,irq_pre_handler_172,irq_pre_handler_173,
		irq_pre_handler_174,irq_pre_handler_175,irq_pre_handler_176,
		irq_pre_handler_177,irq_pre_handler_178,irq_pre_handler_179,
		irq_pre_handler_180,irq_pre_handler_181,irq_pre_handler_182,
		irq_pre_handler_183,irq_pre_handler_184,irq_pre_handler_185,
		irq_pre_handler_186,irq_pre_handler_187,irq_pre_handler_188,
		irq_pre_handler_189,irq_pre_handler_190,irq_pre_handler_191,
		irq_pre_handler_192,irq_pre_handler_193,irq_pre_handler_194,
		irq_pre_handler_195,irq_pre_handler_196,irq_pre_handler_197,
		irq_pre_handler_198,irq_pre_handler_199,irq_pre_handler_200,
		irq_pre_handler_201,irq_pre_handler_202,irq_pre_handler_203,
		irq_pre_handler_204,irq_pre_handler_205,irq_pre_handler_206,
		irq_pre_handler_207,irq_pre_handler_208,irq_pre_handler_209,
		irq_pre_handler_210,irq_pre_handler_211,irq_pre_handler_212,
		irq_pre_handler_213,irq_pre_handler_214,irq_pre_handler_215,
		irq_pre_handler_216,irq_pre_handler_217,irq_pre_handler_218,
		irq_pre_handler_219,irq_pre_handler_220,irq_pre_handler_221,
		irq_pre_handler_222,irq_pre_handler_223,irq_pre_handler_224,
		irq_pre_handler_225,irq_pre_handler_226,irq_pre_handler_227,
		irq_pre_handler_228,irq_pre_handler_229,irq_pre_handler_230,
		irq_pre_handler_231,irq_pre_handler_232,irq_pre_handler_233,
		irq_pre_handler_234,irq_pre_handler_235,irq_pre_handler_236,
		irq_pre_handler_237,irq_pre_handler_238,irq_pre_handler_239,
		irq_pre_handler_240,irq_pre_handler_241,irq_pre_handler_242,
		irq_pre_handler_243,irq_pre_handler_244,irq_pre_handler_245,
		irq_pre_handler_246,irq_pre_handler_247,irq_pre_handler_248,
		irq_pre_handler_249,irq_pre_handler_250,irq_pre_handler_251,
		irq_pre_handler_252,irq_pre_handler_253,irq_pre_handler_254,
		irq_pre_handler_255
	};

	uint64_t intStack = uint64_t(MemoryManager::alloc(1*1024*1024))+1*1024*1024; // Stack grows downwards in memory.
    IDTDescr* idt = (IDTDescr*)MemoryManager::alloc(4096);
    for (int i=0;i<256;i++)
	{
		idt[i].offset_1 = uint64_t(irq_pre_handlers[i]);
		idt[i].selector = 8;
		idt[i].ist = 1;
		idt[i].zero_1=0;
        idt[i].type = 14; // 64-bit interrupt gate
        idt[i].zero_2=0;
        idt[i].dpl = 0;
        idt[i].p = 1;
		idt[i].offset_2 = uint64_t(irq_pre_handlers[i])>>16;
		idt[i].offset_3 = uint64_t(irq_pre_handlers[i])>>32;
		idt[i].zero_3 = 0;
	}
	SystemSegmentDescriptor* sDes;
	uint8_t gdtr[10];
	uint8_t idtr[10];
	*(uint16_t*)(&idtr[0])=256*sizeof(IDTDescr)-1;
	*(uint64_t*)(&idtr[2])=(uint64_t)idt;
	asm volatile("sgdt %0"
		: "=m"(gdtr));
	kassert(*(uint16_t*)&gdtr[0] == 39);
	sDes = (SystemSegmentDescriptor*)((*(uint64_t*)(uint64_t(gdtr)+2))+24);
	sDes->limit1 = sizeof(tss)-1;
	sDes->base1 = uint64_t(tss);
	sDes->base2 = uint64_t(tss)>>16;
    sDes->type = 9; // Available TSS
    sDes->system = 0;
    sDes->dpl = 0;
    sDes->p = 1;
    sDes->limit2 = 0;
    sDes->l = 0;
    sDes->db = 0;
    sDes->g = 0;
    sDes->base3 = uint64_t(tss)>>24;
    sDes->base4 = uint64_t(tss)>>32;
    sDes->res = 0;
	for (int i=0;i<26;i++)
		tss[i] = 0; // TODO: Do we need to set these to some other value?
	tss[9]=intStack;
	tss[10]=intStack>>32;
    uint16_t dummy=24;
    asm volatile("ltr %w0\n\t"
				 "lidt %1"
		:
		: "rm"(dummy), "m"(idtr)
		);
}
