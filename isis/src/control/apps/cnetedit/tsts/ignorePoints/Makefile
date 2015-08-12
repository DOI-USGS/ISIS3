APPNAME = cnetedit

include $(ISISROOT)/make/isismake.tsts

commands:
# original test
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	POINTLIST=$(INPUT)/points.lis \
	ONET=$(OUTPUT)/cnet.net > /dev/null;

# test IGNOREALL=true with ignored input measures / points
	$(APPNAME) CNET=$(INPUT)/cnet.net \
	POINTLIST=$(INPUT)/points.lis \
	IGNOREALL = true \
	ONET=$(OUTPUT)/cnet2.net > /dev/null;
