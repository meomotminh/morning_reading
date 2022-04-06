## Virtual Address and Physical Address

- virtual address (VA)
- physical address (PA)
- 16-bit real mode
    - in real mode, Intel x86 CPU can execute 16-bit code and access the lowest 1 MB of physical memory.
    - memory segment: block of 64 KB that begins from a 16-byte address boundary.
    - 4 16-bit segment registers that point to the segment in memory the CPU: 
        - CS: Code
        - DS: Data
        - SS: Stack
        - ES: Extra
    - within a program, every address is a 16-bit VA - offset in a program's section - offset in section's segment in memory
        - (20-bit) PA = (SegmentRegister << 4) + (16-bit) VA
- Although x86 CPU in real mode can support programs with 4 segments, binary executable program by compiler-linker may futher restrict CPU's ability to access memory.
- Run-time image of a program: Code, Data and Stack
- In BCC, the maximum size of a one-segment program is limited to 64 KB, it can be loaded to and executed from any available segment in memory. Binary executables: I (instruction) space + D (data) space.
- if the data and stack segments were separate, a pointer at run time may point to either the data area or the stack

## I/O Devices and Operations

I/O devices: block and char devices
- Block device: disks and CDROM transfer data in chunks of blocks
- Char device: console, terminal transfer data in bytes

In Programmed I/O (PIO), CPU actively controls each I/O operation.
- Polling: CPU check device status -> if device is ready -> isses I/O command to start the device -> repeatedly check the device status
- I/O by interrupts:
    - I/O by DMA (Direct Memory Access): CPU writes I/O operation info: memory address, data transfer direction, number of bytes to transfer and intened device to DMA controller
    - I/O processor

Operating System:
- is a set of programs and supporting files
- Process management:
    - Process is a sequence of executions 
    - System resource: memory space, I/O devices, CPU time
    - an OS consists of a set of concurrent processes
    - include process creation, process scheduling, changing process execution image, process synchronization and process termination
    - execution image of a process is a memory area containing its code, data and stack
    - in OS, processes are dynamic
    - memory's process must be allocated and deallocated.
    - in system with memory protection hardware, OS kernel must ensure each process can only access its own image to prevent processes from interfering with one another.
- Memory management
- File system support: allow user to save and retrieve info and provide an env for developing application programs.
- Device driver
- Networking

Linux is a general purpose OS
MTX is simple OS

## Var
- global var is unique and have only 1 copy
- static globals are visible only to the file in which they are defined
- non-static globals are visible to all the files
- initialized globals are assigned at compile time
- uninitialized are cleared to 0 when program execution starts
- local var is visible only to the function in which they are defined
- by default, local var is automatic : auto exist and logically disapear when the function exits.
- register var, compiler try to allocate them in CPU registers
- automatic var do not have any allocated memory until function is entered, they cannot be initialized at compile time.
- static local var is permanent and unique, can be initialized
- "volatile" used as memory-mapped I/O locations or global var that are accessed by ISR or thread.
- "volatile" prevent C compiler from optimizing code that operates on such var


## gcc
- Step 1: Convert C source file to assembly code files.
    - invoke C COMPILER: translate .c into .s (containing assembly code)
    - Convert assembly to Object code: program assembly language for specific machine. Assembler translate assembly code into machine code in binary form. ".s" to ".o" is object code
        - header contain size of CODE, DATA AND BSS section:
            - CODE section: machine instructions
            - DATA section: initialized global and static local var
            - BSS section: uninitialized global and static local var
            - relocation info for pointers in CODE and offsets in DATA and BSS
            - Symbol Table containing non-static global, function names and their attributes
    - LINKING: invoke LINKER combines all the .o files and needed library into a singble binary executable file
        - combine all CODE sections of the .o file into a single Code section.
            - start with the default C startup code crt0.o which call main()
        - combine all the DATA sections into a single Data section: initialized globals + static locals
        - combine all BSS sections into a single bss section.
        - Use relocation info in the .o files to adjust pointers in the combined Code section and offsets in the combined Data and bss sections.
        - Use Symbol tables to resolve cross references among individual .o files.
        - Static globals are not in the symbol table, they are unavailable to the linker

    ## Static vs Dynamic linking

    - 2 ways to create a binary executable: static linking and dynamic linking
    - in static linking, uses static library, the linker includes all needed library function code and data into a.out -> make a.out complete and self-contained but large
    - in dynamic linking, uses shared library, library functions are not included in a.out but calls are recorded in a.out as directives:
        - size of a.out is reduced
        - many executing programs can share the same library function in memory
        - modifying library functions does not need to re-compile the source files.
        - Libraries used for dynamic linking is DLLs, Shared Libraries (.so files) in Linux
        - Dynamically loaded (DL) libraries are shared libraries which are loaded only when they are needed. DL libraries are useful as plug-ins and dynamically loaded modules

