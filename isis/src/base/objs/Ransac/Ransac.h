#ifndef Ransac_h
#define Ransac_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <math.h>

namespace Isis {
  //functions for solving the normal equations
  int choleski_solve(double *a, double *b, int nsize, int flag);  //solves the normal
  int inverse (double *a, int nsize);          //inverses ata (only to be used after ata has been choleski decomposed by choleski_sovle)
  int isymp(int row, int col);  //used for memory optimized symetric matricies, returns the single dimensional array location equivalent to the 2D location in the symetric matrix

  int indeces_from_set(int *indeces,int set, int set_size,int n);
  int binomial_coeficient(int n, int k);

  inline int isymp(int row, int col)
  {
    /* returns the index position of ellement [i][j] in the memory optimized symetric matrix a
       row -> row in square symetric matrix
       col -> column in square symetric matrix
     */
    int k;
    ++row;
    ++col;
    if(row < col)  k= row + col *(col -1)/2;
    else      k= col + row * (row - 1)/2;
    --k;
    return k;

  }

  inline int binomial_coeficient(int n, int k)
  {
    if( k > n)
      return 0;

    if( n < 1)
      return 0;

    int i,j,bc;
    bc=1;
    j=k;

    for(i=k-1;i>0;i--)
    {
      j *= i;
    }


    for(i=n-k+1;i<=n;i++)
      bc *=i;

    bc/=j;

    return bc;
  }


  /*  given a desired set size, and the number of items in the population this function returns the indeces
      of set number set.  For example:

      set_size = 3  n=5

      Set  Index0  Index1  Index2
      1  0  1  2
      2  0  1  3
      3  0  1  4
      4  0  1  5
      5  0  2  3
      6  0  2  4
      7  0  2  5
      8  0  3  4
      9  0  3  5
      so indeces_from_set(  , 5, 3, 6) would return 0, 2, 3

      set    -> the number of the set starting from 1
      set_size-> how many are in each set
      n    -> the population size
      indeces <-  the set_size indeces for set number set

      return 1 if successfuly, otherwise -1

   */
  inline int indeces_from_set(int *indeces,int set, int set_size,int n)
  {
    //frist set_size must not be greater than n
    if( set_size > n)
      return -1;

    //set must be >= 0
    if( set < 0)
      return -1;

    //set must also be less than or equal to the number of possible sets
    if( set > binomial_coeficient(n, set_size) )
      return -1;

    int i,j,k;

    for( i =0,j=0;i<set_size-1;i++,j=indeces[i-1]+1)
    {
      k = binomial_coeficient( n - j - 1, set_size - i - 1);
      while( set > k)
      {
        j++;
        set -= k;
        k = binomial_coeficient( n - j - 1, set_size - i - 1);
      }
      indeces[i] = j;
    }

    indeces[i] = j+set-1;

    return 1;
  }


  int decompose (double *, int);
  int foresub (double *,double *, int);
  int backsub (double *,double *, int);

#define SOLVED 1
#define NOT_SOLVABLE 0
#define NO_ERRORS -1


  inline int choleski_solve (double *a, double *b, int nsize, int flag)
  {
    /* solves the set of linear equations square_matrix(a)delta = b
       a   <-> positive definite symetric matrix a becomes L of the LLt choleski decomposition or the inverse of a (a symetric matrix still in memory optimized mode)
       b   <-> array of doubles of size nsize, the constant part of the system of linear equations
       nsize -> number of unkowns, also lenght of b, also square(a) is nsize x nsize
       flag  -> type of solutions sought: 1 decomposition only, 2 decomposition and solve, 3 decompose solve and inverse a
     */

    if (flag < 1 || flag > 3)
      return NOT_SOLVABLE;

    /*************** STEP 1:  DECOMPOSE THE A MATRIX ***************/
    if (! decompose (a, nsize)) return NOT_SOLVABLE;

    if (flag == 1) return SOLVED;

    /*************** STEP 2:  FORWARD SUBSTITUTION *****************/
    if (! foresub (a, b, nsize)) return NOT_SOLVABLE;


    /*************** STEP 2:  BACKWARD SUBSTITUTION ****************/
    if (! backsub (a, b, nsize)) return NOT_SOLVABLE;

    if (flag == 2) return SOLVED;

    /*************** STEP 4:  INVERSE THE A MATRIX *****************/
    inverse (a, nsize);
    return SOLVED;
  }

