# ISIS Project public API

The public API is considered to be all the interfaces that the ISIS
project provides to the community from which the ISIS user base can
build more complex tools. This API therefore includes code-level
structures accessible via software calls to libraries, but also
more user-facing structures like command-line programs and graphical
user interface (GUI) programs. Any "interface" via which a user can
reasonably interact with components of the ISIS project should be
considered part of the "ISIS Project public API" for the purposes
of [Semantic Versioning](https://semver.org).

Examples are provided below, but are not meant to be exhaustive or
complete. The above philosophy should guide our thinking in what
the public API is.


## General Guidance

**Code-level structures**: Function, class, and object names,
argument signatures, and return signatures for functions, classes,
and objects.

	- C++ code exceptions would be those structures above marked with
      a "private" access specifier via the C++ language.
    - Python code exceptions would be those structures above which begin 
      with an underbar ("_", as per Python language tradition).

**User-facing command-line and GUI programs**: program name, argument
list, argument defaults, configuration or preference names and their
defaults, and for those programs with structured output files, the
structure and contents of those output files are analogous to the
return signatures of code-level structures.


This guidance may be more straightforward to understand and apply
for code-level structures which have more formalized argument and
return signatures.  User-facing command-line and GUI programs should
include verbose information in their documentation that describes what
a user can expect as far as the structure of the output.


## More Specific Examples

### Unstructured text files

For anything of an inherently unstructured file-type, like .txt;
there is no guarnatee of its contents; just that it will exist.


### CSV output files

Column names and data types are part of the API.

For example, a program that writes out a CSV should clearly describe
the output columns which include the fieldname, data type, and
whether non-data-type values can be present (e.g. a column of
floating point numbers named "minimum" that could contain the string
"NULL" instead of a floating point value).

New columns can be added (feature), but if a column's characteristics are 
changed or removed, that's a breaking change.


### PVL files

Object, group, and keyword names are a part of the API.

For example, the "minimum" keyword will exist in the "statistics"
group. This also applies to the PVL labels in Cube files.

New aggregates or keywords can be added (feature), but if any are changed
or removed, that's a breaking change.  PVL text does have data types, and
changing something from `MIN = 5` to `MIN = "5"` is changing the data type
of the keyword MIN from an integer to a string.
