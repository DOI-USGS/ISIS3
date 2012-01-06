APPNAME = hijitter

PSP_007556_2010_RED4.balance.cropped.cub.TOLERANCE = .00074
PSP_007556_2010_RED5.balance.cropped.cub.TOLERANCE = .00074

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.cub > $(OUTPUT)/files.lis;
	$(CP) $(INPUT)/*.cub $(OUTPUT)/;
	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/outlist.lis;
	$(APPNAME) FROMLIST=$(OUTPUT)/files.lis MASTER=5 \
	  JITTER=$(INPUT)/PSP_007556_2010.cropped.txt \
	  JITTERCK=$(OUTPUT)/jitter.bc \
	  REGDEF=$(INPUT)/hijitreg.p0745.s1570.def crop=true TO=$(OUTPUT)/outlist.lis > /dev/null;
	$(CP) $(INPUT)/PSP_007556_2010_RED5.balance.cropped.cub $(OUTPUT)/red5.cub > /dev/null;
	spiceinit from=$(OUTPUT)/red5.cub ck=$(OUTPUT)/jitter.bc > /dev/null;
	tabledump from=$(OUTPUT)/red5.cub \
	  to=$(OUTPUT)/cktable.csv name=InstrumentPointing > /dev/null;
	stats from=output/PSP_007556_2010_RED4.balance.cropped.cub to=$(OUTPUT)/red4.stats.pvl > /dev/null;
	stats from=output/PSP_007556_2010_RED5.balance.cropped.cub to=$(OUTPUT)/red5.stats.pvl > /dev/null;
	stats from=output/PSP_007556_2010_RED6.balance.cropped.cub to=$(OUTPUT)/red6.stats.pvl > /dev/null;
	$(RM) $(OUTPUT)/files.lis $(OUTPUT)/outlist.lis > /dev/null;
	$(RM) $(OUTPUT)/jitter.bc $(OUTPUT)/red5.cub print.prt > /dev/null;
#	$(RM) $(OUTPUT)/PSP_007556_2010_RED4.balance.cropped.cub > /dev/null;
#	$(RM) $(OUTPUT)/PSP_007556_2010_RED5.balance.cropped.cub > /dev/null;
# Red 6 has special pixel differences so use stats to compare instead for now since ignorespecial option is gone.
	$(RM) $(OUTPUT)/PSP_007556_2010_RED6.balance.cropped.cub > /dev/null;
#PSP_007556_2010_RED4.balance.cropped.cub.TOLERANCE = .001
#PSP_007556_2010_RED5.balance.cropped.cub.TOLERANCE = .001
#PSP_007556_2010_RED6.balance.cropped.cub.TOLERANCE = .001
#PSP_007556_2010_RED4.balance.cropped.cub.IGNORESPECIAL = yes
#PSP_007556_2010_RED5.balance.cropped.cub.IGNORESPECIAL = yes
#PSP_007556_2010_RED6.balance.cropped.cub.IGNORESPECIAL = yes
