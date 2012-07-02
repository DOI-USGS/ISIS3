APPNAME = mar10clean

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/MVE_030.227.cub to=$(OUTPUT)/cleanTruth.cub \
	> /dev/null;
	# There are 9x9 groups of pixels that result in slightly different DN's on the MAC
        # The FX command converts some column blocks to HRS in oder to avoid OS specific truth data
	fx F1=$(OUTPUT)/cleanTruth.cub to=$(OUTPUT)/temp.cub EQUATION="F1/( (1-(sample>1)*(sample<30)) * (1-(sample>56)*(sample<73)) * (1-(sample>113)*(sample<133)) * (1-(sample>172)*(sample<192)) * (1-(sample>230)*(sample<250)) * (1-(sample>290)*(sample<310)) * (1-(sample>350)*(sample<370)) * (1-(sample>410)*(sample<430)) * (1-(sample>469)*(sample<490)) * (1-(sample>529)*(sample<550)) * (1-(sample>589)*(sample<610)) * (1-(sample>649)*(sample<670)) * (1-(sample>708)*(sample<730)) * (1-(sample>763)*(sample<785)) * (1-(sample>804)*(sample<826)) )" > /dev/null ;
	$(RM) $(OUTPUT)/cleanTruth.cub > /dev/null;
	$(MV) $(OUTPUT)/temp.cub $(OUTPUT)/cleanTruth.cub > /dev/null;
