#include <iostream>
#include "TableField.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[]) {
  Isis::Preference::Preferences(true);

  cout << "Testing integer singleton" << endl;
  Isis::TableField f1("Test",Isis::TableField::Integer);
  cout << "Name      = " << f1.Name() << endl;
  cout << "IsInteger = " << f1.IsInteger() << endl;
  cout << "IsDouble  = " << f1.IsDouble() << endl;
  cout << "IsText    = " << f1.IsText() << endl;
  cout << "IsReal    = " << f1.IsReal() << endl;
  cout << "Size      = " << f1.Size() << endl;
  cout << "Bytes     = " << f1.Bytes() << endl;
  f1 = 15;
  cout << "Value     = " << (int)f1 << endl;
  Isis::PvlGroup g = f1.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  cout << "Testing double singleton" << endl;
  Isis::TableField f2("Test",Isis::TableField::Double);
  cout << "Name      = " << f2.Name() << endl;
  cout << "IsInteger = " << f2.IsInteger() << endl;
  cout << "IsDouble  = " << f2.IsDouble() << endl;
  cout << "IsText    = " << f2.IsText() << endl;
  cout << "IsReal    = " << f2.IsReal() << endl;
  cout << "Size      = " << f2.Size() << endl;
  cout << "Bytes     = " << f2.Bytes() << endl;
  f2 = -3.14;
  cout << "Value     = " << (double)f2 << endl;
  g = f2.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  cout << "Testing text singleton" << endl;
  Isis::TableField f3("Test",Isis::TableField::Text,20);
  cout << "Name      = " << f3.Name() << endl;
  cout << "IsInteger = " << f3.IsInteger() << endl;
  cout << "IsDouble  = " << f3.IsDouble() << endl;
  cout << "IsText    = " << f3.IsText() << endl;
  cout << "IsReal    = " << f3.IsReal() << endl;
  cout << "Size      = " << f3.Size() << endl;
  cout << "Bytes     = " << f3.Bytes() << endl;
  f3 = "Bah humbug"; 
  cout << "Value     = " << (string)f3 << endl;
  g = f3.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  cout << "Testing real singleton" << endl;
  Isis::TableField f4("Test",Isis::TableField::Real);
  cout << "Name      = " << f4.Name() << endl;
  cout << "IsInteger = " << f4.IsInteger() << endl;
  cout << "IsDouble  = " << f4.IsDouble() << endl;
  cout << "IsText    = " << f4.IsText() << endl;
  cout << "IsReal    = " << f4.IsReal() << endl;
  cout << "Size      = " << f4.Size() << endl;
  cout << "Bytes     = " << f4.Bytes() << endl;
  f4 = (float)15.542;
  cout << "Value     = " << (float)f4 << endl;
  g = f4.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  cout << "Testing integer array" << endl;
  Isis::TableField f5("Test",Isis::TableField::Integer,3);
  cout << "Name      = " << f5.Name() << endl;
  cout << "IsInteger = " << f5.IsInteger() << endl;
  cout << "IsDouble  = " << f5.IsDouble() << endl;
  cout << "IsText    = " << f5.IsText() << endl;
  cout << "IsReal    = " << f5.IsReal() << endl;
  cout << "Size      = " << f5.Size() << endl;
  cout << "Bytes     = " << f5.Bytes() << endl;
  vector<int> temp; temp.push_back(3); temp.push_back(2); temp.push_back(1);
  f5 = temp;
  temp.clear();
  temp = f5;
  cout << "Value     = " << temp[0] << " " << temp[1] << " " << temp[2] << endl;
  g = f5.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  cout << "Testing double array" << endl;
  Isis::TableField f6("Test",Isis::TableField::Double,3);
  cout << "Name      = " << f6.Name() << endl;
  cout << "IsInteger = " << f6.IsInteger() << endl;
  cout << "IsDouble  = " << f6.IsDouble() << endl;
  cout << "IsText    = " << f6.IsText() << endl;
  cout << "IsReal    = " << f6.IsReal() << endl;
  cout << "Size      = " << f6.Size() << endl;
  cout << "Bytes     = " << f6.Bytes() << endl;
  vector<double> tmp; tmp.push_back(1.3); tmp.push_back(2.4); tmp.push_back(-9.2);
  f6 = tmp;
  tmp.clear();
  tmp = f6;
  cout << "Value     = " << tmp[0] << " " << tmp[1] << " " << tmp[2] << endl;
  g = f6.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  cout << "Testing real array" << endl;
  Isis::TableField f7("Test",Isis::TableField::Real,3);
  cout << "Name      = " << f7.Name() << endl;
  cout << "IsInteger = " << f7.IsInteger() << endl;
  cout << "IsDouble  = " << f7.IsDouble() << endl;
  cout << "IsText    = " << f7.IsText() << endl;
  cout << "IsReal    = " << f7.IsReal() << endl;
  cout << "Size      = " << f7.Size() << endl;
  cout << "Bytes     = " << f7.Bytes() << endl;
  vector<float> tmp2; tmp2.push_back(1.3); tmp2.push_back(2.4); tmp2.push_back(-9.2);
  f7 = tmp2;
  tmp2.clear();
  tmp2 = f7;
  cout << "Value     = " << tmp2[0] << " " << tmp2[1] << " " << tmp2[2] << endl;
  g = f7.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
 
  cout<< "Testing integer constructor" << endl ;
  Isis::PvlGroup group1("Field");
  
  group1 += Isis::PvlKeyword ("name", "Test");
  group1 += Isis::PvlKeyword("type", "Integer");
  group1 += Isis::PvlKeyword("size", 20);
  
  Isis::TableField f8(group1);
  g = f8.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  
  cout<< "Testing double constructor" << endl ;
  Isis::PvlGroup group2("Field");
  
  group2 += Isis::PvlKeyword ("name", "Test");
  group2 += Isis::PvlKeyword("type", "Double");
  group2 += Isis::PvlKeyword("size", 20);
  
  Isis::TableField f9(group2);
  g = f9.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  
  cout<< "Testing text constructor" << endl ;
  Isis::PvlGroup group3("Field");
  
  group3 += Isis::PvlKeyword ("name", "Test");
  group3 += Isis::PvlKeyword("type", "Text");
  group3 += Isis::PvlKeyword("size", 20);
  
  Isis::TableField f10(group3);
  g = f10.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;

  cout<< "Testing real constructor" << endl ;
  Isis::PvlGroup group4("Field");
  
  group4 += Isis::PvlKeyword ("name", "Test");
  group4 += Isis::PvlKeyword("type", "Real");
  group4 += Isis::PvlKeyword("size", 20);
  
  Isis::TableField f11(group4);
  g = f11.PvlGroup();
  cout << g << endl;
  cout << "----------------------------------------" << endl;
  
  cout<< "Testing erroneous type constructor" << endl ;
  try{
  Isis::PvlGroup group5("Field");
  
  group5 += Isis::PvlKeyword ("name", "Test");			
  group5 += Isis::PvlKeyword("type", "BLAH");			
  group5 += Isis::PvlKeyword("size", 20);				
  
  Isis::TableField f12(group5);				
  g = f12.PvlGroup();						
  cout << g << endl;
  }
  catch (Isis::iException &e){
   e.Report(false);
  }
  cout << "----------------------------------------" << endl;
 
  cout<< "Testing erroneous size constructor" << endl ;
  try{
  Isis::PvlGroup group6("Field");	
  		
  group6 += Isis::PvlKeyword ("name", "Test");
  group6 += Isis::PvlKeyword("type", "Integer");
  group6 += Isis::PvlKeyword("size", -7.3);
  
  Isis::TableField f13(group6);
  g = f13.PvlGroup();
  cout << g << endl;
  
  }
  catch (Isis::iException &e){
    e.Report(false);
  }
  cout << "----------------------------------------" << endl;
 }
