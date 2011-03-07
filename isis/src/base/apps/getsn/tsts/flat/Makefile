APPNAME = getsn

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) FROM=$(INPUT)/voy.cub \
  	  TO=$(OUTPUT)/output.txt \
	  FILE=TRUE \
	  OBSERVATION=TRUE \
	  FORMAT=FLAT > /dev/null;
	sed s:.*/isis:isis:g $(OUTPUT)/output.txt > $(OUTPUT)/output2.txt;
	rm -f $(OUTPUT)/output.txt > /dev/null;
