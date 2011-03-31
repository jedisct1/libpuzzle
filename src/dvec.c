#include "puzzle_common.h"
#include "puzzle_p.h"
#include "puzzle.h"
#include "globals.h"

static void puzzle_init_view(PuzzleView * const view)
{
    view->width = view->height = 0U;
    view->sizeof_map = (size_t) 0U;
    view->map = NULL;
}

static void puzzle_free_view(PuzzleView * const view)
{
    free(view->map);
    view->map = NULL;
}

static void puzzle_init_avglvls(PuzzleAvgLvls * const avglvls)
{
    avglvls->lambdas = 0U;
    avglvls->sizeof_lvls = (size_t) 0U;
    avglvls->lvls = NULL;
}

static void puzzle_free_avglvls(PuzzleAvgLvls * const avglvls)
{
    free(avglvls->lvls);
    avglvls->lvls = NULL;
}

void puzzle_init_dvec(PuzzleContext * const context, PuzzleDvec * const dvec)
{
    (void) context;
    dvec->sizeof_vec = dvec->sizeof_compressed_vec = (size_t) 0U;
    dvec->vec = NULL;
}

void puzzle_free_dvec(PuzzleContext * const context, PuzzleDvec * const dvec)
{
    (void) context;
    free(dvec->vec);
    dvec->vec = NULL;
}

static PuzzleImageTypeCode puzzle_get_image_type_from_fp(FILE * const fp)
{
    #define MAX_SIGNATURE_LENGTH 8U
    static const PuzzleImageType image_types[] = {
        { (size_t) 4U, (const unsigned char *)
            "GIF8", PUZZLE_IMAGE_TYPE_GIF },
        { (size_t) 3U, (const unsigned char *)
            "\xff\xd8\xff", PUZZLE_IMAGE_TYPE_JPEG },
        { (size_t) 8U, (const unsigned char *)
            "\x89PNG\r\n\x1a\n", PUZZLE_IMAGE_TYPE_PNG },
        { (size_t) 0U, NULL, PUZZLE_IMAGE_TYPE_UNKNOWN }
    };
    const PuzzleImageType *image_type = image_types;
    PuzzleImageTypeCode ret = PUZZLE_IMAGE_TYPE_ERROR;
    unsigned char header[MAX_SIGNATURE_LENGTH];
    fpos_t pos;
    
    if (fgetpos(fp, &pos) != 0) {
        return PUZZLE_IMAGE_TYPE_ERROR;
    }
    rewind(fp);
    if (fread(header, (size_t) 1U, sizeof header, fp) != sizeof header) {
        goto bye;
    }
    ret = PUZZLE_IMAGE_TYPE_UNKNOWN;
    do {
        if (image_type->sizeof_signature > sizeof header) {
            puzzle_err_bug(__FILE__, __LINE__);
        }
        if (memcmp(header, image_type->signature,
                   image_type->sizeof_signature) == 0) {
            ret = image_type->image_type_code;
            break;
        }
        image_type++;
    } while (image_type->signature != NULL);
    bye:
    if (fsetpos(fp, &pos) != 0) {
        puzzle_err_bug(__FILE__, __LINE__);
    }    
    return ret;
}

