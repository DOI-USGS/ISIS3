INGEST = tgocassis2isis
SPICE = spiceinit
RDRGEN = tgocassisrdrgen
CDIFF = cubediff

include $(ISISROOT)/make/isismake.tsts

commands:
	$(INGEST) $(TSTARGS) \
	    from=$(INPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub > /dev/null;

	$(RDRGEN) $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.img > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
               $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
	       > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	       $(OUTPUT)/tempLabel1.txt \
	       > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	       $(OUTPUT)/tempLabel2.txt \
	       > $(OUTPUT)/tempLabel3.txt;
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/tempLabel3.txt \
	       > $(OUTPUT)/tempLabel4.txt
	$(SED) 's+\ISIS version.*<+\ISIS version.<+' \
	       $(OUTPUT)/tempLabel4.txt \
               > $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xmlLabel.txt;

	$(INGEST) $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.cub > /dev/null;

	$(CDIFF) $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    from2=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.cub > /dev/null;

	catlab $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.pvl > /dev/null;

	catlab $(TSTARGS) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.reingested.pvl > /dev/null;

	$(RM) $(OUTPUT)/tempLabel1.txt;
	$(RM) $(OUTPUT)/tempLabel2.txt;
	$(RM) $(OUTPUT)/tempLabel3.txt;
	$(RM) $(OUTPUT)/tempLabel4.txt;
	$(RM) $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml;
