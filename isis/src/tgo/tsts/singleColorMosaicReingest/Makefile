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
	$(LS) $(INPUT)/*.xml > $(OUTPUT)/inputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/inputs.lis > $(OUTPUT)/relative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/relative.lis > $(OUTPUT)/basenames.lis;

	$(INGEST) $(TSTARGS) from=$(INPUT)/$\$$\1.xml \
	    to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/basenames.lis > /dev/null;

	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
           SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/basenames.lis > /dev/null;

	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(MAP) $(TSTARGS) fromlist=$(OUTPUT)/cubes.lis to=$(OUTPUT)/equi.map	> /dev/null;

	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_equi.cub \
	    map=$(OUTPUT)/equi.map \
	    pixres=mpp resolution=200 -batchlist=$(OUTPUT)/basenames.lis > /dev/null;

	$(LS) $(OUTPUT)/*equi.cub > $(OUTPUT)/equi.lis;
	tgocassismos \
	    fromlist=$(OUTPUT)/equi.lis \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.cub > /dev/null;

	$(EXPORT) $(TSTARGS) from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.img > /dev/null;

#	$(INGEST) $(TSTARGS) from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.xml \
#	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.reingested.cub > /dev/null;
#
	$(CATLAB) from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.cub \
	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.pvl > /dev/null;
#	$(CATLAB) from=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.reingested.cub \
#	    to=$(OUTPUT)/CAS-M01-2018-05-05T23.11.mosaic.reingested.pvl > /dev/null;
#
	for label in `ls $(OUTPUT)/*.xml`; do \
	  $(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	    $$label > $${label%.xml}1.txt; \
	  $(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	    $${label%.xml}1.txt > $${label%.xml}2.txt; \
	  $(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	    $${label%.xml}2.txt > $${label%.xml}3.txt; \
	  $(SED) 's+\modification_date.*>+\modification_date>+' \
	    $${label%.xml}3.txt > $${label%.xml}4.txt; \
	  $(SED) 's+\ISIS version.*<+\ISIS version.<+' \
	    $${label%.xml}4.txt > $${label%.xml}.xmlLabel.txt; \
	  $(RM) $$label; \
	  $(RM) $${label%.xml}1.txt; \
	  $(RM) $${label%.xml}2.txt; \
	  $(RM) $${label%.xml}3.txt; \
	  $(RM) $${label%.xml}4.txt; \
	done

	$(RM) $(OUTPUT)/*.lis
	$(RM) $(OUTPUT)/*equi*
	$(RM) $(OUTPUT)/*B1.cub
	$(RM) $(OUTPUT)/*_tracking*
