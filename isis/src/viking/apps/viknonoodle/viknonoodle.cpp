#include "Isis.h"
#include <complex>
#include "FourierTransform.h"
#include "ProcessByLine.h"
#include "Statistics.h"

using namespace std; 
using namespace Isis;

void removeNoise (Buffer &in, Buffer &out);
void clean(std::vector< std::complex<double> > &transformed);
int findExtreme(std::vector< std::complex<double> > data);

FourierTransform fft;

double tolerance = 0.5;

void IsisMain()
{
  ProcessByLine p;

  // Setup the input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  tolerance = Isis::Application::GetUserInterface().GetDouble("TOLERANCE");

  // Start the sample processing
  p.StartProcess( removeNoise );
  p.EndProcess();
}

void removeNoise (Buffer &in, Buffer &out) {
  // if we are not near the location of the noise,
  //  copy the input data over to the output cube
  if((in.Line() > 100 && in.Line() < 900) || in.Line() >1100){
    for (int i=0; i<out.size(); i++) {
      out[i] = in[i];
    }
    return;
  }

  // we need to find the data's average so we
  //  can normalize the fourier transform
  int n = in.size();

  Statistics stats;
  stats.AddData(in.DoubleBuffer(), n);
  // we do not use the built in Average method
  // as we count special pixels as zeros
  // i.e. they are not ignored
  double average = stats.Sum()/n;

  // find the first valid pixel
  int index1 = 0;
  while (IsSpecial(in[index1])) index1++;

  // find the last valid pixel
  int index2 = in.size()-1;
  while (IsSpecial(in[index2])) index2--;

  // create a line that will cover at least half the data
  //  this guarentees a more accurate result
  int length = fft.NextPowerOfTwo((index2-index1)/2); // the length of the interval
  std::vector< std::complex<double> > line(length);

  // the last pixel of the second interval
  index2 = index2-line.size() + 1;

  // copy the input cube's data into the complex vector
  //  for the first interval
  for (int i=0; i<length; i++) {
    // special pixels are counted as zeros.
    if (IsSpecial(in[i+index1])) {
      line[i]= 0.0;
    }
    else line[i]=std::complex<double>(in[i+index1]-average);
  }

  // run a fourier transform on it
  std::vector< std::complex<double> > transform1 = fft.Transform(line);

  // copy the input cube's data into the complex vector
  for (int i=0; i<length; i++) {
    if (IsSpecial(in[i])) {
      line[i]= 0.0;
    }
    else line[i]=std::complex<double>(in[i+index2]-average);
  }

  // run a fourier transform on it
  std::vector< std::complex<double> > transform2 = fft.Transform(line);

  // and clean up the noise
  clean(transform1);
  clean(transform2);

  // now invert the transformed data
  std::vector< std::complex<double> > inverse1 = fft.Inverse(transform1);
  std::vector< std::complex<double> > inverse2 = fft.Inverse(transform2);

  // copy the data
  for (int i=0; i<out.size(); i++) {
    if ( IsSpecial(in[i])) out[i] = in[i];
    // if it is only in the first interval,
    //  use the first inverse array
    else if(i < index2) {
	  out[i] = inverse1[i-index1].real() + average;
	}
    // if it is only in the second interval,
    //  use the second inverse array
    else if(i > index1 + length - 1) {
	  out[i] = inverse2[i-index2].real() + average;
	}
    // otherwise, use the average of both inverse arrays
    else {
	  out[i] = ((index1+length-i)*real(inverse1[i-index1]) + (i-index2)*real(inverse2[i-index2]))/(index1+length-index2) + average;
	}
  }
}

// clean the transformed data
void clean(std::vector< std::complex<double> > &transformed) {
  int n = transformed.size();

  // find highest frequency
  int index = findExtreme(transformed);

  // if noise is found, use a butterworth bandstop
  // filter to remove it
  if (abs( transformed[index] ) > tolerance) {
    double d = index; // The cutoff
    double dw = 10.0; // The bandwidth
    int g = 1; // The order
    double B;

    for (int i=0; i<n/2; i++) {
      if(i == d) B = 0.0;
      else B = 1/(1+pow(dw*i/(i*i-d*d), 2*g));

      transformed[i] = std::complex<double> (polar(B*abs(transformed[i]), arg(transformed[i])));
      transformed[n-i-1] = std::complex<double> (polar(B*abs(transformed[n-i-1]), arg(transformed[n-i-1])));
    }
  }
}

// Finds the largest or smallest value in the array
//  not including the first value
int findExtreme(std::vector< std::complex<double> > data) {
  int index = 0;
  double extreme = 0.0;
  // the frequency we are looking for is around 10
  for (unsigned int i=8; i<13; i++) {
    double value = abs(data[i]);
    if (value > extreme) {
      extreme = value;
      index = i;
    }
  }
  return index;
}
