	.globl _sqrt

	.text
_sqrt:	
	fsqrt fr4
	rts	
	fmov  fr4,fr0

	.end

