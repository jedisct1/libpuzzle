#include "puzzle_common.h"
#include "puzzle.h"

#define PUZZLE_VECTOR_SLICE 0.6

int main(void)
{
    PuzzleContext context;
    PuzzleCvec cvec1, cvec2;
    double d1, d2;
    
    puzzle_init_context(&context);
    puzzle_init_cvec(&context, &cvec1);
    puzzle_init_cvec(&context, &cvec2);
    if (puzzle_fill_cvec_from_file(&context, &cvec1,
                                   "pics/pic-a-0.jpg") != 0) {
        fprintf(stderr, "File 1 not found\n");
        exit(0);
    }    
    if (puzzle_fill_cvec_from_file(&context, &cvec2,
                                   "pics/pic-a-1.jpg") != 0) {
        fprintf(stderr, "File 2 not found\n");
        exit(0);
    }
    d1 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec2, 1);
    d2 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec2, 0);
    printf("%g %g\n", d1, d2);
    puzzle_free_cvec(&context, &cvec1);
    puzzle_free_cvec(&context, &cvec2);
    puzzle_free_context(&context);
    if (d1 > PUZZLE_VECTOR_SLICE || d2 > PUZZLE_VECTOR_SLICE) {
        return 2;
    }
    return 0;
}
