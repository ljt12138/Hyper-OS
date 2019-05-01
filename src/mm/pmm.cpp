/*
 * physical memory manager 
 * alloc_page(s) : alloc new page frame(s)
 * free_page(s)  : free allocted page frame(s)
 */

#include <iostream>
#include <list>
#include <cassert>
#include <mutex>
#include "pmm.h"
#include "page_table.h"
#include "pmem.h"
#include "../logging/logging.h"
#include "../env/env.h"
#include "../utils/bus.h"
#include "../utils/allocator/allocator.h"
#include "../utils/allocator/ffma.h"

using std::list;
using logging::debug;
using logging::info;
using logging::log_endl;
using std::mutex;
using std::lock_guard;

typedef allocator::segment segment;

page_frame::page_frame()
{
	this->refer = 0;
	this->alloced = false;
}

void page_frame::ref()
{
	refer++;
}

void page_frame::free()
{
	assert(refer > 0);
	refer--;
}

bool page_frame::die()
{
	return refer == 0;
}

// page frames
page_frame *pages;

// page allocator 
static allocator *alloc;

// mutex for page allocation
static mutex alloc_mutex;

/**
 * get id from page_frame*
 */
int page2id(page_frame *pg)
{
	assert(pg >= pages && pg - pages < PAGE_NUM);
	return pg - pages;
}

/**
 * get page_frame* from id
 */
page_frame* id2page(int id)
{
	assert(id >= 0 && id < PAGE_NUM);
	return pages + id;
}

/**
 * alloc n page frames
 * return page_frame pointer of first page frame(s)
 * @n : number of pages 
 */
page_frame* alloc_pages(int n)
{
	lock_guard<mutex> locker(alloc_mutex);
	int start = alloc->malloc(n);
	page_frame *ret = pages + start;
	ret->paddr = 0;
	ret->alloced = true;
	ret->length = n;
	return ret;
}
 
page_frame* alloc_page()
{ return alloc_pages(1); }

/* free allocted page frames
 * @pg : first page_frame of continuous pages
 */
void free_pages(page_frame *pg)
{
	lock_guard<mutex> locker(alloc_mutex);
	assert(pg->alloced);
	assert(!pg->die());
	pg->free();
	if (pg->die()) { 
		alloc->free(page2id(pg), pg->length);
		pg->alloced = false;
		pg->length = 0;
	}
}

void free_page(page_frame *pg)
{ free_pages(pg); }

/* init physical memory and manager */
void init_pmm()
{
	info << "pmm init." << log_endl;
	init_pm();
	pages = new page_frame[PAGE_NUM];
	alloc = new ffma(PAGE_NUM);
}

/* shutdown physical memory and manager */
void destroy_pmm()
{
	destroy_pm();
	delete alloc;
	delete[] pages;
}

