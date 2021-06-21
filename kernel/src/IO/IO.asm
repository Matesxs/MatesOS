GLOBAL	io_apic_enable
io_apic_enable:
	mov		ecx, 0x1b	; IA32_APIC_BASE_MSR
	rdmsr
	or		eax, 0x800	; IA32_APIC_BASE_MSR_ENABLE
	wrmsr
	ret

GLOBAL io_wait
io_wait:
	out	0x80, al
	ret