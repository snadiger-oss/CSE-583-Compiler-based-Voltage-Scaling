	.text
	.type	_Z12memory_boundP4Nodei,@function
_Z12memory_boundP4Nodei:                # @_Z12memory_boundP4Nodei
# %bb.0:
	xorl	%eax, %eax
	testl	%esi, %esi
	jle	.LBB0_2
	.p2align	4
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movq	(%rdi), %rdi
	addl	8(%rdi), %eax
	decl	%esi
	jne	.LBB0_1
.LBB0_2:
	retq
.Lfunc_end0:
