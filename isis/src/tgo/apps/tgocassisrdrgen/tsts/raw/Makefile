# Verify that we can export a non-projected and non-mosaiked cube as a TGO CaSSIS image.
# Test that we can override the product id with the PRODUCTID parameter.
#
# history 2018-05-18 Kaitlyn Lee - Changed input cube with one that has the
# 				ObservationId. Added the removal of the modification_date and ISIS version.
# 				Added testing for PRODUCTID parameter.

APPNAME = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) from=$(INPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
	           to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.img \
		   > /dev/null;

	# Reingest output and make sure the cubes are the same
	raw2isis from=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.img \
					 to=$(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.cub \
					 SAMPLES=2048 \
					 LINES=252 \
					 BITTYPE=REAL \
					 BYTEORDER=LSB \
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
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/templabel3.txt \
	       > $(OUTPUT)/templabel4.txt
	$(SED) 's+\ISIS version.*<+\ISIS version.<+' \
	       $(OUTPUT)/templabel4.txt \
	       > $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xmlLabel.txt;

	$(RM) $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1.xml \
	      $(OUTPUT)/templabel1.txt \
	      $(OUTPUT)/templabel2.txt \
	      $(OUTPUT)/templabel3.txt \
				$(OUTPUT)/templabel4.txt;

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
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/templabel3.txt \
	       > $(OUTPUT)/templabel4.txt
	$(SED) 's+\ISIS version.*<+\ISIS version.<+' \
	       $(OUTPUT)/templabel4.txt \
	       > $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1_modifiedId.xmlLabel.txt;

	$(RM) $(OUTPUT)/CAS-MCO-2016-11-22T15.45.50.984-BLU-03000-B1_modifiedId.xml \
	      $(OUTPUT)/templabel1.txt \
	      $(OUTPUT)/templabel2.txt \
	      $(OUTPUT)/templabel3.txt \
				$(OUTPUT)/templabel4.txt;
