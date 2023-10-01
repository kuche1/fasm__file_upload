
macro fnc0 name
{
    call name
}

macro fnc1 name, arg1
{
    mov rdi, arg1
    fnc0 name
}

macro fnc2 name, arg1, arg2
{
    mov rsi, arg2
    fnc1 name, arg1
}

macro fnc3 name, arg1, arg2, arg3
{
    mov rdx, arg3
    fnc2 name, arg1, arg2
}

macro fnc4 name, arg1, arg2, arg3, arg4
{
    mov r10, arg4
    fnc3 name, arg1, arg2, arg3
}

macro fnc5 name, arg1, arg2, arg3, arg4, arg5
{
    mov r8, arg5
    fnc4 name, arg1, arg2, arg3, arg4
}

macro fnc6 name, arg1, arg2, arg3, arg4, arg5, arg6
{
    mov r9, arg6
    fnc5 name, arg1, arg2, arg3, arg4, arg5
}

macro fnc818188 name, arg1, arg2, arg3, arg4, arg5, arg6
{
    mov rdi, arg1
    mov sil, arg2
    mov rdx, arg3
    mov r10b, arg4
    mov r8, arg5
    mov r9, arg6
    call name
}
