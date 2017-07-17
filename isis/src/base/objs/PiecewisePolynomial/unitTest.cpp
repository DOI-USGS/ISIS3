#include <cmath>

#include "IException.h"
#include "LinearAlgebra.h"
#include "Preference.h"
#include "PiecewisePolynomial.h"

using namespace Isis;
using namespace std;

double testFunction(double value);
std::vector<double> test3DFunction(double value);
void outputPolynomial(PiecewisePolynomial &poly);
void outputResiduals(std::vector<double> inputs,
                     PiecewisePolynomial &poly);
void output3DResiduals(std::vector<double> inputs,
                       PiecewisePolynomial &poly);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout << "Unit test for PiecewisePolynomial" << endl << endl;

  cout << "Test default constructor" << endl << endl;
  PiecewisePolynomial defaultPoly;
  outputPolynomial(defaultPoly);

  cout << "Create 1D PiecewisePolynomial:" << endl << endl;
  PiecewisePolynomial testPoly(-5, 5, 2, 1);
  outputPolynomial(testPoly);

  cout << endl << "Fit to 1D data:" << endl << endl;
  std::vector<double> times;
  std::vector< std::vector<double> > inputData;
  for (int i = -8; i <= 8; i++) {
    double value = (double)i / 2;
    times.push_back(value);
    inputData.push_back( std::vector<double>( 1, testFunction(value) ) );
  }
  testPoly.fitPolynomials(times, inputData, 2);
  outputPolynomial(testPoly);

  cout << endl;
  outputResiduals(times, testPoly);

  cout << endl << "Create 3D PiecewisePolynomial:" << endl << endl;
  PiecewisePolynomial test3dPoly(-6, 4, 2, 3);
  outputPolynomial(test3dPoly);

  cout << endl << "Fit to 3D data:" << endl << endl;
  std::vector<double> times3D;
  std::vector< std::vector<double> > input3DData;
  for (int i = -10; i <= 6; i++) {
    double value = (double)i / 2;
    times3D.push_back(value);
    input3DData.push_back( test3DFunction(value) );
  }
  test3dPoly.fitPolynomials(times3D, input3DData, 3);
  outputPolynomial(test3dPoly);

  cout << endl;
  output3DResiduals(times3D, test3dPoly);

  cout << endl << "Test fitting to a single point" << endl;

  times3D.resize(1);
  input3DData.clear();
  input3DData.push_back( test3DFunction( times3D.front() ) );
  PiecewisePolynomial pointPoly(-6, 4, 0, 3);
  pointPoly.fitPolynomials(times3D, input3DData, 1);
  outputPolynomial(pointPoly);

  cout << endl;
  output3DResiduals(times3D, pointPoly);

  cout << endl << "Test copy constructor" << endl;
  PiecewisePolynomial copyPoly(pointPoly);
  outputPolynomial(copyPoly);

  cout << endl << "Test assignment operator" << endl;
  copyPoly = test3dPoly;
  outputPolynomial(copyPoly);

  cout << endl << "Test derivatives" << endl;
  std::vector<double> test3dDerivatives = test3dPoly.derivativeVariable(0.0);
  cout << "Derivatives at 0.0:" << endl;
  cout << "  " << toString(test3dDerivatives[0]) << endl;
  cout << "  " << toString(test3dDerivatives[1]) << endl;
  cout << "  " << toString(test3dDerivatives[2]) << endl;

  cout << endl << "Test segment index accessor" << endl;
  cout << "Segment index for time -10.0: " << toString(test3dPoly.segmentIndex(-10.0)) << endl;
  cout << "Segment index for time -1.0: " << toString(test3dPoly.segmentIndex(-1.0)) << endl;
  cout << "Segment index for time 3.0: " << toString(test3dPoly.segmentIndex(3.0)) << endl;

  cout << endl << "Test refitting polynomials" << endl;
  cout << "Refit 3 segment, 3d polynomial to 5 segments." << endl;
  copyPoly.refitPolynomials(5);
  outputPolynomial(copyPoly);

  cout << endl << "Refit 3d zero polynomial to 3 segments." << endl;
  defaultPoly.refitPolynomials(3);
  outputPolynomial(defaultPoly);

  cout << endl << "Test changing the polynomials degree" << endl;
  defaultPoly.setDegree(4);
  outputPolynomial(defaultPoly);

  cout << endl << "Test error throws" << endl << endl;

  cout << endl << "Polynomial fit errors:" << endl;

  try {
    
    std::vector<double> badTimes(3,0.0);
    std::vector< std::vector<double> > badPoints;
    badPoints.push_back( std::vector<double>(1,0.0) );
    PiecewisePolynomial badPoly(-5, 5, 0, 1);
    badPoly.fitPolynomials(badTimes, badPoints, 1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    std::vector<double> badTimes(2,0.0);
    std::vector< std::vector<double> > badPoints;
    for (int i = 0; i < (int)badTimes.size(); i++) {
      badPoints.push_back( std::vector<double>(1,0.0) );
    }
    PiecewisePolynomial badPoly(-5, 5, 2, 1);
    badPoly.fitPolynomials(badTimes, badPoints, 3);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    std::vector<double> badTimes(7,0.0);
    badTimes[1] = 1;
    std::vector< std::vector<double> > badPoints;
    for (int i = 0; i < (int)badTimes.size(); i++) {
      badPoints.push_back( std::vector<double>(1,0.0) );
    }
    PiecewisePolynomial badPoly(-5, 5, 2, 1);
    badPoly.fitPolynomials(badTimes, badPoints, 3);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    std::vector<double> badTimes(7,0.0);
    std::vector< std::vector<double> > badPoints;
    for (int i = 0; i < (int)badTimes.size(); i++) {
      badPoints.push_back( std::vector<double>(1,i) );
      badTimes[i] = i;
    }
    badPoints.back().resize(2,0);
    PiecewisePolynomial badPoly(-5, 5, 2, 1);
    badPoly.fitPolynomials(badTimes, badPoints, 3);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    std::vector<double> badTimes(3,0.0);
    std::vector< std::vector<double> > badPoints;
    for (int i = 0; i < (int)badTimes.size(); i++) {
      badPoints.push_back( std::vector<double>(2,0) );
      badTimes[i] = i;
    }
    PiecewisePolynomial badPoly(-5, 5, 0, 2);
    badPoly.fitPolynomials(badTimes, badPoints, 1);
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Polynomial refitting errors:" << endl;

  try {
    copyPoly.refitPolynomials(-3);
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Attempt to set to negative degree:" << endl;
  try {
    testPoly.setDegree(-1);
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Attempt to set non-positive dimensions:" << endl;
  try {
    testPoly.setDimensions(0);
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Setting coefficients errors:" << endl;
  try {
    std::vector< std::vector<double> > badCoefficients;
    badCoefficients.push_back( std::vector<double>(3, 0.0) );
    testPoly.setCoefficients(4, badCoefficients);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    std::vector< std::vector<double> > badCoefficients;
    badCoefficients.push_back( std::vector<double>(3, 0.0) );
    test3dPoly.setCoefficients(0, badCoefficients);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    std::vector< std::vector<double> > badCoefficients;
    badCoefficients.push_back( std::vector<double>(3, 0.0) );
    badCoefficients.push_back( std::vector<double>(3, 0.0) );
    badCoefficients.push_back( std::vector<double>(2, 0.0) );
    test3dPoly.setCoefficients(0, badCoefficients);
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "Attempt to set less than 2 knots:" << endl;
  try {
    std::vector<double> badKnots(1,0.0);
    testPoly.setKnots(badKnots);
  }
  catch(IException &e) {
    e.print();
  }
}

double testFunction(double value) {
  if (value < 0) {
    return ((value + 1) * (value + 1) - 1);
  }
  else {
    return (-(value - 2) * (value - 2) + 4);
  }
}

std::vector<double> test3DFunction(double value) {
  std::vector<double> outputValue;
  if (value < 0) {
    outputValue.push_back( -0.5*value*value-2*value );
    outputValue.push_back( value/3 );
    outputValue.push_back( 3.0 );
  }
  else {
    outputValue.push_back( value*value-2*value );
    outputValue.push_back( value*value+value/3 );
    outputValue.push_back( -value*value+3.0 );
  }
  return outputValue;
}

void outputPolynomial(PiecewisePolynomial &poly) {
  cout << "Number of segments: " << toString( poly.segments() ) << endl;
  cout << "Space curve dimensions: " << toString( poly.dimensions() ) << endl;
  cout << "Polynomial degree: " << toString( poly.degree() ) << endl;

  std::vector<double> knots = poly.knots();
  cout << "Polynomial knots:" << endl;
  for (int i = 0; i < (int)knots.size(); i++) {
    cout << "  " << toString(knots[i], 6) << endl;
  }

  for (int i = 0 ; i < poly.segments(); i++) {
    std::vector< std::vector<double> > segmentCoeffs = poly.coefficients(i);
    cout << endl << "Segment " << toString(i + 1) << " polynomial:" << endl;;
    for (int j = 0; j < (int)segmentCoeffs.size(); j++) {
      std::vector<double> dimensionsCoeffs = segmentCoeffs[j];
      cout << "Dimension " << toString(j + 1) << " coefficients:" << endl;
      for (int k = 0; k < (int)dimensionsCoeffs.size(); k++) {
        cout << "  " << toString( dimensionsCoeffs[k], 6 ) << endl;
      }
    }
  }
}

void outputResiduals(std::vector<double> inputs,
                     PiecewisePolynomial &poly) {
  int numTests = inputs.size();
  double sumSquareError = 0;
  cout << "Calculating residuals for " + toString(numTests) + " points." << endl;
  for (int i = 0 ; i < numTests; i++) {
    double residual = fabs(poly.evaluate(inputs[i])[0] - testFunction(inputs[i]));
    sumSquareError += residual;
    cout << "  " << toString(inputs[i], 6) <<
            "  " << toString(residual, 6) << endl;
  }
  double rms = std::sqrt(sumSquareError/numTests);
  cout << "RMS Error: " << toString(rms, 6) << endl;
}

void output3DResiduals(std::vector<double> inputs,
                       PiecewisePolynomial &poly) {
  int numTests = inputs.size();
  double sumSquareError = 0;
  cout << "Calculating residuals for " + toString(numTests) + " points." << endl;
  for (int i = 0 ; i < numTests; i++) {
    std::vector<double> computedValues = poly.evaluate( inputs[i] );
    std::vector<double> knownValues = test3DFunction( inputs[i] );
    LinearAlgebra::Vector known(3);
    known[0] = knownValues[0];
    known[1] = knownValues[1];
    known[2] = knownValues[2];
    LinearAlgebra::Vector computed(3);
    computed[0] = computedValues[0];
    computed[1] = computedValues[1];
    computed[2] = computedValues[2];
    double residual = LinearAlgebra::magnitude( LinearAlgebra::subtract(known, computed) );
    sumSquareError += residual*residual;
    cout << "  " << toString(inputs[i], 6) <<
            "  " << toString(residual, 6) << endl;
  }
  double rms = std::sqrt(sumSquareError/numTests);
  cout << "RMS Error: " << toString(rms, 6) << endl;
}
