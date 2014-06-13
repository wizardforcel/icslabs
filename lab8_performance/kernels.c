/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "5110369009",              /* Student ID */

    "Zhang Zichen",     	    /* Your Name */
    "wizard.z@qq.com",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/
/*
 *Change reading's cache optimization to writing's.
 *Do rotate option by 32 * 32 blocks.
 *CPE 2.4 ~ 2.5
 */

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	    for (j = 0; j < dim; j++)
	      dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;
    int tmpd = (dim - 1) * dim;
    int tmps = 0;
    
    for(i = 0; i < dim; i += 32)
    {
      for(j = 0; j < dim; j++)
      {
          //No.1
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.2
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.3
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.4
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.5
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.6
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.7
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.8
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.9
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.10
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.11
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.12
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.13
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.14
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.15
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.16
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.17
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.18
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.19
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.20
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.21
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.22
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.23
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.24
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.25
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.26
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.27
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.28
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.29
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.30
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.31
          dst[tmpd] = src[tmps];
          tmpd++;
          tmps += dim; 

          //No.32
          dst[tmpd] = src[tmps];
          tmps -= 31 * dim;          
          tmps++;
          tmpd -= 31;
          tmpd -= dim;

          /*dst[i * dim + j] = src[(j + 1) * dim - 1 - i];
          dst[i * dim + j + 1] = src[(j + 2) * dim - 1 - i];
          dst[i * dim + j + 2] = src[(j + 3) * dim - 1 - i];
          dst[i * dim + j + 3] = src[(j + 4) * dim - 1 - i];   
          dst[i * dim + j + 4] = src[(j + 5) * dim - 1 - i];
          dst[i * dim + j + 5] = src[(j + 6) * dim - 1 - i];
          dst[i * dim + j + 6] = src[(j + 7) * dim - 1 - i];
          dst[i * dim + j + 7] = src[(j + 8) * dim - 1 - i];
          dst[i * dim + j + 8] = src[(j + 9) * dim - 1 - i];
          dst[i * dim + j + 9] = src[(j + 10) * dim - 1 - i];
          dst[i * dim + j + 10] = src[(j + 11) * dim - 1 - i];
          dst[i * dim + j + 11] = src[(j + 12) * dim - 1 - i];
          dst[i * dim + j + 12] = src[(j + 13) * dim - 1 - i];
          dst[i * dim + j + 13] = src[(j + 14) * dim - 1 - i];
          dst[i * dim + j + 14] = src[(j + 15) * dim - 1 - i];
          dst[i * dim + j + 15] = src[(j + 16) * dim - 1 - i];
          dst[i * dim + j + 16] = src[(j + 17) * dim - 1 - i];
          dst[i * dim + j + 17] = src[(j + 18) * dim - 1 - i];
          dst[i * dim + j + 18] = src[(j + 19) * dim - 1 - i];
          dst[i * dim + j + 19] = src[(j + 20) * dim - 1 - i];
          dst[i * dim + j + 20] = src[(j + 21) * dim - 1 - i];
          dst[i * dim + j + 21] = src[(j + 22) * dim - 1 - i];
          dst[i * dim + j + 22] = src[(j + 23) * dim - 1 - i];
          dst[i * dim + j + 23] = src[(j + 24) * dim - 1 - i];
          dst[i * dim + j + 24] = src[(j + 25) * dim - 1 - i];
          dst[i * dim + j + 25] = src[(j + 26) * dim - 1 - i];
          dst[i * dim + j + 26] = src[(j + 27) * dim - 1 - i];
          dst[i * dim + j + 27] = src[(j + 28) * dim - 1 - i];
          dst[i * dim + j + 28] = src[(j + 29) * dim - 1 - i];
          dst[i * dim + j + 29] = src[(j + 30) * dim - 1 - i];
          dst[i * dim + j + 30] = src[(j + 31) * dim - 1 - i];
          dst[i * dim + j + 31] = src[(j + 32) * dim - 1 - i];*/
      }
    tmpd += dim * dim;
    tmpd += 32;
    tmps += 31 * dim;
    }
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/
/*
 *Spread the double loop of function avg().
 *And consider bound cases.
 *CPE 3.3 ~ 3.4
 */
