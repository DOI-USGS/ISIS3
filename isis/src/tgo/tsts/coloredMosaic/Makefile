# This tests the production of a CaSSIS uncontrolled multi-color mosaic from ingestion to
# export to pds4.
#
# @history 2018-02-22 Adam Goins - Original version, many things borrowed from
#                             uncontrolled single-color cat test.
#          2018-06-18 Kristin Berry - Updated to use defaultrange=map for the cam2map call so that
#                             cubeit will get input cubes all of the same size. 
#

INGEST = tgocassis2isis
SPICE = spiceinit
MAPFILE = mosrange
PROJECT = cam2map
MOSAIC = tgocassismos
EXPORT = isis2pds
GETKEY = getkey
CUBEIT = cubeit

include $(ISISROOT)/make/isismake.tsts

commands:
        # Ingest Red
	$(LS) $(INPUT)/*-RED-*.xml > $(OUTPUT)/redInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/redInputs.lis > $(OUTPUT)/redRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/redRelative.lis > $(OUTPUT)/redBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
            to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/redBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
            SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/redBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*-RED-*.cub > $(OUTPUT)/redCubes.lis;

	# Ingest Blu
	$(LS) $(INPUT)/*-BLU-*.xml > $(OUTPUT)/bluInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/bluInputs.lis > $(OUTPUT)/bluRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/bluRelative.lis > $(OUTPUT)/bluBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
            to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/bluBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
           SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/bluBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*-BLU-*.cub > $(OUTPUT)/bluCubes.lis;

	# Ingest Pan
	$(LS) $(INPUT)/*-PAN-*.xml > $(OUTPUT)/panInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/panInputs.lis > $(OUTPUT)/panRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/panRelative.lis > $(OUTPUT)/panBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
            to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/panBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
	    SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/panBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*-PAN-*.cub > $(OUTPUT)/panCubes.lis;

	# Ingest Nir
	$(LS) $(INPUT)/*-NIR-*.xml > $(OUTPUT)/nirInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/nirInputs.lis > $(OUTPUT)/nirRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/nirRelative.lis > $(OUTPUT)/nirBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
            to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/nirBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
            SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/nirBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*-NIR-*.cub > $(OUTPUT)/nirCubes.lis;

	# MAP FILE
	# Create list of all files.
	$(LS) $(INPUT)/*.xml > $(OUTPUT)/allInputs.lis;
	$(SED) 's/^.*input\//output\//g' $(OUTPUT)/allInputs.lis > $(OUTPUT)/allRelative.lis;
	$(SED) 's/\.xml/.cub/g' $(OUTPUT)/allRelative.lis > $(OUTPUT)/allCubes.lis;
	$(MAPFILE) $(TSTARGS) fromlist=$(OUTPUT)/allCubes.lis to=$(OUTPUT)/equi.map > /dev/null;

	# MOSAICS
	# Create Red Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_redEqui.cub \
            map=$(OUTPUT)/equi.map defaultrange=map \
            pixres=mpp resolution=200 -batchlist=$(OUTPUT)/redBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*redEqui.cub > $(OUTPUT)/redMosaic.lis;
	$(MOSAIC) $(TSTSARGS) fromlist=$(OUTPUT)/redMosaic.lis \
	    to=$(OUTPUT)/redCassisMosaic.cub > /dev/null;

        # Create Blu Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_bluEqui.cub \
             map=$(OUTPUT)/equi.map defaultrange=map \
             pixres=mpp resolution=200 -batchlist=$(OUTPUT)/bluBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*bluEqui.cub > $(OUTPUT)/bluMosaic.lis;
	$(MOSAIC) $(TSTSARGS) fromlist=$(OUTPUT)/bluMosaic.lis \
	    to=$(OUTPUT)/bluCassisMosaic.cub > /dev/null;

        # Create Pan Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_panEqui.cub \
            map=$(OUTPUT)/equi.map defaultrange=map \
            pixres=mpp resolution=200 -batchlist=$(OUTPUT)/panBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*panEqui.cub > $(OUTPUT)/panMosaic.lis;
	$(MOSAIC) $(TSTSARGS) fromlist=$(OUTPUT)/panMosaic.lis \
	   to=$(OUTPUT)/panCassisMosaic.cub > /dev/null;

        # Create Nir Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_nirEqui.cub \
             map=$(OUTPUT)/equi.map defaultrange=map \
             pixres=mpp resolution=200 -batchlist=$(OUTPUT)/nirBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*nirEqui.cub > $(OUTPUT)/nirMosaic.lis;
	$(MOSAIC) $(TSTSARGS) fromlist=$(OUTPUT)/nirMosaic.lis \
	    to=$(OUTPUT)/nirCassisMosaic.cub > /dev/null;

        # Create full color mos
	$(LS) $(OUTPUT)/*CassisMosaic.cub > $(OUTPUT)/mosaicList.lis;
	$(CUBEIT) fromlist=$(OUTPUT)/mosaicList.lis to=$(OUTPUT)/coloredMosaic.cub > /dev/null;
	$(EXPORT) $(TSTSARGS) from=$(OUTPUT)/coloredMosaic.cub to=$(OUTPUT)/coloredMosaic \
             pdsversion=pds4 > /dev/null;
	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
               $(OUTPUT)/coloredMosaic.xml \
	       > $(OUTPUT)/tempLabel1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	       $(OUTPUT)/tempLabel1.txt \
	       > $(OUTPUT)/tempLabel2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	       $(OUTPUT)/tempLabel2.txt \
	       > $(OUTPUT)/tempLabel3.txt;
	$(SED) 's+\modification_date.*>+\modification_date>+' \
	       $(OUTPUT)/tempLabel3.txt \
	       > $(OUTPUT)/tempLabel4.txt;
	$(SED) 's+\ISIS version.*<+\ISIS version.<+' \
	       $(OUTPUT)/tempLabel4.txt \
               > $(OUTPUT)/coloredMosaic.xmlLabel.txt;

	# Cleanup
	$(RM) $(OUTPUT)/coloredMosaic.xml;
	$(RM) $(OUTPUT)/tempLabel1.txt;
	$(RM) $(OUTPUT)/tempLabel2.txt;
	$(RM) $(OUTPUT)/tempLabel3.txt;
	$(RM) $(OUTPUT)/tempLabel4.txt;
	$(RM) $(OUTPUT)/*.lis;
	$(RM) $(OUTPUT)/CAS-MCO*.cub;
	$(RM) $(OUTPUT)/bluCassisMosaic.cub;
	$(RM) $(OUTPUT)/nirCassisMosaic.cub;
	$(RM) $(OUTPUT)/panCassisMosaic.cub;
	$(RM) $(OUTPUT)/redCassisMosaic.cub;
	$(RM) $(OUTPUT)/*_tracking*.cub;
	$(MV) $(OUTPUT)/equi.map $(OUTPUT)/equi.pvl;
