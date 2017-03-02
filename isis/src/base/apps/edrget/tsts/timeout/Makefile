APPNAME = edrget

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) URL=ftp://pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/f253s05.imq TOPATH=$(OUTPUT) TIMEOUT=6 >& $(OUTPUT)/temp.txt;
	$(SED) 's/.*Timeout/Timeout/' < $(OUTPUT)/temp.txt > $(OUTPUT)/temp1.txt;
	$(TAIL) '-2' < $(OUTPUT)/temp1.txt > $(OUTPUT)/ftperror.txt;
	$(APPNAME) URL=https://pdsimage2.wr.usgs.gov/cdroms/Viking_Orbiter/vo_1004/f253sxx/f253s05.imq TOPATH=$(OUTPUT) TIMEOUT=6 >& $(OUTPUT)/temp.txt;
	$(SED) 's/.*Timeout/Timeout/' < $(OUTPUT)/temp.txt > $(OUTPUT)/temp1.txt;
	$(TAIL) '-2' < $(OUTPUT)/temp1.txt > $(OUTPUT)/httperror.txt;
	$(RM) $(OUTPUT)/temp.txt;
	$(RM) $(OUTPUT)/temp1.txt;