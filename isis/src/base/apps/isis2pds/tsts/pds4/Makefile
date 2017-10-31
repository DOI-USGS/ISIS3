APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/dawnEqui1.cub to=$(OUTPUT)/dawnEqui.img pdsversion=pds4 > /dev/null;

	# Change the output xml file to a txt file for test comparison
	mv $(OUTPUT)/dawnEqui.xml $(OUTPUT)/dawnEqui.txt > /dev/null;

	$(RM) $(OUTPUT)/dawnEqui.img > /dev/null; 

