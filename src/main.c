#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

/// MINI-MALLOC //////////////////////////////////
struct interval {
    size_t start;
    size_t end;
};
struct interval entries[512];
size_t next_entry = 0;

const size_t LIMIT = 1024;
char* memory[LIMIT];

bool overlaps(struct interval a, struct interval b) {
    return a.start < b.end && b.start < a.end;
}

bool is_free(struct interval interval) {
    for (int i = 0; i < next_entry; i++) {
        if (overlaps(interval, entries[i])) {
            return false;
        }
    }
    return interval.end <= LIMIT;
}

void *mini_malloc(size_t size) {
    if (next_entry >= 512) return NULL; // out of entries
    size_t i = 0;
    // Find next free block
    struct interval interval = {0, size};
    while (!is_free(interval)) {
        interval.start = entries[i].end;
        interval.end = interval.start + size;
        i++;
        if (i > next_entry) {
            return NULL; // out of memory
        }
    }
    entries[next_entry++] = interval;
    return memory + interval.start;
}

void mini_free(const void *ptr) {
    for (int i = 0; i < next_entry; i++) {
        if (memory + entries[i].start == ptr) {
            entries[i] = entries[--next_entry];
            return;
        }
    }
    printf("Bad free! %p\n", ptr);
}

/// TESTS //////////////////////////////////

void test_fill() {
    void* p = mini_malloc(LIMIT); // allocate all memory
    assert(p);
    mini_free(p);
    assert(next_entry == 0);
}

// Allocate memory and then free it
void test_simple() {
    const size_t n = 5;
    void* buffers[n];
    for (int i = 0; i < n; i++) {
        buffers[i] = mini_malloc(10);
        assert(buffers[i]);
    }
    for (int i = 0; i < n; i++) {
        mini_free(buffers[i]);
    }
    assert(next_entry == 0);
}

// Test memor is re-used after free (implementation dependent)
void test_fragment() {
    void* a = mini_malloc(10);
    void* b = mini_malloc(20);
    mini_free(a);
    void *c = mini_malloc(10);
    assert(a == c);
    mini_free(b);
    mini_free(c);
    assert(next_entry == 0);
}

int main() {
    test_fill();
    test_simple();
    test_fragment();
    return 0;
}