## Executable File Format
- flat binary executable: executable code + initialized data -> loaded into memory in its entirety for execution directly: bootable OS image are usually flat (bootloader)
- a.out executable file: traditional a.out file consists of a header, followed by code, data and bss sections. 
- ELF executable file: Executable and Linking Format file consists of one or more program sections. Each program section can be loaded to a specific memory address. In Linux, default binary executables are ELF files, which are better suited to dynamic linking.

## a.out File:
- header: loadin info and sizes of the a.out file
    - tsize: size of Code section
    - dsize: size of Data section containing initialized globals and static locals.
    - bsize: size of bss section containing uninitialized globals and static locals.
    - total_size: total size of a.out to load.
- Code Section: text section, which contains executable code of the program, begin with standard crt0.o 
- Data Section: contains initialized global and static data.
- symbol table: optional, needed only for run-time debugging

bss section not in a.out, only its size. Automatic local var are not in a.out
- total loading size of a.out = _brk = tsize + dsize + bsize
- the extra memory space above the bss is the HEAP area for dynamic allocation during execution

## Program execution
- sh forks a child process and waits for the child to terminate
- child process runs, uses a.out to create a new execution image by:
    - read the header of a.out to determine the total memory size needed:
        TotalSize = _brk + stackSize
        - stackSize: default value chosen by OS kernel for the program to start.
    - allocates a memory area of TotalSize for the execution image. 
        - allocated memory area is a single piece of contiguous memory
        - load the Code and Data section of a.out into memory area, with stack area at the high address end, clear bss section to 0, so all uninitialized globals and static locals begin with the initial value 0.
        - main(){main();}: segmentation fault
    - abondon the old image and begins to execute the new image.
        - Heap area between bss and Stack is used by C library function malloc()/free()  for dynamic memory allocation in the execution image.
        - _splimit is the stack size limit
        - a.out is first loaded, _brk and _splimit may coincide -> initial Heap size is 0
        - During execution, process may use the brk(address) or sbrk(size) system call to change _brk to a higher address -> increase Heap size
        - Alternatively, malloc() may call brk() or sbrk() implicitly to expand the Heap size.
        - During execution, a stack overflow occurs if the program tries to exend the stack pointer below _splimit
        - On machine with memory protection, this is detected my memory management hardware as an error
        - Subject to max size limit, OS kernel may grow the stack by allocating additional memory in the process address space, allowing the execution to continue.
        - Stack overflow is fatal if the stack cannot be grown further.
    - execution begins from crt0.o calls main(), passing as parameters argc (number of command line parameters) and argv(each argv entry points to a corresponding command line parameter string) to main 

## Program Termination
- Normal Termination:
    - if program execute successfully, main() return to crt0.o -> call library function exit(0) to terminate the process
    - exit() does some clean-up work first such as flush stdout, close I/O streams. Then it issues an _exit() system call, causes the process to enter the OS kernel to terminate.
    - _exit(0) mean normal termination.
    - pid = wait(int *status) release ZOMBIE -> FREE
- Abnormal Termination:
    - error condition / exception
    - kernel trap handler convert the trap error type -> a magic number (SIGNAL) -> deliver to the process -> process terminate
    - CONTROL C : hardware terminate -> interrupt / kill -s signal_number pid # signal_number = 1 to 31

## Function Call in C
- function call use the stack
- C compiler pass parameter on the stack in function all
- during execution, it uses special CPU register (bp or ebp) to point at the stack frame of the current executing function

### Run-Time Stack Usage

```
main()
{
    int a,b,c;
    a = 1; b = 2; c = 3;
    c = sub(a,b);
    printf("c = %d\n", c);
}

int sub(int x, int y)
{
    int u,v;
    u = 4; v= 5;
    return x+y+u+v;
}
``` 

1. executing a.out -> a process image as follows:
    - Code > Data > Heap > Stack > High Address
2. CPU registers
    - PC(IP): point to next instruction to be executed by the CPU
    - SP(SP): point to top of stack
    - FP(BP): point to the stack frame of current active function
    - Return Value Register (AX): register for function return value
3. int C program, main() is called by C startup code crt0.o -> push the return address onto stack and replace PC with the entry address of main()-> PC enter main()
4. upon entry, compiled code of C function does:
    - push FP onto stacak
    - let FP point at the saved FP
    - shift SP downward to allocate space for automatic local var on stack
    - the compiled code may shift SP farther down to alloacte some scratch space
    - a,b,c are automatic local var 
