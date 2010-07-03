APPNAME = equalizer

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) fromlist=$(INPUT)/FromList.lst \
	  holdlist=$(INPUT)/HoldList.lst \
	  outstats=$(OUTPUT)/outstats.pvl \
	  process=calculate \
	  > /dev/null;
