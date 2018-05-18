APPNAME = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
	           to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.img \
		   > /dev/null;

	# Reingest the PDS4 data
	tgocassis2isis from=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xml \
	         to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
		 > /dev/null;

	# Compare the output cube from raw2isis to the original input cube to
	# ensure that they are identical
	cubediff from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
	         from2=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
	         to=$(OUTPUT)/cubediffresults.txt \
		 > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xml \
	       > $(OUTPUT)/templabel1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	       $(OUTPUT)/templabel1.txt \
	       > $(OUTPUT)/templabel2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	       $(OUTPUT)/templabel2.txt \
	       > $(OUTPUT)/templabel3.txt;
	$(SED) 's+\Modification_Detail.*>+\Modification_Detail>+' \
	       $(OUTPUT)/templabel3.txt \
	       > $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xmlLabel.txt;


	$(RM) $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xml \
	      $(OUTPUT)/templabel1.txt \
	      $(OUTPUT)/templabel2.txt \
	      $(OUTPUT)/templabel3.txt;

	# Test PRODUCTID parameter with same input
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
	           to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1_modifiedId.img \
						 PRODUCTID=placeholderId \
		   > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	       $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1_modifiedId.xml \
	       > $(OUTPUT)/templabel1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	       $(OUTPUT)/templabel1.txt \
	       > $(OUTPUT)/templabel2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	       $(OUTPUT)/templabel2.txt \
	       > $(OUTPUT)/templabel3.txt;
	$(SED) 's+\Modification_Detail.*>+\Modification_Detail>+' \
	       $(OUTPUT)/templabel3.txt \
	       > $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1_modifiedId.xmlLabel.txt;


	$(RM) $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1_modifiedId.xml \
	      $(OUTPUT)/templabel1.txt \
	      $(OUTPUT)/templabel2.txt \
	      $(OUTPUT)/templabel3.txt;
