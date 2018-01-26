[bits 16]

; This function trashes registers!
; WORD disk_load_low(BYTE disk, BYTE head, BYTE cylinder, BYTE startsec, BYTE lengthsec, WORD segment, WORD offset);
; Returns in ax 1 for success, 0 for failure.
disk_load_low:
	push bp
	mov bp, sp

	;movzx dx, byte [bp+4]
	;call print_hex

	;movzx dx, byte [bp+6]
	;call print_hex

	;movzx dx, byte [bp+8]
	;call print_hex

	;movzx dx, byte [bp+10]
	;call print_hex

	;mov dx, [bp+12]
	;call print_hex
	
	;mov dx, [bp+14]
	;call print_hex
	
	;mov dx, [bp+16]
	;call print_hex


	mov ax, [bp+14]
	mov es, ax ; segment
	mov bx, [bp+16] ; offset
	mov ah, 2 ; BIOS read sector function
	mov dl, [bp+4] ; Disk
	mov dh, [bp+6] ; Head
	mov ch, [bp+8] ; cylinder
	mov cl, [bp+10] ; startsec
	mov al, [bp+12] ; lengthsec
	int 0x13 ; BIOS interrupt
	jc dloadlow_disk_error ; Jump if error (i.e. carry flag set)
	cmp al, [bp+12] ; if AL (sectors read) != lengthsec
	jne dloadlow_disk_error
	mov ax, 1 ; success
	jmp dloadlow_suc_chk_end
	dloadlow_disk_error:
	mov ax, 0 ; failure
	dloadlow_suc_chk_end:
	  leave
	  ret 14

; Assumes offset is 512-aligned.
; WORD disk_load(const BYTE disk, const WORD startlsec, const WORD lengthlsec, const WORD segment, const WORD offset);
; This function trashes registers.
disk_load:

	push bp
	mov bp, sp

	sub sp, 18 ; [bp-2] : endlsec // [bp-4] : curlsec // [bp-6] : number of heads // [bp-8] : number of cylinders // [bp-10] : number of sectors per track || Loop local variables [bp-12] : curhead // [bp-14] : curcyl // [bp-16] : curpsec // [bp-18] : sectors_to_read

	mov ax, [bp+6]
	add ax, [bp+8]
	mov [bp-2], ax ; endlsec = startlsec + lengthlsec
	mov ax, [bp+6]
	mov [bp-4], ax ; curlsec = startlsec

	mov dl, [bp+4]
	xor ax, ax
	mov es, ax
	xor di, di
	mov ah, 0x08
	int 0x13 ; Read drive parameters
	jc disk_load_failed

	mov byte [bp-6], dh
	mov byte [bp-5], 0
	inc word [bp-6] ; number of heads

	mov byte [bp-8], ch
	mov byte [bp-7], cl
	shr byte [bp-7], 6
	inc word [bp-8] ; number of cylinders

	mov word [bp-10], cx
	and word [bp-10], 111111b ; number of sectors per track


	;mov dx, [bp-6]
	;call print_hex
	;mov dx, [bp-8]
	;call print_hex
	;mov dx, [bp-10]
	;call print_hex

	disk_load_read_loop:

	mov ax, [bp-4]
	cmp ax, [bp-2] ; cmp (curlsec < endlsec)
	jae disk_load_read_loop_end

	mov ax, [bp-4]
	mov bx, [bp-10]
	mov dx, 0
	div bx ; curlsec / SEC_PER_TRK, Stores quotient in AX, remainder in DX
	mov [bp-16], dx
	inc word [bp-16]; curpsec = lsec_to_cyl(curlsec) [= curlsec % SEC_PER_TRK + 1] (Sector numbers are 1-indexed)
	mov bx, [bp-6]
	mov dx, 0
	div bx
	mov [bp-14], ax ; curcyl  = lsec_to_head(curlsec) [= curlsec / SEC_PER_TRK / NUM_HEAD]
	mov [bp-12], dx ; curhead = lsec_to_cyl(curlsec)  [= curlsec / SEC_PER_TRK % NUM_HEAD]
	
	; We need to read min(# of sectors that fit into rest of the current segment , # of sectors in the current track, # of lsectors we still need to read) many sectors from the current track.
	mov ax, [bp-10]
	sub ax, [bp-16]
	inc ax ; SEC_PER_TRK - curpsec + 1 (Sector numbers are 1-indexed)
	mov bx, [bp-2]
	sub bx, [bp-4] ; endlsec - curlsec
	call wmin ; compute min, put result in ax
	mov ebx, 0x10000
	movzx ecx, word [bp+12]
	sub ebx, ecx
	shr ebx, 9 ; (0x10000 - offset)/512 ; At this point, ebx == bx
	call wmin
	mov [bp-18], ax

	push word [bp+12] ; offset
	push word [bp+10] ; segment
	push word [bp-18] ; sec_to_read
	push word [bp-16] ; curpsec
	push word [bp-14] ; curcyl
	push word [bp-12] ; curhead
	push word [bp+4]  ; disk
	call disk_load_low
	cmp ax, 0
	je disk_load_failed
	mov ax, [bp-18]
	add [bp-4], ax ; curlsec += sec_to_read
	movzx eax, word [bp-18]
	shl eax, 9
	movzx ebx, word [bp+12]
	add eax, ebx
	mov word [bp+12], ax ; offset += sec_to_read * 512
	cmp eax, 0x10000
	jb disk_load_read_loop ; If we filled the current segment, switch to the 4096-next segment.
	add word [bp+10], 4096 ; In real mode, each "segment" is 16 bytes long. 4096*16=65536
	jmp disk_load_read_loop

	disk_load_failed:
	  mov ax, 0
	  leave
	  ret 10
	disk_load_read_loop_end:
	  mov ax, 1
	  leave
	  ret 10


;FLOPPY_NUM_HEADS equ 2
;FLOPPY_NUM_CYL equ 80
;FLOPPY_SEC_PER_TRK equ 18
;FLOPPY_BYTE_PER_SEC equ 512