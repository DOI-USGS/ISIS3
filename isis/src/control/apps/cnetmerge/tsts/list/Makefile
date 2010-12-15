APPNAME = cnetmerge

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.ctl > $(OUTPUT)/list.lis;
	$(APPNAME) INPUTTYPE=list \
	FROMLIST=$(OUTPUT)/list.lis \
	TO=$(OUTPUT)/testReplace.pvl \
	DUPLICATEPOINTS=merge \
	REPORT=$(OUTPUT)/report.txt \
	ID=testReplaceMerge DESCRIPTION=NewTextCtl \
	> /dev/null;
	$(RM) $(OUTPUT)/list.lis;
