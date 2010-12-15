APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) INPUTTYPE=cnets \
	FROM1=$(INPUT)/testNetGround.ctl \
	FROM2=$(INPUT)/testNetReferences.ctl \
	TO=$(OUTPUT)/testGround.pvl \
	REPORT=$(OUTPUT)/groundReport.txt \
	DUPLICATEPOINTS=merge \
	ID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
	$(APPNAME) INPUTTYPE=cnets \
	FROM1=$(INPUT)/testNetReferences.ctl \
	FROM2=$(INPUT)/testNetGround.ctl \
	TO=$(OUTPUT)/testReferences.pvl \
	REPORT=$(OUTPUT)/referenceReport.txt \
	DUPLICATEPOINTS=merge \
	OVERWRITEMEASURES=true \
	ID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
