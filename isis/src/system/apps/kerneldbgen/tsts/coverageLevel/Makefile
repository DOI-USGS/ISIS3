# Coverage Level Test for kerneldbgen
#
# This test creates an output database file from the kernel in the input file
# that follow the given filter for reconstructed ck file name patterns. A database
# is output with time coverage at the SPICE segment level and at the SPICE interval 
# level. (There will be one output entry for the spice segment, and several for the
# SPICE interval because a SPICE segment is composed of SPICE intervals.) 
#
# After the output PVL is created, when compared, the DIFF file indicates to
# ignore RunTime and File.  The File keyword is ignored since, depending on 
# where the test is run, files may have different paths. These paths can not be 
# removed since they may be long enough to take up multiple lines.
# 
# This test uses files from the TGO CaSSIS mission, as this is where the problem
# was identified. 
#
# history 2018-05-09 Kristin Berry - Added test for newly added time coverage 
#                                    LEVEL=(SEGMENT*, INTERVAL) option. See #5410
APPNAME = kerneldbgen
include $(ISISROOT)/make/isismake.tsts

commands:
	# Default output level=segment
	$(APPNAME) to=$(OUTPUT)/kernel_segment.db.pvl \
	  type=CK \
	  recondir=$(INPUT) \
	  reconfilter='em16_tgo_sc_??m_*.bc' \
	  sclk=\$$tgo/kernels/sclk/em16_tgo_step_????????.tsc \
          lsk=\$$base/kernels/lsk/naif0012.tls > /dev/null; 

	# Output with level=interval, needed for CaSSIS kernels and potentially other type-6 kernels
	$(APPNAME) to=$(OUTPUT)/kernel_interval.db.pvl \
	  type=CK \
	  level=INTERVAL \
	  recondir=$(INPUT) \
	  reconfilter='em16_tgo_sc_??m_*.bc' \
 	  sclk=\$$tgo/kernels/sclk/em16_tgo_step_????????.tsc \
          lsk=\$$base/kernels/lsk/naif0012.tls > /dev/null;
