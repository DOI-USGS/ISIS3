APPNAME = camstats

include $(ISISROOT)/make/isismake.tsts
 
commands:
	$(APPNAME) FROM=$(INPUT)/input.cub TO=$(OUTPUT)/output.pvl > /dev/null;
	$(CP) $(INPUT)/input.cub $(OUTPUT)/attached.cub;
	$(APPNAME) FROM=$(OUTPUT)/attached.cub ATTACH=true > /dev/null;
