#include "puzzle_common.h"
#include "puzzle_p.h"
#include "puzzle.h"
#include "globals.h"

int puzzle_set_max_width(PuzzleContext * const context,
                         const unsigned int width)
{
    if (width <= 0U) {
        return -1;
    }
    context->puzzle_max_width = width;
    
    return 0;
}

int puzzle_set_max_height(PuzzleContext * const context,
                          const unsigned int height)
{
    if (height <= 0U) {
        return -1;
    }
    context->puzzle_max_height = height;
    
    return 0;
}

int puzzle_set_lambdas(PuzzleContext * const context,
                       const unsigned int lambdas)
{
    if (lambdas <= 0U) {
        return -1;
    }
    context->puzzle_lambdas = lambdas;
    
    return 0;
}

int puzzle_set_p_ratio(PuzzleContext * const context, const double p_ratio)
{
    if (p_ratio < 1.0) {
        return -1;
    }
    context->puzzle_p_ratio = p_ratio;
    
    return 0;
}

int puzzle_set_noise_cutoff(PuzzleContext * const context,
                            const double noise_cutoff)
{
    context->puzzle_noise_cutoff = noise_cutoff;

    return 0;
}

int puzzle_set_contrast_barrier_for_cropping(PuzzleContext * const context,
                                             const double barrier)
{
    if (barrier <= 0.0) {
        return -1;
    }
    context->puzzle_contrast_barrier_for_cropping = barrier;

    return 0;
}

int puzzle_set_max_cropping_ratio(PuzzleContext * const context,
                                  const double ratio)
{
    if (ratio <= 0.0) {
        return -1;
    }
    context->puzzle_max_cropping_ratio = ratio;

    return 0;
}

int puzzle_set_autocrop(PuzzleContext * const context, const int enable)
{
    context->puzzle_enable_autocrop = (enable != 0);
    
    return 0;
}
