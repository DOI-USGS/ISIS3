# This tests the DEBUG, DEBUGLOG, and MAXTHREADS params
#   (debug.txt verifies that MAXTHREADS works correctly)
APPNAME = findfeatures

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           debug=yes \
	           debuglog=$(OUTPUT)/debug_temp.txt \
	           algorithm=brisk/brisk \
	           maxthreads=2 \
	           >& /dev/null;
	$(APPNAME) from=$(INPUT)/EW0211981114G.lev1.cub \
	           match=$(INPUT)/EW0242463603G.lev1.cub \
	           algorithm=brisk/brisk \
	           debug=yes \
	           > $(OUTPUT)/stdoutdebug_temp.txt
	           >& /dev/null;

	# Remove Version, Revision, RunTime, OpenCV_Version, #CPUs, #threads,
	# timestamps, filepaths, Matches/second, Processing, completion seconds
	$(SED) -e 's/\(Version\).*/\1/' \
	       -e 's/\(Revision\).*/\1/' \
	       -e 's/\(RunTime\).*/\1/' \
	       -e 's/\(OpenCV_Version\).*/\1/' \
	       -e 's/\(Number available CPUs:\).*/\1/' \
	       -e 's/\(Number default threads:\).*/\1/' \
	       -e 's/[0-9]\{4\}\-.*:[0-9]\{2\}//' \
	       -e 's|\(Image:\).*/\([^/]*\)|\1   \2|' \
	       -e 's/\(.*second:\).*/\1/' \
	       -e 's/\(Processing.*:\).*/\1/' \
               -e 's/\(.*\) [0-9]*\.[0-9]* \(seconds*\)/\1 \2/' \
	       $(OUTPUT)/debug_temp.txt > $(OUTPUT)/debug.txt;
	# Do same to stdoutdebug_temp, and remove total threads and
	# MatchSolution Group
	$(SED) -e 's/\(Version\).*/\1/' \
	       -e 's/\(Revision\).*/\1/' \
	       -e 's/\(RunTime\).*/\1/' \
	       -e 's/\(OpenCV_Version\).*/\1/' \
	       -e 's/\(Number available CPUs:\).*/\1/' \
	       -e 's/\(Number default threads:\).*/\1/' \
	       -e 's/[0-9]\{4\}\-.*:[0-9]\{2\}//' \
	       -e 's|\(Image:\).*/\([^/]*\)|\1   \2|' \
	       -e 's/\(.*second:\).*/\1/' \
	       -e 's/\(Processing.*:\).*/\1/' \
               -e 's/\(.*\) [0-9]*\.[0-9]* \(seconds*\)/\1 \2/' \
	       -e 's/\(Total threads:\).*/\1/' \
	       -e '/Group/,/End_Group/d' \
	       $(OUTPUT)/stdoutdebug_temp.txt > $(OUTPUT)/stdoutdebug.txt;
	$(RM) $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
