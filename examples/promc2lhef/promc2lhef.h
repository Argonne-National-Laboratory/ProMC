#ifndef promc_LesHouches_H
#define promc_LesHouches_H

namespace ProMC {

class LHAParticle {

public:

  // Constructors.   
  LHAParticle() : idPart(0), statusPart(0), mother1Part(0), 
    mother2Part(0), daughter1Part(0), daughter2Part(0), pxPart(0.), pyPart(0.), 
    pzPart(0.), ePart(0.), mPart(0.) { }
  LHAParticle(int idIn, int statusIn, int mother1In, int mother2In,
    int daughter1In, int daughter2In, double pxIn, double pyIn, double pzIn, 
    double eIn, double mIn) :
    idPart(idIn), statusPart(statusIn), mother1Part(mother1In), 
    mother2Part(mother2In), daughter1Part(daughter1In), daughter2Part(daughter2In), 
    pxPart(pxIn), pyPart(pyIn), pzPart(pzIn), ePart(eIn), mPart(mIn) { }

  // Particle properties.    
  int    idPart, statusPart, mother1Part, mother2Part, daughter1Part, daughter2Part;
  double pxPart, pyPart, pzPart, ePart, mPart;
};

//-----
//

class LHAup {

public:

  // Destructor.
  virtual ~LHAup() {}

  // Print the initialization info; useful to check that setting it worked.
  void   listInit(ostream& os = cout);  

  // Give back info on separate particle.
  int    sizePart()        const {return particles.size();}
  int    id(int part)      const {return particles[part].idPart;}
  int    status(int part)  const {return particles[part].statusPart;}
  int    mother1(int part) const {return particles[part].mother1Part;}
  int    mother2(int part) const {return particles[part].mother2Part;}
  int    daughter1(int part)    const {return particles[part].daughter1Part;}
  int    daughter2(int part)    const {return particles[part].daughter2Part;}
  double px(int part)      const {return particles[part].pxPart;}
  double py(int part)      const {return particles[part].pyPart;}
  double pz(int part)      const {return particles[part].pzPart;}
  double e(int part)       const {return particles[part].ePart;}
  double m(int part)       const {return particles[part].mPart;}


  // Print the info; useful to check that reading an event worked.
  void   listEvent(ostream& os = cout);  

  // Skip ahead a number of events, which are not considered further.
  // Mainly intended for debug when using the LHAupLHEF class.

  // Four routines to write a Les Houches Event file in steps.
  bool   openLHEF(string fileNameIn);
  bool   initLHEF(string infile);
  bool   eventLHEF(string infile);
  bool   closeLHEF();

protected:

  // Constructor. Sets default to be that events come with unit weight.
  LHAup() 
    { ; } 

private:

  // The particle list, stored as a vector of particles.
  vector<LHAParticle> particles;

  // File to which to write Les Houches Event File information.
  string fileName;
  fstream osLHEF;
};


class LHAupFromProMC : public LHAup {

public:

  // Constructor.
  LHAupFromProMC() {
    ;}

  // Destructor.
  ~LHAupFromProMC() {}

};

} //end namespace promc
#endif // promc_LesHouches_H
