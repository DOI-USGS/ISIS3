# CK Test for kerneldbgen
# This test creates an output database file from the kernels in the input file
# that follow the given filter for reconstructed ck file name patterns.
# Note that the array style for this input is \("pattern1","pattern2"\)
#
# After the output PVL is created, when compared, the DIFF file indicates to
# ignore RunTime and File.  The File keyword is ignored since, depending on 
# where the test is run, files may have different paths. These paths can not be 
# removed since they may be long enough to take up multiple lines.
#
# The kernelFileNamesWithPath file is created to pull out the File keyword
# values, in order.  This is then passed into SED, removing the paths and 
# leaving only the ordered kernel names.
APPNAME = kerneldbgen
include $(ISISROOT)/make/isismake.tsts

commands:
	$(APPNAME) to=$(OUTPUT)/kernel.db.pvl \
	  type=CK \
	  recondir=$(INPUT) \
	  reconfilter=\("mro_sc_2005-10*.bc","mro_sc_2005-12-*.bc"\) \
	  sclk=\$$mro/kernels/sclk/MRO_SCLKSCET.00006.tsc \
	  lsk=\$$base/kernels/lsk/naif0008.tls > /dev/null; \
	grep "mro_sc_2005" $(OUTPUT)/kernel.db.pvl > $(OUTPUT)/kernelFileNamesWithPath.txt;
	$(SED) s+.*/++ $(OUTPUT)/kernelFileNamesWithPath.txt > $(OUTPUT)/kernelFileNamesWhitespace.txt;
	# Remove whitespace - this is needed in the rare circumstance that the input file path
	# is exactly 66 characters in length
	$(SED) 's/^[[:space:]]*//g' $(OUTPUT)/kernelFileNamesWhitespace.txt > $(OUTPUT)/orderedKernelFileNames.txt;
	$(RM) $(OUTPUT)/kernelFileNamesWithPath.txt;
	$(RM) $(OUTPUT)/kernelFileNamesWhitespace.txt;
