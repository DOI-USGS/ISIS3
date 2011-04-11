APPNAME = cnetsplit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) cnet=$(INPUT)/autoseed.net \
	  onet_format=$(OUTPUT)/outsplit?? \
	  num_output_files=2 > /dev/null;
	  cnetmerge inputtype=cnets \
	  from1=$(OUTPUT)/outsplit01.net \
	  from2=$(OUTPUT)/outsplit02.net \
	  id=autoseed \
	  description=cnetsplit \
	  to=$(OUTPUT)/truth.net > /dev/null;
	  $(RM) $(OUTPUT)/outsplit*;
