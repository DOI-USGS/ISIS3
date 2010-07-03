#include "ForstnerOperator.h"
#include "Chip.h"
#include "FourierTransform.h"
#include "tnt/tnt_array2d.h"
#include "jama/jama_lu.h"
#include <complex>

namespace Isis {
  /**
   * This method returns the amount of interest for the given chip.
   * 
   * @param chip 
   * 
   * @return the amount of interest for this chip
   */
  double ForstnerOperator::Interest (Chip &chip) {
    Isis::FourierTransform ft;

    int nSamp = ft.NextPowerOfTwo(chip.Samples()-1);
    int nLine = ft.NextPowerOfTwo(chip.Lines()-1);

    TNT::Array2D< std::complex<double> > Guu(nLine, nSamp);
    TNT::Array2D< std::complex<double> > Guv(nLine, nSamp);
    TNT::Array2D< std::complex<double> > Gvv(nLine, nSamp);

    // calculate the diagonal gradients
    //  (if any of the four pixels are special
    //   the gradients are zeroed out)
    //  and perform the fourier transform in the
    // line direction on the 3 matrices
    for (int i=0; i<Guu.dim1(); i++) {
      std::vector< std::complex<double> > line1(Guu.dim2());
      std::vector< std::complex<double> > line2(Guv.dim2());
      std::vector< std::complex<double> > line3(Gvv.dim2());

      double gu, gv;

      for (int j=0; j<Guu.dim2(); j++) {
        if (i>chip.Lines()-2 || j>chip.Samples()-2 ||
            IsSpecial(chip.GetValue(j+1,i+1)) || IsSpecial(chip.GetValue(j+1,i+2)) ||
            IsSpecial(chip.GetValue(j+2,i+1)) || IsSpecial(chip.GetValue(j+2,i+2)) ) {
          gu = 0.0;
          gv = 0.0;
        }
        else {
          gu = chip.GetValue(j+1,i+1)-chip.GetValue(j+2,i+2);
          gv = chip.GetValue(j+2,i+1)-chip.GetValue(j+1,i+2);
        }

        line1[j] = gu*gu;
        line2[j]  = gu*gv;
        line3[j] = gv*gv;
      }

      std::vector< std::complex<double> > transform1 = ft.Transform(line1);
      std::vector< std::complex<double> > transform2 = ft.Transform(line2);
      std::vector< std::complex<double> > transform3 = ft.Transform(line3);

      //copy the transformed data back into the matrices
      for (int j=0; j<Guu.dim2(); j++) {
        Guu[i][j] = transform1[j];
        Guv[i][j] = transform2[j];
        Gvv[i][j] = transform3[j];
      }
    }

    // perform the fourier transform in the
    // sample direction on the 3 matrices
    for (int j=0; j<Guu.dim2(); j++) {
      std::vector< std::complex<double> > samp1(Guu.dim1());
      std::vector< std::complex<double> > samp2(Guv.dim1());
      std::vector< std::complex<double> > samp3(Gvv.dim1());

      for (int i=0; i<Guu.dim1(); i++) {
        samp1[i] = Guu[i][j];
        samp2[i] = Guv[i][j];
        samp3[i] = Gvv[i][j];
      }

      std::vector< std::complex<double> > transform1 = ft.Transform(samp1);
      std::vector< std::complex<double> > transform2 = ft.Transform(samp2);
      std::vector< std::complex<double> > transform3 = ft.Transform(samp3);

      //copy the transformed data back into the matrices
      for (int i=0; i<Guu.dim1(); i++) {
        Guu[i][j] = transform1[i];
        Guv[i][j] = transform2[i];
        Gvv[i][j] = transform3[i];
      }
    }

    // First, multiply the three transformed matrices
    // Then, compute the 2d inverse of the
    //  transformed data starting with the line direction
    //  For convenience, put it back in Guu
    for (int i=0; i<Guu.dim1(); i++) {
      std::vector< std::complex<double> > line(Guu.dim2());

      for (int j=0; j<Guu.dim2(); j++) {
        line[j] = Guu[i][j]*Guv[i][j]*Gvv[i][j];
      }

      std::vector< std::complex<double> > transform = ft.Transform(line);

      //copy the transformed data back into the matrix
      for (int j=0; j<Guu.dim2(); j++) {
        Guu[i][j] = transform[j];
      }
    }

    // After inverting, the matrix will contain N in the upper right
    // The trace of N determines the roundness of the chip
    //     and the determinant determines the chip's weight
    // In this case, we will look only at the weight
    TNT::Array2D<double> N(chip.Lines()-1, chip.Samples()-1);

    // And then the sample direction
    for (int j=0; j<N.dim2(); j++) {
      std::vector< std::complex<double> > samp(Guu.dim1());

      for (int i=0; i<Guu.dim1(); i++) {
        samp[i] = Guu[i][j];
      }

      std::vector< std::complex<double> > transform = ft.Transform(samp);

      //copy the transformed data back into the matrix
      for (int i=0; i<N.dim1(); i++) {
        N[i][j] = real(transform[i]);
      }
    }

    JAMA::LU<double> lu(N);

    // use LU decomposition to calculate the determinant
    return abs(lu.det());
  }
}

extern "C" Isis::InterestOperator *ForstnerOperatorPlugin (Isis::Pvl &pvl) {
  return new Isis::ForstnerOperator(pvl);
}

