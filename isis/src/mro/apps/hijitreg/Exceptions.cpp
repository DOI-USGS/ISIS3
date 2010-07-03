/*	Exception

PIRL CVS ID: $Id: Exceptions.cpp,v 1.2 2009/02/23 16:36:10 slambright Exp $

Copyright (C) 2003  Arizona Board of Regents on behalf of the Planetary
Image Research Laboratory, Lunar and Planetary Laboratory at the
University of Arizona.

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License, version 2.1,
as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.

*/

#include <sstream>
#include <cstring>
#include "Exceptions.hh"

#if defined (DEBUG)

/*=*****************************************************************************
Debug Controls

Define any of the following options to obtain the desired debug reports.
*******************************************************************************/

#define	DEBUG_ALL			(-1)
#define	DEBUG_EXCEPTIONS	(1 << 0)

#include <iostream>
using std::cerr;
using std::endl;

#endif
//	End DEBUG

namespace UA
{
namespace HiRISE
{
/*==============================================================================
Constants
*/
const char* const
	Exception::ID = 
		"UA::HiRISE::Exception ($Revision: 1.2 $ $Date: 2009/02/23 16:36:10 $)";

/*==============================================================================
Constructor
*/

Exception::Exception
(
	const std::string& message,
	const char* caller_id
)
:	Message
	(
		std::string (ID)
		+ (caller_id ? (std::string ("\n") + caller_id) : "")
		+ '\n' + message
	)
{
	if (! Message.empty () && Message [Message.size () - 1] == '\n')
		//	Remove the trailing newline.
		Message.erase (Message.size () - 1);
	User_Message_Index =
		strlen (ID) + (caller_id ? (strlen (caller_id) + 2) : 1);
#if ((DEBUG) & DEBUG_EXCEPTIONS)
	cerr
		<< ">-< Exception" << endl
		<< "    message -" << endl << message << endl
		<< "    caller_id -" << endl;
	if (caller_id)
		cerr << caller_id << endl;
	else
		cerr << "NULL" << endl;
	cerr
		<< "    Message -" << endl << Message << endl
		<< "    User_Message_Index: " << User_Message_Index << endl;
#endif
}

/*==============================================================================
Accessors
*/

const char* Exception::what ()
const
throw ()
{ return Message.c_str (); }

std::string Exception::message ()
const
throw ()
{
	//	Exclude the Exception ID but include the caller_id (if it exists).
	std::string::size_type index = strlen (ID) + 1;
	if (index > Message.size ())
		index = 0;
	return Message.substr (index);
}

void Exception::message
(
	const std::string& new_message
)
{
#if ((DEBUG) & DEBUG_EXCEPTION)
	cerr
		<< ">>> Exception::message" << endl
		<< "    Message -" << endl << Message << endl
		<< "    new_message -" << endl << new_message << endl
		<< "    Message.length:     " << Message.length () << endl
		<< "    User_Message_Index: " << User_Message_Index << endl
		<< "    new_message.length: " << new_message.length () << endl;
#endif
	if (User_Message_Index >= Message.length ())
		//	Then there is no message to replace; append on the next line.
		(Message += '\n') += new_message;
	else
		//	Replace the trailing user message portion of the Message.
		Message.replace (User_Message_Index,
			Message.length () - User_Message_Index, new_message);
	if (! Message.empty () && Message [Message.size () - 1] == '\n')
		//	Remove the trailing newline.
		Message.erase (Message.size () - 1);
#if ((DEBUG) & DEBUG_EXCEPTION)
	cerr
		<< "<<< Exception::message" << endl;
#endif
}

}   //  namespace HiRISE
}   //  namespace UA
