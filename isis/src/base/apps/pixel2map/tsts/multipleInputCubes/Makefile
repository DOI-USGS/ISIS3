APPNAME = pixel2map

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(APPNAME) fromtype=fromlist fromlist= $(OUTPUT)/files.lis \
        to=$(OUTPUT)/combinedCubes.sinu.cub > /dev/null;
	$(RM) $(OUTPUT)/files.lis;
