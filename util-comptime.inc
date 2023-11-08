
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
; increasing this increases the end user upload speed (provided that his internet is good enough)
; at a rate of about 3MiB per double the size (so about 32/30MiB, 64/33MiB, 128/36MiB)


replace_weird_characters.REPLACEMENT_CHAR equ '_'


macro fnc1 name, arg1 {
    mov rdi, arg1
    call name
}

macro fnc2 name, arg1, arg2 {
    mov rsi, arg2
    fnc1 name, arg1
}

macro fnc3 name, arg1, arg2, arg3 {
    mov rdx, arg3
    fnc2 name, arg1, arg2
}

macro fnc4 name, arg1, arg2, arg3, arg4 {
    mov r10, arg4
    fnc3 name, arg1, arg2, arg3
}

macro fnc6 name, arg1, arg2, arg3, arg4, arg5, arg6 {
    mov r9, arg6
    mov r8, arg5
    mov r10, arg4
    mov rdx, arg3
    mov rsi, arg2
    mov rdi, arg1
    call name
}
