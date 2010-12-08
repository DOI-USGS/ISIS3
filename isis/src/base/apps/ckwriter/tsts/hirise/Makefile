APPNAME = ckwriter

include $(ISISROOT)/make/isismake.tsts

#  Only use 12 digits in the output of tabledump due to precision issues
commands:
	$(APPNAME) FROM=$(INPUT)/PSP_001446_1790_RED2_0.cub \
	TO=$(OUTPUT)/PSP_001446_1790_RED2_0.bc > /dev/null;
	$(CP) $(INPUT)/PSP_001446_1790_RED2_0.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/PSP_001446_1790_RED2_0.cub \
	  ck=$(OUTPUT)/PSP_001446_1790_RED2_0.bc > /dev/null;
	tabledump from=$(OUTPUT)/PSP_001446_1790_RED2_0.cub \
	  to=$(OUTPUT)/PSP_001446_1790_RED2_0.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/PSP_001446_1790_RED2_0.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/PSP_001446_1790_RED2_0.txt;
	$(RM) $(OUTPUT)/PSP_001446_1790_RED2_0.cub 
	$(RM) $(OUTPUT)/PSP_001446_1790_RED2_0.bc
	$(RM) $(OUTPUT)/PSP_001446_1790_RED2_0.dat


