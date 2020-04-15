# ISIS Templates

The files in this directory are meant to be used as a starting point
for developing new ISIS program files or guidelines for how existing
files should be modified.


## Generic Boilerplate for Most Files

If a file has significant content (original material that authors
could claim copyrights on) please ensure that the following text
is placed in a comment near the top of the file, or in some other
appropriate location, depending on file type.

    This is free and unencumbered software released into the public domain.

    The authors of ISIS do not claim copyright on the contents of this file.
    For more details about the LICENSE terms and the AUTHORS, you will
    find files of those names at the top level of this repository.

Also include this as a separate comment underneath the above,
to make it easier to find-and-replace if we ever need to:

    SPDX-License-Identifier: CC0-1.0

Configuration files and minor READMEs like this one don't really
need it, as the blanket LICENSE file covers those to the extent
needed.  PR reviewers should be able to help advise if you are
unsure.
