APPNAME = hyb1pds4gen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/corrected_iof_st_2459265790_w.cub \
	           to=$(OUTPUT)/corrected_iof_st_2459265790_w.img \
	           pds4logical="urn:nasa:pds:mybundle:mycollection:myproduct" > /dev/null;
	# Remove parts of output that can occur in any order and convert to txt for comparison
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/corrected_iof_st_2459265790_w.xml \
	       > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/tempLabel1.txt \
	       > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\<description>Created PDS4 output product from ISIS cube.*<+\<description>Created PDS4 output product from ISIS cube.<+' \
	       $(OUTPUT)/tempLabel2.txt \
	       > $(OUTPUT)/corrected_iof_st_2459265790_w.xmlLabel.txt;

	$(RM) $(OUTPUT)/corrected_iof_st_2459265790_w.xml;# > /dev/null; 
	$(RM) $(OUTPUT)/tempLabel1.txt;# > /dev/null; 
	$(RM) $(OUTPUT)/tempLabel2.txt;# > /dev/null; 

