
This package contains five Android analysis scripts: 
- analyzeAndroidData
- extractAndroidData
- extractObjectHeaders
- processCarvedPackageXML
- extractObjectChunks (doesn't do much yet)

Further this package contains folder which contain library files used by the main scripts. 

One remark about the package carving: 
This folder contains a file sqliteTemplates.py. This file holds the templates for carving sqlite records. Templates can be added here, 
but some logic for handling the carving results needs to implement also. This is done in the extractAndroidData.py script. 

The scripts can be run seperately. Each script will create a log file in the defined output folder. Check this script to see if the script run correctly. 
Exceptions will also be output in the log file (not on your screen). 

Some scripts depend on the python lxml package which should be installed beforehand. This package can be downloaded from http://pypi.python.org/pypi/lxml/2.2. 

Run the script with the -h switch to see help. 

-- Ivo Pooters