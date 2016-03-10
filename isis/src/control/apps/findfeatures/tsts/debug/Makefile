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

	# Remove version
	$(SED) -i'' 's/\(Version\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove Revision
	$(SED) -i'' 's/\(Revision\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove RunTime
	$(SED) -i'' 's/\(RunTime\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove OpenCV_Version
	$(SED) -i'' 's/\(RunTime\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove Number available CPUs
	$(SED) -i'' 's/\(Number available CPUs:\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove Number default threads
	$(SED) -i'' 's/\(Number default threads:\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove Total threads
	  # (don't need to for DEBUGLOG, since we specified MAXTHREADS)
	$(SED) -i'' 's/\(Total threads:\).*/\1/' \
	  $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove timestamps YYYY-MM-DDThh:mm:ss
	$(SED) -i'' 's/[0-9]\{4\}\-.*:[0-9]\{2\}//' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Replace filepaths with filename
	$(SED) -i'' 's|\(Image:\).*/\([^/]*\)|\1   \2|' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove anything after Matches/second
	$(SED) -i'' 's/\(.*second:\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Replace anything after Processing ...:
	$(SED) -i'' 's/\(Processing.*:\).*/\1/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove completion seconds
	$(SED) -i'' 's/\(.*\)[0-9]\+\.[0-9]* \(seconds*\)/\1\2/' \
	  $(OUTPUT)/debug_temp.txt $(OUTPUT)/stdoutdebug_temp.txt;
	# Remove the MatchSolution Group from stdoutdebug
	$(SED) -i'' '/Group/,/End_Group/d' \
	  $(OUTPUT)/stdoutdebug_temp.txt;

	$(MV) $(OUTPUT)/debug_temp.txt $(OUTPUT)/debug.txt;
	$(MV) $(OUTPUT)/stdoutdebug_temp.txt $(OUTPUT)/stdoutdebug.txt;
