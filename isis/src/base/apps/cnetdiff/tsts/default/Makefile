APPNAME = cnetdiff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/cnet.net from2=$(INPUT)/cnet.net \
	  to=$(OUTPUT)/compareSame1.txt > /dev/null;
	$(APPNAME) from=$(INPUT)/cnet2.net from2=$(INPUT)/cnet2.net \
	  to=$(OUTPUT)/compareSame2.txt > /dev/null;
	$(APPNAME) from=$(INPUT)/cnet.net from2=$(INPUT)/cnet2.net \
	  to=$(OUTPUT)/compareDifferent.txt > /dev/null;
	$(APPNAME) from=$(INPUT)/cnet.net from2=$(INPUT)/cnet2.net \
	  to=$(OUTPUT)/compareDifferent2.txt DIFF=$(INPUT)/cnet.diff > /dev/null;
