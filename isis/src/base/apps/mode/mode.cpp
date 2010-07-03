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
bool propagate;
double low;
double high;
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
  low       = -DBL_MAX;
  high      = DBL_MAX;
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
  if (ui.GetString("PIXELS") == "ALL"){
    p.StartProcess(FilterAll);
    p.EndProcess();
  }
  else if (ui.GetString("PIXELS") == "INSIDE"){
    p.StartProcess(FilterValid);
    p.EndProcess();
  }
  else if (ui.GetString("PIXELS") == "OUTSIDE"){
    p.StartProcess(FilterInvalid);
    p.EndProcess();
  }
}

//Function which loops through every pixel int the boxcar,
//and outputs the mode value to the center pixel, if
//the center pixel is valid
void FilterValid(Buffer &in, double &v){
  double centerPixel = in[(in.size()-1)/2];

  //Check if the center pixel is valid
  //Valid is defined as a Special Pixel declared as a
  //valid type, or a normal value between low and high.
  //If the center pixel does not meet these requirements,
  //write the original value to the center and move on
  if ( IsSpecial(centerPixel) ){
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
  }

  //Gather all non-special pixels into a vector and determine
  //the mode, provided there are enough for filtering
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

  //Determine the most common(mode) pixel value
  double modeVal = centerPixel;
  sort(boxdata.begin(), boxdata.end());
  int count = 1;
  int maxCount = 1;
  for (unsigned int i=1; i<boxdata.size(); i++){
    if (boxdata[i] == boxdata[i-1]){
      count++;
    }
    else{
      if (count>maxCount){
        modeVal = boxdata[i-1];
        maxCount = count;
      }
      count = 1;
    }
  }
  v = modeVal;
}


//Function to loop through the boxcar and find and write
//the mode value to the center pixel, but only if the
//center pixel is invalid
void FilterInvalid(Buffer &in, double &v){
  double centerPixel = in[(in.size()-1)/2];

  //Check if the center pixel is valid
  //Valid is defined as a Special Pixel declared as a
  //valid type, or a normal value between low and high.
  //If the center is valid, write the original value to the
  //center, and move the boxcar
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
  }

  //Now, build a vector containing non-Special pixels, 
  //and, if there are enough, process the vector to
  //determine the mode. If there aren't enough, write
  //a user selected value to the center.
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
   //Sort the vector, and determine the mode pixel value
  double modeVal = centerPixel;
  sort(boxdata.begin(), boxdata.end());
  int count = 1; 
  int maxCount = 1;
  for (unsigned int i=1; i<boxdata.size(); i++){
    if (boxdata[i] == boxdata[i-1]){
      count++;
    }
    else{
      if (count>maxCount){
        modeVal = boxdata[i-1];
        maxCount = count;
      }
      count = 1;
    }
  }
  v = modeVal;
}

//Function to process the boxcar and determine the mode,
//regardless of validity of the center pixel
void FilterAll(Buffer &in, double &v){
  double centerPixel = in[(in.size()-1)/2];

  //Check if the center pixel is valid
  //Valid is defined as a Special Pixel declared as a
  //valid type, or a normal value between low and high.
  //If the center is valid, write the original value to the
  //center, and move the boxcar
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

  //Now, build a vector containing non-Special pixels, 
  //and, if there are enough, process the vector to
  //determine the mode. If there aren't enough, write
  //a user selected value to the center.
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
   //Sort the vector, and determine the mode pixel value
  double modeVal = centerPixel;
  sort(boxdata.begin(), boxdata.end());
  int count = 1;
  int maxCount = 1;
  for (unsigned int i=1; i<boxdata.size(); i++){
   if (boxdata[i] == boxdata[i-1]){
      count++;
    }
    else{
      if (count>maxCount){
        modeVal = boxdata[i-1];
        maxCount = count; 
      }
      count = 1;
    }
  }
  v = modeVal;
}
