	.globl _isqrt

	.text
_isqrt:	
	fsrra fr4
	rts	
	fmov  fr4,fr0

	.end

