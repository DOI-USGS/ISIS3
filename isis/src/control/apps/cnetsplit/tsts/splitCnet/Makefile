APPNAME = cnetsplit

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) cnet=$(INPUT)/autoseed.net \
	  onet_prefix=$(OUTPUT)/outsplit?? \
	  num_output_files=2 > /dev/null;
	  cnetmerge inputtype=cnets \
	  cnet=$(OUTPUT)/outsplit01.net \
	  cnet2=$(OUTPUT)/outsplit02.net \
	  networkid=autoseed \
	  description=cnetsplit \
	  onet=$(OUTPUT)/truth.net > /dev/null;
	  $(RM) $(OUTPUT)/outsplit*;
