/* ------------------------------ assembly file ----------------------------- */
.global _h                  ! IMPORT _h from C
.global _getbp,_mysum,_g    ! EXPORT global symbols to C
_getbp:
    mov ax,bp
    ret
_mysum:                     ! int mysum(int x, int y)
    push bp
    mov  bp, sp             ! establish stack frame
    mov  ax, 4[bp]          ! AX = x
    add  ax, 6[bp]          ! add y to AX
    add  ax, _h             ! add _h to AX
    mov  sp, bp             ! return to caller
    pop  bp
    ret
_g: .word 1234              ! global _g defined here