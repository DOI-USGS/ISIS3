# CK Test for kerneldbgen
# This test creates an output database file from the kernels in the input file
# that follow the given filter for reconstructed ck file name patterns.
# Note that the array style for this input is \("pattern1","pattern2"\)
#
# After the output PVL is created, when compared, the DIFF file indicates to
# ignore RunTime and File.  The File keyword is set to IgnoreFilePaths since, depending on 
# where the test is run, files may have different paths. 
#
APPNAME = kerneldbgen
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/kernel.db.pvl \
	  type=CK \
	  recondir=$(INPUT) \
	  reconfilter=\("mro_sc_2005-10*.bc","mro_sc_2005-12-*.bc"\) \
	  sclk=\$$mro/kernels/sclk/MRO_SCLKSCET.00006.tsc \
	  lsk=\$$base/kernels/lsk/naif0008.tls > /dev/null; \

