APPNAME = spiceserver

include $(ISISROOT)/make/isismake.tsts

# In order to understand the input/output files, run this:
#   cat input/request.txt | xxd -r -ps
#
# or for the output:
#   cat output/result.txt | xxd -r -ps
#
# To understand the doubly-encoded parts, just copy the hex
#   and run this:
#   echo "...long hex string..." | xxd -r -ps
#
# Valid text comes out magically. I promise! :)

commands:
	$(APPNAME) from=$(INPUT)/requestHex.txt checkversion=false to=$(OUTPUT)/resultHex.txt \
	  tempfile=$(OUTPUT)/tempCube.cub > /dev/null; \
	cat $(OUTPUT)/resultHex.txt | xxd -r -ps >> $(OUTPUT)/decoded1.txt;\
	awk '/\<kernels_label\>/{getline; print}' $(OUTPUT)/decoded1.txt | xxd -r -ps >> $(OUTPUT)/kernelsGroup.pvl; \
	awk '/\<instrument_position\>/{getline; print}' $(OUTPUT)/decoded1.txt | xxd -r -ps >> $(OUTPUT)/tmp.txt; \
	awk '{print} /End_Object/ {exit}' $(OUTPUT)/tmp.txt >> $(OUTPUT)/instrumentPosition.pvl;\
	$(RM) $(OUTPUT)/decoded1.txt;
	$(RM) $(OUTPUT)/resultHex.txt;
	$(RM) $(OUTPUT)/tmp.txt;
