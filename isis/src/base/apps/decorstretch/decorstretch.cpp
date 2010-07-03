#include "Isis.h"
#include "tnt_array2d.h"
#include "PrincipalComponentAnalysis.h"
#include "ProcessBySpectra.h"
#include "Statistics.h"
#include  "GaussianStretch.h"
#include <iomanip>

using namespace std;
using namespace Isis;

void GetData(Buffer &in);
void Transform (Buffer &in, Buffer &out);
void NormalizeAndInvert (Buffer &in, Buffer &out);

PrincipalComponentAnalysis pca(0);
vector<GaussianStretch *> stretches;

string tmpFilename = "Temporary_DecorrelationStretch_Transform.cub";

void IsisMain()
{
  ProcessByBrick p;
  Cube *icube = p.SetInputCube("FROM");
  int numDimensions = icube->Bands();
  p.SetBrickSize(128, 128, numDimensions);

  // The output cube with no attributes and real pixel type
  Isis::CubeAttributeOutput cao;
  cao.PixelType(Isis::Real);

  p.SetOutputCube(tmpFilename, cao, icube->Samples(), icube->Lines(), icube->Bands());

  // Get the data for the transform matrix
  pca = Isis::PrincipalComponentAnalysis(numDimensions);
  ProcessByBrick p2;
  p2.SetBrickSize(128, 128, numDimensions);
  p2.SetInputCube("FROM");
  p2.Progress()->SetText("Computing Transform");
  p2.StartProcess(GetData);
  p2.EndProcess();
  pca.ComputeTransform();

  p.Progress()->SetText("Transforming Cube");
  p.StartProcess(Transform);
  p.EndProcess();

  Isis::CubeAttributeInput cai;

  Cube *icube2 = p.SetInputCube(tmpFilename, cai);
  for (int i=0; i<numDimensions; i++) {
    stretches.push_back(new GaussianStretch( *(icube2->Histogram(i+1)) ));
  }
  p.SetOutputCube("TO");

  p.SetBrickSize(128, 128, numDimensions);
  p.Progress()->SetText("Stretching Cube");
  p.StartProcess(NormalizeAndInvert);

  for (int i=0; i<numDimensions; i++) delete stretches[i];
  stretches.clear();

  p.EndProcess();

  remove(tmpFilename.c_str());
}

void GetData(Buffer &in){
  pca.AddData(in.DoubleBuffer(), in.size()/in.BandDimension());
}

// Processing routine for the pca with one input cube
void Transform (Buffer &in, Buffer &out){
  for (int i=0; i<in.SampleDimension(); i++) {
    for (int j=0; j<in.LineDimension(); j++) {
      TNT::Array2D<double> pre(1, in.BandDimension());
      for (int k=0;k<pre.dim2(); k++) {
        int index = i+j*in.SampleDimension()+k*in.SampleDimension()*in.LineDimension();
        pre[0][k] = in[index];
      }

      TNT::Array2D<double> post = pca.Transform(pre);

      for (int k=0; k<post.dim2(); k++) {
        int index = i+j*in.SampleDimension()+k*in.SampleDimension()*in.LineDimension();
        out[index] = post[0][k];
      }
    }
  }
}

// Processing routine for the pca with two input cubes
void NormalizeAndInvert (Buffer &in, Buffer &out){
  for (int i=0; i<in.SampleDimension(); i++) {
    for (int j=0; j<in.LineDimension(); j++) {
      TNT::Array2D<double> pre(1, in.BandDimension());
      for (int k=0;k<pre.dim2(); k++) {
        int index = i+j*in.SampleDimension()+k*in.SampleDimension()*in.LineDimension();
        // Stretch the data before inverting it
        // NOTE: this needs to be modified to use a GAUSSIAN STRETCH
        //pre[0][k] = (in[index]-bandStats[k]->Average())/bandStats[k]->StandardDeviation();
        pre[0][k] = stretches[k]->Map(in[index]);
      }

      TNT::Array2D<double> post = pca.Inverse(pre);

      for (int k=0; k<post.dim2(); k++) {
        int index = i+j*in.SampleDimension()+k*in.SampleDimension()*in.LineDimension();
        out[index] = post[0][k];
      }
    }
  }
}

