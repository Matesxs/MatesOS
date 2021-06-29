#include "stivale2.h"
#include "stddef.h"
#include "../library/string.h"
#include "../library/memory.h"

static uint8_t stack[4096 * 32];

static struct stivale2_header_tag_smp smp = {
		.tag = {
				.identifier = STIVALE2_HEADER_TAG_SMP_ID,
				.next = 0},
		.flags = 0};

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
		.tag = {
				.identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
				.next = (uintptr_t)&smp},
		.flags = 0};

static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
		.tag = {
				.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
				.next = (uint64_t)&terminal_hdr_tag},
		.framebuffer_width = 0,
		.framebuffer_height = 0,
		.framebuffer_bpp = 0};

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header stivale_hdr = {
		.entry_point = 0,
		.stack = (uintptr_t)stack + sizeof(stack),
		.flags = (1 << 1),
		.tags = (uintptr_t)&framebuffer_hdr_tag};

void *stivale2_get_tag(stivale2_struct *stivale2_struct, uint64_t id)
{
	stivale2_tag *current_tag = (stivale2_tag *)stivale2_struct->tags;
	while (current_tag)
	{
		if (current_tag == NULL)
			return NULL;
		if (current_tag->identifier == id)
			return current_tag;
		current_tag = (stivale2_tag *)current_tag->next;
	}

	return NULL;
}


void stivale2_get_module(stivale2_struct_tag_modules *modules, stivale2_module *module, const char *name)
{
	for (size_t i = 0; i < modules->module_count; i++)
  {
		stivale2_module me = modules->modules[i];
		if (strcmp(name, me.string) == 0)
		{
			module->begin = me.begin;
			module->end = me.end;
			memcpy(module->string, me.string, 128 * sizeof(char));
			return;
		}
	}
}
