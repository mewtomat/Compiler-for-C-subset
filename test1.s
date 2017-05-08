	.file	1 "test1.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 1
	.abicalls
	.text
	.align	2
	.globl	f
	.ent	f
	.type	f, @function
f:
	.set	nomips16
	.frame	$fp,32,$31		# vars= 16, regs= 1/0, args= 0, gp= 8
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-32
	sw	$fp,28($sp)
	move	$fp,$sp
	.cprestore	0
	move	$3,$4
	li	$2,100			# 0x64
	sw	$2,8($fp)
	li	$2,101			# 0x65
	sw	$2,12($fp)
	li	$2,102			# 0x66
	sw	$2,16($fp)
	lw	$2,8($fp)
	lw	$4,12($fp)
	lw	$5,16($fp)
	sw	$2,0($3)
	sw	$4,4($3)
	sw	$5,8($3)
	move	$2,$3
	move	$sp,$fp
	lw	$fp,28($sp)
	addiu	$sp,$sp,32
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	f
	.align	2
	.globl	g
	.ent	g
	.type	g, @function
g:
	.set	nomips16
	.frame	$fp,40,$31		# vars= 24, regs= 1/0, args= 0, gp= 8
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-40
	sw	$fp,36($sp)
	move	$fp,$sp
	.cprestore	0
	li	$2,1234			# 0x4d2
	sw	$2,24($fp)
	li	$2,23456			# 0x5ba0
	sw	$2,20($fp)
	li	$2,34567			# 0x8707
	sw	$2,16($fp)
	li	$2,45678			# 0xb26e
	sw	$2,12($fp)
	li	$2,56789			# 0xddd5
	sw	$2,8($fp)
	li	$2,1			# 0x1
	move	$sp,$fp
	lw	$fp,36($sp)
	addiu	$sp,$sp,40
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	g
	.rdata
	.align	2
$LC0:
	.ascii	"%d\012\000"
	.text
	.align	2
	.globl	main
	.ent	main
	.type	main, @function
main:
	.set	nomips16
	.frame	$fp,64,$31		# vars= 32, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$31,60($sp)
	sw	$fp,56($sp)
	move	$fp,$sp
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	.cprestore	16
	addiu	$2,$fp,24
	move	$4,$2
	.option	pic0
	jal	f
	nop

	.option	pic2
	lw	$28,16($fp)
	.option	pic0
	jal	g
	nop

	.option	pic2
	lw	$28,16($fp)
	sll	$3,$2,2
	addiu	$2,$fp,24
	addu	$2,$2,$3
	lw	$3,0($2)
	lui	$2,%hi($LC0)
	addiu	$4,$2,%lo($LC0)
	move	$5,$3
	lw	$25,%call16(printf)($28)
	nop
	jalr	$25
	nop

	lw	$28,16($fp)
	move	$sp,$fp
	lw	$31,60($sp)
	lw	$fp,56($sp)
	addiu	$sp,$sp,64
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.ident	"GCC: (Debian 4.3.5-4) 4.3.5"
