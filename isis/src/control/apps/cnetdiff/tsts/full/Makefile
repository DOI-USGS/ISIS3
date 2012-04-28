APPNAME = cnetdiff

include $(ISISROOT)/make/isismake.tsts

commands:
	cnetpvl2bin from=$(INPUT)/cnet.pvl to=$(OUTPUT)/cnet.net > /dev/null;
	cnetpvl2bin from=$(INPUT)/cnet2.pvl to=$(OUTPUT)/cnet2.net > /dev/null;
	
	$(APPNAME) from=$(OUTPUT)/cnet.net from2=$(OUTPUT)/cnet.net \
	  to=$(OUTPUT)/compareSame1.txt report=full > /dev/null;
	$(APPNAME) from=$(OUTPUT)/cnet2.net from2=$(OUTPUT)/cnet2.net \
	  to=$(OUTPUT)/compareSame2.txt report=full > /dev/null;
	$(APPNAME) from=$(OUTPUT)/cnet.net from2=$(OUTPUT)/cnet2.net \
	  to=$(OUTPUT)/compareDifferent.txt report=full > /dev/null;
	$(APPNAME) from=$(OUTPUT)/cnet.net from2=$(OUTPUT)/cnet2.net \
	  to=$(OUTPUT)/compareDifferent2.txt DIFF=$(INPUT)/cnet.diff \
	  report=full > /dev/null;
	
	rm $(OUTPUT)/cnet.net > /dev/null;
	rm $(OUTPUT)/cnet2.net > /dev/null;
