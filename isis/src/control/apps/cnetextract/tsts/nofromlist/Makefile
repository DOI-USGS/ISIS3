# Test cnetextract with no FROMLIST given using NOIGNORE and MEASURELESS filters 
# NOTE -- since no FROMLIST is given, no TOLIST can be created
# The following will be extracted
#		--every non-ignored point with at least one measure
#		for each of these points the following measures will be extracted:
#			--every non-ignored measure
#			--every ignored reference measure
APPNAME = cnetextract

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) cnet=$(INPUT)/cnet.net \
	  prefix=$(OUTPUT)/out \
	  onet=$(OUTPUT)/newNet.net \
	  networkid=new \
	  description=new \
	  noignore=true \
	  nomeasureless=true \
	  > /dev/null;
