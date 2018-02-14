# This tests the production of a CaSSIS uncontrolled single-color mosaic from ingestion to
# export to pds4. 
#
# @history 2017-02-13 Kristin Berry - Original version, many things borrowed from 
#                             singleFrameletProjection cat test. 
#

INGEST = tgocassis2isis
SPICE = spiceinit
MAPFILE = mosrange
PROJECT = cam2map
MOSAIC = automos
EXPORT = isis2pds

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
	$(MOSAIC) $(TSTSARGS) fromlist=$(OUTPUT)/mosaic.lis mosaic=$(OUTPUT)/cassisMosaic.cub > /dev/null;
	$(EXPORT) $(TSTSARGS) from=$(OUTPUT)/cassisMosaic.cub to=$(OUTPUT)/cassisMosaic \
             pdsversion=pds4 > /dev/null;
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
              $(OUTPUT)/cassisMosaic.xml > $(OUTPUT)/cassisMosaic1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
             $(OUTPUT)/cassisMosaic1.txt > $(OUTPUT)/cassisMosaic2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
             $(OUTPUT)/cassisMosaic2.txt > $(OUTPUT)/cassisMosaic3.txt;
	$(SED) 's+\Modification_Detail.*>+\Modification_Detail>+' \
             $(OUTPUT)/cassisMosaic3.txt > $(OUTPUT)/cassisMosaic.txt;

	# Cleanup
	$(RM) $(OUTPUT)/cassisMosaic.xml;
	$(RM) $(OUTPUT)/cassisMosaic1.txt;
	$(RM) $(OUTPUT)/cassisMosaic2.txt;
	$(RM) $(OUTPUT)/cassisMosaic3.txt;
	$(MV) $(OUTPUT)/equi.map $(OUTPUT)/equi.pvl
	$(RM) $(OUTPUT)/cubes.lis
	$(RM) $(OUTPUT)/inputs.lis
	$(RM) $(OUTPUT)/basenames.lis
	$(RM) $(OUTPUT)/relative.lis
	$(RM) $(OUTPUT)/mosaic.lis

