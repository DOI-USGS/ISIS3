APPNAME = automos

include $(ISISROOT)/make/isismake.tsts

commands:
	ls $(INPUT)/*.cub > $(OUTPUT)/files.lis;                     \
	$(APPNAME) from=$(OUTPUT)/files.lis mosaic=$(OUTPUT)/out.cub \
	  > /dev/null;                                               \
	$(RM) $(OUTPUT)/files.lis;
