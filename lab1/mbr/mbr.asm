[BITS 16]                               ; 16 bits program
[ORG 0x7C00]                            ; starts from 0x7c00, where MBR lies in memory

L:
    sti
    mov ah, 15                          ; clear the screen
    int 10h
    mov ah, 0
    int 10h

    mov bh, 0
.stt1:
    inc bh
    mov si, OSH                         ; si points to string OSH

.print_str:
    mov ch,0
    lodsb                               ; load char to al
    cmp al, 0                           ; is it the end of the string?
    je .stt3                            ; if true, then halt the system
    mov ah, 0x0e                        ; if false, then set AH = 0x0e 
    int 0x10                            ; call BIOS interrupt procedure, print a char to screen
    jmp .print_str                      ; loop over to print all chars

.stt3:                                  ; time control
    inc ch
    mov cl, [0x046c]                    ; read [0x046c]

.cy:
    cmp cl, [0x046c]                    ; and check if the same
    je .cy
    cmp ch, 19
    je  .stt2
    jmp .stt3

.stt2:
    cmp bh, 5                           ; control cycle
    je .hlt
    jmp .stt1

.hlt:
    hlt


OSH db `Hello, OSH 2020 Lab1!`, 0dh, 0ah, 0       ; our string, null-terminated

TIMES 510 - ($ - $$) db 0               ; the size of MBR is 512 bytes, fill remaining bytes to 0
DW 0xAA55  