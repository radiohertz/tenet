#include <tenet/asm.h>
#include <boot/stivale.h>
#include <stdint.h>
#include <stddef.h>
#include <core/string.h>

#define LOOP while (1)

enum {
    STACK_SIZE = 4096 * 4
};

static uint8_t STACK[STACK_SIZE];


static struct stivale2_header_tag_framebuffer FB = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0,
};

static struct stivale2_header_tag_terminal TERM = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = (uint64_t)&FB
    },
    .flags = 0
};

__attribute__((section(".stivale2hdr"), used)) static struct stivale2_header HDR = {
    .entry_point = 0,
    .stack = (uint64_t)&STACK[STACK_SIZE - 1],
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uint64_t)&TERM
};

void* get_tag_by_id(struct stivale2_tag* head, uint64_t id)
{
    struct stivale2_tag* tmp = head;
    while (tmp) {
        if (tmp->identifier == id) {
            return tmp;
        }
        tmp = (void*)tmp->next;
    }

    return NULL;
}

typedef void (*terminal)(const char*, uint64_t);

__attribute__((noreturn)) void _start(struct stivale2_struct* info)
{
    struct stivale2_struct_tag_terminal* t = get_tag_by_id((void*)info->tags, STIVALE2_STRUCT_TAG_TERMINAL_ID);
    if (!t) {
        LOOP {
            asm("nop");
        }
    }

    terminal term = (terminal)t->term_write;
    term("Helo", 4);

    LOOP {
        asm("mov $0xbeef, %rax");
    }
}
