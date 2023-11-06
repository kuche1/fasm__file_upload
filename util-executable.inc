
include "linux-comptime.inc"
include "util-comptime.inc"


create_folder:
; input:
;   rdi - path to folder (cstring)
;
; output:
;   rax - 0 on success
;
; trash:
;   none

    push rsi

    mov rsi, S_IRWXU
    or rsi, S_IRWXG
    or rsi, S_IROTH
    or rsi, S_IXOTH

    sys0 SYSCALL_MKDIR

    pop rsi

    ret


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


; file_endswith_str:
; ; input:
; ;   rdi - fd - cursor position might be changed after fnc call
; ;   rsi - str
; ;   rdx - str size
; ;
; ; output:
; ;   rax
; ;       0 - false
; ;       1 - true
; ;
; ; trash:
; ;   none

;     push rsi
;     push rdx

;     sys_lseek rdi, 0, SEEK_CUR ; TODO not optimal
;     ; size of file is now in rax

;     pop rdx
;     push rdx

;     cmp rax, rdx
;     jl .false

;     ASDASDASDASDSA

;     .false:
;         mov rax, 0
;     jmp .return

;     .return:

;     pop rdx
;     pop rsi

;     ret


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


open_file_read_write_at:
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

    mov rdx, O_RDWR
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


read_1B_buf.reset_cache:
; input:
;   none
;
; output:
;   none
;
; trash:
;   none

    mov [read_1B_buf.cache_start], 0
    mov [read_1B_buf.cache_end], 0

    ret


read_1B_buf:
; input:
;   rdi - fd
;   rsi - buffer (the byte read will be written here)
;
; output:
;   rax
;       >=0 - number of bytes read (should never be >=2)
;       <0  - failure
;
; trash:
;   none

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
        push rdx
        mov rsi, read_1B_buf.cache
        mov rdx, READ_1B_BUF.CACHE_SIZE
        sys0 SYSCALL_READ
        pop rdx
        pop rsi

        cmp rax, 0
        jle .return

        mov [read_1B_buf.cache_end], rax
        mov [read_1B_buf.cache_start], 0
    jmp .extract_next_char_from_cache

    .return:

    ret


read_buf:
; input:
;   rdi - fd
;   rsi - buffer (the bytes read will be written here)
;   rdx - bytes to read
;
; output:
;   rax
;       >=0 - amount bytes read
;       <0  - failure
;
; trash:
;   none

    ; TODO this is REALLY BAD but it will have to do for now

    read_bytes equ rbx

    push read_bytes
    push rsi

    mov read_bytes, 0

    .read_next_byte_loop:

        cmp rdx, read_bytes
        je .return

        call read_1B_buf
        cmp rax, 0
        jl .read_error

        inc rsi
        inc read_bytes

    jmp .read_next_byte_loop

    .read_error:
        cmp read_bytes, 0
        jg .return ; ignore the error, just return the amount of bytes read

        mov read_bytes, rax ; not optimal but I want to reduce the amount of spaghett
    jmp .return

    .return:

    mov rax, read_bytes

    pop rsi
    pop read_bytes

    restore read_bytes

    ret


send_file:
; input:
;   rdi - socket fd
;   rsi - file fd - cursor will be changed
;
; output:
;   rax
;       1 - success
;       0 - failure
;
; trash:
;   none

    socket_fd equ r15
    file_fd equ r14
    bytes_left equ r13

    push socket_fd
    push file_fd
    push bytes_left
    push rdi
    push rsi
    push rdx
    push r10

    mov socket_fd, rdi
    mov file_fd, rsi

    ; get file size
    sys_lseek file_fd, 0, SEEK_END
    mov bytes_left, rax

    ; go back to the start
    sys_lseek file_fd, 0, SEEK_SET

    .send_file_loop:
        sys_sendfile socket_fd, file_fd, 0, bytes_left ; this seems to always return at 2GB

        cmp rax, 0
        jle .return_failure ; network error

        sub bytes_left, rax
        cmp bytes_left, 0
        je .return_success ; file fully sent

    jmp .send_file_loop
    .send_file_done:

    .return_failure:
        mov rax, 0
        jmp .return
    
    .return_success:
        mov rax, 1
        jmp .return

    .return:

    pop r10
    pop rdx
    pop rsi
    pop rdi
    pop bytes_left
    pop file_fd
    pop socket_fd

    restore socket_fd
    restore file_fd
    restore bytes_left

    ret


