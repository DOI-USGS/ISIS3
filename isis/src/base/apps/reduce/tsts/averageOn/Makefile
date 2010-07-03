APPNAME = reduce

labels3.txt.IGNORELINES = ByteOrder StartByte Bytes
labels6.txt.IGNORELINES = ByteOrder StartByte Bytes

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from= $(INPUT)/peaks.cub \
	  to= $(OUTPUT)/reduceTruth3.cub \
	  algorithm=average \
	  mode=total \
	  ons=100 \
	  onl=100 > /dev/null;
	catlab from=$(OUTPUT)/reduceTruth3.cub > $(OUTPUT)/labels3.txt;
	$(APPNAME) from= $(OUTPUT)/reduceTruth3.cub \
          to= $(OUTPUT)/reduceTruth6.cub \
          algorithm=average \
	  mode=total \
	  ons=50 \
          onl=50 > /dev/null;
	catlab from=$(OUTPUT)/reduceTruth6.cub > $(OUTPUT)/labels6.txt;
