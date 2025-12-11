	.text
	.type	main,@function
main:                                   # @main
# %bb.0:
	pushq	%rbx
	subq	$80, %rsp
	movl	$1, 8(%rsp)
	leaq	16(%rsp), %rax
	movq	%rax, (%rsp)
	movl	$2, 24(%rsp)
	leaq	32(%rsp), %rax
	movq	%rax, 16(%rsp)
	movl	$3, 40(%rsp)
	leaq	48(%rsp), %rax
	movq	%rax, 32(%rsp)
	movl	$4, 56(%rsp)
	leaq	64(%rsp), %rax
	movq	%rax, 48(%rsp)
	movl	$5, 72(%rsp)
	movq	%rsp, %rbx
	movq	%rbx, 64(%rsp)
	leaq	.Lstr(%rip), %rdi
	callq	puts@PLT
	movq	%rbx, %rdi
	movl	$5, %esi
	callq	_Z12memory_boundP4Nodei
	leaq	.L.str.1(%rip), %rdi
	movl	%eax, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	leaq	.Lstr.4(%rip), %rdi
	callq	puts@PLT
	movss	.LCPI2_0(%rip), %xmm0           # xmm0 = [1.0E+0,0.0E+0,0.0E+0,0.0E+0]
	movl	$5, %edi
	callq	_Z13compute_boundfi
	cvtss2sd	%xmm0, %xmm0
	leaq	.L.str.3(%rip), %rdi
	movb	$1, %al
	callq	printf@PLT
	xorl	%eax, %eax
	addq	$80, %rsp
	popq	%rbx
	retq
.Lfunc_end2:
