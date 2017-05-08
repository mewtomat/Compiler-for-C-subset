	.file	1 "order.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 1
	.abicalls
	.text
	.align	2
	.globl	main
	.ent	main
	.type	main, @function
main:
	.set	nomips16
	.frame	$fp,56,$31		# vars= 40, regs= 1/0, args= 0, gp= 8
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-56
	sw	$fp,52($sp)
	move	$fp,$sp
	.cprestore	0
	lw	$3,36($fp)
	li	$2,5			# 0x5
	bne	$3,$2,$L2
	nop

	li	$2,3			# 0x3
	sw	$2,36($fp)
	b	$L5
	nop

$L2:
	li	$2,4			# 0x4
	sw	$2,36($fp)
$L5:
	move	$sp,$fp
	lw	$fp,52($sp)
	addiu	$sp,$sp,56
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.ident	"GCC: (Debian 4.3.5-4) 4.3.5"
