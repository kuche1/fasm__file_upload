
; https://flatassembler.net/docs.php?article=manual#1.2.1

1B equ db
2B equ dw
4B equ dd
8B equ dq

1byte equ byte
2bytes equ word
4bytes equ dword ; double word
8bytes equ qword ; quad word

r1B equ rb


READ_1B_BUF.CACHE_SIZE equ 32 ; in bytes


macro fnc8 name, arg1 {
    mov rdi, arg1
    call name
}

macro fnc88 name, arg1, arg2 {
    mov rsi, arg2
    fnc8 name, arg1
}

macro fnc888 name, arg1, arg2, arg3 {
    mov rdx, arg3
    fnc88 name, arg1, arg2
}

macro fnc8888 name, arg1, arg2, arg3, arg4 {
    mov r10, arg4
    fnc888 name, arg1, arg2, arg3
}

macro fnc818188 name, arg1, arg2, arg3, arg4, arg5, arg6 {
    mov r9, arg6
    mov r8, arg5
    mov r10b, arg4
    mov rdx, arg3
    mov sil, arg2
    mov rdi, arg1
    call name
}
