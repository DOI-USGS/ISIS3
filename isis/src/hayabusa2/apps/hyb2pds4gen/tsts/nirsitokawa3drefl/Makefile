APPNAME = hyb1pds4gen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/2392975548_lvl3_0.refl.cub \
		   to=$(OUTPUT)/2392975548_lvl3_0.refl.img \
		   pds4logical="urn:jaxa:pds:mybundle:mycollection:myproduct" \
		   > /dev/null;

	# Remove parts of output that can occur in any order and convert to txt for comparison
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/2392975548_lvl3_0.refl.xml \
	       > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/tempLabel1.txt \
	       > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\<description>Created PDS4 output product from ISIS cube.*<+\<description>Created PDS4 output product from ISIS cube.<+' \
	       $(OUTPUT)/tempLabel2.txt \
	       > $(OUTPUT)/2392975548_lvl3_0.refl.xmlLabel.txt;

	$(RM) $(OUTPUT)/2392975548_lvl3_0.refl.xml > /dev/null; 
	$(RM) $(OUTPUT)/tempLabel1.txt > /dev/null; 
	$(RM) $(OUTPUT)/tempLabel2.txt > /dev/null; 

