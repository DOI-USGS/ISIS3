#include "DbProfile.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  DbProfile p("test profile");

  cout << "Valid: " << p.isValid() << endl;
  cout << "Size: " << p.size() << endl;
  p.setName("test profile");
  cout << "Name: " << p.Name() << endl;
  cout << "Adding a key..." << endl; 
  p.add("foo", "bar");
  cout << "Valid: " << p.isValid() << endl;
  cout << "Size: " << p.size() << endl;
  cout << "Count: " << p.count("foo") << endl;
  cout << "Exists: " << p.exists("boo") << endl;
  cout << "Exists: " << p.exists("foo") << endl;
  cout << "() operator: " << p("foo") << endl;
  cout << "Value: " << p.value("foo", 0) << endl;

  cout << "Test getting non-existing key BadKey\n";
  cout.flush();
  try {
    cout << "BadKey =";
    cout << p("BadKey") << endl;
  } catch ( iException &ie ) {
    ie.Report();
  }

  return 0;
}
