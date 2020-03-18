# 实验报告

### 构建初始内存盘

思路：init为启动后首个执行文件，利用busybox，使用shell脚本构建设备文件并执行可执行文件。

```
#!/bin/sh
mknod dev/ttyS0 c 4 64
mknod dev/fb0 c 29 0
./1
./2
./3
./5
/bin/sh
```



### 使用汇编语言，编写 x86 裸金属 (bare-metal) 程序 

思路：利用8254 计时器计时，在助教提供的代码基础上增加循环，输出五次。细节可见注释

```.asm
[BITS 16]                                      ; 16 bits program
[ORG 0x7C00]                            ; starts from 0x7c00, where MBR lies in memory

L:
    sti
    mov ah, 15                              ; clear the screen
    int 10h
    mov ah, 0
    int 10h

    mov bh, 0
.stt1:
    inc bh
    mov si, OSH                            ; si points to string OSH

.print_str:
    mov ch,0
    lodsb                                         ; load char to al
    cmp al, 0                                  ; is it the end of the string?
    je .stt3                                       ; if true, then halt the system
    mov ah, 0x0e                          ; if false, then set AH = 0x0e 
    int 0x10                                     ; call BIOS interrupt procedure, print a char to screen
    jmp .print_str                         ; loop over to print all chars

.stt3:                                               ; time control
    inc ch 
    mov cl, [0x046c]                    ; read [0x046c]

.cy:
    cmp cl, [0x046c]                    ; and check if the same
    je .cy
    cmp ch, 19
    je  .stt2
    jmp .stt3

.stt2:
    cmp bh, 5                                   ; control cycle
    je .hlt
    jmp .stt1

.hlt:
    hlt


OSH db `Hello, OSH 2020 Lab1!`, 0dh, 0ah, 0       ; our string, null-terminated

TIMES 510 - ($ - $$) db 0               ; the size of MBR is 512 bytes, fill remaining bytes to 0
DW 0xAA55  
```




### 思考题

1.目前，越来越多的 PC 使用 UEFI 启动。请简述使用 UEFI 时系统启动的流程，并与传统 BIOS 的启动流程比较。

```
UEFI启动流程：
1. 系统开机 - 上电自检（Power On Self Test 或 POST）。
2. UEFI 固件被加载，并由它初始化启动要用的硬件。
3. 固件读取其引导管理器以确定从何处（比如，从哪个硬盘及分区）加载哪个 UEFI 应用。
4. 固件按照引导管理器中的启动项目，加载UEFI 应用。
5. 已启动的 UEFI 应用还可以启动其他应用（对应于 UEFI shell 或 rEFInd 之类的引导管理器的情况）或者启动内核及initramfs（对应于GRUB之类引导器的情况），这取决于 UEFI 应用的配置

UEFI与BIOS区别
​	BIOS，就是上电自检后，跑完POST，会去读取启动设备的 0磁	道 1扇区 上面的前512字节（MBR）的数据。这就是我们常说的启动	扇区，boot sector，BIOS会看最后两个字节是不是55 AA，如果是	就用一条jmp指令跳过去就是了。而UEFI引导则是基于文件系统，	boot的时候，加载启动设备上面的uefi loader。
```



2.MBR 能够用于编程的部分只有 510 字节，而目前的系统引导器（如 GRUB2）可以实现很多复杂的功能：如从不同的文件系统中读取文件、引导不同的系统启动、提供美观的引导选择界面等，用 510 字节显然是无法做到这些功能的。它们是怎么做到的？

```
	grub从最简单的mbr中的代码加载其后的更大的一点的代码而可以识别“文件系统”，然后跳转到具体的“文件系统”中，执行grub命令，加载存在于具体“文件系统”中的内核和内存盘
```



3.为什么我们编写 C 语言版本的 init 程序在编译时需要静态链接？我们能够在这里使用动态链接吗

```
不能，因为Linux运行环境当中是不带动态库的，程序可能无法正常运行。
```



4.在使用 `make menuconfig` 调整 Linux 编译选项的时候，你会看到有些选项是使用 `[M]` 标记的。它们是什么意思？在你的 `init` 启动之前的整个流程中它们会被加载吗？如果不，在正常的 Linux 系统中，它们是怎么被加载的？

```
即将开发完成的设备驱动程序文件(C、H文件)直接用gcc工具（或交叉工具链）进行编译，然后通过insmod（插入内核）或者（rmmod）从内核卸载的方式进行使用或删除。在init 启动之前的整个流程中它们不会被加载，在做最后的产品时，一般会通过写一个脚本文件，使得系统在上电的时候能够自动的insmod相关驱动程序。
```

