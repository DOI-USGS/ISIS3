#include "Isis.h"
#include "ProcessByQuickFilter.h"
#include "UserInterface.h"

using namespace std; 
using namespace Isis;

// Which pixel types to filter
bool filterNull;
bool filterLis;
bool filterLrs;
bool filterHis;
bool filterHrs;
bool propagate;

// Prototype
void FilterAll (Buffer &in, Buffer &out, QuickFilter &filter);
void FilterValid (Buffer &in, Buffer &out, QuickFilter &filter);
void FilterInvalid (Buffer &in, Buffer &out, QuickFilter &filter);

void IsisMain() {
  //Set up ProcessByQuickFilter
  ProcessByQuickFilter p;

  // Obtain input and output cubes
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");

  // Find out which special pixels are to be filtered
  UserInterface &ui = Application::GetUserInterface();
  filterNull = ui.GetBoolean ("NULL");
  filterLis = ui.GetBoolean ("LIS");
  filterLrs = ui.GetBoolean ("LRS");
  filterHis = ui.GetBoolean ("HIS");
  filterHrs = ui.GetBoolean ("HRS");
  propagate = (ui.GetString("REPLACEMENT") == "CENTER");

  //Set the Boxcar Parameters
  int lines = ui.GetInteger("LINES");
  int samples = ui.GetInteger("SAMPLES");
  double low = -DBL_MAX;
  double high = DBL_MAX;
  int minimum;
  if (ui.WasEntered("LOW")) {
    low = ui.GetDouble("LOW");
  }
  if (ui.WasEntered("HIGH")) {
    high = ui.GetDouble("HIGH");
  }
  if (ui.GetString("MINOPT") == "PERCENTAGE") {
    int size = lines * samples;
    double perc = ui.GetDouble("MINIMUM") / 100;
    minimum = (int) (size * perc);
  }
  else {
    minimum = (int)ui.GetDouble("MINIMUM");
  }

  p.SetFilterParameters(samples, lines, low, high, minimum);

  //Start the appropriate filter method
  if (ui.GetString("FILTER") == "ALL"){
    p.StartProcess(FilterAll);
    p.EndProcess();
  }
  else if(ui.GetString("FILTER") == "INSIDE") {
    p.StartProcess(FilterValid);
    p.EndProcess();
  }
  else if (ui.GetString("FILTER") == "OUTSIDE") {
    p.StartProcess(FilterInvalid);
    p.EndProcess();
  }
}

//Function to loop through the line, and determine the
//average value of the pixels around each valid pixel, writing that
//average to the output at the pixel index
void FilterValid (Buffer &in, Buffer &out, QuickFilter &filter) {

  //Loop through each pixel in the line
  for (int i=0; i<filter.Samples(); i++) {
    //Check against all Special Pixel types to be filtered, if the 
    //center meets requirements, run the Average
    if (IsSpecial(in[i])) {
      if ((IsNullPixel(in[i])) && (filterNull)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
          if (in.Line()==57 && i==60) {
            cout<< "Count: " << filter.Count(i) << "Average:" <<filter.Average(i)<<endl;
          }
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsLisPixel(in[i])) && (filterLis)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsLrsPixel(in[i])) && (filterLrs)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsHisPixel(in[i])) && (filterHis)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsHrsPixel(in[i])) && (filterHrs)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else {
        out[i] = in[i];
      }
    }
    //If the pixel is Non-Special, run the filter, unless the
    //center is not valid, as determined by Low and High
    else{
      if (in[i]<filter.Low() || in[i]>filter.High()){
        out[i] = in[i];
      }
      else{
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
    }
  }
}

//Function to loop through each pixel in a line and find the average
//of the surrounding pixels, and write it to the output, if the 
//center pixel does not meet the requirements for validity
void FilterInvalid (Buffer &in, Buffer &out, QuickFilter &filter) {
  for (int i=0; i<filter.Samples(); i++) {
    //Check the center against all Special Pixel types, if it
    //is a Special Pixel type marked for filtering, 
    // run the Average filter, otherwise leave it alone
    if (IsSpecial(in[i])) {
      if ((IsNullPixel(in[i])) && (filterNull)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsLisPixel(in[i])) && (filterLis)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsLrsPixel(in[i])) && (filterLrs)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsHisPixel(in[i])) && (filterHis)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsHrsPixel(in[i])) && (filterHrs)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else {
        out[i] = in[i];
      }
    }
    //If the pixel is not Special, check if it is outside the
    //valid range, as determined by Low and High, if so, run the
    //average filter
    else {
      if (in[i]>=filter.Low() && in[i]<=filter.High()){
        out[i] = in[i];
      }
      else{
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
    }
  }
}

//Function to run the Average filter on all pixels, regardless
//of their value
void FilterAll (Buffer &in, Buffer &out, QuickFilter &filter) {
  for (int i=0; i<filter.Samples(); i++) {
    //Check the center against all Special Pixel types, if it
    //is a Special Pixel type marked for filtering, 
    // run the Average filter, otherwise leave it alone
    if (IsSpecial(in[i])) {
      if ((IsNullPixel(in[i])) && (filterNull)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsLisPixel(in[i])) && (filterLis)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsLrsPixel(in[i])) && (filterLrs)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsHisPixel(in[i])) && (filterHis)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else if ((IsHrsPixel(in[i])) && (filterHrs)) {
        if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
          out[i] = filter.Average(i);
        }
        else {
          out[i] = in[i];
        }
      }
      else {
        out[i] = in[i];
      }
    }
    //If the Pixel is non-Special, run the average filter, unless there
    //aren't enough valid pixels for filtering and the center pixel value
    // is to be propagated
    else {
      if (filter.Count(i)>=filter.MinimumPixels() || !propagate){
        out[i] = filter.Average(i);
      }
      else {
        out[i] = in[i];
      }
    }
  }
}

