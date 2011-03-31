dnl config.m4 for extension libpuzzle

dnl If your extension references something external, use with:

PHP_ARG_WITH(libpuzzle, for libpuzzle support,
             [  --with-libpuzzle        Include libpuzzle support])

if test "$PHP_LIBPUZZLE" != "no"; then
  for i in $PHP_LIBPUZZLE /usr/local /usr; do
    if test -x "$i/bin/gdlib-config"; then
      GDLIB_CONFIG=$i/bin/gdlib-config
      break
    fi
  done
  GDLIB_LIBS=$($GDLIB_CONFIG --ldflags --libs)
  GDLIB_INCS=$($GDLIB_CONFIG --cflags)
  
  PHP_EVAL_LIBLINE($GDLIB_LIBS, LIBPUZZLE_SHARED_LIBADD)
  PHP_EVAL_INCLINE($GDLIB_INCS)
                            
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/puzzle.h"  # you most likely want to change this
  if test -r $PHP_LIBPUZZLE/$SEARCH_FOR; then # path given as parameter
     LIBPUZZLE_DIR=$PHP_LIBPUZZLE
  else # search default path list
     AC_MSG_CHECKING([for libpuzzle files in default path])
     for i in $SEARCH_PATH ; do
       if test -r $i/$SEARCH_FOR; then
         LIBPUZZLE_DIR=$i
         AC_MSG_RESULT(found in $i)
       fi
     done
  fi

  if test -z "$LIBPUZZLE_DIR"; then
     AC_MSG_RESULT([not found])
     AC_MSG_ERROR([Please reinstall the libpuzzle distribution])
  fi

  dnl # --with-libpuzzle -> add include path
  PHP_ADD_INCLUDE($LIBPUZZLE_DIR/include)

  PHP_ADD_LIBRARY_WITH_PATH(puzzle, $LIBPUZZLE_DIR/lib,
                            LIBPUZZLE_SHARED_LIBADD)
                          
  PHP_SUBST(LIBPUZZLE_SHARED_LIBADD)

  PHP_NEW_EXTENSION(libpuzzle, libpuzzle.c, $ext_shared)
fi
