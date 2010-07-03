using namespace std;

#include "Message.h"

string Isis::Message::MemoryAllocationFailed () {
  string message;
  message = "Unable to allocate dynamic memory";
  return message;
}
