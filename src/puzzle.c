#define DEFINE_GLOBALS 1
#include "puzzle_common.h"
#include "puzzle_p.h"
#include "puzzle.h"
#include "globals.h"

void puzzle_init_context(PuzzleContext * const context)
{
    *context = puzzle_global_context;
}

void puzzle_free_context(PuzzleContext * const context)
{
    (void) context;
}

void puzzle_err_bug(const char * const file, const int line)
{
    fprintf(stderr, "*BUG* File: [%s] Line: [%d]\n", file, line);
    abort();
}

