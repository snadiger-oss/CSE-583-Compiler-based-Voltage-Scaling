	.text
	.type	_Z13compute_boundfi,@function
_Z13compute_boundfi:                    # @_Z13compute_boundfi
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
