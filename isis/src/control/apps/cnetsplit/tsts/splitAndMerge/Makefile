# Split a control net with 6 control points in two, then merge it back to one.
#    Output folder will contain
#      outsplit01 with the first 3 points
#      outsplit02 with the last 3 points
#      truth.net - merged control net --should have same points as input cnet
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
