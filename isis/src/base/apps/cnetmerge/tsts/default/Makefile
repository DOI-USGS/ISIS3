APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) INPUTTYPE=cnets \
	FROM1=$(INPUT)/testNet.ctl \
	FROM2=$(INPUT)/testNet2.ctl \
	TO=$(OUTPUT)/testReplace.pvl \
	DUPLICATEPOINTS=merge \
	ID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
