	.file	1 "test.c"
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
	.frame	$fp,64,$31		# vars= 48, regs= 1/0, args= 0, gp= 8
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	
	addiu	$sp,$sp,-64
	sw	$fp,60($sp)
	move	$fp,$sp
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	.cprestore	0
	sw	$4,64($fp)
	lwc1	$f0,12($fp)
	nop
	cvt.s.w	$f2,$f0
	lwc1	$f0,32($fp)
	nop
	add.s	$f0,$f2,$f0
	.set	macro
	trunc.w.s $f0,$f0,$2
	.set	nomacro
	swc1	$f0,20($fp)
	lui	$2,%hi($LC0)
	lwc1	$f0,%lo($LC0)($2)
	nop
	swc1	$f0,32($fp)
	lui	$2,%hi($LC1)
	lwc1	$f0,%lo($LC1)($2)
	nop
	swc1	$f0,24($fp)
	lwc1	$f2,24($fp)
	mtc1	$0,$f0
	nop
	c.eq.s	$f2,$f0
	nop
	bc1f	$L2
	nop

	lwc1	$f2,32($fp)
	mtc1	$0,$f0
	nop
	c.eq.s	$f2,$f0
	nop
	bc1t	$L7
	nop

$L2:
	li	$3,1			# 0x1
	sw	$3,40($fp)
	b	$L5
	nop

$L7:
	sw	$0,40($fp)
$L5:
	lwc1	$f2,40($fp)
	nop
	cvt.s.w	$f0,$f2
	swc1	$f0,28($fp)
	li	$2,3			# 0x3
	sw	$2,20($fp)
	lw	$2,20($fp)
	nop
	sw	$2,48($fp)
	lw	$3,48($fp)
	nop
	sll	$2,$3,1
	sw	$2,48($fp)
	lw	$3,48($fp)
	nop
	sll	$2,$3,2
	lw	$3,48($fp)
	nop
	subu	$3,$2,$3
	mtc1	$3,$f0
	nop
	cvt.s.w	$f0,$f0
	swc1	$f0,32($fp)
	sw	$0,32($fp)
	move	$sp,$fp
	lw	$fp,60($sp)
	addiu	$sp,$sp,64
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.rdata
	.align	2
$LC0:
	.word	1073741824
	.align	2
$LC1:
	.word	1082130432
	.ident	"GCC: (Debian 4.3.5-4) 4.3.5"