/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
      for (j = 0; j < dim; j++)
        dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j, tmp, tmpi;

    //UpLeft
    dst[0].red = (src[0].red + src[1].red + src[dim].red + src[dim + 1].red) >> 2;
    dst[0].blue = (src[0].blue + src[1].blue + src[dim].blue + src[dim + 1].blue) >> 2;
    dst[0].green = (src[0].green + src[1].green + src[dim].green + src[dim + 1].green) >> 2;
    //UpRight
    dst[dim - 1].red = (src[dim - 1].red + src[dim - 2].red + 
                        src[dim * 2 - 1].red + src[dim * 2 - 2].red) >> 2;
    dst[dim - 1].blue = (src[dim - 1].blue + src[dim - 2].blue + 
                         src[dim * 2 - 1].blue + src[dim * 2 - 2].blue) >> 2;
    dst[dim - 1].green = (src[dim - 1].green + src[dim - 2].green + 
                          src[dim * 2 - 1].green + src[dim * 2 - 2].green) >> 2;
    //DownLeft
    dst[dim * (dim - 1)].red = (src[dim * (dim - 1)].red + src[dim * (dim - 1) + 1].red + 
                                src[dim * (dim - 2)].red + src[dim * (dim - 2) + 1].red) >> 2;
    dst[dim * (dim - 1)].blue = (src[dim * (dim - 1)].blue + src[dim * (dim - 1) + 1].blue + 
                                 src[dim * (dim - 2)].blue + src[dim * (dim - 2) + 1].blue) >> 2;
    dst[dim * (dim - 1)].green = (src[dim * (dim - 1)].green + src[dim * (dim - 1) + 1].green + 
                                  src[dim * (dim - 2)].green + src[dim * (dim - 2) + 1].green) >> 2;
    //DownRight
    dst[dim * dim - 1].red = (src[dim * dim - 1].red + src[dim * dim - 2].red + 
                              src[dim * (dim - 1) - 1].red + src[dim * (dim - 1) - 2].red) >> 2;
    dst[dim * dim - 1].blue = (src[dim * dim - 1].blue + src[dim * dim - 2].blue + 
                               src[dim * (dim - 1) - 1].blue + src[dim * (dim - 1) - 2].blue) >> 2;
    dst[dim * dim - 1].green = (src[dim * dim - 1].green + src[dim * dim - 2].green + 
                                src[dim * (dim - 1) - 1].green + src[dim * (dim - 1) - 2].green) >> 2;
    //UpEdge
    for (j = 1; j < dim - 1; j++) 
    {
        dst[j].red = (src[j].red + src[j - 1].red + src[j + 1].red + 
                      src[j + dim].red + src[j + 1 + dim].red + src[j - 1 + dim].red) / 6;
        dst[j].blue = (src[j].blue + src[j - 1].blue + src[j + 1].blue + 
                       src[j + dim].blue + src[j + 1 + dim].blue + src[j - 1 + dim].blue) / 6;
        dst[j].green = (src[j].green + src[j - 1].green + src[j + 1].green + 
                        src[j + dim].green + src[j + 1 + dim].green + src[j - 1 + dim].green) / 6;
    }
    //DownEdge
    for (j = dim * (dim - 1) + 1; j < dim * dim - 1; j++) 
    {
        dst[j].red = (src[j].red + src[j - 1].red + src[j + 1].red + 
                      src[j - dim].red + src[j + 1 - dim].red + src[j - 1 - dim].red) / 6;
        dst[j].blue = (src[j].blue + src[j - 1].blue + src[j + 1].blue + 
                       src[j - dim].blue + src[j + 1 - dim].blue + src[j - 1 - dim].blue) / 6;
        dst[j].green = (src[j].green + src[j - 1].green + src[j + 1].green + 
                        src[j - dim].green + src[j + 1 - dim].green + src[j - 1 - dim].green) / 6;
    }
    //LeftEdge
    for (j = dim; j < dim * (dim - 1); j += dim) 
    {
        dst[j].red = (src[j].red + src[j - dim].red + src[j + 1].red + 
                      src[j + dim].red + src[j + 1 + dim].red + src[j - dim + 1].red) / 6;
        dst[j].blue = (src[j].blue + src[j - dim].blue + src[j + 1].blue + 
                       src[j + dim].blue + src[j + 1 + dim].blue + src[j - dim + 1].blue) / 6;
        dst[j].green = (src[j].green + src[j - dim].green + src[j + 1].green + 
                        src[j + dim].green + src[j + 1 + dim].green + src[j - dim + 1].green) / 6;
    }
    //RightEdge
    for (j = dim + dim - 1; j < dim * dim - 1; j += dim) 
    {
        dst[j].red = (src[j].red + src[j - 1].red + src[j - dim].red + 
                      src[j + dim].red + src[j - dim - 1].red + src[j - 1 + dim].red) / 6;
        dst[j].blue = (src[j].blue + src[j - 1].blue + src[j - dim].blue + 
                       src[j + dim].blue + src[j - dim - 1].blue + src[j - 1 + dim].blue) / 6;
        dst[j].green = (src[j].green + src[j - 1].green + src[j - dim].green + 
                        src[j + dim].green + src[j - dim - 1].green + src[j - 1 + dim].green) / 6;
    }
    //Center
    tmpi = dim;
    for (i = 1; i < dim - 1; i++)
    {
        for (j = 1; j < dim - 1; j++)
        {
            tmp = tmpi + j;
            dst[tmp].red = (src[tmp].red + src[tmp - 1].red + src[tmp + 1].red + src[tmp - dim].red + src[tmp - dim - 1].red + 
                            src[tmp - dim + 1].red + src[tmp + dim].red + src[tmp + dim + 1].red + src[tmp + dim - 1].red) / 9;
            dst[tmp].green = (src[tmp].green + src[tmp - 1].green + src[tmp + 1].green + src[tmp - dim].green + src[tmp - dim - 1].green + 
                              src[tmp - dim + 1].green + src[tmp + dim].green + src[tmp + dim + 1].green + src[tmp + dim - 1].green) / 9;
            dst[tmp].blue = (src[tmp].blue + src[tmp - 1].blue + src[tmp + 1].blue + src[tmp - dim].blue + src[tmp - dim - 1].blue + 
                             src[tmp - dim + 1].blue + src[tmp + dim].blue + src[tmp + dim + 1].blue + src[tmp + dim - 1].blue) / 9;
        }
        tmpi += dim;
    }
}


/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&naive_smooth, naive_smooth_descr);    
    add_smooth_function(&smooth, smooth_descr);
    /* ... Register additional test functions here */
}
