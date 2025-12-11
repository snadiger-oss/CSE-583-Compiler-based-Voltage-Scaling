	.file	"test_kernels.cpp"
	.text
	.globl	_Z12memory_boundP4Nodei         # -- Begin function _Z12memory_boundP4Nodei
	.p2align	4
	.type	_Z12memory_boundP4Nodei,@function
_Z12memory_boundP4Nodei:                # @_Z12memory_boundP4Nodei
	.cfi_startproc
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
	.size	_Z12memory_boundP4Nodei, .Lfunc_end0-_Z12memory_boundP4Nodei
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2, 0x0                          # -- Begin function _Z13compute_boundfi
.LCPI1_0:
	.long	0x3f800347                      # float 1.00010002
.LCPI1_1:
	.long	0x399d4952                      # float 3.00000014E-4
.LCPI1_2:
	.long	0x3f80068e                      # float 1.00020003
.LCPI1_3:
	.long	0xb8d1b717                      # float -9.99999974E-5
	.text
	.globl	_Z13compute_boundfi
	.p2align	4
	.type	_Z13compute_boundfi,@function
_Z13compute_boundfi:                    # @_Z13compute_boundfi
	.cfi_startproc
# %bb.0:
	testl	%edi, %edi
	jle	.LBB1_3
# %bb.1:
	movss	.LCPI1_0(%rip), %xmm1           # xmm1 = [1.00010002E+0,0.0E+0,0.0E+0,0.0E+0]
	movss	.LCPI1_1(%rip), %xmm2           # xmm2 = [3.00000014E-4,0.0E+0,0.0E+0,0.0E+0]
	movss	.LCPI1_2(%rip), %xmm3           # xmm3 = [1.00020003E+0,0.0E+0,0.0E+0,0.0E+0]
	movss	.LCPI1_3(%rip), %xmm4           # xmm4 = [-9.99999974E-5,0.0E+0,0.0E+0,0.0E+0]
	.p2align	4
.LBB1_2:                                # =>This Inner Loop Header: Depth=1
	mulss	%xmm1, %xmm0
	addss	%xmm2, %xmm0
	mulss	%xmm3, %xmm0
	addss	%xmm4, %xmm0
	mulss	%xmm0, %xmm0
	decl	%edi
	jne	.LBB1_2
.LBB1_3:
	retq
.Lfunc_end1:
	.size	_Z13compute_boundfi, .Lfunc_end1-_Z13compute_boundfi
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2, 0x0                          # -- Begin function main
.LCPI2_0:
	.long	0x3f800000                      # float 1
	.text
	.globl	main
	.p2align	4
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$80, %rsp
	.cfi_def_cfa_offset 96
	.cfi_offset %rbx, -16
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
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str.1,@object                # @.str.1
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str.1:
	.asciz	"memory_bound result = %d\n"
	.size	.L.str.1, 26

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"compute_bound result = %f\n"
	.size	.L.str.3, 27

	.type	.Lstr,@object                   # @str
.Lstr:
	.asciz	"Running memory_bound..."
	.size	.Lstr, 24

	.type	.Lstr.4,@object                 # @str.4
.Lstr.4:
	.asciz	"Running compute_bound..."
	.size	.Lstr.4, 25

	.ident	"clang version 20.1.8 (https://github.com/llvm/llvm-project.git 87f0227cb60147a26a1eeb4fb06e3b505e9c7261)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
