/**
 * syscall/syscalls/sys_exec_prog.cpp
 * create_proc syscall
 */
#include "sys_exec_prog.h"

sys_exec_prog::sys_exec_prog(size_t pid, program *prog)
{
	this->pid;
	this->prog = prog;
}

int sys_exec_prog::process()
{
	return exec_program(pid, prog);
}
