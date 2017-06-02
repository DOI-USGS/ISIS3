APPNAME = lromakeflat

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/pushframe.lis
	$(APPNAME) fromlist=$(OUTPUT)/pushframe.lis \
	  to=$(OUTPUT)/pushframe frameletheight=3\
	  imagetype=pushframe stdevtol=3 \
	  > /dev/null;
	rm $(OUTPUT)/pushframe.lis
