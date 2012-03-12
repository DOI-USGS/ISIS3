#include "DbProfile.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  DbProfile p("test profile");

  cerr << "Valid: " << p.isValid() << endl;
  cerr << "Size: " << p.size() << endl;
  p.setName("test profile");
  cerr << "Name: " << p.Name() << endl;
  cerr << "Adding a key..." << endl;
  p.add("foo", "bar");
  cerr << "Valid: " << p.isValid() << endl;
  cerr << "Size: " << p.size() << endl;
  cerr << "Count: " << p.count("foo") << endl;
  cerr << "Exists: " << p.exists("boo") << endl;
  cerr << "Exists: " << p.exists("foo") << endl;
  cerr << "() operator: " << p("foo") << endl;
  cerr << "Value: " << p.value("foo", 0) << endl;

  cerr << "Test getting non-existing key BadKey\n";
  cerr.flush();
  try {
    cerr << "BadKey =";
    cerr << p("BadKey") << endl;
  }
  catch(IException &ie) {
    ie.print();
  }

  return 0;
}
