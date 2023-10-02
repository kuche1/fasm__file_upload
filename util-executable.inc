
include "linux-comptime.inc"
include "util-comptime.inc"


open_file_write_at:
; input:
;   rdi - fd of folder that contains file (cstring)
;   rsi - file name
;
; output:
;   rax - fd (negative if there was an error)
;
; trash:
;   none

    push rdx
    push r10

    mov rdx, O_WRONLY
    or rdx, O_CREAT ; bitwise or
    or rdx, O_TRUNC

    mov r10, S_IRUSR
    or r10, S_IWUSR
    or r10, S_IRGRP
    or r10, S_IROTH

    sys_openat rdi, rsi, rdx, r10

    pop r10
    pop rdx

    ret


open_folder_read:
; input:
;   rdi - path (cstring)
;
; output:
;   rax - fd (negative if there was an error)
;
; trash:
;   none

    push rsi
    push rdx

    mov rsi, O_DIRECTORY
    or rsi, O_RDONLY ; bitwise or

    mov rdx, 0

    sys_open rdi, rsi, rdx

    pop rdx
    pop rsi

    ret
