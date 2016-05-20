# This test will print errors thrown by the applicationName application.
APPNAME = seedgrid

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test: No target specified." \
	          > $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/notarget.map \
	             minlat=40 \
	             maxlat=50 \
	             minlon=280 \
	             maxlon=290 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: No target radii." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/targetunknown_noradii.map \
	             minlat=40 \
	             maxlat=50 \
	             minlon=280 \
	             maxlon=290 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: milat > maxlat." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/sinusoidal.map \
	             minlat=40 \
	             maxlat=10 \
	             minlon=280 \
	             maxlon=290 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: minlat < -90." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/sinusoidal.map \
	             minlat=-140 \
	             maxlat=50 \
	             minlon=280 \
	             maxlon=290 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: maxlat > 90." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/sinusoidal.map \
	             minlat=40 \
	             maxlat=100 \
	             minlon=280 \
	             maxlon=290 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: minlon > maxlon." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/sinusoidal.map \
	             minlat=40 \
	             maxlat=50 \
	             minlon=280 \
	             maxlon=90 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: minlon > domainmin." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/sinusoidal.map \
	             minlat=40 \
	             maxlat=50 \
	             minlon=-280 \
	             maxlon=90 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
	echo -e "\nError Test: lonrange > 360." \
	         >> $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) map=$(INPUT)/sinusoidal.map \
	             minlat=40 \
	             maxlat=50 \
	             minlon=280 \
	             maxlon=1290 \
	             xstep=100000 \
	             ystep=100000 \
	             networkid=NET001 \
	             pointid=PT?? \
	             description="Test control network" \
	             onet=$(OUTPUT)/outnet.net \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
