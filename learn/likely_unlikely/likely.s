	.file	"likely.c"
	.text
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"Positive"
.LC1:
	.string	"Zero or Negative"
	.text
	.p2align 4
	.globl	print
	.type	print, @function
print:
.LFB51:
	.cfi_startproc
	endbr64
	testl	%edi, %edi
	jle	.L2
	leaq	.LC0(%rip), %rdi
	jmp	puts@PLT
	.p2align 4,,10
	.p2align 3
.L2:
	leaq	.LC1(%rip), %rdi
	jmp	puts@PLT
	.cfi_endproc
.LFE51:
	.size	print, .-print
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC2:
	.string	"Please provide a positive/zero/negative number. : "
	.section	.rodata.str1.1
.LC3:
	.string	"%d"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB52:
	.cfi_startproc
	endbr64
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	leaq	.LC2(%rip), %rsi
	movl	$1, %edi
	movq	%fs:40, %rax
	movq	%rax, 8(%rsp)
	xorl	%eax, %eax
	call	__printf_chk@PLT
	leaq	.LC3(%rip), %rdi
	xorl	%eax, %eax
	leaq	4(%rsp), %rsi
	call	__isoc99_scanf@PLT
	movl	4(%rsp), %edi
	call	print
	movq	8(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L7
	xorl	%eax, %eax
	addq	$24, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L7:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE52:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 12.3.0-1ubuntu1~23.04) 12.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
