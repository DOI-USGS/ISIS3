APPNAME = cnetcombinept

include $(ISISROOT)/make/isismake.tsts
#cnetcombinept cnetbase=Extract_thmir_dups.net onet=out.net
commands:
	$(CP) $(INPUT)/*.net $(OUTPUT)
	$(APPNAME) cnetbase=$(INPUT)/Extract_thmir_dups.net \
	onet=$(OUTPUT)/testout_Extract_thmir_dups.net > /dev/null;
	cnetbin2pvl from=$(OUTPUT)/testout_Extract_thmir_dups.net \
				to=$(OUTPUT)/testout_Extract_thmir_dups.pvl \
				> /dev/null;

	$(RM) $(OUTPUT)/*.net;
