
include "linux-comptime.inc"
include "util-comptime.inc"


cstr_to_uint:
; input:
;   rdi - cstring
;
; output:
;   rax - uint value
;
; trash:
;   none

    push rbx
    push rcx
    push rdi

    xor rax, rax ; fancy way of doing `mov rax, 0`
    xor rbx, rbx

    mov rcx, 10

    .process_next_char:

        mov bl, [rdi]
        inc rdi

        cmp bl, 0
        je .end_of_string_reaced

        sub bl, '0' ; if we have something other than '0123456789' in `bl` we will get a silent error here
        mul rcx ; multiply rax by 10
        add rax, rbx

    jmp .process_next_char

    .end_of_string_reaced:

    pop rdi
    pop rcx
    pop rbx

    ret


open_file_read_at:
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

    mov rdx, O_RDONLY

    mov r10, 0

    sys0 SYSCALL_OPENAT

    pop r10
    pop rdx

    ret


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

    sys0 SYSCALL_OPENAT

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

    sys0 SYSCALL_OPEN

    pop rdx
    pop rsi

    ret


read_1B_buf: ; TODO untested
; input:
;   rdi - fd
;   rsi - buffer (the byte read will be written here)
;
; output:
;   rax
;       >1  - will never happen
;       1   - success
;       <=0 - failure
;
; trash:
;   none

    push rdi
    push rsi

    mov rax, [read_1B_buf.cache_start]
    cmp rax, [read_1B_buf.cache_end]
    je .load_cache_with_next_read

    .extract_next_char_from_cache:
        mov rax, read_1B_buf.cache
        add rax, [read_1B_buf.cache_start]

        mov al, [rax]
        mov [rsi], al

        inc [read_1B_buf.cache_start]

        mov rax, 1
        jmp .return

    .load_cache_with_next_read:
        push rsi
        mov rsi, read_1B_buf.cache
        mov rdx, READ_1B_BUF.CACHE_SIZE
        sys0 SYSCALL_READ
        pop rsi

        cmp rax, 0
        jle .return

        mov [read_1B_buf.cache_end], rax
        mov [read_1B_buf.cache_start], 0
    jmp .extract_next_char_from_cache

    .return:

    pop rsi
    pop rdi

    ret
