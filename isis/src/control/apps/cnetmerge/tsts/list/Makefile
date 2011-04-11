APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.ctl > $(OUTPUT)/list.lis;
	$(APPNAME) INPUTTYPE=list \
	CLIST=$(OUTPUT)/list.lis \
	ONET=$(OUTPUT)/testReplace.net \
	DUPLICATEPOINTS=merge \
	LOG=$(OUTPUT)/report.txt \
	NETWORKID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
	$(RM) $(OUTPUT)/list.lis;
