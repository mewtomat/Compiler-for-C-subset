.data
S1: .asciiz "\n"
.text
.globl main
.ent main
mtc1 $0,$f0

main:
		#calle work = setting up space for fp, locals,other registers
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
li $7, 212
sub $sp, $sp, $7
sw $ra, 176($sp)
sw $s0, 172($sp)
sw $s1, 168($sp)
sw $s2, 164($sp)
sw $s3, 160($sp)
sw $s4, 156($sp)
sw $s5, 152($sp)
sw $s6, 148($sp)
sw $s7, 144($sp)
sw $t0, 140($sp)
sw $t1, 136($sp)
sw $t2, 132($sp)
sw $t3, 128($sp)
sw $t4, 124($sp)
sw $t5, 120($sp)
sw $t6, 116($sp)
sw $t7, 112($sp)
sw $t8, 108($sp)
sw $t9, 104($sp)
s.s $f1, 100($sp)
s.s $f2, 96($sp)
s.s $f3, 92($sp)
s.s $f4, 88($sp)
s.s $f5, 84($sp)
s.s $f6, 80($sp)
s.s $f7, 76($sp)
s.s $f8, 72($sp)
s.s $f9, 68($sp)
s.s $f10, 64($sp)
s.s $f11, 60($sp)
s.s $f13, 56($sp)
s.s $f14, 52($sp)
s.s $f15, 48($sp)
s.s $f16, 44($sp)
s.s $f17, 40($sp)
s.s $f18, 36($sp)
s.s $f19, 32($sp)
s.s $f20, 28($sp)
s.s $f21, 24($sp)
s.s $f22, 20($sp)
s.s $f23, 16($sp)
s.s $f24, 12($sp)
s.s $f25, 8($sp)
s.s $f26, 4($sp)
s.s $f27, 0($sp)
		#done setting up space for fp, locals,other registers
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -4
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 1
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -8
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 2
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -12
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 3
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -16
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 4
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -20
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 5
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -24
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 6
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -28
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 7
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:assign_astnode
		#generate_code:identifier_astnode
li $t0, -32
add $t0, $t0, $fp
		#/generate_code:identifier_astnode
		#generate_code:intconst_astnode
li $t1, 8
		#/generate_code:intconst_astnode
sw $t1, 0($t0) 
		#/generate_code:assign_astnode
		#generate_code:printf
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t0, -4
add $t0, $t0, $fp
lw $t0, 0($t0)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t1, -8
add $t1, $t1, $fp
lw $t1, 0($t1)
		#/generate_code:identifier_astnode
add $t0, $t0, $t1
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t1, -12
add $t1, $t1, $fp
lw $t1, 0($t1)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t2, -16
add $t2, $t2, $fp
lw $t2, 0($t2)
		#/generate_code:identifier_astnode
add $t1, $t1, $t2
		#/generate_code:op_binary_astnode
add $t0, $t0, $t1
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t1, -20
add $t1, $t1, $fp
lw $t1, 0($t1)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t2, -24
add $t2, $t2, $fp
lw $t2, 0($t2)
		#/generate_code:identifier_astnode
add $t1, $t1, $t2
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t2, -32
add $t2, $t2, $fp
lw $t2, 0($t2)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t3, -28
add $t3, $t3, $fp
lw $t3, 0($t3)
		#/generate_code:identifier_astnode
add $t2, $t2, $t3
		#/generate_code:op_binary_astnode
add $t1, $t1, $t2
		#/generate_code:op_binary_astnode
add $t0, $t0, $t1
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t1, -4
add $t1, $t1, $fp
lw $t1, 0($t1)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t2, -8
add $t2, $t2, $fp
lw $t2, 0($t2)
		#/generate_code:identifier_astnode
add $t1, $t1, $t2
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t2, -12
add $t2, $t2, $fp
lw $t2, 0($t2)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t3, -16
add $t3, $t3, $fp
lw $t3, 0($t3)
		#/generate_code:identifier_astnode
add $t2, $t2, $t3
		#/generate_code:op_binary_astnode
add $t1, $t1, $t2
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t2, -20
add $t2, $t2, $fp
lw $t2, 0($t2)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t3, -24
add $t3, $t3, $fp
lw $t3, 0($t3)
		#/generate_code:identifier_astnode
add $t2, $t2, $t3
		#/generate_code:op_binary_astnode
		#generate_code:op_binary_astnode
		#generate_code:identifier_astnode
li $t3, -32
add $t3, $t3, $fp
lw $t3, 0($t3)
		#/generate_code:identifier_astnode
		#generate_code:identifier_astnode
li $t4, -28
add $t4, $t4, $fp
lw $t4, 0($t4)
		#/generate_code:identifier_astnode
add $t3, $t3, $t4
		#/generate_code:op_binary_astnode
add $t2, $t2, $t3
		#/generate_code:op_binary_astnode
add $t1, $t1, $t2
		#/generate_code:op_binary_astnode
add $t0, $t0, $t1
		#/generate_code:op_binary_astnode
li $v0, 1
move $a0, $t0
syscall
la $t0, S1
li $v0, 4
move $a0, $t0
syscall
		#/generate_code:printf
EXITmain:
		#callee clearing up stack
l.s $f27, 0($sp)
l.s $f26, 4($sp)
l.s $f25, 8($sp)
l.s $f24, 12($sp)
l.s $f23, 16($sp)
l.s $f22, 20($sp)
l.s $f21, 24($sp)
l.s $f20, 28($sp)
l.s $f19, 32($sp)
l.s $f18, 36($sp)
l.s $f17, 40($sp)
l.s $f16, 44($sp)
l.s $f15, 48($sp)
l.s $f14, 52($sp)
l.s $f13, 56($sp)
l.s $f11, 60($sp)
l.s $f10, 64($sp)
l.s $f9, 68($sp)
l.s $f8, 72($sp)
l.s $f7, 76($sp)
l.s $f6, 80($sp)
l.s $f5, 84($sp)
l.s $f4, 88($sp)
l.s $f3, 92($sp)
l.s $f2, 96($sp)
l.s $f1, 100($sp)
lw $t9, 104($sp)
lw $t8, 108($sp)
lw $t7, 112($sp)
lw $t6, 116($sp)
lw $t5, 120($sp)
lw $t4, 124($sp)
lw $t3, 128($sp)
lw $t2, 132($sp)
lw $t1, 136($sp)
lw $t0, 140($sp)
lw $s7, 144($sp)
lw $s6, 148($sp)
lw $s5, 152($sp)
lw $s4, 156($sp)
lw $s3, 160($sp)
lw $s2, 164($sp)
lw $s1, 168($sp)
lw $s0, 172($sp)
lw $ra, 176($sp)
li $7, 212
add $sp, $sp, $7
lw $fp, 0($sp)
addi $sp, $sp , 4
j $ra
		#callee's clearing done
.end main


