INGEST = tgocassis2isis
SPICE = spiceinit
MAP = mosrange
PROJECT = cam2map
EXPORT = tgocassisrdrgen

include $(ISISROOT)/make/isismake.tsts

commands:
	$(LS) $(INPUT)/*.xml > $(OUTPUT)/basenames.lis;
	$(SED) -i 's/input\///g' $(OUTPUT)/basenames.lis > /dev/null;
	$(SED) -i 's/\.xml//g' $(OUTPUT)/basenames.lis > /dev/null;
	$(INGEST) $(TSTARGS) from=$(INPUT)/$\$$\1.xml \
            to=$(OUTPUT)/$\$$\1.cub -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	$(SPICE) $(TSTARGS) from=$(OUTPUT)/$\$$\1.cub \
           SPKPREDICTED=true CKPREDICTED=true -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	$(LS) $(OUTPUT)/*.cub > $(OUTPUT)/cubes.lis;
	$(MAP) fromlist=$(OUTPUT)/cubes.lis to=$(OUTPUT)/equi.map	> /dev/null;
	$(PROJECT) from=$(OUTPUT)/$\$$\1.cub to=$(OUTPUT)/$\$$\1_equi.cub \
             map=$(OUTPUT)/equi.map \
             pixres=mpp resolution=200 -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	$(EXPORT)
