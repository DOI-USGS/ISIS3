APPNAME = lromakeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/framing.lis
	$(APPNAME) fromlist=$(OUTPUT)/framing.lis \
	  to=$(OUTPUT)/framing \
	  imagetype=framing stdevtol=3 \
	  > /dev/null;
	rm $(OUTPUT)/framing.lis
