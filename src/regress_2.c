#include "puzzle_common.h"
#include "puzzle.h"

int main(void)
{
    PuzzleContext context;
    PuzzleCvec cvec1, cvec2, cvec3, cvec4, cvec5, cvec6;
    double d1, d2, d3, d4, d5, d6;
    
    puzzle_init_context(&context);
    puzzle_init_cvec(&context, &cvec1);
    puzzle_init_cvec(&context, &cvec2);
    puzzle_init_cvec(&context, &cvec3);
    puzzle_init_cvec(&context, &cvec4);
    puzzle_init_cvec(&context, &cvec5);
    puzzle_init_cvec(&context, &cvec6);
    if (puzzle_fill_cvec_from_file
        (&context, &cvec1, "pics/luxmarket_tshirt01.jpg") != 0) {
        fprintf(stderr, "File 1 not found\n");
        exit(0);
    }    
    if (puzzle_fill_cvec_from_file
        (&context, &cvec2, "pics/luxmarket_tshirt01_black.jpg") != 0) {
        fprintf(stderr, "File 2 not found\n");
        exit(0);
    }
    if (puzzle_fill_cvec_from_file
        (&context, &cvec3, "pics/luxmarket_tshirt01_sal.jpg") != 0) {
        fprintf(stderr, "File 3 not found\n");
        exit(0);
    }
    if (puzzle_fill_cvec_from_file
        (&context, &cvec4, "pics/luxmarket_tshirt01_sheum.jpg") != 0) {
        fprintf(stderr, "File 4 not found\n");
        exit(0);
    }
    if (puzzle_fill_cvec_from_file
        (&context, &cvec5, "pics/duck.gif") != 0) {
        fprintf(stderr, "File 5 not found\n");
        exit(0);
    }
    if (puzzle_fill_cvec_from_file
        (&context, &cvec6, "pics/pic-a-0.jpg") != 0) {
        fprintf(stderr, "File 6 not found\n");
        exit(0);
    }
    d1 = puzzle_vector_normalized_distance(&context, &cvec2, &cvec1, 1);
    d2 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec2, 1);
    d3 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec3, 1);
    d4 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec4, 1);
    d5 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec5, 1);
    d6 = puzzle_vector_normalized_distance(&context, &cvec1, &cvec6, 1);
    printf("%g %g %g %g %g %g\n", d1, d2, d3, d4, d5, d6);
    puzzle_free_cvec(&context, &cvec1);
    puzzle_free_cvec(&context, &cvec2);    
    puzzle_free_cvec(&context, &cvec3);    
    puzzle_free_cvec(&context, &cvec4);    
    puzzle_free_cvec(&context, &cvec5);    
    puzzle_free_cvec(&context, &cvec6);
    puzzle_free_context(&context);
    if ((int) (d1 * 100.0) != (int) (d2 * 100.0)) {
        return 1;
    }
    if (d1 > PUZZLE_CVEC_SIMILARITY_THRESHOLD ||
        d3 > PUZZLE_CVEC_SIMILARITY_THRESHOLD ||
        d4 > PUZZLE_CVEC_SIMILARITY_THRESHOLD ||
        d5 < PUZZLE_CVEC_SIMILARITY_THRESHOLD ||
        d6 < PUZZLE_CVEC_SIMILARITY_THRESHOLD) {
        return 2;
    }
    return 0;
}
