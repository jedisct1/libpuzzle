#ifndef __PUZZLE_P_H__
#define __PUZZLE_P_H__ 1

#include <math.h>
#include <gd.h>

typedef struct PuzzleView_ {
    unsigned int width;
    unsigned int height;
    size_t sizeof_map;
    unsigned char *map;
} PuzzleView;

typedef struct PuzzleAvgLvls_ {
    unsigned int lambdas;
    size_t sizeof_lvls;
    double *lvls;
} PuzzleAvgLvls;

typedef enum PuzzleImageTypeCode_ {
    PUZZLE_IMAGE_TYPE_ERROR, PUZZLE_IMAGE_TYPE_UNKNOWN, PUZZLE_IMAGE_TYPE_JPEG,
        PUZZLE_IMAGE_TYPE_GIF, PUZZLE_IMAGE_TYPE_PNG
} PuzzleImageTypeCode;

typedef struct PuzzleImageType_ {
    const size_t sizeof_signature;    
    const unsigned char *signature;
    const PuzzleImageTypeCode image_type_code;
} PuzzleImageType;

#ifndef SIZE_MAX
# define SIZE_MAX ((size_t) -1)
#endif

#define PUZZLE_DEFAULT_LAMBDAS 9
#define PUZZLE_DEFAULT_MAX_WIDTH  3000
#define PUZZLE_DEFAULT_MAX_HEIGHT 3000
#define PUZZLE_DEFAULT_NOISE_CUTOFF 2.0
#define PUZZLE_DEFAULT_P_RATIO 2.0
#define PUZZLE_MIN_P 2
#define PUZZLE_PIXEL_FUZZ_SIZE 1
#define PUZZLE_NEIGHBORS 8
#define PUZZLE_MIN_SIZE_FOR_CROPPING 100
#if     PUZZLE_MIN_SIZE_FOR_CROPPING < 4
# error PUZZLE_MIN_SIZE_FOR_CROPPING
#endif
#define PUZZLE_DEFAULT_CONTRAST_BARRIER_FOR_CROPPING 0.05
#define PUZZLE_DEFAULT_MAX_CROPPING_RATIO 0.25
#define PUZZLE_DEFAULT_ENABLE_AUTOCROP 1

#define PUZZLE_VIEW_PIXEL(V, X, Y) (*((V)->map + (V)->width * (Y) + (X)))
#define PUZZLE_AVGLVL(A, X, Y) (*((A)->lvls + (A)->lambdas * (Y) + (X)))

#define PUZZLE_CONTEXT_MAGIC 0xdeadbeef

#ifndef MIN
# define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif
#ifndef MAX
# define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif
#define SUCC(A) ((A) + 1)
#define PRED(A) ((A) - 1)

void puzzle_err_bug(const char * const file, const int line);

#endif
