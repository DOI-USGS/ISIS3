# Tests invalid lat/lon ranges.
APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	# minlat > maxlat
	if [ `$(APPNAME) target=moon minlat=-10 maxlat=-20 spacing=latlon      \
	minlon=14.3 maxlon=16.6 latstep=0.35 lonstep=0.71                      \
	networkid=NET001 pointid=PT?? description="Test control network"       \
	onet=$(OUTPUT)/outnet.net >& $(OUTPUT)/error1.txt` ]; then             \
	  true;                                                                \
	fi;                                                                    \

	# minlon > maxlon
	if [ `$(APPNAME) target=moon minlat=-10 maxlat=0 spacing=latlon        \
	minlon=14.3 maxlon=10 latstep=0.35 lonstep=0.71                        \
	networkid=NET001 pointid=PT?? description="Test control network"       \
	onet=$(OUTPUT)/outnet.net >& $(OUTPUT)/error2.txt` ]; then             \
	  true;                                                                \
	fi;     

	$(CAT) $(OUTPUT)/error1.txt $(OUTPUT)/error2.txt >                     \
	  $(OUTPUT)/errors.txt;                                                \
	$(RM) $(OUTPUT)/error1.txt;                                            \
	$(RM) $(OUTPUT)/error2.txt
