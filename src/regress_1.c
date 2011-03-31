#include "puzzle_common.h"
#include "puzzle.h"

#define EXPECTED_RESULT 111444570

int main(void)
{
    PuzzleContext context;
    PuzzleCvec cvec;
    PuzzleCompressedCvec compressed_cvec;
    unsigned int sum;
    
    puzzle_init_context(&context);
    puzzle_init_compressed_cvec(&context, &compressed_cvec);
    puzzle_init_cvec(&context, &cvec);
    if (puzzle_fill_cvec_from_file(&context, &cvec,
                                   "pics/luxmarket_tshirt01.jpg") != 0) {
        fprintf(stderr, "File not found\n");
        exit(0);
    }
    puzzle_compress_cvec(&context, &compressed_cvec, &cvec);
    puzzle_free_cvec(&context, &cvec);
    puzzle_init_cvec(&context, &cvec);
    puzzle_uncompress_cvec(&context, &compressed_cvec, &cvec);
    puzzle_cvec_cksum(&context, &cvec, &sum);
    puzzle_free_cvec(&context, &cvec);
    puzzle_free_compressed_cvec(&context, &compressed_cvec);
    puzzle_free_context(&context);
    printf("%u %u\n", sum, (unsigned int) EXPECTED_RESULT);

    return sum != EXPECTED_RESULT;
}
