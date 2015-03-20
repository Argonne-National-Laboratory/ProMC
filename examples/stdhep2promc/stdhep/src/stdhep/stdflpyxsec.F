
      subroutine STDFLPYXSEC(N1)

C   fill stdhep common block 1 with Pythia information
C
C   STDECOM  - center-of-mass energy
C   STDXSEC  - cross-section
C   STDSEED1 - random number seed
C   STDSEED2 - random number seed
C   NEVTREQ  - number of events to be generated
C   NEVTGEN  - number of events actually generated
C   NEVTWRT  - number of events written to output file
c   nevtlh   - number of Les Houches events written to output file
C
C   N1 - number of events to be generated
C

#include "stdcm1.inc"
#include "stdcnt.inc"
#include "pydatr.inc"
#include "pyint1.inc"
#include "pyint5.inc"
#include "pypars.inc"

      integer N1

      STDSEED1 = MRPY(1)
      STDSEED2 = 0.
      STDECOM = VINT(1)
      STDXSEC = PARI(1)
      NEVTREQ = N1
      NEVTGEN = NGEN(0,3)
      NEVTWRT = nstdwrt
      nevtlh  = nlhwrt
      generatorname = "Pythia"
      pdfname = " "
      return
      end
