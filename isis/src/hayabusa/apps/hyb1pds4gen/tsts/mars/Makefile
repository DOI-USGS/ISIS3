APPNAME = hyb1pds4gen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/st_0515854093_p.cub \
		   to=$(OUTPUT)/st_0515854093_p.img \
		   pds4logical="urn:nasa:pds:mybundle:mycollection:myproduct" \
		   > /dev/null;

	# Remove parts of output that can occur in any order and convert to txt for comparison
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/st_0515854093_p.xml \
	       > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/tempLabel1.txt \
	       > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\<description>Created PDS4 output product from ISIS cube.*<+\<description>Created PDS4 output product from ISIS cube.<+' \
	       $(OUTPUT)/tempLabel2.txt \
	       > $(OUTPUT)/st_0515854093_p.xmlLabel.txt;

	$(RM) $(OUTPUT)/st_0515854093_p.xml > /dev/null; 
	$(RM) $(OUTPUT)/tempLabel1.txt > /dev/null; 
	$(RM) $(OUTPUT)/tempLabel2.txt > /dev/null; 

