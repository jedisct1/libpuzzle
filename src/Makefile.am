lib_LTLIBRARIES = \
	libpuzzle.la

libpuzzle_la_LDFLAGS = -version-info 1:0

libpuzzle_la_SOURCES = \
	puzzle.c \
	tunables.c \
	dvec.c \
	cvec.c \
	compress.c \
	vector_ops.c \
	puzzle_common.h \
	puzzle_p.h \
	globals.h \
	puzzle.h

include_HEADERS = \
	puzzle.h

noinst_HEADERS = \
	puzzle_common.h \
	puzzle_p.h \
	globals.h

bin_PROGRAMS = \
	puzzle-diff
        
puzzle_diff_SOURCES = \
	puzzle-diff.c \
	puzzle_common.h \
	puzzle.h

puzzle_diff_LDADD = \
	libpuzzle.la

TESTS = \
	regress_1 \
	regress_2 \
	regress_3

check_PROGRAMS = \
	regress_1 \
	regress_2 \
	regress_3

regress_1_SOURCES = \
	regress_1.c \
	puzzle_common.h \
	puzzle.h

regress_2_SOURCES = \
	regress_2.c \
	puzzle_common.h \
	puzzle.h

regress_3_SOURCES = \
	regress_3.c \
	puzzle_common.h \
	puzzle.h

regress_1_LDADD = \
	libpuzzle.la

regress_2_LDADD = \
	libpuzzle.la

regress_3_LDADD = \
	libpuzzle.la

SUBDIRS = \
	pics
