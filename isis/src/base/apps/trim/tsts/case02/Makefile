.IGNORE:

APPNAME = trim

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/isisTruth.cub \
	  to=$(OUTPUT)/trimTruth2.cub \
	  top=0 \
	  bottom=0 \
	  left=0 \
	  right=0 > $(OUTPUT)/error.txt 2>&1;
