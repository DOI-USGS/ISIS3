INGEST = tgocassis2isis
SPICE = spiceinit
MAP = mosrange
PROJECT = cam2map
EXPORT = tgocassisrdrgen

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
	$(EXPORT) $(TSTARGS) from=$(OUTPUT)/$\$$\1_equi.cub \
	    to=$(OUTPUT)/$\$$\1_equi.img -batchlist=$(OUTPUT)/basenames.lis > /dev/null;
	for label in `ls $(OUTPUT)/*.xml`; do \
	  $(SED) 's+\Product_Observational.*>+\Product_Observational>+' \
	    $$label > $${label%.xml}1.txt; \
	  $(SED) 's+\description.*>+\description>+' \
	    $${label%.xml}1.txt > $${label%.xml}2.txt; \
	  $(SED) 's+\modification_date.*>+\modification_date>+' \
	    $${label%.xml}2.txt > $${label%.xml}3.txt; \
	  $(SED) 's+\FSW_HEADER.*>+\FSW_HEADER>+' \
	    $${label%.xml}3.txt > $${label%.xml}4.txt; \
	  $(SED) 's+\PEHK_HEADER.*>+\PEHK_HEADER>+' \
	    $${label%.xml}4.txt > $${label%.xml}5.txt; \
	  $(SED) 's+\Modification_Detail.*>+\Modification_Detail>+' \
	    $${label%.xml}5.txt > $${label%.xml}.txt; \
	  $(RM) $$label; \
	  $(RM) $${label%.xml}1.txt; \
	  $(RM) $${label%.xml}2.txt; \
	  $(RM) $${label%.xml}3.txt; \
 	  $(RM) $${label%.xml}4.txt; \
  	  $(RM) $${label%.xml}5.txt; \
	done;
	$(MV) $(OUTPUT)/equi.map $(OUTPUT)/equi.pvl
	$(RM) $(OUTPUT)/inputs.lis
	$(RM) $(OUTPUT)/relative.lis
	$(RM) $(OUTPUT)/basenames.lis
	$(RM) $(OUTPUT)/cubes.lis
