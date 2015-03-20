#ifndef LParticle_H 
#define LParticle_H

using namespace std;
#include "TMath.h"
#include "TObject.h"
#include "TLorentzVector.h"
#include <vector>
#include <map>
#include <string>
 

class LParticle: public TObject {
private:
  Int_t m_type;              // particle type number
  Int_t m_status;            // status code
  Int_t m_charge;            // Particle charge
  // Double 8 bytes in memory, written as a 4 bytes float
  Double32_t m_px;
  Double32_t m_py;
  Double32_t m_pz;
  Double32_t m_mass;
  Double32_t m_x;
  Double32_t m_y;
  Double32_t m_z;
  Double32_t m_t;
  Int_t   m_mother1;
  Int_t   m_mother2;
  Int_t   m_daughter1;
  Int_t   m_daughter2;

  std::vector<double>  parameters;
public:


  LParticle();
  LParticle(Double32_t px, Double32_t py, Double32_t pz, Double32_t m){m_px=px; m_py=py; m_pz=pz; m_mass=m;};  
  LParticle(Int_t charge);
  ~LParticle();

  Int_t GetType() {return m_type;};
  Int_t GetStatus(){return m_status;};
  Int_t GetCharge(){return m_charge;};

  void     SetCharge(Int_t q) {m_charge = q;};
  void     SetType(Int_t q) {m_type = q;};
  void     SetStatus(Int_t q) {m_status = q;};
  void     SetVertex(Float_t x, Float_t y, Float_t z, Float_t t) {m_x=x; m_y=y; m_z=z; m_t=t;};
  void     SetDaughters(Int_t d1, Int_t d2) { m_daughter1=d1; m_daughter1=d2;}
  void     SetMothers(Int_t m1, Int_t m2) { m_mother1=m1; m_mother1=m2;}
  void     SetParameter(double q) { parameters.push_back( q ); };
  std::vector<double>   GetParameters() { return parameters; };

ClassDef(LParticle,1)// Monte Carlo particle object
};

#endif

