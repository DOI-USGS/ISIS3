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
	$(APPNAME) from=$(INPUT)/request.txt checkversion=false to=$(OUTPUT)/result.txt \
	  > /dev/null;
