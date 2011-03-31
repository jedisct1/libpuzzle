#include "puzzle_common.h"
#include "puzzle_p.h"
#include "puzzle.h"
#include "globals.h"

int puzzle_vector_sub(PuzzleContext * const context,
                      PuzzleCvec * const cvecr,
                      const PuzzleCvec * const cvec1,
                      const PuzzleCvec * const cvec2,
                      const int fix_for_texts)
{
    size_t remaining;
    signed char c1, c2, cr;

    (void) context;
    if (cvec1->sizeof_vec != cvec2->sizeof_vec ||
        cvec1->sizeof_vec <= (size_t) 0U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if (cvecr->vec != NULL) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    cvecr->sizeof_vec = cvec1->sizeof_vec;    
    if ((cvecr->vec = calloc(cvecr->sizeof_vec, sizeof *cvecr->vec)) == NULL) {
        return -1;
    }
    remaining = cvec1->sizeof_vec;
    if (fix_for_texts != 0) {
        do {
            remaining--;
            c1 = cvec1->vec[remaining];
            c2 = cvec2->vec[remaining];
            if ((c1 == 0 && c2 == -2) || (c1 == -2 && c2 == 0)) {
                cr = -3;
            } else if ((c1 == 0 && c2 == +2) || (c1 == +2 && c2 == 0)) {
                cr = +3;
            } else {
                cr = c1 - c2;
            }
            cvecr->vec[remaining] = cr;
        } while (remaining > (size_t) 0U);        
    } else {
        do {
            remaining--;
            cvecr->vec[remaining] =
                cvec1->vec[remaining] - cvec2->vec[remaining];
        } while (remaining > (size_t) 0U);
    }
    return 0;
}

double puzzle_vector_euclidean_length(PuzzleContext * const context,
                                      const PuzzleCvec * const cvec)
{
    unsigned long t = 0U;
    unsigned long c;
    int c2;
    size_t remaining;

    (void) context;
    if ((remaining = cvec->sizeof_vec) <= (size_t) 0U) {
        return 0.0;
    }
    do {
        remaining--;
        c2 = (int) cvec->vec[remaining];
        c = (unsigned long) (c2 * c2);
        if (ULONG_MAX - t < c) {
            puzzle_err_bug(__FILE__, __LINE__);
        }
        t += c;
    } while (remaining > (size_t) 0U);
    
    return sqrt((double) t);
}

double puzzle_vector_normalized_distance(PuzzleContext * const context,
                                         const PuzzleCvec * const cvec1,
                                         const PuzzleCvec * const cvec2,
                                         const int fix_for_texts)
{
    PuzzleCvec cvecr;
    double dt, dr;

    puzzle_init_cvec(context, &cvecr);
    puzzle_vector_sub(context, &cvecr, cvec1, cvec2, fix_for_texts);
    dt = puzzle_vector_euclidean_length(context, &cvecr);
    puzzle_free_cvec(context, &cvecr);
    dr = puzzle_vector_euclidean_length(context, cvec1)
        + puzzle_vector_euclidean_length(context, cvec2);
    if (dr == 0.0) {
        return 0.0;
    }
    return dt / dr;
}
