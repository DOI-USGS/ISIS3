APPNAME = lromakeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/linescan.lis
	$(APPNAME) fromlist=$(OUTPUT)/linescan.lis \
	  to=$(OUTPUT)/linescan numlines=1\
	  imagetype=linescan stdevtol=3 \
	  > /dev/null;
	rm $(OUTPUT)/linescan.lis
