.section ".text.vector"
.globl VectorTable

.balign 0x1000

VectorTable:
l:
	b l

.balign 0x80

asm_irq_handler:
	stp   x0,  x1,  [sp, #-16]!
	stp   x2,  x3,  [sp, #-16]!
	stp   x4,  x5,  [sp, #-16]!
	stp   x6,  x7,  [sp, #-16]!
	stp   x8,  x9,  [sp, #-16]!
	stp   x10, x11, [sp, #-16]!
	stp   x12, x13, [sp, #-16]!
	stp   x14, x15, [sp, #-16]!
	stp   x16, x17, [sp, #-16]!
	stp   x18, x19, [sp, #-16]!
	bl    IRQHandler
	ldp   x18, x19, [sp], #16
	ldp   x16, x17, [sp], #16
	ldp   x14, x15, [sp], #16
	ldp   x12, x13, [sp], #16
	ldp   x10, x11, [sp], #16
	ldp   x8,  x9,  [sp], #16
	ldp   x6,  x7,  [sp], #16
	ldp   x4,  x5,  [sp], #16
	ldp   x2,  x3,  [sp], #16
	ldp   x0,  x1,  [sp], #16
	eret

.balign 0x1000
