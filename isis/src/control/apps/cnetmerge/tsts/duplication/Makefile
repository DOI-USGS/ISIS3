APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) INPUTTYPE=cnets \
	CNET=$(INPUT)/testNet.ctl \
	CNET2=$(INPUT)/testNet2.ctl \
	ONET=$(OUTPUT)/testReplace.net \
	DUPLICATEPOINTS=merge \
	LOG=$(OUTPUT)/report.txt \
	NETWORKID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
