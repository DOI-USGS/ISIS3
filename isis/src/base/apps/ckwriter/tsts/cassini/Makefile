APPNAME = ckwriter

include $(ISISROOT)/make/isismake.tsts

#  Only use 12 digits in the output of tabledump due to precision issues
commands:
	$(APPNAME) FROM=$(INPUT)/N1487301032_2.cub \
	TO=$(OUTPUT)/N1487301032_2.bc > /dev/null;
	$(CP) $(INPUT)/N1487301032_2.cub $(OUTPUT)
	spiceinit from=$(OUTPUT)/N1487301032_2.cub \
	  ck=$(OUTPUT)/N1487301032_2.bc > /dev/null;
	tabledump from=$(OUTPUT)/N1487301032_2.cub \
	  to=$(OUTPUT)/N1487301032_2.dat \
	  name=InstrumentPointing > /dev/null;
	cat $(OUTPUT)/N1487301032_2.dat \
	  | sed 's/\([0-9][0-9]*\.[0-9]\{12\}\)\([0-9][0-9]*\)/\1/g' \
	  > $(OUTPUT)/N1487301032_2.txt;
	$(RM) $(OUTPUT)/N1487301032_2.cub 
	$(RM) $(OUTPUT)/N1487301032_2.bc
	$(RM) $(OUTPUT)/N1487301032_2.dat


