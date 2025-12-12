	.text
	.type	main,@function
main:                                   # @main
# %bb.0:
	pushq	%rax
	callq	run
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end1:
