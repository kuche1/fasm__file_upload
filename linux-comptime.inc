
; https://chromium.googlesource.com/chromiumos/docs/+/HEAD/constants/syscalls.md
;
; syscall args go to:
; arg0(syscall-number)=rax arg1=rdi arg2=rsi arg3=rdx arg4=r10 arg5=r8 arg6=r9
; (hepefully) all syscall return to `rax`

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; comtime constants: syscall ids

SYSCALL_READ equ 0
SYSCALL_WRITE equ 1
SYSCALL_OPEN equ 2
SYSCALL_CLOSE equ 3
SYSCALL_LSEEK equ 8
SYSCALL_SENDFILE equ 40
SYSCALL_SOCKET equ 41
SYSCALL_ACCEPT equ 43
SYSCALL_SHUTDOWN equ 48
SYSCALL_BIND equ 49
SYSCALL_LISTEN equ 50
SYSCALL_SETSOCKOPT equ 54
SYSCALL_FORK equ 57
SYSCALL_EXIT equ 60
SYSCALL_FTRUNCATE equ 77
SYSCALL_GETDENTS equ 78
SYSCALL_CHDIR equ 80
SYSCALL_OPENAT equ 257

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; comtime constants: files

FILE_STDIN equ 0
FILE_STDOUT equ 1
FILE_STDERR equ 2

; more flags
; https://pubs.opengroup.org/onlinepubs/007904875/functions/open.html
; https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
O_RDONLY equ 0
O_WRONLY equ 1
O_RDWR equ 2
O_CREAT equ 64 ; Create a file if it doesn’t exist.
O_TRUNC equ 512 ; If the file exists and is a regular file, and the file is successfully opened O_RDWR or O_WRONLY, its length shall be truncated to 0, and the mode and owner shall be unchanged. It shall have no effect on FIFO special files or terminal device files. Its effect on other file types is implementation-defined. The result of using O_TRUNC with O_RDONLY is undefined.
O_DIRECTORY equ 65536 ; used when opening directories (and not files)
; O_EXCL	Prevent creation if it already exists.
; O_ APPEND	Opens the file and places the cursor at the end of the contents.
; O_ASYNC	Enable input and output control by signal.
; O_CLOEXEC	Enable close-on-exec mode on the open file.
; O_NONBLOCK	Disables blocking of the file opened.
; O_TMPFILE	Create an unnamed temporary file at the specified path.

S_IRUSR equ 256
S_IWUSR equ 128
S_IRGRP equ 32
S_IROTH equ 4

SEEK_SET equ 0
SEEK_CUR equ 1
SEEK_END equ 2

DT_REG equ 8 ; regular file

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; comtime constants: sockets

AF_INET equ 2 ; set socket domain to IPV4
SOCKET_TYPE_TCP equ 1

SOL_SOCKET equ 1
SO_REUSEADDR equ 2
SO_REUSEPORT equ 15
SO_RCVTIMEO equ 20

SHUT_RD equ 0 ; shitdown read
SHUT_WR equ 1 ; shutdown write
SHUT_RDWR equ 2 ; shutdown read and write

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; macros: syscalls: shorthands

macro sys0 syscall_id
{
    mov rax, syscall_id
    syscall
}

macro sys1 syscall_id, arg1
{
    mov rdi, arg1
    sys0 syscall_id
}

macro sys2 syscall_id, arg1, arg2
{
    mov rsi, arg2
    sys1 syscall_id, arg1
}

macro sys3 syscall_id, arg1, arg2, arg3
{
    mov rdx, arg3
    sys2 syscall_id, arg1, arg2
}

macro sys4 syscall_id, arg1, arg2, arg3, arg4
{
    mov r10, arg4
    sys3 syscall_id, arg1, arg2, arg3
}

macro sys5 syscall_id, arg1, arg2, arg3, arg4, arg5
{
    mov r8, arg5
    sys4 syscall_id, arg1, arg2, arg3, arg4
}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; macros: syscalls: generic

macro sys_exit ret_code {
    sys1 SYSCALL_EXIT, ret_code
}

macro sys_fork {
    sys0 SYSCALL_FORK
}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; macros: syscalls: files

macro sys_chdir new_cwd {
    sys1 SYSCALL_CHDIR, new_cwd
}

macro sys_close fd {
    sys1 SYSCALL_CLOSE, fd
}

macro sys_ftruncate fd, length {
    sys2 SYSCALL_FTRUNCATE, fd, length
}

macro sys_getdents fd, linux_dirent, linux_dirent_size {
    sys3 SYSCALL_GETDENTS, fd, linux_dirent, linux_dirent_size
}

macro sys_lseek fd, offset, whence {
    sys3 SYSCALL_LSEEK, fd, offset, whence
}

macro sys_open file_name, flags, mode {
    sys3 SYSCALL_OPEN, file_name, flags, mode
}

macro sys_openat folder_fd, file_name, flags, mode {
    sys4 SYSCALL_OPENAT, folder_fd, file_name, flags, mode
}

macro sys_read fd, buf, size {
    sys3 SYSCALL_READ, fd, buf, size
}

macro sys_write fd, msg, msg_len {
    sys3 SYSCALL_WRITE, fd, msg, msg_len
}

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; macros: syscalls: networking

macro sys_accept sockfd, addr, addr_len {
    sys3 SYSCALL_ACCEPT, sockfd, addr, addr_len
}

macro sys_bind sockfd, addr, addr_len {
    sys3 SYSCALL_BIND, sockfd, addr, addr_len
}

macro sys_listen sockfd, max_connections {
    sys2 SYSCALL_LISTEN, sockfd, max_connections
}

macro sys_sendfile fd_out, fd_in, offset, count {
    sys4 SYSCALL_SENDFILE, fd_out, fd_in, offset, count
}

macro sys_setsockopt fd, level, optname, optval, optlen {
    sys5 SYSCALL_SETSOCKOPT, fd, level, optname, optval, optlen
}

macro sys_shutdown socket, how {
    sys2 SYSCALL_SHUTDOWN, socket, how
}

macro sys_socket domain, type, protocol {
    sys3 SYSCALL_SOCKET, domain, type, protocol
}
