#ifndef __GLOBALS_H__
#define __GLOBALS_H__ 1

#ifdef DEFINE_GLOBALS
# define GLOBAL0(A) A
# define GLOBAL(A, B) A = B
#else
# define GLOBAL0(A) extern A
# define GLOBAL(A, B) extern A
#endif

GLOBAL(PuzzleContext puzzle_global_context,
{
        /* unsigned int puzzle_max_width */ PUZZLE_DEFAULT_MAX_WIDTH _COMA_
        /* unsigned int puzzle_max_height */ PUZZLE_DEFAULT_MAX_HEIGHT _COMA_
        /* unsigned int puzzle_lambdas */ PUZZLE_DEFAULT_LAMBDAS _COMA_
        /* double puzzle_p_ratio */ PUZZLE_DEFAULT_P_RATIO _COMA_
        /* double puzzle_noise_cutoff */ PUZZLE_DEFAULT_NOISE_CUTOFF _COMA_
        /* double puzzle_contrast_barrier_for_cropping */
        PUZZLE_DEFAULT_CONTRAST_BARRIER_FOR_CROPPING _COMA_
        /* double puzzle_max_cropping_ratio */
        PUZZLE_DEFAULT_MAX_CROPPING_RATIO _COMA_
        /* int puzzle_enable_autocrop */ PUZZLE_DEFAULT_ENABLE_AUTOCROP _COMA_
        /* unsigned long magic */ PUZZLE_CONTEXT_MAGIC _COMA_        
});
#endif
