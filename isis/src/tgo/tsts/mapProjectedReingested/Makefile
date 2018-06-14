# This tests the workflow of ingesting raw CaSSIS data, spiceiniting, projecting, exporting and
# reingesting. 
#
# @history 2018-05-17 Summer Stapleton - Original version, many things borrowed from 
#                             singleFrameletProjection cat test. 
#

INGEST = tgocassis2isis
SPICE = spiceinit
MAP = mosrange
PROJECT = cam2map
EXPORT = tgocassisrdrgen
CATLAB = catlab

include $(ISISROOT)/make/isismake.tsts

commands:
	$(INGEST) $(TSTARGS)  \
	    from=$(INPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.xml \
            to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub > /dev/null;

	$(SPICE) $(TSTARGS)  \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
            SPKPREDICTED=true \
	    CKPREDICTED=true > /dev/null;

	$(PROJECT) $(TSTARGS)  \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.cub \
            map=$(INPUT)/equi.map \
            pixres=mpp \
	    resolution=200 > /dev/null;

	$(EXPORT) $(TSTARGS)  \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.cub \
            to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.img > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
               $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.xml \
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
               > $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.xmlLabel.txt;

	$(INGEST) $(TSTARGS)  \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.xml \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.reingested.cub > /dev/null;

	$(CATLAB) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.pvl > /dev/null;	

	$(CATLAB) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.pvl > /dev/null;	

	$(CATLAB) \
	    from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.reingested.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.reingested.pvl > /dev/null;	

	$(RM) $(OUTPUT)/CAS-M01-2018-05-05T23.11.48.767-RED-01029-B1.equi.xml;
	$(RM) $(OUTPUT)/tempLabel1.txt;
	$(RM) $(OUTPUT)/tempLabel2.txt;
	$(RM) $(OUTPUT)/tempLabel3.txt;
	$(RM) $(OUTPUT)/tempLabel4.txt;

