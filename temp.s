.text
main:
addi t0, $fp, -4
li t1, 4
sw t1, 0(t0) 
lw t0, 0(t0)
addi t0, $fp, -8
li t1, 9
sw t1, 0(t0) 
lw t0, 0(t0)
addi t0, $fp, -12
lw t1, -4($fp)
lw t2, -8($fp)
add t1, t1, t2
sw t1, 0(t0) 
lw t0, 0(t0)
