//
// Created by tomly on 08/09/2026.
//

#include <stdint.h>
#include <stm32f401re_interrupts.h>

#define NUM_CORE_EXCEPTIONS (16)
#define IRQ_VECTOR_INDEX(irq) ((irq) + (NUM_CORE_EXCEPTIONS))

#define WEAK_HANDLER(handler) void handler(void) __attribute__((weak, alias("Default_Handler")))

int main(void);
static void Reset_Handler(void);
static void Default_Handler(void);

WEAK_HANDLER(NMI_Handler);
WEAK_HANDLER(HardFault_Handler);
WEAK_HANDLER(MMFault_Handler);
WEAK_HANDLER(BusFault_Handler);
WEAK_HANDLER(UsageFault_Handler);
WEAK_HANDLER(SVCall_Handler);
WEAK_HANDLER(DebugMon_Handler);
WEAK_HANDLER(PendSV_Handler);
WEAK_HANDLER(SysTick_Handler);
WEAK_HANDLER(EXTI0_Handler);
WEAK_HANDLER(EXTI1_Handler);
WEAK_HANDLER(EXTI2_Handler);
WEAK_HANDLER(EXTI3_Handler);
WEAK_HANDLER(EXTI4_Handler);
WEAK_HANDLER(EXTI9_5_Handler);
WEAK_HANDLER(EXTI15_10_Handler);

extern uint32_t _stack_top[];
extern uint32_t _data_loadaddr[];

extern uint32_t _data[];
extern uint32_t _edata[];
extern uint32_t _bss[];
extern uint32_t _ebss[];

__attribute__ ((section(".isr_vector")))
__attribute__ ((used)) // Force emission of this object despite not being used in this file
const uint32_t isr_vector[] =
{
	(uint32_t)_stack_top,
	(uint32_t)Reset_Handler,
	(uint32_t)NMI_Handler,
	(uint32_t)HardFault_Handler,
	(uint32_t)MMFault_Handler,
	(uint32_t)BusFault_Handler,
	(uint32_t)UsageFault_Handler,
	(uint32_t)0, // Reserved
	(uint32_t)0, // Reserved
	(uint32_t)0, // Reserved
	(uint32_t)0, // Reserved
	(uint32_t)SVCall_Handler,
	(uint32_t)DebugMon_Handler,
	(uint32_t)0, // Reserved
	(uint32_t)PendSV_Handler,
	(uint32_t)SysTick_Handler,

	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_0)]     = (uint32_t)EXTI0_Handler,
	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_1)]     = (uint32_t)EXTI1_Handler,
	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_2)]     = (uint32_t)EXTI2_Handler,
	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_3)]     = (uint32_t)EXTI3_Handler,
	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_4)]     = (uint32_t)EXTI4_Handler,
	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_9_5)]   = (uint32_t)EXTI9_5_Handler,
	[IRQ_VECTOR_INDEX(IRQ_EXTI_LINE_15_10)] = (uint32_t)EXTI15_10_Handler,
};

void Reset_Handler(void)
{
	uint32_t num_words = _edata - _data;
	for (int i = 0; i < num_words; i++)
	{
		_data[i] = _data_loadaddr[i];
	}

	num_words = _ebss - _bss;
	for (int i = 0; i < num_words; i++)
	{
		_bss[i] = 0;
	}

	main();

	while (1);
}

void Default_Handler(void)
{
	while (1);
}
