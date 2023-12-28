	.file	"swap.i"
	.text
	.globl	swap
	.type	swap, @function
swap:
.LFB0:
	.cfi_startproc
	movq	$buf+4, bufp1(%rip)
	movq	bufp0(%rip), %rax
	movl	(%rax), %edx
	movl	buf+4(%rip), %ecx
	movl	%ecx, (%rax)
	movl	%edx, buf+4(%rip)
	ret
	.cfi_endproc
.LFE0:
	.size	swap, .-swap
	.local	bufp1
	.comm	bufp1,8,8
	.globl	bufp0
	.data
	.align 8
	.type	bufp0, @object
	.size	bufp0, 8
bufp0:
	.quad	buf
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-16)"
	.section	.note.GNU-stack,"",@progbits