str_endswith:
; input:
;   rdi - str to search in
;   rsi - str size
;   rdx - substr
;   r10 - substr size
;
; output:
;   rax
;       1 - str ends with substr
;       0 - str doesn't end with substr
;
; trash:
;   none

    str equ rdi
    str_len equ rsi
    substr equ rdx
    substr_len equ r10
    substr_start equ r15

    push str
    ; push str_len
    push substr
    ; push substr_len
    push substr_start

    mov substr_start, substr

    cmp str_len, substr_len
    jl .return_false

    add str, str_len
    add substr, substr_len

    .loop:
        dec str
        dec substr

        cmp substr, substr_start
        jl .return_true

        mov al, [str]
        cmp al, [substr]
        jne .return_false
    jmp .loop

    .return_false:
        mov rax, 0
        jmp .return
    
    .return_true:
        mov rax, 1
        jmp .return

    .return:

    pop substr_start
    ; pop substr_len
    pop substr
    ; pop str_len
    pop str

    restore str
    restore str_len
    restore substr
    restore substr_len
    restore substr_start

    ret


str_indexof:
; input:
;   rdi - str to search in
;   rsi - str to search in size
;   rdx - substr
;   r10 - substr size
;
; output:
;   rax
;       >=0 - index of substr in str
;       <0  - substr cannot be found in str
;
; trash:
;   none

    str equ rdi
    str_len equ rsi
    substr equ rdx
    substr_len equ r10
    cur_idx equ r11
    c2 equ r12b
    c2_reg equ r12

    push str
    push str_len
    push cur_idx
    push c2_reg
    push substr
    push substr_len

    mov cur_idx, 0
    mov c2, [substr]

    ; do that for later
    inc substr
    dec substr_len

    .cmp_next_byte:

        cmp str_len, substr_len
        jl .return_false

        mov al, [str]
        inc str
        dec str_len

        cmp al, c2
        jne .not_equal

        .not_equal:
            call str_startswith
            cmp rax, 0
            je .nothing_found
                mov rax, cur_idx
                jmp .return
            .nothing_found:
        inc cur_idx

    jmp .cmp_next_byte

    .return_false:
        mov rax, -1
        jmp .return

    .return:

    pop substr_len
    pop substr
    pop c2_reg
    pop cur_idx
    pop str_len
    pop str

    restore str
    restore str_len
    restore substr
    restore substr_len
    restore cur_idx

    ret


str_startswith:
; input:
;   rdi - str
;   rsi - str size
;   rdx - substr
;   r10 - substr size
;
; output:
;   rax
;       0 - no
;       1 - yes
;
; trash:
;   none

    push rdi
    push rdx
    push r10

    cmp rsi, r10
    jl .doesnt_start_with

    .cmp_next_byte:

        cmp r10, 0
        jle .does_start_with

        mov al, [rdi]
        inc rdi

        mov ah, [rdx]
        inc rdx

        cmp ah, al
        jne .doesnt_start_with

        dec r10

    jmp .cmp_next_byte

    .doesnt_start_with:
        mov rax, 0
        jmp .return

    .does_start_with:
        mov rax, 1
        jmp .return

    .return:

    pop r10
    pop rdx
    pop rdi

    ret


str_clear:
; input:
;   rdi - str
;   rsi - size
;
; output:
;   none
;
; trash:
;   none

    push rdi
    push rsi

    .clear_next_byte:

        cmp rsi, 0
        je .return

        mov byte [rdi], 0
        inc rdi

    jmp .clear_next_byte

    .return:

    pop rsi
    pop rdi

    ret


str_copy:
; input:
;   rdi - str dest
;   rsi - str source
;   rdx - bytes to copy
;
; output:
;   none
;
; trash:
;   none

    push rax
    push rdi
    push rsi
    push rdx

    .copy_next_byte:

        cmp rdx, 0
        je .return

        mov al, [rsi]
        inc rsi

        mov [rdi], al
        inc rdi

        dec rdx

    jmp .copy_next_byte

    .return:

    pop rdx
    pop rsi
    pop rdi
    pop rax

    ret
