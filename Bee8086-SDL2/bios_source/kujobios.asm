; KujoBIOS - source code for BueniaDev's custom Intel 8086 BIOS. Compiles with NASM.
; Copyright 2022 BueniaDev (buenia.mbemu@gmail.com)
;
; Revision 1.00 (alpha)
;
; This work is licensed under the GNU GPL v3. See included LICENSE file.

	cpu	8086

org	100h			; BIOS is loaded at offset 0xF000:0100

; These values (BIOS ID string, BIOS date and so forth) go at the very top of memory

main:
	jmp	bios_entry

biosstr	db	'KujoBIOS Revision 1.00', 0, 0		; Right here!
mem_top	db	0xea, 0, 0x01, 0, 0xf0, '02/02/22', 0, 0xfe, 0

bios_entry:
	; Set up initial stack to F000:F000
	mov sp, 0xF000
	mov ss, sp

	push cs
	pop es

; Main BIOS entry point

; First, Karen-chan sets up the last 16 bytes of memory, including the boot jump, BIOS date and machine ID byte
boot:
	mov ax, 0
	push ax
	popf

	push cs
	push cs
	pop ds
	pop ss
	mov sp, 0xf000

	cld
	mov ax, 0xffff
	mov es, ax
	mov di, 0
	mov si, mem_top
	mov cx, 16
	rep movsb

; Then, she sets up the IVT, first by zeroing out the table...
	cld
	mov ax, 0
	mov es, ax
	mov di, 0
	mov cx, 512
	rep stosw

; ...and then loading in the pointers to the interrupt handlers
	mov di, 0
	mov si, int_table
	mov cx, [itbl_size]
	rep movsb

; Then, she sets up the BIOS data area
	mov ax, 0x40
	mov es, ax
	mov di, 0
	mov si, bios_data
	mov cx, 0x100
	rep movsb

; Lastly, she reads the boot sector code of floppy drive 0, loads it into 0:7C00...
	mov ax, 0
	mov es, ax

	mov ax, 0x201
	mov dh, 0
	mov dl, 0
	mov cx, 1
	mov bx, 0x7c00
	int 13h

; ...and then jumps to that code (at 0:7C00)
	jmp 0:0x7c00

; TODO: Karen-chan needs to implement proper interrupt handlers

; INT 03H - Handler for unimplemented interrupt handlers
int3:
	mov dx, 0x4F8
	out dx, ax
int3_loop:
	jmp int3_loop

int0:
	jmp int0

int1:
int2:
int4:
int5:
int6:
int7:
	iret

int8:
	mov al, 0x8
	int 3

int9:
	mov al, 0x9
	int 3

inta:
	mov al, 0xa
	int 3

intb:
	mov al, 0xb
	int 3

intc:
	mov al, 0xc
	int 3

intd:
	mov al, 0xd
	int 3

inte:
	mov al, 0xe
	int 3
intf:
	mov al, 0xf
	int 3

; INT 10h handler - video services

video_funcs	dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk
		dw int10_unk

int10_unk:
	mov al, 0x10
	int 3

int10:
	cld
	push bp
	push ds
	push cx
	push bx
	push ax
	mov bx, 0x40
	mov ds, bx
	mov bl, [ds:10h]
	and bl, 0x30
	cmp bl, 0x30
	mov bx, 0xB800
	jne dispatch
	mov bx, 0xB000
dispatch:
	push bx
	mov bp, sp
	call int10_dispatch
	pop si
	pop ax
	pop bx
	pop cx
	pop ds
	pop bp
	iret

int10_dispatch:
	cmp ah, 0x0F
	jbe is_ok

invalid:
	mov al, 0x10
	int 3

is_ok:
	mov bl, ah
	shl bl, 1
	mov bh, 0
	jmp [cs:bx + video_funcs]
	ret
	
	

int11:
	mov al, 0x11
	int 3

int12:
	mov al, 0x12
	int 3

int13:
	mov al, 0x13
	int 3

int14:
	mov al, 0x14
	int 3

int15:
	mov al, 0x15
	int 3

int16:
	mov al, 0x16
	int 3

int17:
	mov al, 0x17
	int 3

int18:
	mov al, 0x18
	int 3

int19:
	mov al, 0x19
	int 3

int1a:
	mov al, 0x1a
	int 3

int1b:
	mov al, 0x1b
	int 3

int1c:
	mov al, 0x1c
	int 3

int1d:
	mov al, 0x1d
	int 3

int1e:
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0
	db 0

; Interrupt table
int_table	dw int0
		dw 0xF000
		dw int1
		dw 0xF000
		dw int2
		dw 0xF000
		dw int3
		dw 0xF000
		dw int4
		dw 0xF000
		dw int5
		dw 0xF000
		dw int6
		dw 0xF000
		dw int7
		dw 0xF000
		dw int8
		dw 0xF000
		dw int9
		dw 0xF000
		dw inta
		dw 0xF000
		dw intb
		dw 0xF000
		dw intc
		dw 0xF000
		dw intd
		dw 0xF000
		dw inte
		dw 0xF000
		dw intf
		dw 0xF000
		dw int10
		dw 0xF000
		dw int11
		dw 0xF000
		dw int12
		dw 0xF000
		dw int13
		dw 0xF000
		dw int14
		dw 0xF000
		dw int15
		dw 0xF000
		dw int16
		dw 0xF000
		dw int17
		dw 0xF000
		dw int18
		dw 0xF000
		dw int19
		dw 0xF000
		dw int1a
		dw 0xF000
		dw int1b
		dw 0xF000
		dw int1c
		dw 0xF000
		dw int1d
		dw 0xF000
		dw int1e
itbl_size	dw $-int_table

bios_data:

com1addr	dw	0
com2addr	dw	0
com3addr	dw	0
com4addr	dw	0
lpt1addr	dw	0
lpt2addr	dw	0
lpt3addr	dw	0
lpt4addr	dw	0
equip		dw	0x21
		db	0
memsize		dw	0x280
		db	0
		db	0
keyflags1	db	0
keyflags2	db 	0
		db	0
kbbuf_head	dw	kbbuf - bios_data
kbbuf_tail	dw	kbbuf - bios_data
kbbuf: times 32	db	'X'
drivecal	db	0
diskmotor	db	0
motorshutoff	db	0x07
disk_laststatus	db	0
times 7		db	0
vidmode		db	0x03
vid_cols	dw	80
page_size	dw	0x1000
		dw	0
curpos_x	db	0
curpos_y	db	0
times 7		dw	0
cur_v_end	db	7
cur_v_start	db	6
disp_page	db	0
ending:		times (0xff-($-com1addr)) db	0
