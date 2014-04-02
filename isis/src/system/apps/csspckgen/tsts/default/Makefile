APPNAME = csspckgen

output.db.txt.IGNORELINES = RunTime LeapsecondKernel

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/output.db.txt > /dev/null;
	$(SED) "s#\(File = .cassini/kernels/pck/cpck\)[0-9][0-9][A-Z][a-z][a-z][0-9][0-9][0-9][0-9]\(.tpc\)#\1_DateRemovedForTest\2#" \
	    $(OUTPUT)/output.db.txt > $(OUTPUT)/tmp.txt;
	$(MV) $(OUTPUT)/tmp.txt $(OUTPUT)/output.db.txt;
