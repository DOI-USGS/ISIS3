#!/bin/csh


# Setup tab completion
if ( -f $ISISROOT/bin/isiscomplete ) then
  set newBinaries = `/bin/ls $ISISROOT/bin/xml | grep -v qt.conf | sed s%\.xml%%`
  set i = 0
  set loop = 0
  set chunk = ""
  foreach app ($newBinaries)
    @ i = $i + 1
    @ loop = $loop + 1
    set chunk = "$chunk $app"
    if ( "$i" == 100 || "$loop" == $#newBinaries) then
      eval `$ISISROOT/bin/isiscomplete $chunk`
      set i = 1
      set chunk = ""
    endif
  end
  unset i
  unset loop
  unset chunk
  unset app
  unset newBinaries
endif
