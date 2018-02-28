# This tests the production of a CaSSIS uncontrolled multi-color mosaic from ingestion to
# export to pds4.
#
# @history 2018-02-22 Adam Goins - Original version, many things borrowed from
#                             uncontrolled single-color cat test.
#

INGEST = tgocassis2isis
SPICE = spiceinit
MAPFILE = mosrange
PROJECT = cam2map
MOSAIC = automos
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

	# Ingest Blu
	$(LS) $(INPUT)/*-BLU-*.xml > $(OUTPUT)/bluInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/bluInputs.lis > $(OUTPUT)/bluRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/bluRelative.lis > $(OUTPUT)/bluBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
  	to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/bluBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
  	SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/bluBasenames.lis > /dev/null;

	# Ingest Pan
	$(LS) $(INPUT)/*-PAN-*.xml > $(OUTPUT)/panInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/panInputs.lis > $(OUTPUT)/panRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/panRelative.lis > $(OUTPUT)/panBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
		to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/panBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
		SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/panBasenames.lis > /dev/null;

	# Ingest Nir
	$(LS) $(INPUT)/*-NIR-*.xml > $(OUTPUT)/nirInputs.lis;
	$(SED) 's/^.*input\///g' $(OUTPUT)/nirInputs.lis > $(OUTPUT)/nirRelative.lis;
	$(SED) 's/\.xml//g' $(OUTPUT)/nirRelative.lis > $(OUTPUT)/nirBasenames.lis;
	$(INGEST) $(TSTARGS) from=$(INPUT)/\$$\1.xml \
  	to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/nirBasenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
  	SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/nirBasenames.lis > /dev/null;

	# MAP FILE
	# Create list of all files.
	$(LS) $(INPUT)/*.xml > $(OUTPUT)/allInputs.lis;
	$(SED) 's/^.*input\//output\//g' $(OUTPUT)/allInputs.lis > $(OUTPUT)/allRelative.lis;
	$(SED) 's/\.xml/.cub/g' $(OUTPUT)/allRelative.lis > $(OUTPUT)/allCubes.lis;
	$(MAPFILE) $(TSTARGS) fromlist=$(OUTPUT)/allCubes.lis to=$(OUTPUT)/equi.map > /dev/null;

	# MOSAICS
	# Create Red Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_redEqui.cub \
		map=$(OUTPUT)/equi.map \
  	pixres=mpp resolution=200 -batchlist=$(OUTPUT)/redBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*redEqui.cub > $(OUTPUT)/redMosaic.lis;
	$(MOSAIC) $(TSTSARGS) \
  	fromlist=$(OUTPUT)/redMosaic.lis \
  	mosaic=$(OUTPUT)/redCassisMosaic.cub \
  	grange='USER' \
  	MAXLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLatitude grpname=Mapping`  \
  	MINLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLatitude grpname=Mapping`  \
  	MAXLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLongitude grpname=Mapping` \
  	MINLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLongitude grpname=Mapping` > /dev/null;

	# Create Blu Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_bluEqui.cub \
		map=$(OUTPUT)/equi.map \
  	pixres=mpp resolution=200 -batchlist=$(OUTPUT)/bluBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*bluEqui.cub > $(OUTPUT)/bluMosaic.lis;
	$(MOSAIC) $(TSTSARGS) \
  	fromlist=$(OUTPUT)/bluMosaic.lis \
  	mosaic=$(OUTPUT)/bluCassisMosaic.cub \
  	grange='USER' \
  	MAXLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLatitude grpname=Mapping`  \
  	MINLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLatitude grpname=Mapping`  \
  	MAXLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLongitude grpname=Mapping` \
  	MINLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLongitude grpname=Mapping` > /dev/null;

	# Create Pan Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_panEqui.cub \
  	map=$(OUTPUT)/equi.map \
  	pixres=mpp resolution=200 -batchlist=$(OUTPUT)/panBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*panEqui.cub > $(OUTPUT)/panMosaic.lis;
	$(MOSAIC) $(TSTSARGS) \
  	fromlist=$(OUTPUT)/panMosaic.lis \
   	mosaic=$(OUTPUT)/panCassisMosaic.cub \
   	grange='USER' \
   	MAXLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLatitude grpname=Mapping`  \
   	MINLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLatitude grpname=Mapping`  \
   	MAXLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLongitude grpname=Mapping` \
   	MINLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLongitude grpname=Mapping` > /dev/null;

	# Create Nir Mosaic
	$(PROJECT) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_nirEqui.cub \
  	map=$(OUTPUT)/equi.map \
  	pixres=mpp resolution=200 -batchlist=$(OUTPUT)/nirBasenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*nirEqui.cub > $(OUTPUT)/nirMosaic.lis;
	$(MOSAIC) $(TSTSARGS) \
  	fromlist=$(OUTPUT)/nirMosaic.lis \
  	mosaic=$(OUTPUT)/nirCassisMosaic.cub \
  	grange='USER' \
  	MAXLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLatitude grpname=Mapping`  \
  	MINLAT = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLatitude grpname=Mapping`  \
  	MAXLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MaximumLongitude grpname=Mapping` \
  	MINLON = `$(GETKEY) from=$(OUTPUT)/equi.map keyword=MinimumLongitude grpname=Mapping` > /dev/null;

	# Create full color mos
	$(LS) $(OUTPUT)/*CassisMosaic.cub > $(OUTPUT)/mosaicList.txt;
	$(CUBEIT) fromlist=$(OUTPUT)/mosaicList.txt to=$(OUTPUT)/coloredMosaic.cub > /dev/null;
	$(EXPORT) $(TSTSARGS) from=$(OUTPUT)/coloredMosaic.cub to=$(OUTPUT)/coloredMosaic \
		pdsversion=pds4 > /dev/null;

	$(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
              $(OUTPUT)/coloredMosaic.xml > $(OUTPUT)/coloredMosaic1.txt;
	$(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
             $(OUTPUT)/coloredMosaic1.txt > $(OUTPUT)/coloredMosaic2.txt;
	$(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
             $(OUTPUT)/coloredMosaic2.txt > $(OUTPUT)/coloredMosaic3.txt;
	$(SED) 's+\Modification_Detail.*>+\Modification_Detail>+' \
             $(OUTPUT)/coloredMosaic3.txt > $(OUTPUT)/coloredMosaic.txt;

	# Cleanup
	$(RM) $(OUTPUT)/*B1*cub;
	$(RM) $(OUTPUT)/*.lis;
	$(RM) $(OUTPUT)/bluCassisMosaic.cub;
	$(RM) $(OUTPUT)/nirCassisMosaic.cub;
	$(RM) $(OUTPUT)/panCassisMosaic.cub;
	$(RM) $(OUTPUT)/redCassisMosaic.cub;
	$(RM) $(OUTPUT)/*coloredMosaic1.txt;
	$(RM) $(OUTPUT)/*coloredMosaic2.txt;
	$(RM) $(OUTPUT)/*coloredMosaic3.txt;
	$(RM) $(OUTPUT)/*Cubes.lis
	$(RM) $(OUTPUT)/*Inputs.lis
	$(RM) $(OUTPUT)/*Basenames.lis
	$(RM) $(OUTPUT)/*Relative.lis
	$(RM) $(OUTPUT)/*Mosaic.lis
	$(MV) $(OUTPUT)/equi.map $(OUTPUT)/equi.pvl
