#include "puzzle_common.h"
#include "puzzle_p.h"
#include "puzzle.h"
#include "globals.h"

static int puzzle_median_cmp(const void * const a_, const void * const b_)
{
    const double a = * (const double *) a_;
    const double b = * (const double *) b_;
    
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    }
    return 0;
}

static double puzzle_median(double * const vec, size_t size)
{
    size_t n;
    size_t o;
    double avg;

    if (size <= (size_t) 0U) {
        return 0.0;
    }
    qsort((void *) vec, size, sizeof *vec, puzzle_median_cmp);
    if ((n = size / (size_t) 2U) == (size_t) 0U) {
        if (size > (size_t) 1U) {
            o = (size_t) 1U;
        } else {
            o = (size_t) 0U;
        }
    } else {
        o = n + (size_t) 1U;
    }
    if (o < n) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    avg = (vec[n] + vec[o]) / 2.0;
    if (avg < vec[n] || avg > vec[o]) {
        avg = vec[n];
    }
    return avg;
}

int puzzle_fill_cvec_from_dvec(PuzzleContext * const context,
                               PuzzleCvec * const cvec,
                               const PuzzleDvec * const dvec)
{
    size_t s;
    const double *dvecptr;
    signed char *cvecptr;
    double *lights = NULL, *darks = NULL;
    size_t pos_lights = (size_t) 0U, pos_darks = (size_t) 0U;
    size_t sizeof_lights, sizeof_darks;
    double lighter_cutoff, darker_cutoff;
    int err = 0;    
    double dv;

    if ((cvec->sizeof_vec = dvec->sizeof_compressed_vec) <= (size_t) 0U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if ((cvec->vec = calloc(cvec->sizeof_vec, sizeof *cvec->vec)) == NULL) {
        return -1;
    }
    sizeof_lights = sizeof_darks = cvec->sizeof_vec;
    if ((lights = calloc(sizeof_lights, sizeof *lights)) == NULL ||
        (darks = calloc(sizeof_darks, sizeof *darks)) == NULL) {
        err = -1;
        goto out;
    }
    dvecptr = dvec->vec;
    s = cvec->sizeof_vec;
    do {
        dv = *dvecptr++;
        if (dv >= - context->puzzle_noise_cutoff &&
            dv <= context->puzzle_noise_cutoff) {
            continue;
        }
        if (dv < context->puzzle_noise_cutoff) {
            darks[pos_darks++] = dv;
            if (pos_darks > sizeof_darks) {
                puzzle_err_bug(__FILE__, __LINE__);
            }
        } else if (dv > context->puzzle_noise_cutoff) {
            lights[pos_lights++] = dv;
            if (pos_lights > sizeof_lights) {
                puzzle_err_bug(__FILE__, __LINE__);
            }
        }
    } while (--s != (size_t) 0U);
    lighter_cutoff = puzzle_median(lights, pos_lights);
    darker_cutoff = puzzle_median(darks, pos_darks);    
    free(lights);
    lights = NULL;
    free(darks);
    darks = NULL;
    dvecptr = dvec->vec;
    cvecptr = cvec->vec;
    s = cvec->sizeof_vec;
    do {
        dv = *dvecptr++;
        if (dv >= - context->puzzle_noise_cutoff &&
            dv <= context->puzzle_noise_cutoff) {
            *cvecptr++ = 0;
        } else if (dv < 0.0) {
            *cvecptr++ = dv < darker_cutoff ? -2 : -1;
        } else {
            *cvecptr++ = dv > lighter_cutoff ? +2 : +1;
        }
    } while (--s != (size_t) 0U);
    if ((size_t) (cvecptr - cvec->vec) != cvec->sizeof_vec) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    out:
    free(lights);
    free(darks);
    
    return err;
}

void puzzle_init_cvec(PuzzleContext * const context, PuzzleCvec * const cvec)
{
    (void) context;
    cvec->sizeof_vec = (size_t) 0U;
    cvec->vec = NULL;
}

void puzzle_free_cvec(PuzzleContext * const context, PuzzleCvec * const cvec)
{
    (void) context;    
    free(cvec->vec);
    cvec->vec = NULL;
}

int puzzle_dump_cvec(PuzzleContext * const context,
                     const PuzzleCvec * const cvec)
{
    size_t s = cvec->sizeof_vec;
    const signed char *vecptr = cvec->vec;
    
    (void) context;    
    if (s <= (size_t) 0U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    do {
        printf("%d\n", *vecptr++);
    } while (--s != (size_t) 0U);
    
    return 0;
}

int puzzle_cvec_cksum(PuzzleContext * const context,
                      const PuzzleCvec * const cvec, unsigned int * const sum)
{
    size_t s = cvec->sizeof_vec;
    const signed char *vecptr = cvec->vec;

    (void) context;    
    *sum = 5381;
    do {
        *sum += *sum << 5;
        *sum ^= (unsigned int) *vecptr++;
    } while (--s != (size_t) 0U);
    
    return 0;
}

int puzzle_fill_cvec_from_file(PuzzleContext * const context,
                               PuzzleCvec * const cvec,
                               const char * const file)
{
    PuzzleDvec dvec;
    int ret;
    
    puzzle_init_dvec(context, &dvec);
    if ((ret = puzzle_fill_dvec_from_file(context, &dvec, file)) == 0) {
        ret = puzzle_fill_cvec_from_dvec(context, cvec, &dvec);
    }
    puzzle_free_dvec(context, &dvec);
    
    return ret;
}
