APPNAME = isis2ascii

include $(ISISROOT)/make/isismake.tsts
vesta64ascii.txt.SKIPLINES = 1


commands:
	$(APPNAME) from = $(INPUT)/isisTruth.cub \
	  to = $(OUTPUT)/isisTruthAsciiUser_temp.txt setpixelvalues = yes \
	  nullvalue = 0 lrsvalue = 0 lisvalue = 0 hisvalue = 255 \
	  hrsvalue = 255 > /dev/null;
	$(SED) 's+/.*/input/+input/+' $(OUTPUT)/isisTruthAsciiUser_temp.txt > $(OUTPUT)\
	/isisTruthAsciiUser.txt;
	$(RM) $(OUTPUT)/isisTruthAsciiUser_temp.txt;