static int puzzle_autocrop_axis(PuzzleContext * const context,
                                PuzzleView * const view,
                                unsigned int * const crop0,
                                unsigned int * const crop1,
                                const unsigned int axisn,
                                const unsigned int axiso,
                                const int omaptrinc, const int nmaptrinc)
{
    double *chunk_contrasts;
    size_t sizeof_chunk_contrasts;
    double chunk_contrast = 0.0, total_contrast = 0.0, barrier_contrast;
    unsigned char level = 0U;
    unsigned char previous_level = 0U;
    unsigned int chunk_n, chunk_o;
    unsigned int chunk_n1, chunk_o1;
    unsigned int max_crop;
    const unsigned char *maptr;

    chunk_n1 = axisn - 1U;
    chunk_o1 = axiso - 1U;
    *crop0 = 0U;
    *crop1 = chunk_n1;
    if (axisn < (unsigned int) PUZZLE_MIN_SIZE_FOR_CROPPING ||
        axiso < (unsigned int) PUZZLE_MIN_SIZE_FOR_CROPPING) {
        return 1;
    }
    sizeof_chunk_contrasts = chunk_n1 + 1U;
    if ((chunk_contrasts = calloc(sizeof_chunk_contrasts,
                                  sizeof *chunk_contrasts)) == NULL) {
        return -1;
    }
    maptr = view->map;
    if (axisn >= INT_MAX || axiso >= INT_MAX) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if (INT_MAX / axisn < axiso) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    chunk_n = chunk_n1;
    do {
        chunk_contrast = 0.0;
        chunk_o = chunk_o1;
        do {
            level = *maptr;
            if (previous_level > level) {
                chunk_contrast += (double) (previous_level - level);
            } else {
                chunk_contrast += (double) (level - previous_level);
            }
            maptr += omaptrinc;
        } while (chunk_o-- != 0U);
        chunk_contrasts[chunk_n] = chunk_contrast;
        total_contrast += chunk_contrast;
        maptr += nmaptrinc;
    } while (chunk_n-- != 0U);
    barrier_contrast =
        total_contrast * context->puzzle_contrast_barrier_for_cropping;
    total_contrast = 0.0;
    *crop0 = 0U;
    do {
        total_contrast += chunk_contrasts[*crop0];
        if (total_contrast >= barrier_contrast) {
            break;
        }
    } while ((*crop0)++ < chunk_n1);
    total_contrast = 0.0;    
    *crop1 = chunk_n1;
    do {
        total_contrast += chunk_contrasts[*crop1];
        if (total_contrast >= barrier_contrast) {
            break;
        }        
    } while ((*crop1)-- > 0U);
    free(chunk_contrasts);
    if (*crop0 > chunk_n1 || *crop1 > chunk_n1) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    max_crop = (unsigned int)
        round((double) chunk_n1 * context->puzzle_max_cropping_ratio);
    if (max_crop > chunk_n1) {
        puzzle_err_bug(__FILE__, __LINE__);        
    }
    *crop0 = MIN(*crop0, max_crop);
    *crop1 = MAX(*crop1, chunk_n1 - max_crop);

    return 0;
}

static int puzzle_autocrop_view(PuzzleContext * context,
                                PuzzleView * const view)
{
    unsigned int cropx0, cropx1;
    unsigned int cropy0, cropy1;
    unsigned int x, y;
    unsigned char *maptr;

    if (puzzle_autocrop_axis(context, view, &cropx0, &cropx1,
                             view->width, view->height,
                             (int) view->width,
                             1 - (int) (view->width * view->height)) < 0 ||
        puzzle_autocrop_axis(context, view, &cropy0, &cropy1,
                             view->height, view->width,
                             1, 0) < 0) {
        return -1;
    }
    if (cropx0 > cropx1 || cropy0 > cropy1) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    maptr = view->map;
    y = cropy0;
    do {
        x = cropx0;
        do {
            *maptr++ = PUZZLE_VIEW_PIXEL(view, x, y);
        } while (x++ != cropx1);
    } while (y++ != cropy1);
    view->width = cropx1 - cropx0 + 1U;
    view->height = cropy1 - cropy0 + 1U;
    view->sizeof_map = (size_t) view->width * (size_t) view->height;
    if (view->width <= 0U || view->height <= 0U ||
        SIZE_MAX / view->width < view->height) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    return 0;
}

