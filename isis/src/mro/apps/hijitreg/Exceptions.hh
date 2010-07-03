/*	HiRISE Exceptions

PIRL CVS ID: $Id: Exceptions.hh,v 1.1.1.1 2006/10/31 23:18:15 isis3mgr Exp $

Copyright (C) 2003, 2004, 2005  Arizona Board of Regents on behalf of the
Planetary Image Research Laboratory, Lunar and Planetary Laboratory at
the University of Arizona.

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

#ifndef	_UA_HIRISE_EXCEPTIONS_HEADER_
#define _UA_HIRISE_EXCEPTIONS_HEADER_

#include <exception>
#include <stdexcept>
#include <string>

namespace UA
{

namespace HiRISE
{

/*=*****************************************************************************
Exception Class
*******************************************************************************/

/**	An exception thrown by UA::HiRISE classes.

	This class is at the root of a hierarchy of classes used to signal
	exceptions:

	<dl>
	<dt><code>Error</code>
		<dd>An unrecoverable error condition.
		<dl>
		<dt><code>Invalid_Argument</code> [<code>std::invalid_argument</code>]
			<dd>An invalid or otherwise unrecognized argument value from
			which no recovery is possible.
		<dt><code>Out_of_Range</code> [<code>std::out_of_range</code>]
			<dd>A value is outside the acceptable range.
		</dl>
	</dl>

	An Exception has a message string that describes the reason for the
	exception. The <code>message</code> method will return this message,
	while the <code>what</code> method will prepend the Exception class
	ID to the message and return that.

	@author		Bradford Castalia, Christian Schaller - UA/PIRL
	@version	$Revision: 1.1.1.1 $
*/
class Exception
:	public std::exception
{

public:

/*==============================================================================
Constants
*/
//!	Class identification name with source code version and date.
static const char* const
	ID;

/*==============================================================================
Constructors
*/

/**	Constructs an Exception with a message.

	If the message ends with a newline character ('\n'), it is removed.
	If a caller_id is provided, it preceeds the message string separated
	by a newline character.

	@param	message			The message string. Default: ""
	@param	caller_id		An optional C-string identifying the source
			of the Exception. Default: NULL
*/
explicit Exception
(
	const std::string& message = "",
	const char* caller_id = NULL
);

//	Destructor
virtual ~Exception () throw () {}

/*==============================================================================
Accessors
*/

/**	Gets a C-string that describes the condition that created the
	Exception.

	@return	The C-string that includes the Exception ID as the first
			followed by the caller_id and the message string.
*/
const char* what () const throw ();

/**	Gets the user-provided called_id (if any) and message string.
	
	@return	The message string, which is the caller_id followed by
			the user message.
*/
std::string message () const throw ();

/**	Sets the message string.

	If the message ends with a newline character ('\n'), it is removed.
	If a caller_id was provided when the Exception was created it
	remains; the new message is substituted in place of the previous
	message.
	
	@param	new_message		A string to replace the previous message.
*/
void message (const std::string& new_message);

private:

/*==============================================================================
Private Data
*/

std::string
	Message;
std::string::size_type
	User_Message_Index;

};	//	Exception class

/*=*****************************************************************************
Error Struct (Class)
*******************************************************************************/

/**	Error exception.

	An unrecoverable error condition.
*/
struct Error
:	public Exception
{
/**	Constructs an Error Exception with a message.

	@param	message			The message string. Default: ""
	@param	caller_id		An optional C-string identifying the source
			of the Exception. Default: NULL
*/
explicit Error
(
	const std::string& message = "",
	const char *caller_id = NULL
)
:	Exception (std::string ("Error: ") + message, caller_id)
{}

};	//	Error struct

/*=*****************************************************************************
Invalid_Argument Struct (Class)
*******************************************************************************/

/**	Invalid_Argument exception.

	An invalid or otherwise unrecognized argument value was detected.
*/
struct Invalid_Argument
:	public Error,
	public std::invalid_argument
{
/**	Constructs an Invalid_Argument Error with a message.

	@param	message			The message string. Default: ""
	@param	caller_id		An optional C-string identifying the source
			of the Error. Default: NULL
*/
explicit Invalid_Argument
(
	const std::string& message = "",
	const char *caller_id = NULL
)
:	Error (std::string ("Invalid_Argument\n") + message, caller_id),
	std::invalid_argument
	(
		std::string (Exception::ID)
		+ (caller_id ? (std::string ("\n") + caller_id) : "")
	)
{}

};	//	Invalid_Argument struct

/*=*****************************************************************************
Out_of_Range Struct (Class)
*******************************************************************************/

/**	Out_of_Range exception.

	A value is outside the acceptable range.
*/
struct Out_of_Range :
	public Error,
	public std::out_of_range
{
/**	Constructs an Out_of_Range Error with a message.

	@param	message			The message string. Default: ""
	@param	caller_id		An optional C-string identifying the source
			of the Error. Default: NULL
*/
explicit Out_of_Range
(
	const std::string& message = "",
	const char *caller_id = NULL
)
:	Error (std::string ("Out_of_Range\n") + message, caller_id),
	std::out_of_range
	(
		std::string (Exception::ID)
		+ (caller_id ? (std::string ("\n") + caller_id) : "")
	)
{}

};	//	Out_of_Range struct

}	//	HiRISE namespace

}	//	UA namespace

#endif
