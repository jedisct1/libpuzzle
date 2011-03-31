#ifndef PHP_LIBPUZZLE_H
#define PHP_LIBPUZZLE_H

extern zend_module_entry libpuzzle_module_entry;
#define phpext_libpuzzle_ptr &libpuzzle_module_entry

#ifdef PHP_WIN32
#define PHP_LIBPUZZLE_API __declspec(dllexport)
#else
#define PHP_LIBPUZZLE_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(libpuzzle);
PHP_MSHUTDOWN_FUNCTION(libpuzzle);
PHP_RINIT_FUNCTION(libpuzzle);
PHP_RSHUTDOWN_FUNCTION(libpuzzle);
PHP_MINFO_FUNCTION(libpuzzle);

PHP_FUNCTION(puzzle_set_max_width);
PHP_FUNCTION(puzzle_set_max_height);
PHP_FUNCTION(puzzle_set_lambdas);
PHP_FUNCTION(puzzle_set_noise_cutoff);
PHP_FUNCTION(puzzle_set_p_ratio);
PHP_FUNCTION(puzzle_set_contrast_barrier_for_cropping);
PHP_FUNCTION(puzzle_set_max_cropping_ratio);
PHP_FUNCTION(puzzle_set_autocrop);

PHP_FUNCTION(puzzle_fill_cvec_from_file);
PHP_FUNCTION(puzzle_compress_cvec);
PHP_FUNCTION(puzzle_uncompress_cvec);
PHP_FUNCTION(puzzle_vector_normalized_distance);

ZEND_BEGIN_MODULE_GLOBALS(libpuzzle)
	PuzzleContext  global_context;
ZEND_END_MODULE_GLOBALS(libpuzzle)

/* In every utility function you add that needs to use variables 
   in php_libpuzzle_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as LIBPUZZLE_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define LIBPUZZLE_G(v) TSRMG(libpuzzle_globals_id, zend_libpuzzle_globals *, v)
#else
#define LIBPUZZLE_G(v) (libpuzzle_globals.v)
#endif

#endif	/* PHP_LIBPUZZLE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
