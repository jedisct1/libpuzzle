#include "puzzle_common.h"
#include "puzzle_p.h"
#include "puzzle.h"
#include "globals.h"

void puzzle_init_compressed_cvec(PuzzleContext * const context,
                                 PuzzleCompressedCvec * const compressed_cvec)
{
    (void) context;
    compressed_cvec->sizeof_compressed_vec = (size_t) 0U;
    compressed_cvec->vec = NULL;
}

void puzzle_free_compressed_cvec(PuzzleContext * const context,
                                 PuzzleCompressedCvec * const compressed_cvec)
{
    (void) context;    
    free(compressed_cvec->vec);
    compressed_cvec->vec = NULL;    
}

int puzzle_compress_cvec(PuzzleContext * const context,
                         PuzzleCompressedCvec * const compressed_cvec,
                         const PuzzleCvec * const cvec)
{
#define PC_NM(X) ((unsigned char) ((X) + 2))
    size_t remaining = cvec->sizeof_vec;
    const signed char *ptr;    
    unsigned char *cptr;

    (void) context;
    compressed_cvec->sizeof_compressed_vec =
        (cvec->sizeof_vec + (size_t) 2U) / (size_t) 3U;
    if ((compressed_cvec->vec =
         calloc(compressed_cvec->sizeof_compressed_vec,
                sizeof *compressed_cvec->vec)) == NULL) {
        return -1;
    }
    ptr = cvec->vec;
    cptr = compressed_cvec->vec;
    while (remaining > (size_t) 3U) {
        *cptr++ = PC_NM(ptr[0]) + PC_NM(ptr[1]) * 5U +
            PC_NM(ptr[2]) * (5U * 5U);
        ptr += 3U;
        remaining -= 3U;
    }
    if (remaining == (size_t) 1U) {
        *cptr++ = PC_NM(ptr[0]);
        compressed_cvec->vec[0] |= 128U;
    } else if (remaining == (size_t) 2U) {
        *cptr++ = PC_NM(ptr[0]) + PC_NM(ptr[1]) * 5U;
        if (compressed_cvec->sizeof_compressed_vec < (size_t) 2U) {
            puzzle_err_bug(__FILE__, __LINE__);
        }
        compressed_cvec->vec[1] |= 128U;
    }
    if ((size_t) (cptr - compressed_cvec->vec) !=
        compressed_cvec->sizeof_compressed_vec) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    return 0;
}

int puzzle_uncompress_cvec(PuzzleContext * const context,
                           const PuzzleCompressedCvec * const compressed_cvec,
                           PuzzleCvec * const cvec)
{
#define PC_FL(X) ((X) & 127U)
#define PC_NP(X) ((signed char) (X) - 2)
    
    size_t remaining;    
    unsigned char trailing_bits;
    const unsigned char *cptr = compressed_cvec->vec;
    signed char *ptr;
    unsigned char c;

    (void) context;
    if (cvec->vec != NULL) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if ((remaining = compressed_cvec->sizeof_compressed_vec) < (size_t) 2U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    trailing_bits = ((cptr[0] & 128U) >> 7) | ((cptr[1] & 128U) >> 6);
    if (trailing_bits > 2U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    cvec->sizeof_vec = (size_t) 3U *
        (compressed_cvec->sizeof_compressed_vec - trailing_bits) +
        trailing_bits;
    if (compressed_cvec->sizeof_compressed_vec >
        SIZE_MAX / (size_t) 3U - (size_t) 2U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if ((cvec->vec = calloc(cvec->sizeof_vec, sizeof *cvec->vec)) == NULL) {
        return -1;
    }
    if (trailing_bits != 0U) {
        if (remaining <= (size_t) 0U) {
            puzzle_err_bug(__FILE__, __LINE__);
        }
        remaining--;
    }
    ptr = cvec->vec;
    while (remaining > (size_t) 0U) {
        c = PC_FL(*cptr++);
        *ptr++ = PC_NP(c % 5U);
        c /= 5U;
        *ptr++ = PC_NP(c % 5U);
        c /= 5U;
        *ptr++ = PC_NP(c % 5U);
        remaining--;
    }
    if (trailing_bits == 1U) {
        *ptr++ = PC_NP(PC_FL(*cptr) % 5U);        
    } else if (trailing_bits == 2U) {
        c = PC_FL(*cptr);
        *ptr++ = PC_NP(c % 5U);
        *ptr++ = PC_NP(c / 5U % 5U);        
    }
    if ((size_t) (ptr - cvec->vec) != cvec->sizeof_vec) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    return 0;
}
