.section ".text.vector"
.globl VectorTable

.balign 0x1000

VectorTable:

// Current EL with SP0

.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #1
	b			asm_sync
.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #1
	b			asm_irq
.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #1
	b			asm_fiq
.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #1
	b			asm_serr

// Current EL with SPx

.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #2
	b			asm_sync
.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #2
	b			asm_irq
.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #2
	b			asm_fiq
.balign 0x80
	stp   x0,  x1,  [sp, #-16]!
	mov		w1, #2
	b			asm_serr

// Lower EL AArch64

.balign 0x80
lela64_sync:
	b			lela64_sync
.balign 0x80
lela64_irq:
	b			lela64_irq
.balign 0x80
lela64_fiq:
	b			lela64_fiq
.balign 0x80
lela64_serr:
	b			lela64_serr

// Lower EL AArch32

.balign 0x80
lela32_sync:
	b			lela32_sync
.balign 0x80
lela32_irq:
	b			lela32_irq
.balign 0x80
lela32_fiq:
	b			lela32_fiq
.balign 0x80
lela32_serr:
	b			lela32_serr

.balign 0x100
asm_sync:
	stp   x2,  x3,  [sp, #-16]!
	stp   x4,  x5,  [sp, #-16]!
	stp   x6,  x7,  [sp, #-16]!
	stp   x8,  x9,  [sp, #-16]!
	stp   x10, x11, [sp, #-16]!
	stp   x12, x13, [sp, #-16]!
	stp   x14, x15, [sp, #-16]!
	stp   x16, x17, [sp, #-16]!
	stp   x18, x19, [sp, #-16]!
	stp   x20, x21, [sp, #-16]!
	stp   x22, x23, [sp, #-16]!
	stp   x24, x25, [sp, #-16]!
	stp   x26, x27, [sp, #-16]!
	stp   x28, x29, [sp, #-16]!
	mrs		x0,	elr_el1
	stp   x30, x0, [sp, #-16]!

	bl		SyncHandler

	ldp   x30, x0, [sp], #16
  msr		elr_el1, x0
	ldp   x28, x29, [sp], #16
	ldp   x26, x27, [sp], #16
	ldp   x24, x25, [sp], #16
	ldp   x22, x23, [sp], #16
	ldp   x20, x21, [sp], #16
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
loop_sync:
	b			loop_sync


.balign 0x100
asm_irq:
	stp   x2,  x3,  [sp, #-16]!
	stp   x4,  x5,  [sp, #-16]!
	stp   x6,  x7,  [sp, #-16]!
	stp   x8,  x9,  [sp, #-16]!
	stp   x10, x11, [sp, #-16]!
	stp   x12, x13, [sp, #-16]!
	stp   x14, x15, [sp, #-16]!
	stp   x16, x17, [sp, #-16]!
	stp   x18, x19, [sp, #-16]!
	stp   x20, x21, [sp, #-16]!
	stp   x22, x23, [sp, #-16]!
	stp   x24, x25, [sp, #-16]!
	stp   x26, x27, [sp, #-16]!
	stp   x28, x29, [sp, #-16]!
	mrs		x0,	elr_el1
	stp   x30, x0, [sp, #-16]!

	bl    IRQHandler

	ldp   x30, x0, [sp], #16
	msr		elr_el1, x0
	ldp   x28, x29, [sp], #16
	ldp   x26, x27, [sp], #16
	ldp   x24, x25, [sp], #16
	ldp   x22, x23, [sp], #16
	ldp   x20, x21, [sp], #16
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

.balign 0x100
asm_fiq:
	stp   x2,  x3,  [sp, #-16]!
	stp   x4,  x5,  [sp, #-16]!
	stp   x6,  x7,  [sp, #-16]!
	stp   x8,  x9,  [sp, #-16]!
	stp   x10, x11, [sp, #-16]!
	stp   x12, x13, [sp, #-16]!
	stp   x14, x15, [sp, #-16]!
	stp   x16, x17, [sp, #-16]!
	stp   x18, x19, [sp, #-16]!
	stp   x20, x21, [sp, #-16]!
	stp   x22, x23, [sp, #-16]!
	stp   x24, x25, [sp, #-16]!
	stp   x26, x27, [sp, #-16]!
	stp   x28, x29, [sp, #-16]!
	mrs		x0,	elr_el1
	stp   x30, x0, [sp, #-16]!

	bl    FIQHandler

	ldp   x30, x0, [sp], #16
	msr		elr_el1, x0
	ldp   x28, x29, [sp], #16
	ldp   x26, x27, [sp], #16
	ldp   x24, x25, [sp], #16
	ldp   x22, x23, [sp], #16
	ldp   x20, x21, [sp], #16
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

.balign 0x100
asm_serr:
	stp   x2,  x3,  [sp, #-16]!
	stp   x4,  x5,  [sp, #-16]!
	stp   x6,  x7,  [sp, #-16]!
	stp   x8,  x9,  [sp, #-16]!
	stp   x10, x11, [sp, #-16]!
	stp   x12, x13, [sp, #-16]!
	stp   x14, x15, [sp, #-16]!
	stp   x16, x17, [sp, #-16]!
	stp   x18, x19, [sp, #-16]!
	stp   x20, x21, [sp, #-16]!
	stp   x22, x23, [sp, #-16]!
	stp   x24, x25, [sp, #-16]!
	stp   x26, x27, [sp, #-16]!
	stp   x28, x29, [sp, #-16]!
	mrs		x0,	elr_el1
	stp   x30, x0, [sp, #-16]!

	bl    SerrorHandler

	ldp   x30, x0, [sp], #16
	msr		elr_el1, x0
	ldp   x28, x29, [sp], #16
	ldp   x26, x27, [sp], #16
	ldp   x24, x25, [sp], #16
	ldp   x22, x23, [sp], #16
	ldp   x20, x21, [sp], #16
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

