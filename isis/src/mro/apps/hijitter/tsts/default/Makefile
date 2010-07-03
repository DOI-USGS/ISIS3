APPNAME = hijitter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(CP) $(INPUT)/*.cub $(OUTPUT)/;
	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/outlist.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis MASTER=5 \
	  JITTER=$(INPUT)/PSP_007556_2010.cropped.txt \
	  REGDEF=$(INPUT)/hijitreg.p0745.s1570.def TO=$(OUTPUT)/outlist.lis > /dev/null ;
	$(RM) $(OUTPUT)/files.lis $(OUTPUT)/outlist.lis > /dev/null;
