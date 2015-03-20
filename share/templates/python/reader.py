#! /usr/bin/python
#  Reading ProMC file using CPython
#  S.Chekanov (ANL)
import sys,zipfile

if len(sys.argv) != 2:
    print "Usage: python reader [inputfile]"
    sys.exit()

sys.path.append("modules/")
from ProMCHeader_pb2 import *
from ProMC_pb2 import *
from ProMCStat_pb2 import *
from ProMCDescription_pb2 import *

 
z = zipfile.ZipFile(sys.argv[1], "r")
print "File version=",z.read("version")
print "File description=",z.read("promc_description")
print "Nr of events=", z.read("promc_nevents")

# retrive information from the header file
header=ProMCHeader()
header.ParseFromString(z.read("header"))
print "ID1=",header.id1
print "ID2=",header.id2
print "MomentumUnit=",header.MomentumUnit
print "LengthUnit  =",header.LengthUnit
gev=float(header.MomentumUnit)


# some PDG is stored?
pdata=header.particleData 
print "Name   mass (GeV)   ID"
for i in range(len(pdata)):
      print pdata[i].name,pdata[i].mass,pdata[i].id
 
######### loop over events ####################
tot=0;
for n in z.namelist():
    if n.isdigit(): 
      print "Event=",str(n)  
      event=ProMCEvent()
      event.ParseFromString(z.read(n))

      if (int(n)<3): # debug first 3 events 
        pdg_id=event.particles.pdg_id
        Px=event.particles.Px
        Py=event.particles.Py
        Pz=event.particles.Pz
        for j in range(len(pdg_id)):
               print j," px=",Px[j]/gev," py=",Py[j]/gev," pz=",Pz[j]/gev
z.close()
print "Total record size=",tot/1048576, ' MB'
