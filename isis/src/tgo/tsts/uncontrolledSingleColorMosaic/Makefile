# This tests the production of a CaSSIS uncontrolled single-color mosaic from ingestion to
# export to pds4. 
#
# @history 2017-02-13 Kristin Berry - Original version, many things borrowed from 
#                             singleFrameletProjection cat test. 
#          2018-06-16 Kristin Berry - Upated to use tgocassismos and tgocassisrdrgen.
#

INGEST = tgocassis2isis
SPICE = spiceinit
MAPFILE = mosrange
PROJECT = cam2map
MOSAIC = tgocassismos
EXPORT = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.xml > $(OUTPUT)/inputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/inputs.lis > $(OUTPUT)/relative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/relative.lis > $(OUTPUT)/basenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
            to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
           SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(MAPFILE) $(TSTARGS) fromlist=$(OUTPUT)/cubes.lis to=$(OUTPUT)/equi.map > /dev/null;
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_equi.cub \
             map=$(OUTPUT)/equi.map \
             pixres=mpp resolution=200 -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*equi.cub > $(OUTPUT)/mosaic.lis;
	$(MOSAIC) $(TSTSARGS) fromlist=$(OUTPUT)/mosaic.lis to=$(OUTPUT)/cassisMosaic.cub > /dev/null;

	$(EXPORT) $(TSTSARGS) from=$(OUTPUT)/cassisMosaic.cub to=$(OUTPUT)/cassisMosaic.img > /dev/null;
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
               $(OUTPUT)/cassisMosaic.xml \
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
               > $(OUTPUT)/cassisMosaic.xmlLabel.txt;
	catlab from=$(OUTPUT)/cassisMosaic.cub \
	       to=$(OUTPUT)/cassisMosaic.pvl > /dev/null;

	# Cleanup
	$(MV) $(OUTPUT)/equi.map $(OUTPUT)/equi.pvl
	$(RM) $(OUTPUT)/cassisMosaic.xml;
	$(RM) $(OUTPUT)/tempLabel1.txt;
	$(RM) $(OUTPUT)/tempLabel2.txt;
	$(RM) $(OUTPUT)/tempLabel3.txt;
	$(RM) $(OUTPUT)/tempLabel4.txt;
	$(RM) $(OUTPUT)/*.lis
	$(RM) $(OUTPUT)/*equi.cub;
	$(RM) $(OUTPUT)/*B1.cub;
	$(RM) $(OUTPUT)/*_tracking*;
