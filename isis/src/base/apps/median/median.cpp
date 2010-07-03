#include "Isis.h"
#include "ProcessByBoxcar.h"
#include "SpecialPixel.h"
#include <algorithm>
#include <vector>

using namespace std;
using namespace Isis;

bool filterNull;
bool filterHrs;
bool filterHis;
bool filterLrs;
bool filterLis;
double low;
double high;
bool propagate;
unsigned int  minimum;

void FilterAll(Buffer &in, double &v);
void FilterValid(Buffer &in, double &v);
void FilterInvalid(Buffer &in, double &v);

void IsisMain(){
  //Set up ProcessByBoxcar
  ProcessByBoxcar p;

  //Obtain input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  //Set up Boxcar size 
  UserInterface &ui = Application::GetUserInterface();
  int samples = ui.GetInteger("SAMPLES");
  int lines = ui.GetInteger("LINES");
  p.SetBoxcarSize(samples, lines);

  //Determine which pixels are valid, and how many are
  //necessary for processing
  filterNull = ui.GetBoolean("NULL");
  filterHrs  = ui.GetBoolean("HRS");
  filterLrs  = ui.GetBoolean("LRS");
  filterHis  = ui.GetBoolean("HIS");
  filterLis  = ui.GetBoolean("LIS");
  if (ui.GetString("MINOPT") == "PERCENTAGE") {
    int size = lines * samples;
    double perc = ui.GetDouble("MINIMUM") / 100;
    minimum = (int) (size * perc);
  }
  else {
    minimum = (int) ui.GetDouble("MINIMUM");
  }
  low         = -DBL_MAX;
  high        = DBL_MAX;
  if (ui.WasEntered("LOW")){
    low = ui.GetDouble("LOW");
  }
  if (ui.WasEntered("HIGH")){
    high = ui.GetDouble("HIGH");
  }

  //Determine what to do if there are too few
  //non-Special pixels
  propagate = (ui.GetString("REPLACEMENT") == "CENTER");

  //Check for filter style, and process accordingly
  if (ui.GetString("FILTER") == "ALL"){
    p.StartProcess(FilterAll);
    p.EndProcess();
  }
  else if (ui.GetString("FILTER") == "INSIDE"){
    p.StartProcess(FilterValid);
    p.EndProcess();
  }
  else if (ui.GetString("FILTER") == "OUTSIDE"){
    p.StartProcess(FilterInvalid);
    p.EndProcess();
  }
}

//Function which loops through every pixel in the boxcar,
//and outputs the median value to the center pixel, if 
//the center pixel is valid.
void FilterValid(Buffer &in, double &v){
  double centerPixel = in[(in.size()-1)/2];

  //Check if the center pixel is a Special Pixel type to be
  //filtered. If not, ignore the pixel and move on
  if (IsSpecial(centerPixel) ){
    if ((IsNullPixel(centerPixel)) && (!filterNull)) {
        v = centerPixel;
        return;
    }
    else if ((IsLisPixel(centerPixel)) && (!filterLis)) {
        v = centerPixel;
        return;
    }
    else if ((IsLrsPixel(centerPixel)) && (!filterLrs)) {
        v = centerPixel;
        return;
    }
    else if ((IsHisPixel(centerPixel)) && (!filterHis)) {
        v = centerPixel;
        return;
    }
    else if ((IsHrsPixel(centerPixel)) && (!filterHrs)) {
        v = centerPixel;
        return;
    }
  }
  else if (centerPixel<low || centerPixel>high){
    v = centerPixel;
    return;
  }

  //Build a vector containing the non-Special pixel values
  //from the input buffer. If there are not enough to meet
  //the minimum requirements, write a user-selected value
  //to the center. If there are, sort the vector and write
  //the median value to the center.
  std::vector<double> boxdata(0);
  for (int i=0; i<in.size(); i++){
    if (!IsSpecial(in[i]) && in[i]>=low && in[i]<=high){
      boxdata.push_back(in[i]);
    }
    else{
      continue;
    }
  }
  if (boxdata.size()<minimum){
    if (propagate){
      v = centerPixel;
      return;
    }
    else {
      v = Isis::Null;
      return;
    }
  }
  sort(boxdata.begin(), boxdata.end());
  v = boxdata[(boxdata.size()-1)/2];
}

//Function to loop through the boxcar and find and write
//the median value to the center pixel, but only if the 
//center pixel is invalid
void FilterInvalid(Buffer &in, double &v){
  double centerPixel = in[(in.size()-1)/2];

  //Check for Special Pixels and handle according to user
  //input.
  if (IsSpecial(centerPixel)){
    if ((IsNullPixel(centerPixel)) && (!filterNull)) {
        v = centerPixel;
        return;
    }
    else if ((IsLisPixel(centerPixel)) && (!filterLis)) {
        v = centerPixel;
        return;
    }
    else if ((IsLrsPixel(centerPixel)) && (!filterLrs)) {
        v = centerPixel;
        return;
    }
    else if ((IsHisPixel(centerPixel)) && (!filterHis)) {
        v = centerPixel;
        return;
    }
    else if ((IsHrsPixel(centerPixel)) && (!filterHrs)) {
        v = centerPixel;
        return;
    }
  }
  else if (centerPixel>=low && centerPixel<=high){
      v = centerPixel;
      return;
  }

  //Build a vector of non-Special pixel values from the
  //input boxcar, then sort and find the median value.
  //If there aren't enough to meet the minimum requirements,
  //write a user-selected value to the center pixel.
  std::vector<double> boxdata(0);

  for (int i=0; i<in.size(); i++){
    if (!IsSpecial(in[i]) && in[i]>=low && in[i]<=high){
      boxdata.push_back(in[i]);
    }
    else{
      continue;
    }
  }
  if (boxdata.size()<minimum){
    if (propagate){
      v = centerPixel;
      return;
    }
    else {
      v = Isis::Null;
      return;
    }
  }
  sort(boxdata.begin(), boxdata.end());
  v = boxdata[(boxdata.size()-1)/2];
}

//Function to find the median value of the boxcar and 
//write it to the center, regardless of the validity
//of the center pixel value
void FilterAll(Buffer &in, double &v){
  double centerPixel = in[(in.size()-1)/2];

  //Check for Special Pixels and handle according to user
  //input.
  if (IsSpecial(centerPixel)){
    if ((IsNullPixel(centerPixel)) && (!filterNull)) {
        v = centerPixel;
        return;
    }
    else if ((IsLisPixel(centerPixel)) && (!filterLis)) {
        v = centerPixel;
        return;
    }
    else if ((IsLrsPixel(centerPixel)) && (!filterLrs)) {
        v = centerPixel;
        return;
    }
    else if ((IsHisPixel(centerPixel)) && (!filterHis)) {
        v = centerPixel;
        return;
    }
    else if ((IsHrsPixel(centerPixel)) && (!filterHrs)) {
        v = centerPixel;
        return;
    }
  }

  //Build a vector of non-Special pixel values from the
  //input boxcar, then sort and find the median value.
  //If there aren't enough to meet the minimum requirements,
  //write a user-selected value to the center pixel.
  std::vector<double> boxdata(0);

  for (int i=0; i<in.size(); i++){
    if (!IsSpecial(in[i]) && in[i]>=low && in[i]<=high){
      boxdata.push_back(in[i]);
    }
    else{
      continue;
    }
  }
  if (boxdata.size()<minimum){
    if (propagate){
      v = centerPixel;
      return;
    }
    else {
      v = Isis::Null;
      return;
    }
  }
  sort(boxdata.begin(), boxdata.end());
  v = boxdata[(boxdata.size()-1)/2];
}

