APPNAME = hijitter

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(CP) $(INPUT)/*.cub $(OUTPUT)/;
	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/outlist.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis MASTER=5 \
	  JITTER=$(INPUT)/PSP_007556_2010.cropped.txt \
	  JITTERCK=$(OUTPUT)/jitter.bc \
	  REGDEF=$(INPUT)/hijitreg.p0745.s1570.def TO=$(OUTPUT)/outlist.lis > /dev/null;
	$(CP) $(INPUT)/PSP_007556_2010_RED5.balance.cropped.cub $(OUTPUT)/red5.cub > /dev/null;
	spiceinit from=$(OUTPUT)/red5.cub ck=$(OUTPUT)/jitter.bc > /dev/null;
	tabledump from=$(OUTPUT)/red5.cub \
	  to=$(OUTPUT)/cktable.txt name=InstrumentPointing > /dev/null;
	$(RM) $(OUTPUT)/files.lis $(OUTPUT)/outlist.lis > /dev/null;
	$(RM) $(OUTPUT)/jitter.bc $(OUTPUT)/red5.cub > /dev/null;
