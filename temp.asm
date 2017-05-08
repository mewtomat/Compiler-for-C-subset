registers.pop5.7 printf.asm
Using syscalls for output can quickly become tedious, and output routines can quickly
muddy up even the neatest code, since it requires several assembly instructions just
to print out a number. To make matters worse, there is no syscall which prints out a
single ASCII character.
To help my own coding, I wrote the following printf function, which behaves
like a simplified form of the printf function in the standard C library. It implements
only a fraction of the functionality of the real printf, but enough to be useful. See
the comments in the code for more information.
1 ## Daniel J. Ellard -- 03/13/94
2 ## printf.asm--
3 ## an implementation of a simple printf work-alike.
4
5 ## printf--
6 ## A simple printf-like function. Understands just the basic forms
7 ## of the %s, %d, %c, and %% formats, and can only have 3 embedded
8 ## formats (so that all of the parameters are passed in registers).
9 ## If there are more than 3 embedded formats, all but the first 3 are
10 ## completely ignored (not even printed).
11 ## Register Usage:
12 ## $a0,$s0 - pointer to format string
13 ## $a1,$s1 - format argument 1 (optional)
14 ## $a2,$s2 - format argument 2 (optional)
15 ## $a3,$s3 - format argument 3 (optional)
16 ## $s4 - count of formats processed.
17 ## $s5 - char at $s4.
18 ## $s6 - pointer to printf buffer
19 ##
20 .text
21 .globl printf
22 printf:
23 subu $sp, $sp, 36 # set up the stack frame,
24 sw $ra, 32($sp) # saving the local environment.
25 sw $fp, 28($sp)
26 sw $s0, 24($sp)
27 sw $s1, 20($sp)
28 sw $s2, 16($sp)
29 sw $s3, 12($sp)
30 sw $s4, 8($sp)
31 sw $s5, 4($sp)
5.7. PRINTF.ASM 81
32 sw $s6, 0($sp)
33 addu $fp, $sp, 36
34
35 # grab the arguments:
36 move $s0, $a0 # fmt string
37 move $s1, $a1 # arg1 (optional)
38 move $s2, $a2 # arg2 (optional)
39 move $s3, $a3 # arg3 (optional)
40
41 li $s4, 0 # set # of formats = 0
42 la $s6, printf_buf # set s6 = base of printf buffer.
43
44 printf_loop: # process each character in the fmt:
45 lb $s5, 0($s0) # get the next character, and then
46 addu $s0, $s0, 1 # bump up $s0 to the next character.
47
48 beq $s5, ’%’, printf_fmt # if the fmt character, then do fmt.
49 beq $0, $s5, printf_end # if zero, then go to end.
50
51 printf_putc:
52 sb $s5, 0($s6) # otherwise, just put this char
53 sb $0, 1($s6) # into the printf buffer,
54 move $a0, $s6 # and then print it with the
55 li $v0, 4 # print_str syscall
56 syscall
57
58 b printf_loop # loop on.
59
60 printf_fmt:
61 lb $s5, 0($s0) # see what the fmt character is,
62 addu $s0, $s0, 1 # and bump up the pointer.
63
64 beq $s4, 3, printf_loop # if we’ve already processed 3 args,
65 # then *ignore* this fmt.
66 beq $s5, ’d’, printf_int # if ’d’, print as a decimal integer.
67 beq $s5, ’s’, printf_str # if ’s’, print as a string.
68 beq $s5, ’c’, printf_char # if ’c’, print as a ASCII char.
69 beq $s5, ’%’, printf_perc # if ’%’, print a ’%’
70 b printf_loop # otherwise, just continue.
71
72 printf_shift_args: # shift over the fmt args,
73 move $s1, $s2 # $s1 = $s2
74 move $s2, $s3 # $s2 = $s3
75
82 CHAPTER 5. MIPS ASSEMBLY CODE EXAMPLES
76 add $s4, $s4, 1 # increment # of args processed.
77
78 b printf_loop # and continue the main loop.
79
80 printf_int: # deal with a %d:
81 move $a0, $s1 # do a print_int syscall of $s1.
82 li $v0, 1
83 syscall
84 b printf_shift_args # branch to printf_shift_args
85
86 printf_str: # deal with a %s:
87 move $a0, $s1 # do a print_string syscall of $s1.
88 li $v0, 4
89 syscall
90 b printf_shift_args # branch to printf_shift_args
91
92 printf_char: # deal with a %c:
93 sb $s1, 0($s6) # fill the buffer in with byte $s1,
94 sb $0, 1($s6) # and then a null.
95 move $a0, $s6 # and then do a print_str syscall
96 li $v0, 4 # on the buffer.
97 syscall
98 b printf_shift_args # branch to printf_shift_args
99
100 printf_perc: # deal with a %%:
101 li $s5, ’%’ # (this is redundant)
102 sb $s5, 0($s6) # fill the buffer in with byte %,
103 sb $0, 1($s6) # and then a null.
104 move $a0, $s6 # and then do a print_str syscall
105 li $v0, 4 # on the buffer.
106 syscall
107 b printf_loop # branch to printf_loop
108
109 printf_end:
110 lw $ra, 32($sp) # restore the prior environment:
111 lw $fp, 28($sp)
112 lw $s0, 24($sp)
113 lw $s1, 20($sp)
114 lw $s2, 16($sp)
115 lw $s3, 12($sp)
116 lw $s4, 8($sp)
117 lw $s5, 4($sp)
118 lw $s6, 0($sp)
119 addu $sp, $sp, 36 # release the stack frame.
5.7. PRINTF.ASM 83
120 jr $ra
# return.
121
122 .data
123 printf_buf: .space
2
124
125 ## end of printf.asm