  /*******************************************************************
    FUNCTION DECOMPOSE
   *******************************************************************/
  inline int decompose (double *a, int nsize)
  {
    /* decomposes the memory optimized symetric matirx a into LLt (Choleski decomposition)
       a <-> positive definite symetric matrix a becomes L of the LLt choleski decomposition
       n  -> size of the square matrix a represents or number of unknowns
     */
    double sum;
    double *ap1 = a - 1;
    double *ap2;
    double *ap3;
    double *ap4;
    double *ap5;
    double *ap6 = a;
    double *ap7;
    int i, j, k, m, n = 2;

    ap7 = a;
    for (k = 0; k < nsize; k++)
    {
      ap2 = a;
      ap3 = ap7 + 1;
      ap4 = a -1;
      m = 2;
      for (i = 0; i < k; i++)
      {
        sum = 0.0;
        ap5 = ap1 + 1;
        ap4++;
        for (j = 0; j < i; j++)
        {
          sum += *ap4++ * *ap5++;
        }
        if (*ap2 == 0.0)
          return NOT_SOLVABLE;
        *ap3 = (*ap3 - sum) / *ap2;ap3++;
        ap2 += m++;
      }
      sum = 0.0;
      ap1++;
      for (j = 0; j < k; j++)
      {
        sum += *ap1 * *ap1;ap1++;
      }
      /*if ((*ap6 - sum) <= 0.0)
        return (errcode (06));
       *ap6  = sqrt(*ap6 - sum);*/


      //if ((*ap6 - sum) <= 1e-20)
      if ((*ap6 - sum) <= 0)
        *ap6 = sqrt(-(*ap6 - sum));
      else
        *ap6  = sqrt(*ap6 - sum);


      ap7  = ap6;
      ap6 += n++;
    }
    return SOLVED;
  }

  /*******************************************************************
    FUNCTION FORESUB
   *******************************************************************/
  inline int foresub (double *a,double *b,int nsize)
  {
    int i, j;
    double sum;
    double *ap1 = a;
    double *bp1 = b;
    double *bp2;

    if (*ap1 == 0.0)
      return NOT_SOLVABLE;
    *bp1++ /= *ap1++;
    for (i = 1; i < nsize; i++)
    {
      sum = 0.0;
      for (j = 0, bp2 = b; j < i; j++)
      {
        sum += *ap1++ * *bp2++;
      }
      if (*ap1 == 0.0)
        return NOT_SOLVABLE;
      *bp1 = (*bp1 - sum) / *ap1;bp1++;ap1++;
    }
    return SOLVED;
  }

  /*******************************************************************
    FUNCTION BACKSUB
   *******************************************************************/
  inline int backsub (double *a,double *b,int nsize)
    //double *a,*b;
    //int nsize;
  {
    int i,j;
    int k = nsize - 1;
    int m = nsize;
    int n;
    double sum;
    double *ap1 = a + (k + k * (k + 1) / 2);
    double *ap2 = ap1 - 1;
    double *ap3;
    double *bp1 = b + k;
    double *bp2 = bp1;
    double *bp3;

    if (*ap1 == 0.0)
      return NOT_SOLVABLE;
    *bp1 = *bp1 / *ap1;bp1--;
    for (i = k - 1; i >= 0; i--)
    {
      sum = 0.0;
      n = nsize - 1;
      for (j = nsize - 1, ap3 = ap2--, bp3 = bp2; j >= i + 1; j--)
      {
        sum += *ap3 * *bp3--;
        ap3 -= n--;
      }
      ap1 -= m--;
      if (*ap1 == 0.0)
        return NOT_SOLVABLE;
      *bp1 = (*bp1 - sum) / *ap1;bp1--;
    }
    return SOLVED;
  }

  /*******************************************************************
    FUNCTION INVERSE
   *******************************************************************/
  inline int inverse (double *a, int nsize)
    //double *a;
    //int nsize;
  {
    int i, j, k, m, n;
    double sum;
    double *ap1 = a;
    double *ap2;
    double *ap3;
    double *ap4;
    double *ap5;

    /* FIRST INVERSE ALL DIAGONAL ELEMENTS */
    m = 2;
    while (ap1 < (a + (nsize * (nsize + 1)/2)))
    {
      *ap1  = 1. / *ap1;
      ap1 += m++;
    }

    /* DO FIRST SET OF MANIPULATIONS */
    ap5 = a + 1;
    for (i = 1; i < nsize; i++)
    {
      ap4 = ap5 - 1;
      m = i + 1;
      n = i;
      for (j = m; j <= nsize; j++)
      {
        ap1 = ap5 - 1;
        ap2 = ap3 = ap4 + i;
        sum = 0.0;
        for (k = i; k <= n;)
        {
          sum += *ap1 * *ap2++;
          ap1 += k++;
        }
        ap4 += j;
        *ap3 = -(*ap4) * sum;
        n = j;
      }
      ap5 += m;
    }

    /* DO SECOND SET OF MANIPULATIONS */
    n = 0;
    ap1 = ap2 = ap3 = ap4 = a;
    for (i = 0; i < nsize;)
    {
      ap5 = ap4;
      m = 0;
      for (j = i; j < nsize;)
      {
        ap5 += j;
        ap1 = ap3 = ap5 + i + n;
        ap2 = ap5 + m++;
        sum = 0.0;
        for (k = j; k < nsize;)
        {
          sum += *ap1 * *ap2;
          ap1 += ++k;
          ap2 += k;
        }
        *ap3 = sum;
        j++;
      }
      ap4 += ++i;
      n--;
    }
    return SOLVED;
  }
}

#endif
