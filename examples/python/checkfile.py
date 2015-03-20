#! /usr/bin/python

import zipfile
tot=0;
z = zipfile.ZipFile("Pythia8.promc", "r")


# read version
line = z.read("version")
print "File version=",line

# read description 
line = z.read("description")
print "File description=",line


for filename in z.namelist():
        bytes = z.read(filename)
        print "Entry=",filename," has size=",len(bytes), ' bytes' 
        tot=tot+len(bytes) 
print "Total size=",tot/1048576, ' MB' 
