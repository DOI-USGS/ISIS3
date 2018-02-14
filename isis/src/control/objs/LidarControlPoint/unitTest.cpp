#include "ControlPoint.h"
#include "iTime.h"
#include "LidarControlPoint.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main() {
  Preference::Preferences(true);
  
  cout << "LidarControlPoint Unit Test" << endl;
  
  LidarControlPoint lcp(iTime(13.00), 10, 12);
  
  
  cout << "Testing EditLocking" << endl;
  lcp.SetEditLock(true);
  
  if (lcp.IsEditLocked()) {
    cout << "Edit locked" << endl;
  }
  else {
    cout << "Error: Not edit locked" << endl;
  }
  
  if (lcp.setRange(12.0) == ControlPoint::Status::PointLocked) {
    cout << "Cannot set range because the point is edit locked" << endl;
  }
  else {
    cout << "Error: The point was not properly edit locked. Was able to set the range." << endl;
  }
  if (lcp.setSigmaRange(100.0) == ControlPoint::Status::PointLocked) {
    cout << "Cannot set sigma range because the point is edit locked" << endl;
  }
  else {
    cout << "Error: The point was not properly edit locked. Was able to set the sigma range." << endl;
  }
  if (lcp.setTime(iTime(42.0)) == ControlPoint::Status::PointLocked) {
    cout << "Cannot set time because the point is edit locked" << endl;
  }
  else {
    cout << "Error: The point was not properly edit locked. Was able to set the time." << endl;
  }
  
  
  cout << "Testing Setters and Getters" << endl;
  lcp.SetEditLock(false);
  
  if (lcp.setRange(12.0) == ControlPoint::Status::Success) {
    cout << "Set the range" << endl;
    
    if (lcp.range() != 12.0) {
      cout << "Incorrect range!" << endl;
    }
  }
  else {
    cout << "Error: Could not set the range" << endl;
  }
  if (lcp.setSigmaRange(100.0) == ControlPoint::Status::Success) {
    cout << "Set the sigma range" << endl;
    
    if (lcp.sigmaRange() != 100.0) {
      cout << "Error: Incorrect sigma range!" << endl;
    }
  }
  else {
    cout << "Error: Could not set the sigma range." << endl;
  }
  if (lcp.setTime(iTime(42.0)) == ControlPoint::Status::Success) {
    cout << "Set the time" << endl;
    
    if (lcp.time() != iTime(42.0)) {
      cout << "Error: Incorrect time!" << endl;
    }
  }
  else {
    cout << "Error: Could not set the time." << endl;
  }
}