5. CPU starts exeucte code a = 1, b = 2, c = 3. Assume sizeof(int) is 4 bytes -> location of a is -4(FP), b is -8(FP), c is -12(FP) bytes from FP 
    - movl $ 2, -8(%ebp) where $ 2 is 2 and %ebp is the ebp register
6. main() calls sub() by c = sub(a,b) 
    - push parameters in reverse order 
    - call sub() which pushes the current PC onto stack and replace PC with the entry address of sub, causing the CPU to enter sub()
7. sub() is C:
    - push FP and let FP point at the saved FP
    - Shift SP downward to allocate space for local var u,v
    - compiled code may shift SP farther for some temp space on stack

## Stack Frames

While inside a function, it can only access global var, parameters passed in by caller and local vars. Global and static local var are in the combined Data section, which can be referenced by a fixed base register. Parameter and automatic locals have different copies on each invocation of the function.

The stack area visible to a function is called the stack FRAME of a function-> FP is called Stack Frame Pointer

crt0.o -> main() -> A(par_a) -> B(par_b) -> C(par_c)

For each function call, the stack would grow one more frame for the called function. The frame at the stack top is the stack frame of the current executing function, which is pointed by the CPU's frame pointer. Thus, the function call sequence is maintained in the stack as a link list. 

By convention, the CPU's FP = 0 when crt0.o is entered from the OS kernel. So the stack frame link list ends with a 0. when a function returns, its stack frame is deallocated and the stack shrinks back,

## Return from Function call

