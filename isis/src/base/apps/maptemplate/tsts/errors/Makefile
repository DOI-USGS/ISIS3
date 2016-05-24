# This test will print errors thrown by the applicationName application.
APPNAME = maptemplate

include $(ISISROOT)/make/isismake.tsts

commands:
	echo -e "Error Test: Unrecognized target and no radii given." \
	          > $(OUTPUT)/error.txt;
	if [[ ` \
	  $(APPNAME) clon=0.0 \
	             map=$(OUTPUT)/maptemplateTruth.txt \
	             targopt=user \
	             targetname=ObiWan  \
	             2>> $(OUTPUT)/error.txt \
	             > /dev/null` ]]; \
	then true; \
	fi;
