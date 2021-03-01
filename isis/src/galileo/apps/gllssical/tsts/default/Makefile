APPNAME = gllssical

include $(ISISROOT)/make/isismake.tsts

commands:
	# Test old spiceinited data
	$(APPNAME) FROM=$(INPUT)/3439R.cub TO=$(OUTPUT)/3439R.cal.cub > /dev/null;
	catlab FROM=$(OUTPUT)/3439R.cal.cub TO=$(OUTPUT)/3439R.cal.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/1213r.cub TO=$(OUTPUT)/1213r.cal.cub > /dev/null;
	catlab FROM=$(OUTPUT)/1213r.cal.cub TO=$(OUTPUT)/1213r.cal.pvl > /dev/null;

	# Test newly re-spiceinited data
	$(APPNAME) FROM=$(INPUT)/3439R.respiceinit.cub TO=$(OUTPUT)/3439R.respiceinit.cub > /dev/null;
	catlab FROM=$(OUTPUT)/3439R.respiceinit.cub TO=$(OUTPUT)/3439R.respiceinit.pvl > /dev/null;
	$(APPNAME) FROM=$(INPUT)/1213r.respiceinit.cub TO=$(OUTPUT)/1213r.respiceinit.cub > /dev/null;
	catlab FROM=$(OUTPUT)/1213r.respiceinit.cub TO=$(OUTPUT)/1213r.respiceinit.pvl > /dev/null;

	# Test non-spiceinited data: throws an error
	if [ `$(APPNAME) \
	  FROM=$(INPUT)/3439R.nospice.cub TO=$(OUTPUT)/broken.cub 2>> $(OUTPUT)/errors_temp.txt > /dev/null` ]; \
	  then true; \
	  fi;

	# Remove everything in brackets like filenames/paths from error messages
	$(SED) 's/\[\([^"]*\)\]//g' $(OUTPUT)/errors_temp.txt \
	  > $(OUTPUT)/errors.txt; 

	# Cleanup
	$(RM) $(OUTPUT)/errors_temp.txt;
	$(RM) $(OUTPUT)/broken.cub;


