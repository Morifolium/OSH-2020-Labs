[BITS 16]                               ; 16 bits program
[ORG 0x7C00]                            ; starts from 0x7c00, where MBR lies in memory

L:
mov si, OSH                             ; si points to string OSH
.print_str:
    lodsb                               ; load char to al
    cmp al, 0                           ; is it the end of the string?
    je .hlt                             ; if true, then halt the system
    mov ah, 0x0e                        ; if false, then set AH = 0x0e 
    int 0x10                            ; call BIOS interrupt procedure, print a char to screen
    jmp .print_str                      ; loop over to print all chars

.hlt:
    hlt

OSH db `Hello, OSH 2020 Lab1!`, 0       ; our string, null-terminated

TIMES 510 - ($ - $$) db 0               ; the size of MBR is 512 bytes, fill remaining bytes to 0
DW 0xAA55                               ; magic number, mark it as a valid bootloader to BIOS
