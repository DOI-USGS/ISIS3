.IGNORE:

APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.ctl > $(OUTPUT)/list.lis;

	$(APPNAME) INPUTTYPE=list \
	CLIST=$(OUTPUT)/list.lis \
	ONET=$(OUTPUT)/error.net \
	LOG=$(OUTPUT)/report.txt \
	NETWORKID=testErrorMerge DESCRIPTION=NewTextCtl \
	2> $(OUTPUT)/error1.txt > /dev/null;

	$(APPNAME) INPUTTYPE=list \
	CLIST=$(OUTPUT)/list.lis \
	ONET=$(OUTPUT)/nolog.net \
	NETWORKID=testErrorMergeNoLog DESCRIPTION=NewTextCtl \
	2> $(OUTPUT)/error2.txt > /dev/null;

	$(APPNAME) INPUTTYPE=cnets \
	CNET2=$(INPUT)/testNet1.ctl \
	ONET=$(OUTPUT)/nobase.net \
	NETWORKID=testErrorMergeNoBase DESCRIPTION=NewTextCtl \
	2> $(OUTPUT)/error3.txt > /dev/null;

	$(RM) $(OUTPUT)/list.lis;