static int puzzle_getview_from_gdimage(PuzzleContext * const context,
                                       PuzzleView * const view,
                                       gdImagePtr gdimage)
{
    unsigned int x, y;
    const unsigned int x0 = 0U, y0 = 0U;
    unsigned int x1, y1;
    unsigned char *maptr;
    int pixel;
    
    view->map = NULL;    
    view->width = (unsigned int) gdImageSX(gdimage);
    view->height = (unsigned int) gdImageSY(gdimage);
    view->sizeof_map = (size_t) (view->width * view->height);
    if (view->width > context->puzzle_max_width ||
        view->height > context->puzzle_max_height) {
        return -1;
    }
    if (view->sizeof_map <= (size_t) 0U ||
        INT_MAX / view->width < view->height ||
        SIZE_MAX / view->width < view->height ||
        (unsigned int) view->sizeof_map != view->sizeof_map) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    x1 = view->width - 1U;
    y1 = view->height - 1U;
    if (view->width <= 0U || view->height <= 0U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if ((view->map = calloc(view->sizeof_map, sizeof *view->map)) == NULL) {
        return -1;
    }
    if (x1 > INT_MAX || y1 > INT_MAX) { /* GD uses "int" for coordinates */
        puzzle_err_bug(__FILE__, __LINE__);
    }
    maptr = view->map;
    x = x1;
    if (gdImageTrueColor(gdimage) != 0) {
        do {
            y = y1;
            do {
                pixel = gdImageGetTrueColorPixel(gdimage, (int) x, (int) y);
                *maptr++ = (unsigned char)
                    ((gdTrueColorGetRed(pixel) * 77 +
                      gdTrueColorGetGreen(pixel) * 151 +
                      gdTrueColorGetBlue(pixel) * 28 + 128) / 256);
            } while (y-- != y0);
        } while (x-- != x0);
    } else {
        do {
            y = y1;
            do {
                pixel = gdImagePalettePixel(gdimage, x, y);
                *maptr++ = (unsigned char)
                    ((gdimage->red[pixel] * 77 +
                      gdimage->green[pixel] * 151 +
                      gdimage->blue[pixel] * 28 + 128) / 256);
            } while (y-- != y0);
        } while (x-- != x0);
    }
    return 0;
}

static double puzzle_softedgedlvl(const PuzzleView * const view,
                                  const unsigned int x, const unsigned int y)
{
    unsigned int lvl = 0U;
    unsigned int ax, ay;
    unsigned int count = 0U;
    const unsigned int xlimit = x + PUZZLE_PIXEL_FUZZ_SIZE;
    const unsigned int ylimit = y + PUZZLE_PIXEL_FUZZ_SIZE;
    if (x >= view->width || y >= view->height || xlimit <= x || ylimit <= y) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if (x > PUZZLE_PIXEL_FUZZ_SIZE) {
        ax = x - PUZZLE_PIXEL_FUZZ_SIZE;
    } else {
        ax = 0U;
    }
    do {
        if (ax >= view->width) {
            break;
        }
        if (y > PUZZLE_PIXEL_FUZZ_SIZE) {            
            ay = y - PUZZLE_PIXEL_FUZZ_SIZE;
        } else {
            ay = 0U;
        }
        do {
            if (ay >= view->height) {
                break;
            }
            count++;
            lvl += (unsigned int) PUZZLE_VIEW_PIXEL(view, ax, ay);
        } while (ay++ < ylimit);
    } while (ax++ < xlimit);
    if (count <= 0U) {
        return 0.0;
    }
    return (double) lvl / (double) count;
}

static double puzzle_get_avglvl(const PuzzleView * const view,
                                const unsigned int x, const unsigned int y,
                                const unsigned int width,
                                const unsigned int height)
{
    double lvl = 0.0;
    const unsigned int xlimit = x + width - 1U;
    const unsigned int ylimit = y + height - 1U;
    unsigned int ax, ay;

    if (width <= 0U || height <= 0U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if (xlimit < x || ylimit < y) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    ax = x;
    do {
        if (ax >= view->width) {
            puzzle_err_bug(__FILE__, __LINE__);
        }
        ay = y;
        do {
            if (ay >= view->height) {
                puzzle_err_bug(__FILE__, __LINE__);
            }
            lvl += puzzle_softedgedlvl(view, ax, ay);
        } while (ay++ < ylimit);
    } while (ax++ < xlimit);
    
    return lvl / (double) (width * height);
}

static int puzzle_fill_avglgls(PuzzleContext * const context,
                               PuzzleAvgLvls * const avglvls,
                               const PuzzleView * const view,
                               const unsigned int lambdas)
{
    double width = (double) view->width;
    double height = (double) view->height;
    double xshift, yshift;
    double x, y;
    unsigned int p;
    unsigned int lx, ly;
    unsigned int xd, yd;
    unsigned int px, py;
    unsigned int lwidth, lheight;
    double avglvl;

    avglvls->lambdas = lambdas;
    avglvls->sizeof_lvls = (size_t) lambdas * lambdas;
    if (UINT_MAX / lambdas < lambdas ||
        (unsigned int) avglvls->sizeof_lvls != avglvls->sizeof_lvls) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if ((avglvls->lvls = calloc(avglvls->sizeof_lvls,
                                sizeof *avglvls->lvls)) == NULL) {
        return -1;
    }
    xshift = (width -
              (width * (double) lambdas / (double) SUCC(lambdas))) / 2.0;
    yshift = (height -
              (height * (double) lambdas / (double) SUCC(lambdas))) / 2.0;
    p = (unsigned int) round(MIN(width, height) /
                             (SUCC(lambdas) * context->puzzle_p_ratio));
    if (p < PUZZLE_MIN_P) {
        p = PUZZLE_MIN_P;
    }
    lx = 0U;
    do {
        ly = 0U;
        do {
            x = xshift + (double) lx * PRED(width) / SUCC(lambdas);
            y = yshift + (double) ly * PRED(height) / SUCC(lambdas);
            lwidth = (unsigned int) round
                (xshift + (double) SUCC(lx) * PRED(width) /
                 (double) SUCC(lambdas) - x);
            lheight = (unsigned int) round
                (yshift + (double) SUCC(ly) * PRED(height) /
                 (double) SUCC(lambdas) - y);
            if (p < lwidth) {                
                xd = (unsigned int) round(x + (lwidth - p) / 2.0);
            } else {
                xd = (unsigned int) round(x);
            }
            if (p < lheight) {
                yd = (unsigned int) round(y + (lheight - p) / 2.0);
            } else {
                yd = (unsigned int) round(y);
            }
            if (view->width - xd < p) {
                px = 1U;
            } else {
                px = p;
            }
            if (view->height - yd < p) {
                py = 1U;
            } else {
                py = p;
            }
            if (px > 0U && py > 0U) {
                avglvl = puzzle_get_avglvl(view, xd, yd, px, py);
            } else {
                avglvl = 0.0;
            }
            PUZZLE_AVGLVL(avglvls, lx, ly) = avglvl;
        } while (++ly < lambdas);
    } while (++lx < lambdas);
    
    return 0;
}

static unsigned int puzzle_add_neighbors(double ** const vecur,
                                         const unsigned int max_neighbors,
                                         const PuzzleAvgLvls * const avglvls,
                                         const unsigned int lx,
                                         const unsigned int ly)
{
    unsigned int ax, ay;
    unsigned int xlimit, ylimit;
    unsigned int neighbors = 0U;
    const double ref = PUZZLE_AVGLVL(avglvls, lx, ly);
    
    if (max_neighbors != 8U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if (lx >= avglvls->lambdas - 1U) {
        xlimit = avglvls->lambdas - 1U;
    } else {
        xlimit = lx + 1U;
    }
    if (ly >= avglvls->lambdas - 1U) {
        ylimit = avglvls->lambdas - 1U;
    } else {
        ylimit = ly + 1U;
    }
    if (lx <= 0U) {
        ax = 0U;
    } else {
        ax = lx - 1U;
    }
    do {
        if (ly <= 0U) {
            ay = 0U;
        } else {
            ay = ly - 1U;
        }
        do {
            if (ax == lx && ay == ly) {
                continue;
            }
            *(*vecur)++ = ref - PUZZLE_AVGLVL(avglvls, ax, ay);
            neighbors++;
            if (neighbors <= 0U) {
                puzzle_err_bug(__FILE__, __LINE__);
            }
        } while (ay++ < ylimit);
    } while (ax++ < xlimit);
    if (neighbors > max_neighbors) {
        puzzle_err_bug(__FILE__, __LINE__);
    }    
    return neighbors;
}

static int puzzle_fill_dvec(PuzzleDvec * const dvec,
                            const PuzzleAvgLvls * const avglvls)
{
    unsigned int lambdas;
    unsigned int lx, ly;
    double *vecur;
    
    lambdas = avglvls->lambdas;
    dvec->sizeof_compressed_vec = (size_t) 0U;
    dvec->sizeof_vec = (size_t) (lambdas * lambdas * PUZZLE_NEIGHBORS);
    if (SIZE_MAX /
        ((size_t) (lambdas * lambdas)) < (size_t) PUZZLE_NEIGHBORS ||
        (unsigned int) dvec->sizeof_vec != dvec->sizeof_vec) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    if ((dvec->vec = calloc(dvec->sizeof_vec, sizeof *dvec->vec)) == NULL) {
        return -1;
    }
    vecur = dvec->vec;
    lx = 0U;
    do {
        ly = 0U;
        do {
            (void) puzzle_add_neighbors(&vecur, PUZZLE_NEIGHBORS,
                                        avglvls, lx, ly);
        } while (++ly < lambdas);
    } while (++lx < lambdas);
    dvec->sizeof_compressed_vec = (size_t) (vecur - dvec->vec);

    return 0;
}

int puzzle_fill_dvec_from_file(PuzzleContext * const context,
                               PuzzleDvec * const dvec,
                               const char * const file)
{
    gdImagePtr gdimage = NULL;
    FILE *fp;
    PuzzleView view;
    PuzzleAvgLvls avglvls;
    PuzzleImageTypeCode image_type_code;
    int ret = 0;

    if (context->magic != PUZZLE_CONTEXT_MAGIC) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    puzzle_init_view(&view);
    puzzle_init_avglvls(&avglvls);
    puzzle_init_dvec(context, dvec);
    if ((fp = fopen(file, "rb")) == NULL) {
        return -1;
    }
    image_type_code = puzzle_get_image_type_from_fp(fp);
    switch (image_type_code) {
    case PUZZLE_IMAGE_TYPE_JPEG:
        gdimage = gdImageCreateFromJpeg(fp);
        break;
    case PUZZLE_IMAGE_TYPE_PNG:
        gdimage = gdImageCreateFromPng(fp);
        break;
    case PUZZLE_IMAGE_TYPE_GIF:
        gdimage = gdImageCreateFromGif(fp);
        break;
    default:
        gdimage = NULL;
    }
    (void) fclose(fp);
    if (gdimage == NULL) {
        return -1;
    }
    ret = puzzle_getview_from_gdimage(context, &view, gdimage);
    gdImageDestroy(gdimage);
    if (ret != 0) {
        goto out;
    }
    if (context->puzzle_enable_autocrop != 0 &&
        (ret = puzzle_autocrop_view(context, &view)) < 0) {
        goto out;
    }
    if ((ret = puzzle_fill_avglgls(context, &avglvls,
                                   &view, context->puzzle_lambdas)) != 0) {
        goto out;
    }
    ret = puzzle_fill_dvec(dvec, &avglvls);
    out:    
    puzzle_free_view(&view);
    puzzle_free_avglvls(&avglvls);
    
    return ret;
}

int puzzle_dump_dvec(PuzzleContext * const context,
                     const PuzzleDvec * const dvec)
{
    size_t s = dvec->sizeof_compressed_vec;
    const double *vecptr = dvec->vec;

    (void) context;    
    if (s <= (size_t) 0U) {
        puzzle_err_bug(__FILE__, __LINE__);
    }
    do {
        printf("%g\n", *vecptr++);
    } while (--s != (size_t) 0U);
    
    return 0;
}
