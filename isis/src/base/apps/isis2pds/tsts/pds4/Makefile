APPNAME = isis2pds

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/dawnEqui1.cub to=$(OUTPUT)/dawnEqui.img pdsversion=pds4 > /dev/null;

	# Remove parts of output that can occur in any order and convert to txt for comparison
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/dawnEqui.xml > $(OUTPUT)/dawnEqui.txt;

	$(RM) $(OUTPUT)/dawnEqui.xml > /dev/null; 
	$(RM) $(OUTPUT)/dawnEqui.img > /dev/null; 

