using namespace std;

#include <sstream>
#include "Message.h"

string Isis::Message::MissingDelimiter (const char d) {
  ostringstream os;
  os << "Missing delimiter [" << d << "]"; 
  return os.str();
}

string Isis::Message::MissingDelimiter (const char d, const string &near) {
  ostringstream os;

  os << "Missing delimiter [" << d << "] at or near ["; 
  if (near.size () <= 20) {
    os << near << "]";
  }
  else {
    os << near.substr(0,20) << " ...]";
  }
  return os.str();
}
