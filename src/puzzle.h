#ifndef __PUZZLE_H__
#define __PUZZLE_H__ 1

#define PUZZLE_VERSION_MAJOR 0
#define PUZZLE_VERSION_MINOR 11

typedef struct PuzzleDvec_ {
    size_t sizeof_vec;
    size_t sizeof_compressed_vec;
    double *vec;
} PuzzleDvec;

typedef struct PuzzleCvec_ {
    size_t sizeof_vec;
    signed char *vec;
} PuzzleCvec;

typedef struct PuzzleCompressedCvec_ {
    size_t sizeof_compressed_vec;
    unsigned char *vec;
} PuzzleCompressedCvec;

typedef struct PuzzleContext_ {
    unsigned int puzzle_max_width;
    unsigned int puzzle_max_height;
    unsigned int puzzle_lambdas;
    double puzzle_p_ratio;
    double puzzle_noise_cutoff;
    double puzzle_contrast_barrier_for_cropping;
    double puzzle_max_cropping_ratio;
    int puzzle_enable_autocrop;
    unsigned long magic;    
} PuzzleContext;

void puzzle_init_context(PuzzleContext * const context);
void puzzle_free_context(PuzzleContext * const context);
int puzzle_set_max_width(PuzzleContext * const context,
                         const unsigned int width);
int puzzle_set_max_height(PuzzleContext * const context,
                          const unsigned int height);
int puzzle_set_lambdas(PuzzleContext * const context,
                       const unsigned int lambdas);
int puzzle_set_noise_cutoff(PuzzleContext * const context,
                            const double noise_cutoff);
int puzzle_set_p_ratio(PuzzleContext * const context,
                       const double p_ratio);
int puzzle_set_contrast_barrier_for_cropping(PuzzleContext * const context,
                                             const double barrier);
int puzzle_set_max_cropping_ratio(PuzzleContext * const context,
                                  const double ratio);
int puzzle_set_autocrop(PuzzleContext * const context,
                        const int enable);
void puzzle_init_cvec(PuzzleContext * const context,
                      PuzzleCvec * const cvec);
void puzzle_init_dvec(PuzzleContext * const context,
                      PuzzleDvec * const dvec);
int puzzle_fill_dvec_from_file(PuzzleContext * const context,
                               PuzzleDvec * const dvec,
                               const char * const file);
int puzzle_fill_cvec_from_file(PuzzleContext * const context,
                               PuzzleCvec * const cvec,
                               const char * const file);
int puzzle_fill_cvec_from_dvec(PuzzleContext * const context,
                               PuzzleCvec * const cvec,
                               const PuzzleDvec * const dvec);
void puzzle_free_cvec(PuzzleContext * const context,
                      PuzzleCvec * const cvec);
void puzzle_free_dvec(PuzzleContext * const context,
                      PuzzleDvec * const dvec);
int puzzle_dump_cvec(PuzzleContext * const context,
                     const PuzzleCvec * const cvec);
int puzzle_dump_dvec(PuzzleContext * const context,
                     const PuzzleDvec * const dvec);
int puzzle_cvec_cksum(PuzzleContext * const context,
                      const PuzzleCvec * const cvec, unsigned int * const sum);
void puzzle_init_compressed_cvec(PuzzleContext * const context,
                                 PuzzleCompressedCvec * const compressed_cvec);
void puzzle_free_compressed_cvec(PuzzleContext * const context,
                                 PuzzleCompressedCvec * const compressed_cvec);
int puzzle_compress_cvec(PuzzleContext * const context,
                         PuzzleCompressedCvec * const compressed_cvec,
                         const PuzzleCvec * const cvec);
int puzzle_uncompress_cvec(PuzzleContext * const context,
                           const PuzzleCompressedCvec * const compressed_cvec,
                           PuzzleCvec * const cvec);
int puzzle_vector_sub(PuzzleContext * const context,
                      PuzzleCvec * const cvecr,
                      const PuzzleCvec * const cvec1,
                      const PuzzleCvec * const cvec2,
                      const int fix_for_texts);
double puzzle_vector_euclidean_length(PuzzleContext * const context,
                                      const PuzzleCvec * const cvec);
double puzzle_vector_normalized_distance(PuzzleContext * const context,
                                         const PuzzleCvec * const cvec1,
                                         const PuzzleCvec * const cvec2,
                                         const int fix_for_texts);

#define PUZZLE_CVEC_SIMILARITY_THRESHOLD 0.6
#define PUZZLE_CVEC_SIMILARITY_HIGH_THRESHOLD 0.7
#define PUZZLE_CVEC_SIMILARITY_LOW_THRESHOLD 0.3
#define PUZZLE_CVEC_SIMILARITY_LOWER_THRESHOLD 0.2

#define _COMA_ ,

#endif
