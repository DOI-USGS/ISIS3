#
# # TODO: Clean up this file!
#
# import os, sys, subprocess
#
# # Constants
#
#
# libFolders = ['/Users/smcmich1/isis_cmake/opt/usgs/v006/3rdParty/lib',
#               '/Users/smcmich1/isis_cmake/opt/usgs/v006/ports/lib',
#               '/Users/smcmich1/isis_cmake/opt/usgs/v006/ports/libexec']
# #individualLibs = ['/Users/smcmich1/isis_cmake//opt/usgs/v006/ports/libexec/qt5/lib/QtCore.framework/Versions/5/QtCore']
#
# qtLibs = ('QtXmlPatterns QtXml QtNetwork '+
#                  'QtSql QtGui QtCore QtSvg '+
#                  'QtTest QtWebKit QtOpenGL '+
#                  'QtConcurrent QtDBus '+
#                  'QtMultimedia QtMultimediaWidgets '+
#                  'QtNfc QtPositioning QtPrintSupport '+
#                  'QtQml QtQuick QtQuickParticles '+
#                  'QtQuickTest QtQuickWidgets QtScript '+
#                  'QtScriptTools QtSensors QtSerialPort '+
#                  'QtWebKitWidgets QtWebSockets QtWidgets '+
#                  'QtTest QtWebChannel QtWebEngine QtWebEngineCore QtWebEngineWidgets').split()
# individualLibs = ['/Users/smcmich1/isis_cmake//opt/usgs/v006/ports/libexec/qt5/lib/'+x+'.framework/Versions/5/'+x for x in qtLibs]
# print individualLibs
#
# usgFolder = '/opt/usgs/v006/'
#
# rpathFolder = '/Users/smcmich1/isis_cmake/'
# ignoreFolder = '/Users/smcmich1/isis_cmake/'
#
#
# # Process one file
#
# def fixFile(fullPath, libName):
#     '''Fix the paths of a single library file'''
#
#     # Get list of libraries loaded by this library
#     cmd = ['otool', '-l', fullPath]
#     p = subprocess.Popen(cmd, stdout=subprocess.PIPE)
#     otoolOutput, err = p.communicate()
#
#     #print otoolOutput
#
#     # Search for abs paths to the USGS hard coded location
#     needRpath = False
#     lines     = otoolOutput.split()
#     for line in lines:
#
#         # Some lines need to be skipped
#         if (usgFolder not in line) or (ignoreFolder in line) or ('@rpath' in line):
#             continue
#
#         needRpath = True
#
#         # Set up the command to fix this line
#         if libName in line: # Hande LC_ID_DYLIB lines
#             cmd = ' '.join(['install_name_tool', '-id', '@rpath'+line, fullPath])
#         else: # Handle LC_LOAD_DYLIB lines
#             cmd = ' '.join(['install_name_tool', '-change', line, '@rpath'+line, fullPath])
#         print cmd
#         os.system(cmd)
#
#     # If any paths were changed, add the rpath.
#     if needRpath:
#         # Make sure this rpath is not there
#         cmd = ' '.join(['install_name_tool', '-delete_rpath', '/opt/usgs/v006/ports/lib', fullPath])
#         print cmd
#         os.system(cmd)
#         # Add the correct rpath
#         cmd = ' '.join(['install_name_tool', '-add_rpath', rpathFolder, fullPath])
#         print cmd
#         os.system(cmd)
#         print 'Fixed file ' + fullPath
#
# # Main function
#
# for folder in libFolders:
#
#     libs = os.listdir(folder)
#     for lib in libs:
#
#         # Only modify .dylib files
#         if 'dylib' not in lib:
#             continue
#
#         # Get the full path
#         fullPath = os.path.join(folder, lib)
#         #print lib
#
#         fixFile(fullPath, lib)
#
#
# for fullPath in individualLibs:
#
#     ## Only modify .dylib files
#     #if 'dylib' not in lib:
#     #    continue
#
#     # Get the full path
#     libName = os.path.basename(fullPath)
#     #print lib
#
#     fixFile(fullPath, libName)
#
# print 'Finished modifying files!'
