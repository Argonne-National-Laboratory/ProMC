#include "LParticle.h"
#include "TMath.h"
     
ClassImp(LParticle)


LParticle::LParticle() {
  m_type=0; 
  m_charge =0;
  m_status=0;
  parameters.clear(); 
}


LParticle::~LParticle() {

}



LParticle::LParticle(Int_t charge) {
  m_charge = charge;
  m_status=0;
  m_type=0;
  parameters.clear(); 
}


