/* --------------------------- hd-booter bs.s file -------------------------- */
    BOOSEG = 0x9800
    SSP    = 32*1024    ! 32KB bss + stack may be adjusted
    .globl _main, _prints, _dap, _dp, _bsector, _vm_gdt    ! IMPORT
    .globl _diskr,_getc,_putc,_getds,_setds,               ! EXPORT
    .globl _cp2himem,_jmp_setup
! MBR loaded at 0x07C0. Load entire booter to 0x9800
start;  mov ax, #BOOTSEG
    mov es, ax
    xor bx, bx  ! clear BX = 0
    mov dx, #0x0080 ! head 0, hd
    xor cx, cx 
    incb cl     ! cyl 0, sector 1
    incb cl 
    mov ax, #0x0220 ! READ 32 sectors, booter size up to 16KB
    int 0x13
! far jump to (0x9800, next) to continue execution there
    jmpi next, BOOSEG   ! CS=BOOTSEG, IP=next
next:
    mov ax, cs  ! set CPU segment registers
    mov ds, ax  ! we know ES, CS=0x9800. Let DS = CS
    mov ss, ax  
    mov es, ax  ! CS = DS = SS = ES = 0x9800
    mov sp, #SSP ! 32 KB stack
    call _main  ! call main() in CPU
    test ax, ax ! check return value from main()
    je error        ! main() return 0 if error
    jmpi 0x7C00, 0x0000 ! otherwise, as a chain booter
_diskr:
    mov dx, #0x0080     ! drive=0x80 for hd
    mov ax, #0x4200    
    mov si, #_dap
    int 0x13        ! call BIOS int13-42 read the block
    jb error        ! to error if carryBit is on
    ret
error:
    mov bx, #bad
    push bx
    call _prints
    int 0x19        ! reboot
bad:    .asciz  "\n\rError!\n\r"
_jmp_setup:
    mov ax, 0x9000  ! for SETUP in 2.6 kernel
    mov ds, ax      ! DS must point at 0x9000
    jmpi 0, 0x9020  ! jmpi to execute SETUP at 0x9020

! for each batch of k<=16 blocks, load to RM=0x10000 (at most 64KB) then call cp2himem() to copy it to VM=0x100000 + k*4096
_cp2himem:
    push bp
    mov  bp, sp     
    mov  cx, 4[bp]  ! words to copy (32*1024 or less)
    mov  si, #_vm_gdt
    mov  ax, #0x8700
    int 0x15
    jc error
    pop bp
    ret

/* -------------------------------- bc.c file ------------------------------- */
#define BOOTSEG 0x9800
#include "bio.c" // IO function
#include "bootLinux.c"  // C code for Linux booter
int main(){
    (1). initialize dap for int13-42 calls
    (2). read MBR sector
    (3). print partition table
    (4). prompt for a partition to boot
    (5). if (partition type == LINUX) bootLInux(partition);
    (6). load partition's local MBR to 0x07C0; chain boot from partition local MBR
}

/* ---------------------------- bootLInux.c file ---------------------------- */
boot-Linux-bzImage algorithm:
{
    (1). read superblock to get blockSIze, inodeSIze, inodes_per_group
    (2). read Group desc 0 to get inode start block
    (3). read in the root INODE and let INODE *ip point at root INODE
    (4). prompt for a Linux kernel image filename to boot
    (5). tokenize image filename and search for image's INODE
    (6). handle symbolic-link filenames
    (7). load BOOT+SETUP of Linux bzImage to 0x9000
    (8). set video mode word at 506 in BOOT to 773 (for small font)
    (9). set root dev word at 508 in BOOT to (0x03, pno)
    (10). set bootflags word at offset 16 in SETUP to 0x2001
    (11). compute number of kernel sectors in last block of SETUP
    (12). load kernel sectors to 0x1000, then cp2himem() to 1MB
    (13). load kernel blocks to high memory, each time load 64KB
    (14). load initrd image to 32MB in high memory
    (15). write initrd address and size to offset (24,28) in SETUP
    (16). jmp_setup() to execute SETUP code at 0x9020
}