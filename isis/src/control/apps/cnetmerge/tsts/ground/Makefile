APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) INPUTTYPE=cnets \
	BASE=$(INPUT)/testNetGround.ctl \
	CNET2=$(INPUT)/testNetReferences.ctl \
	ONET=$(OUTPUT)/testGround.net \
	LOG=$(OUTPUT)/groundReport.txt \
	DUPLICATEPOINTS=merge \
	NETWORKID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
	$(APPNAME) INPUTTYPE=cnets \
	BASE=$(INPUT)/testNetReferences.ctl \
	CNET2=$(INPUT)/testNetGround.ctl \
	ONET=$(OUTPUT)/testReferences.net \
	LOG=$(OUTPUT)/referenceReport.txt \
	DUPLICATEPOINTS=merge \
	OVERWRITEMEASURES=true \
	NETWORKID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
