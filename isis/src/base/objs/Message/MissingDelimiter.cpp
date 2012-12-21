using namespace std;

#include <sstream>

#include "IString.h"
#include "Message.h"

QString Isis::Message::MissingDelimiter(char d) {
  return "Missing delimiter [" + toString(d) + "]";
}

QString Isis::Message::MissingDelimiter(char d, const QString &near) {
  QString message = "Missing delimiter [" + toString(d) + "] at or near [";
  
  if(near.size() <= 20) {
    message += near + "]";
  }
  else {
    message += near.mid(0, 20) + " ...]";
  }

  return message;
}
