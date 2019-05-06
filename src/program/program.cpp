/**
 * program/program.cpp
 * abstract base class of all user-mode programs
 */
#include <cstdlib>
#include <cassert>
#include "program.h"
#include "../mm/pmem_info.h"
#include "../mm/pmem.h"
#include "../env/env.h"
#include "../utils/panic.h"

using handle_type::type;

template<typename T>
handle<T>::handle()
{
	prog = nullptr;
	addr = 0;
}

template<typename T>
handle<T>::handle(program *prog)
{
	this->prog = prog;
}

template<typename T>
handle<T>::handle(size_t addr, program *prog, type t)
{
	this->prog = prog;
	this->addr = addr;
	this->this_type = t;
}

/* set handle to val */
template<typename T>
handle<T>& handle<T>::operator = (const handle<T> &val)
{
	for (size_t i = 0; i < sizeof(T); i++)
		prog->prog_write(addr + i,
						 prog->prog_read(val.get_addr() + i));
	return *this;
}

/* set handle to val */
template<typename T>
handle<T>& handle<T>::operator = (const T &val)
{
	const char* buf = &val;
	for (size_t i = 0; i < sizeof(T); i++)
		prog->prog_write(addr + i, buf[i]);
	return *this;
}

/* get T from handle */
template<typename T>
handle<T>::operator T() const
{
	char *buf = new char[sizeof(T)];
	for (size_t i = 0; i < sizeof(T); i++)
		buf[i] = prog->prog_read(addr + i);
	delete[] buf;
	T *buf_T = (T*) buf;
	return *buf;
}

template<typename T>
size_t handle<T>::get_addr() const
{
	return this->addr;
}

template<typename T>
program* handle<T>::get_prog() const
{
	return this->prog;
}

template<typename T>
size_t* handle<T>::get_addr_addr()
{
	return &addr;
}

program::program()
{
	compiling = true;
	data = nullptr;
	text_size = data_size = bss_size = 0;
	running = false;
	this->static_init(); // init static info
	compile();           // simulate compile
}

program::~program()
{
	assert(data != nullptr);
	free(data);
}

size_t program::get_text_size() const
{
	return this->text_size;
}

size_t program::get_data_size() const
{
	return this->data_size;
}

size_t program::get_bss_size() const
{
	return this->bss_size;
}

size_t program::get_stack_size() const
{
	return this->stack_size;
}

void program::run()
{
	this->running = true;
}

bool program::is_running() const
{
	return running;
}

bool program::is_compiling() const
{
	return compiling;
}

void program::compile()
{
	long long tot_static = (long long)this->text_size
	    + (long long)this->bss_size
		+ (long long)this->data_size;
	if (tot_static >= VM_SIZE)
		panic("compile failed : MLE");
	this->stack_size = VM_SIZE - tot_static;
	compiling = false;
}

void program::static_init()
{
	
}

char program::prog_read(size_t addr)
{
	return pm::read(addr);
}

void program::prog_write(size_t addr, char data)
{
	return pm::write(addr, data);
}
