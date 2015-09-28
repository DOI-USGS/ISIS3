#include "Isis.h"

#include "GaussianStretch.h"
#include "PrincipalComponentAnalysis.h"
#include "ProcessBySpectra.h"
#include "Statistics.h"

#include <iomanip>

//Third-party matrix library
#include "tnt_array2d.h"

using namespace std;
using namespace Isis;

void getData(Buffer &in);
void transform(Buffer &in, Buffer &out);
void normalizeAndInvert(Buffer &in, Buffer &out);

PrincipalComponentAnalysis pca(0);
vector<GaussianStretch *> stretches;

QString tmpFileName = "Temporary_DecorrelationStretch_Transform.cub";

void IsisMain() {
  ProcessByBrick p;
  Cube *icube = p.SetInputCube("FROM");
  int numDimensions = icube->bandCount();
  p.SetBrickSize(128, 128, numDimensions);

  // The output cube with no attributes and real pixel type
  Isis::CubeAttributeOutput cao;
  cao.setPixelType(Isis::Real);

  p.SetOutputCube(tmpFileName, cao, icube->sampleCount(), icube->lineCount(), icube->bandCount());

  // Get the data for the transform matrix
  pca = Isis::PrincipalComponentAnalysis(numDimensions);
  ProcessByBrick p2;
  p2.SetBrickSize(128, 128, numDimensions);
  p2.SetInputCube("FROM");
  p2.Progress()->SetText("Computing transform");
  p2.StartProcess(getData);
  p2.EndProcess();
  pca.ComputeTransform();

  p.Progress()->SetText("Transforming Cube");
  p.StartProcess(transform);
  p.EndProcess();

  Isis::CubeAttributeInput cai;

  Cube *icube2 = p.SetInputCube(tmpFileName, cai);
  for (int i = 0; i < numDimensions; i++) {
    stretches.push_back(new GaussianStretch(*(icube2->histogram(i + 1) ) ) );
  }

  p.SetOutputCube("TO");
  p.SetBrickSize(128, 128, numDimensions);
  p.Progress()->SetText("Stretching Cube");
  p.StartProcess(normalizeAndInvert);

  for (int i = 0; i < numDimensions; i++) {
     delete stretches[i];
  }

  stretches.clear();
  p.EndProcess();
  remove(tmpFileName.toAscii().data() );
}


void getData(Buffer &in) {
  pca.AddData(in.DoubleBuffer(), in.size() / in.BandDimension() );
}

// Processing routine for the pca with one input cube
void transform(Buffer &in, Buffer &out) {

  for (int i = 0; i < in.SampleDimension(); i++) {

    for (int j = 0; j < in.LineDimension(); j++) {

      TNT::Array2D<double> pre(1, in.BandDimension());
      for (int k = 0; k < pre.dim2(); k++) {
        int index = i + j * in.SampleDimension() + k * in.SampleDimension() * in.LineDimension();
        pre[0][k] = in[index];
      }

      TNT::Array2D<double> post = pca.Transform(pre);

      for (int k = 0; k < post.dim2(); k++) {
        int index = i + j * in.SampleDimension() + k * in.SampleDimension() * in.LineDimension();
        out[index] = post[0][k];
      }
    }
  }
}

// Processing routine for the pca with two input cubes
void normalizeAndInvert(Buffer &in, Buffer &out) {

  for (int i = 0; i < in.SampleDimension(); i++) {
    for (int j = 0; j < in.LineDimension(); j++) {
      TNT::Array2D<double> pre(1, in.BandDimension() );
      for (int k = 0; k < pre.dim2(); k++) {
        int index = i + j * in.SampleDimension() + k * in.SampleDimension() * in.LineDimension();
        // Stretch the data before inverting it
        // NOTE: this needs to be modified to use a GAUSSIAN STRETCH
        //pre[0][k] = (in[index]-bandStats[k]->Average())/bandStats[k]->StandardDeviation();
        pre[0][k] = stretches[k]->Map(in[index]);
      }

      TNT::Array2D<double> post = pca.Inverse(pre);

      for (int k = 0; k < post.dim2(); k++) {
        int index = i + j * in.SampleDimension() + k * in.SampleDimension() * in.LineDimension();
        out[index] = post[0][k];
      }
    }
  }
}

