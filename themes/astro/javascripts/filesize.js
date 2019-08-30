//SCRIPT: Calculate file size
//Filename: filesize.js
//Purpose:  This script is a collection of functions that are used
//          by the scipt filesize.js, which is used by an html page
//          that has a file size calculator.  The functions take care
//          of calculations, formatting and checking input values.
//
//
//  Functions:
//       formatD - formats output with decimals
//       formatC - formats output with commas
//       checkValue - checks input for non number values
//       checkPos -  checks input for posative values
//
// History:
//   2006-07-10 dls changed char indexing in string from [i] to charAt(i)
//                  changed the way passBit was being handled
//                  both changes fixed MSIE problems

//___________________________________________________________________
//This function formats the number of decimal places in the output.
//Much of this code was copied from fomatting code that was written
//by David Mosley.  You can find the code at:
// http://www.chimie.fundp.ac.be/javas/js_format.html

var status = true;

function formatD (newSize){

//==================================================
//Formatting Code
//==================================================

  var len=newSize.toString().length;
  var width=20;
  var dpls=2;
  var lt1=false;
  var res;
  var txt = "";

// Work with absolute value
  var absx=Math.abs(newSize);
// Nasty fix to deal with numbers < 1 and problems with leading zeros!
  if ((absx < 1) && (absx > 0)) {
    lt1 = true;
    absx+=10;
  }

// Get postion of decimal point
  var pt_pos = absx.toString().indexOf(".");

  if ( pt_pos == -1) {
    res="";
    res+= absx.toString();
    res+= ".";

    for (var i = 0; i < dpls; i++) {
      res += 0;
    }
  }
  else {
    res = Math.round(absx * Math.pow(10,dpls));
    res=res.toString();
    if (res.length ==
        Math.round(Math.floor(absx * Math.pow(10,dpls))).toString().length) {
      res = res.substring(0,pt_pos) + "." +
            res.substring(pt_pos,res.length);
    }
    else {
      pt_pos++;
      res = res.substring(0,pt_pos) + "." +
            res.substring(pt_pos,res.length);

    }

// Remove leading 1 from  numbers < 1 (Nasty fix!)
    if (lt1) {
      res=res.substring(1,res.length);

    }
  }
//===============================================
//end of formatting code
//===============================================

  return res;

}
//__________________________________________________________________________
// This function puts commas in to a number.  It is used in these
// scripts when the user requests the output in bytes.

function formatC (newSize){

  var res = "";


  var stringSize = newSize.toString();
  var len=newSize.toString().length;
  n = 0;
  for (i = (len -1); i>-1; i--) {


    if (n == 3) {
      res = "," + res;
      n = 0;
    }
    res = stringSize.charAt(i) + res;
    n = n + 1;
  }

  return res;

}
//__________________________________________________________________________
//*****************************************************
//SCRIPT: check input values to make sure is valid
//FUNCTION NAME: checkValue
//PURPOSE:
// This script will check an input value to ensure that
// a number has been entered.  It first converts the input
// value to a string.  then it looks a each element of the
// string which has been converted to a number,  and uses
// the function isNaN to test is the input is valid.
//
//This function was written to work with filesize.js and
//check input line or sample value to ensure it is valid.
//*******************************************************

//This function checks for valid numeric input (is input a number)
function checkValue (inValue){
// create a status varable.
// convert input value to string and get length
  var string = inValue.toString();
  var len = string.length;

// loop through each element of input value(string)
  for (i=0; i<len; i++) {
    test = Number(string.charAt(i));

// use the isNan function to test if input is valid
    if (isNaN(test) == 1) {  //if isNaN returns true then output an alert box
      status = false;
      myMessage = " Invalid input value see The value: ( " + string.charAt(i)  + ") ";
      alert (myMessage);
    }
  }
  return status;
}

//__________________________________________________________________________
//SCRIPT: check if an input value is a posative number
//FUNCTION NAME: checkPos
//PURPOSE: This script will check an input value to see if the
//         value is posative.  If a nefgative number is found the
//         an alert box is set off. This script is used to test the
//         line and sample input input.

function checkPos (inValue){
  if (inValue < 1) {
    myMessage ="The entered vaule is negative see: ( " + inValue + ") ";
    alert (myMessage);
    status = false;
  }
  return status;
}
//__________________________________________________________________________
//SCRIPT: Calculate file size.
//FUNCTION NAME:  fsize
//PURPOSE:  This function will take input of line, sample, and byte type
//          and calculate the file size.  The user can also specify the \
//          output units and the function will calculate.

function fsize() {
  var passLine=document.myForm.inLine.value ;
  var passSamp=document.myForm.inSamp.value ;
  var passBit=Number(document.myForm.inBit[document.myForm.inBit.selectedIndex].text) ;
  var passType=document.myForm.outType[document.myForm.outType.selectedIndex].value ;
  var answer=document.myForm.answer ;
  var txt="";
  status=true;

// check the input values
  checkPos(passLine);

  if (status == true) {
    checkValue(passLine);
  }

  checkPos(passSamp);
  if (status == true) {
    checkValue(passSamp);
  }

  if (passBit==8) {
    size = passLine*passSamp*1.0;
  }
  else if (passBit==16) {
    size = passLine*passSamp*2.0;
  }
  else {
    size = passLine*passSamp*4.0;
  }


  if (passType=="b") {
    newSize = size;
//      txt = newSize + " Bytes";
  }
  else if (passType=="k") {
    newSize = (size / 1024);
  }
  else if (passType=="m") {
    newSize = ((size / 1024) / 1024);
  }
  else if (passType=="g") {
    newSize = (((size / 1024) / 1024) / 1024);
  }


  if (passType=="b") {
    res = newSize;
    txt = formatC(newSize) + " Bytes";
  }
  else if (passType=="k") {
    txt = formatD(newSize) + " Kilobytes";
  }
  else if (passType=="m") {
    txt = formatD(newSize) + " Megabytes";

  }
  else if (passType=="g") {
    txt =  formatD(newSize) + " Gigabytes";
  }
  if (status == false) {
    txt = "Entry Error";
  }
  document.myForm.answer.value = txt;
}