when sub)( executes return x+y+u+v, it evaluates the expression abd puts the resulting value in a return value register (AX). Then it deallocates the local var by

. copy FP into SP;  // SP now points to the saved FP in stack
. pop stack into FP; // restore FP, which now point to caller's stack frame, leaving the return PC on the stack top
. Then execute the RET instruction, which pops the stack top into PC register-> cause CPU to execute from the saved return address of the caller.

8. Upon return, the caller function catches the return value in the return register (AX). Then it cleans the parameter a,b from the stack (adding 8 to SP). This restore the stack to the origianl situation before the function call. THen continues to execute the next instruction.

Note that some compilers, GCC ver 4, allocate automatic local var in increasing address order. 

## LOng Jump

main() --> A() --> B() --> C()

normally return to the calling function. It is also possible to return directly to an earlier function in the calling sequence by a long jump

Principle of long jump: when a function finish, it returns by the (callerPC, callerFP) in the current stack frame. If replace (callerPC, callerFP) with (savedPC, savedFP) of an earlier function, execution would return to that function directly. Beside (savedPC, savedFP) setjmp() can also save CPU general registers and the original SP, so that longjmp() can restore the complete environment of the returned function-> used to abort a function in a calling sequence, causing execution to resume from a known env saved earlier. Although rarely used in user mode programs, it is a common technique in systems programming. For exp, it may be used in a signal catcher to bypass a user mode function that caused an exception or trap error. 

## Link C program with Assembly Code

In system programming, it is often necessary to access hardware, such as CPU registers and I/O port locations, etc. In these situations, assembly is necessary. It is therefore important to know how to link C programs with assembly code

In C, function names are global and function types are int by default

In every function call, the return value is in the AX register

## Link Library
A link library contains precompiled object code. During linking, the linker searches the link libraries for any function code and data needed by a program. Link libraries used by the BCC linker are standard Unix archive files, which can be manipulated by the ar utility program.

## Mailman's Algorithm

In computer system, a problem arises very often is as follows. M blocks, numbered 0 to M-1. Each block has N houses, numbered 0 to N-1. Each house has a unique block address (block, house) where 0 <= block < M, 0 <= house < N
given block address (block, house) how to convert it to a linear address LA and vice versa

Linear_address LA = N*block + house;
Block_address  BA = (LA/N, LA%N);

## Application of Mailman's Algorithm

1. Test, Set and Clear bits in C: In C, the smallest addressable unit is a char or byte. manipulate bit in bitmap. 
char buf[1024] has 8192 bits number BIT 1234 -> (BIT/8, BIT%8)

.TST a bit for 1 or 0: if (buf[i] & (1 << j))
.SET a bit to 1      : buf[i] |= (1 << j)
.CLR a bit to 0      : buf[i] &= ~(1 << j)

some C compilers allow specifying bits in a structure:
struct bits{
    unsigned int bit0: 1;
    unsigned int bit123: 3;
    unsigned int otherbits: 27;
    unsigned int bit31: 1;
}var;

2. Convert INODE number to inode position on disk. In an EXT2 file system, each file has a unique INODE structure. On the file system disk, inodes begin in the inode_table block. Each disk block contains

INODES_PER_BLOCK = BLOCK_SIZE/sizeof(INODE)

inodes. Each inode has a unique inode number, ino = 1,2..., counted linearly from 1. Given an ino e.g 1234, determine which disk block contains the inode and which inode is it in that block?

    block = (ino - 1) / INODES_PER_BLOCK + inode_table
    inode = (ino - 1) % INODES_PER_BLOCK

similarly, converting double and triple indirect logical block numbers to physical block numbers in an EXT2 file system also depends on the Mailman's algorithm

3. convert linear disk block number to CHS = (cylinder, head, sector) format: Floppy disk and old hard disk use CHS addressing but file systems always use linear block addressing. The algorithm can be used to convert a disk block number to CHS when calling BIOS INT13.

## EXT2 File System

Linux used EXT2 as the default file system. EXT3 is an extension of EXT. The main addition in EXT3 is a journal file, which records changes made to the file system in a journal log. The log allows for quicker recovery from errors in case of a file system crash. An EXT3 file system with no error is identical to EXT2 file system.

The newest extension of EXT3 is EXT4 which difference in allocation of disk blocks. In EXT4, block numbers are 48 bits. Instead of discrete disk blocks, EXT4 allocates contiguous ranges of disk blocks, called extents.

### EXT2 File System Data Structures

- Block 0 : Boot block is not used by the file system , contain booter program for booting up an OS from the disk
- Block 1 : Super block at byte offset 1024 in hard disk partitions, contains info about the entire file system. Some important fields are :
    struct ext2_super_block {
        u32 s_inodes_count; // total number of inodes
        u32 s_blocks_count
        u32 s_r_blocks_count
        u32 s_free_block_count
        u32 s_free_inodes_count
        u32 s_first_data_block
        u32 s_log_block_size
        u32 s_log_frag_size
        u32 s_blocks_per_group
        ...
    }

Block #2: Group Descriptor Block (int s_first_data_block + 1 on hard disk): EXT2 divides disk blocks into groups. Each group contains 8192 (32 K on HD) block. Each group is described by a group descriptor structure:

    struct ext2_group_desc{
        u32 bg_block_bitmap 
        u32 bg_inode_bitmap
        u32 bg_inode_table
        ...
    }

FD has 1440 block -> B2 contain 1 group descriptor. Bitmaps are in block 3 and 4, inodes start from block 5

Block #3 bitmap: is a sequence of bits used to represent some kind of items: disk blocks or inodes. a bitmap is used to allocate and deallocate items. In a bitmap, 0 means the corresponding item is FREE, 1 means IN_USE. FD has 1440 blocks but block#0 is not used for file system. So Bitmap has only 1439 valid bits. Invalid bits are treated as IN_USE and set to 1.

Block#4: Inode Bitmap is a data structure used to represent a file. Status of each inode is represented by a bit in the Imap in B4. In EXT2 FS, the first 10 inodes are reserved so the Imap of an empty EXT2 FS starts with 10 1, follows by 0. Invalid bits are set to 1

Block#5: Inode blocks. file is represented by a unique inode structure of 128 (256 in EXT4) bytes

struct ext2_inode {
    u16 i_mode
    u16 i_uid   // owner uid
    u32 i_size  // file size in bytes
    u32 i_atime // time fields in seconds
    u32 i_ctime // since 00:00:00 1-1-1970
    u32 i_mtime
    u32 i_dtime
    u16 i_gid   // group ID
    u16 i_links_count // hard-link count
    u32 i_blocks  // number of 512-byte sector
    u32 i_flags
    ...

}

i_mode: file's type, usage and permission
    - leading 4 bits: 1000 for REG file, 0100 for DIR
    - last 9 bits are rwx for file protection
i_block[15] array contains disk block of a file:
    - direct blocks: i_block[0] to i_block[11] points to direct disk blocks
    - indirect blocks: i_block[12] points to disk block which contains 256 block numbers, each of which point to a disk block
    - double indirect blocks: i_block[13] points to block which point to 256 blocks, each of which point to 256 disk blocks
    - triple indirect blocks: i_block[14] 

inode size (128 or 256) divides block size (1KB or 4KB) evently, so that every inode block contains an integral number of inodes. no of inode blocks = ninode / 8

ninode = 360 -> no of inode blocks = 45

each node has a unique inode number = inode pos in the inode blocks + 1
inode pos count from 0 but inode's pos count from 1
the root directory inode number is 2

Data block: immediately after inode blocks are data blocks. assuming 360 inodes, the 1st real data block is B50, which is i_block[0] of the root directory

EXT2 directory entries: A directory contains dir_entry structures, in which the name field contains 1 to 255 chars- So the dir_entry's rec_len also varies

struct ext2_dir_entry_2 {
    u32 inode;
    u16 rec_len
    u8 name_len
    u8 file_type
    char name[]
}

## Traverse EXT2 File System Tree

./a/b/c

1. read in the superblock, which is at byte offset 1024 -> check magic number s_magic(0xEF53) to verify it's an EXT2 FS
2. read in the group descriptor block (1 + s_first_data_block) to access the group 0 descriptor. From group descriptor's bg_inode_table entry, find inodes begin block number, call it InodesBeginBlock
3. Read in InodeBeginBlock to get the inode of /, which is INODE#2
4. Tokenize the pathname into component strings and let the number of components be n
5. Start from the root INODE in (3) -> search for name[0] in its data blocks. number of entries in a DIR is small-> a DIR inode only has 12 direct data blocks-> each data block of a DIR INODE contains dir_entry structure of the form

[ino rlen nlen NAME][ino rlen nlen NAME]....

where NAME is a sequence of nlen chars (without a terminating NULL char). For each data block, read the block into memory and use a dir_entry *dp to point at the loaded data block. Then use nlen to extract NAME as a string a compare it with name[0]. if not match, step to the next dir_entry by:

dp = (dir_entry *)((char *)dp + dp->rlen);

and continue. if name[0] exists, we can find its dir_entry and hence its inode number.

6. Use the inode number, ino to locate the corresponding INODE. 

blk = (ino - 1)/INODES_PER_BLOCK + InodesBeginBlock
offset = (ino - 1) % INODES_PER_BLOCK

Then read in the INODE of /a-> determine wheter it's a DIR

7. Since step 5-6 will be repeated n times

    u32 search(INODE *iondePtr, char *name){
        // search for name in the data blocks of this INODE
        // if found, return its ino, else return 0
    }

    Assume: n, name[0],..., name[n-1] are globals
    INODE *ip, points at INODE of /
    for (i = 0; i<n; i++){
        ino = search(ip, name[i]);
        if (!ino){
            // can't find name[i]
            exit;
        }
        // use ino to read in INODE and let ip point to INODE
    }

    if the search loop ends successfully, ip must point at the INODE of pathname. Traversing large EXT2 FS with many groups is similar

    ## BCC Cross-Compiling Package

    BCC consists of an assembler, a C compiler and a linker. It is a cross compiling package, which generate 16-bit code for execution on real-mode PC or PC emulators.

    Currently BCC is included in most Linux distribution

    a.out is intended for execution on ELKS (Embedded Linux Kernel System) or ELKS simulator

    ## Running MTX

    system image of MTX is an EXT2 file system :
        - bin : binary executable programs
        - dev : device special files
        - etc : passwd file
        - user : user home directories
        - boot : mtx kernel images
    
    2 types of MTX images:
        - FDimage: small MTX system, run on disk. Each FDDimages is a bootable floppy disk image, can be used as the FD of a virtual machine. 
        - HDimage: full-sized MTX images intended for hard disks. it must be installed to a hard disk partition, along with MTX hard disk booter

## From login to Command Execution

- first step -> boot up the MTX kernel
- initialize the system and mounts a root file system from the boot device
- create a process P0, which run only in kernel mode
- P0 fork a child process P1, which executes an INIT program in user mode
- P1 play the same role as the INIT process of Unix/Linux
- when P1 run, it forks a child process on each of the login terminals. then wait for any of the login process to terminate
- each login process executes the same login program, in which it opens its own terminal (special file) to get the file descriptor in = 0 for read, out = 1 and err = 2 for write. 
- then it display login to its own terminal and waits for user to login
- login name and password in the /etc/passwd file in the form: loginname:password:gid:uid:user-full-name:home-directory:program
- Verify. login process becomes user process by acquiring the user's gid and uid. It change directory to user's home-directory and executes the listed program, which is usually the command interpreter sh.
- User can enter command for the sh process to execute
- when the user logout, the sh process terminate, which wakes up the INIT process P1, which forks another login process on the terminal

A command is usually a binary executable file. By default all executable programs are in the /bin directory. If an executable file is not in  /binm it must be entered by a full pathname. Given a command line, such as "cat filename", sh forks a child process to execute the cat command and waits for the child process to terminate. The child process executes the command by changing its execution image to the cat program, passing as parameter filename to the program. When the child process terminate, it wakes up the parent sh process, which prompts for another command

# CHAP 3 BOOTING OPERATING SYSTEM

## Booting
short for bootstrap process of loading an operating system image into computer memory and starting up the operating system
booting system of Intel x86 based 
BIOS (Basic Input Output System) program stored in ROM 
after power on, PC's CPU start to execute BIOS -> BIOS perform POST (Power-on Self Test) to check system hardware for proper operation -> searches for a device to boot.
Bootable devices are maintained in a programmable CMOS memory, order: floppy disk, CDROM, hard disk, etc. 
If BIOS find a bootable device, it tries to boot from that device, otherwise, it display a "no bootable device found"

## Bootable Devices
stoarge device supported by BIOS for booting, contains a booter and a bootable system image. During booting, BIOS Loads the first 512 bytes of the booter to the memory location (segment, offset) = (0x0000, 0x7C00) = 0x07C00 and jumps to there to execute the booter-> up to the booter to do the rest.

PC only have 64KB of RAM, memory below 0x7C00 is reserved for interrupt vectors, BIOS and BASIC. The first usable memory begins at 0x08000. When execution starts, the actions of booter are:
- load the rest of the booter into memory and execute the complete booter
- find and load the os image into memory
- send CPU to execute the startup code of the OS kernel, which start up the OS

## Floppy Disk Booting
booting requires writing a booter to the beginning part of a device, sector 0 of a hard disk, known as the Master Boot Record (MBR). However, writing to a hard disk is risky, a mistake may render the hard disk non-bootable or destroy the disk partition table 

Virtual machines are more convenient. missing or corrupted system files. In these situations, it is very useful to have an alternative way to boot up the machine to repair or rescue the system.

(1) FD contains booter followed by a bootable OS image: a FD is dedicated to booting, it contains a booter in sector 0, followed by a bootable OS image in consecutive sectors

The size of the OS image, number of sectors, is either in the beginning part of the OS image or patched in the booter itself, so the booter can determine how many sectors of the OS image to load. The loading address is knownm.

Booter load the OS image sectors to the specified address and then send the CPU to execute the loaded OS image.

(2) FD with a bootable image and a RAM disk image: to run, most OS kernels require a root file system, baisc file system which contains enough special files, commands and shared libraries, needed by the OS kernel.

Assuming root file system already existed on a separate device, OS kernel can be instructed to mount the appropriate device as the root file system. Boot disk is used to boot up the kernel, which is compiled with RAM disk support. root disk is a compressed RAM disk image of a root file system. When linux kernel start up, it prompts and waits for a root disk to be inserted -> the kernel load the root disk content to a RAM disk area in memory, un-compress the RAM disk image and mounts the RAM disk as the root file system

real mode MTX image size is 128KB -> EXT2 file system 1024-128=896 blocks, populates with files needed by the MTX kernel and place the MTX kernel in the last 128 blocks of the disk. Block 0 is not used by the file system, contain MTX booter

During booting, booter load MTX kernel from the last 128 disk blocks and transfer contro to the MTX kernel
when MTX kernel start, it mounts the FD as the root file system.

(3) FD is a file system with bootable image files: FD is complete file system containing a bootable OS image as a regular file. OS image can be placed directly under the root directory. During booting, booter first finds the OS image file, then it loads the image'S disk block into memory and sends CPU to execute the loaded OS image

Bootable MTX kernels are files in the /boot directory. Block 0 of the disk contains a MTX booter
During booting, booter prompt for a MTX kernel to boot
with a bootable file name, the booter find the image file and load its disk blocks to the segment 0x1000
when loading complete, it transfer control to the kernel image
when the MTX kernel start up, it mounts the FD as the root file system and runs on the same FD. how to make comlete LInux file system small enough to fit in a single FD-> BusyBox is about 400KB, yet support all basic command of Unix

(4) FD wutg a viiter fir HD booting: FD based booter for booting from hard disk partitions. During booting, the booter is loaded from FD, once exectuon starts, all actions are for booting system images from hard disk partitions. Since the hard disk is accessed in read-only mode, this avoids any chances of corrupting the hard disk

## Hard Disk Booting
IDE hard disks but same to SCSI and SATA hard disks
(1) Hard Disk Partitions: devided into several partitions. each is formatted as a unique file system and contain a different OS. the partitions are defined by a partition table in the first (MBR) sector of the disk. IN the MBR the partition table begin at the byte offset 0x1BE, has 16-byte entries for 4 primary partitions. If needed one of partitions can be EXTEND type

Each partition is assigned a unique number for identification.

(2) Hard Disk Booting Sequence: when booting from a hard disk BIOS loads the MBR booter to the memory location (0x0000, 0x7C00) and executes it as usual

MBR booter may ask for a partition to boot-> load MBR of the partition to (0x0000, 0x7C00) and executes the local MBR booter. It is then up to the local MBR booter to finish the booting task. Such a MBR booter is commonly known as a chain-boot-loader or pass-the-buck booter since all it does is to usher to the next booter and says "u do it"

the Linux bootloader LILO can be installerd in the MBR for booting Linux as well as DOS and Windows
In general, a MBR booter cannot perform the entire booting task by itself due to its small size and limited capacity but the beginning part of a multi-stage booter: BIOS loads stage 1 and executes it first, then stage 1 loads and execute stage 2... 

## CD/DVD booting
CDROM is used for data storage with propiertary booting methods provided by different computer vendors. El-Torito bootable CD specification
(1) the El-Torito CDROM boot protocol: 3 different ways to setup a CDROM for booting
(2) Emulation booting: boot image must be either floppy disk image or a (single-partition) hard disk image.
During booting, BIOS loads the first 512 bytes of a booter from the boot image to (0x0000, 0x07C0) and execute the booter as usual. In addition, BIOS also emulates the CD/DVD drive as either a FD or HD. If the booting image size is 1.44 or 2.88 MB, it emulates the CD as the first floppy disk otherwise hard drive.
Once boot up, the boot image on the CD can be accessed as the emulated drive thorugh BIOS. The envi is identiccal to that of booting up from the emulated drive. If emulated boot image is a FD, after booting up the bootable FD image can be accessed as drive A. if the boot image is hard disk image, after booting the image become drive C
(3) No-emulation Booting: boot image can be any binary execuatable code. For real-mode OS images, a separate booter is not necessary because the entire OS image can be booted into memory directly.

During booting, booter itself can be loaded directly, but when the booter tries to load the OS image, it needs a device number of the CD drive to make BIOS calles (0x81 for the first IDE slave or 0x82 for the second IDE mster)

WHen BIOS invoke a booter, it also passes the emulated drive number in the CPU'S DL register- the booter must catch the drive number and use it to make BIOS calls. Similar to emulation booting, while it is easy to boot up an OS image from the CD, to access the contents on the CD is another matter. In order to access the contents, a booted up OS must have drivers to interpret the iso9660 file system on the CD

## USB Drive Booting
USB drives are similar to hard disks, can be divided into partitions. to be bootable, some BIOS require a USB drive to have an active partition. During booting, BIOS emulates the USB drive as the usual C drive (0x80). The env is the smae as that of booting from the first hard disk

If the PC's BIOS suport USB booting, Linux kernel will boot up from the USB partition. When Linux kernel start, it only activates drivers for IDE and SCSI devices but not for USB drives

## Boot Linux with Initial RAM disk Image
standard way-> allow a single generic Linux kernel to be used on many different Linux config. An initrd is a RAM disk image which serves as a temporary root file system when the Linux kernel first start up. While running on a RAM disk, the Linux kernel executes a sh script, initrc, wihch directs the kernel to load the driver modules of the real root device-> when the real root device is activated and ready, the kernel discard the RAM disk and mount the real root device as the root file system

## Network Booting
The basic requirement is to establish a network connection to a server machine in a networkm such as a server running the BOOTP Protocol in a TCP/IP network. Once the connection is made, booting code or the entire kernel code can be downloaded from the server to the local machine. After that, the booting sequence is the same as before

## Develop Booter Programs
A Linux distribution usually comes with a default booter, either LILO or GRUB

### Requirements of Booter Programs
Before developing booter program, point out the unique requirements:
1. need assembly code because it must manipulate CPU registers and make BIOS calls
2. when PC start, it is in the 16-bit real mode, in which the CPU can only execute 16-bit code and access the lowest 1MB memmory. To create a booter, must use a compiler-linker that generates 16-bit code (cannot use GCC because GCC compiler generates 32 or 64 bit code) -> BCC package under Linux
3. binary executable generated by BCC uses a single-segment memory model, in which the code, data and stack segments are all the same. Such program can be loaded to and executed from any available segment in memory. A segment is a memory area that begins at a 16-byte boundary. During execution, the CPU's CS, DS and SS registers must all point to the same segment of the program
4. Booters differ from ordinary programs in many aspects. A booter's size (code + static data) is extremely limited 512 or 1024 bytes to fit in 1 or 2 disk sectors.

When running ordinary program, OS will load the entire program into memory and set up the program's execution env before execution start. In contrast, booter only has 512 bytes loaded at 0x07C00. If the booter is larger than 512 bytes, it must load the missing parts in by itself. If the booter's initial memory area is needed by the OS, it must be moved to a different location in order not to clobbered by the incoming OS image. In addition, a booter must manage its own execution env, set up CPU segment registers and establish a stack

5. A booter cannot use the standard library I/O function, such as gets() and printf(). These funtions depend on OS support. The only available support is BIOS. If needed, a booter must implement its own I/O function by calling only BIOS

6. When developing an ordinary program, we may use tools gdb for debugging. In contrast, no tool to debug a booter

## Online and offline Booters
In offline, the booter is told which OS image to boot. booter first find the OS image and builds a small database for the booter to use. Database may contains the disk blocks or ranges of disk blocks of the OS image

During booting, an offline booter simply uses the pre-built database to load the OS image. LILO is offiline Booter, uses a lilo.conf file to build a map file in the /boot directory and then install the LILO booter to the MBR or the local MBR of the hard disk partition. It uses map file to load the Linux image

The disadvantage is user must install the booter again when the OS image is moved or changed

An online booter (GRUB) can find and load an OS image file directly

### Boot MTX from FD sectors
Word 0 is a jump instruction, word 1 is the code section size in 16-byte clicks and word 2 is the data section size in bytes

During booting, booter may extract these values to determine the number of sectors of the MTX kernel to load. loading segment address is 0x1000. under linux, use BCC to generate a binary executable without header and dump it to the begginging of a floppy disk

'''
as86 -o bs.o bs.s   # assemble bs.s into bs.o
bcc  -c -ansi bc.c  # compile bc.c into bc.o

# link bs.o and bc.o into a binary executable without header
ld86 -d -o booter bs.o bc.o /usr/lib/bcc/libc.a

# dump booter to sector 0 of a FD
dd if=booter of=/dev/fd0 bs=512 count=1 conv=notrunc
'''

where the special file name /dev/fd0 is the first floppy drive. if the target is not a real device but an image file, simply replace /dev/fd0 with the image file name. the parameter conv=notrunc is necessary to prevent dd from truncating the image file.

Building process can be automated by using Makefile or a sh script. For simple compile-link task, a sh script is adequate and actually more convinient

'''
# usage: mk filename
as86 -o bs.o bs.s   # bs.s file not change
bcc -c -ansi $1.c
ld86 -d -o $1 bs.o $1.o /usr/lib/bcc/libc.a
dd if=$1 of=IMAGE bs=512 count=1 conv=notrunc
'''

In assembly, start: is the entry point of the booter program. During booting BIOS loads sector 0 of the boot disk to (0x000,0x7C00) and jumps to there to execute the booter

assume booter must be relocated to a different memory area. instead of moving the booter, the code call BIOS int13 to load the first 2 sectors of the boot disk to the segment 0x9800. The FD drive hardware can load a complete track of 18 sectors at a time. After loading the booter to the new segment, it does a far jump, jmpi next, 0x9800, which sets CPU's (CS, IP)=(0x9800, next), causing the CPU to continue execution from the offset next in the segment 0x9800

choice of 0x9800 is based on simple principle, the booter should be relocated to a higher memory area with enough space to run, leaving as much space as possible in the low memory area for loading the OS image.

The segment 0x9800 is 32KB below the ROM area, which begins at the segment 0xA000. Thi sgive the booter a 32KB address space, big enough for a powerful booter. when execution continues, both ES and CS already point to 0x9800. The assembly code set DS and SS to 0x9800 also to conform to the one-segment memory model of the program. THen it set the stack pointer to 32KB above SS.

In some PCs, the RAM area above 0x9F000 may be reserved by BIOS for special usage. ON these machines the stack pointer can be set to a lower address, eg. 16KB from SS, as long as the booter still has enough bss and stack space to run. With a stack, the program can start to make calls. It calls main() in C, which implements the actual work of the booter. Then main() return, it sends the CPU to execute the loaded MTX image at (0x1000,0)

the remaining assembly code contains functions for I/O and loading disk sectors. funtion getc() and putc() are simple, readfd(), setes() and inces() deserve explaination. to load an OS image, a booter must be able to load disk sectors into memory. BIOS supports disk I/O functions via int13, which takes parameters in CPU registers:

    DH=head(0-1), DL=drive (0 for FD drive 0)
    CH=cyl (0-79), CL=sector (1-18)
    AH=2(READ), AL=number of sectors to read
    Memory address: (segment, offset)=(ES, BX)
    return status: carry bit=0 means no error, 1 means error

readfd(cyl, head, sector) calls BIOS int13 to load NSEC sectors into memory, NSEC is a global imported from C. The zero-counted parameters (cyl, head, sector) are computed in C code. Since BIOS counts sectors from 1, the sector value is + 1 to suit BIOS. When loading disk sectors BIOS uses (ES, BX) as real memory address. Since BX = 0, the loading address is (ES,0). Thus, ES must be set, by the setes(segment) functions, to a desired loading segment before calling readfd()

The function code loads the parameters into CPU registers and issues int 0x13. after loading NSEC sectors, it uses inces() to increse ES by NSEC sectors to load the next NSEC vectors,

the error() function is used to trap any error during booting. it print an error msg, followed by reboot.

The use of NSEC as a global rather than as a parameter to readfd() serves:
- illustrates the cross reference of globals between assembly and C code
- if a value does not change often, it should not be passed as parameter bc increase code size.

