// -*- C++ -*-
//
// Package:    L1CaloTriggerProducer
// Class:      FastL1GlobalAlgo
// 
/**\class FastL1GlobalAlgo

 Description: Global algorithm.

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Chi Nhan Nguyen
//         Created:  Mon Feb 19 13:25:24 CST 2007
// $Id: FastL1GlobalAlgo.cc,v 1.2 2007/04/11 07:42:35 beaudett Exp $
//

#include "FastSimulation/L1CaloTriggerProducer/interface/FastL1GlobalAlgo.h"

//
// constructors and destructor
//
FastL1GlobalAlgo::FastL1GlobalAlgo(const edm::ParameterSet& iConfig)
{  
  m_RMap = FastL1RegionMap::getL1RegionMap();

  // Get L1 config
  m_L1Config.EMSeedEnThreshold = iConfig.getParameter<double>("EMSeedEnThreshold");
  m_L1Config.EMActiveLevel = iConfig.getParameter<double>("EMActiveLevel");
  m_L1Config.HadActiveLevel = iConfig.getParameter<double>("HadActiveLevel");
  m_L1Config.noTauVetoLevel = iConfig.getParameter<double>("noTauVetoLevel");	
  m_L1Config.hOeThreshold = iConfig.getParameter<double>("hOeThreshold");
  m_L1Config.FGEBThreshold = iConfig.getParameter<double>("FGEBThreshold");
  m_L1Config.FGEEThreshold = iConfig.getParameter<double>("FGEEThreshold");

  m_L1Config.MuonNoiseLevel = iConfig.getParameter<double>("MuonNoiseLevel");
  m_L1Config.EMNoiseLevel = iConfig.getParameter<double>("EMNoiseLevel");
  m_L1Config.HadNoiseLevel = iConfig.getParameter<double>("HadNoiseLevel");
  m_L1Config.QuietRegionThreshold = iConfig.getParameter<double>("QuietRegionThreshold");
  m_L1Config.JetSeedEtThreshold = iConfig.getParameter<double>("JetSeedEtThreshold");

  m_L1Config.CrystalEBThreshold = iConfig.getParameter<double>("CrystalEBThreshold");
  m_L1Config.CrystalEEThreshold = iConfig.getParameter<double>("CrystalEEThreshold");

  m_L1Config.TowerEBThreshold = iConfig.getParameter<double>("TowerEBThreshold");
  m_L1Config.TowerEEThreshold = iConfig.getParameter<double>("TowerEEThreshold");
  m_L1Config.TowerHBThreshold = iConfig.getParameter<double>("TowerHBThreshold");
  m_L1Config.TowerHEThreshold = iConfig.getParameter<double>("TowerHEThreshold");

  m_L1Config.TowerEBScale = iConfig.getParameter<double>("TowerEBScale");
  m_L1Config.TowerEEScale = iConfig.getParameter<double>("TowerEEScale");
  m_L1Config.TowerHBScale = iConfig.getParameter<double>("TowerHBScale");
  m_L1Config.TowerHEScale = iConfig.getParameter<double>("TowerHEScale");

  // new: 
  // turn off fg bit
  m_L1Config.noFGThreshold = iConfig.getParameter<double>("noFGThreshold");	
  
  m_L1Config.EmInputs = iConfig.getParameter <std::vector<edm::InputTag> >("EmInputs");
  m_L1Config.TowerInput = iConfig.getParameter<edm::InputTag>("TowerInput");

}

FastL1GlobalAlgo::~FastL1GlobalAlgo()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//


// ------------ Dump out CaloTower info  ------------
void
FastL1GlobalAlgo::CaloTowersDump(edm::Event const& e) {
  /*
  std::vector<edm::Handle<CaloTowerCollection> > prods;
    
  edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << "Start!";
  try {
    e.getManyByType(prods);

    std::vector<edm::Handle<CaloTowerCollection> >::iterator i;

    for (i=prods.begin(); i!=prods.end(); i++) {
      const CaloTowerCollection& c=*(*i);
	
      for (CaloTowerCollection::const_iterator j=c.begin(); j!=c.end(); j++) {
	edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << *j;
      }
    }
  } catch (...) {
    edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << "No CaloTowers.";
  }
  edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << "End!";
  */


  edm::Handle<CaloTowerCollection> input;
    
  edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << "Start!";
  e.getByLabel(m_L1Config.TowerInput,input);

  for (CaloTowerCollection::const_iterator j=input->begin(); j!=input->end(); j++) {
    edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << *j;
  }

  edm::LogInfo("FastL1GlobalAlgo::CaloTowersDump") << "End!";
}


// ------------ For sort()  ------------
namespace myspace {
  bool 
  //FastL1GlobalAlgo::greaterEt( const reco::Candidate& a, const reco::Candidate& b ) {
  greaterEt( const reco::Candidate& a, const reco::Candidate& b ) {
    return (a.et()>b.et());
  }
}

// ------------ Find Jets  ------------
void
FastL1GlobalAlgo::findJets() {

  m_TauJets.clear();
  m_CenJets.clear();
  m_ForJets.clear();

  // only barrel right now!
  // barrel: 7-14 with 22 ieta
  //for (int i=0; i<396 ; i++) {
  for (int i=0; i<396; i++) {
    // barrel/endcap part only right now
    //if ((i%22)>0 && (i%22)<21) {
    if ((i%22)>3 && (i%22)<18) {
      if (m_Regions.at(i).SumEt()>m_L1Config.JetSeedEtThreshold) {
	if (isMaxEtRgn_Window33(i)) {
	  if (isTauJet(i)) {
	    addJet(i,true);    
	  } else {
	    addJet(i,false);    
	  }
	}
      } else {
	m_Regions[i].BitInfo.SumEtBelowThres = true;
     }
    }
  }

}


// ------------ Add a jet  ------------
void
FastL1GlobalAlgo::addJet(int iRgn, bool taubit) {
  std::pair<double, double> p = m_RMap->getRegionCenterEtaPhi(iRgn);

  //double e     = m_Regions.at(iRgn).GetJetE();
  double et     = TPEnergyRound(m_Regions.at(iRgn).GetJetEt(),2,2);

  double eta   = p.first;
  double phi   = p.second;
  double theta = 2.*atan(exp(-eta));
  double ex = et*cos(phi);
  double ey = et*sin(phi);
  //double ex = e*sin(theta)*cos(phi);
  //double ey = e*sin(theta)*sin(phi);
  double e = ex/sin(theta)/cos(phi);
  double ez = e*cos(theta);

  m_Regions[iRgn].BitInfo.eta = eta;
  m_Regions[iRgn].BitInfo.phi = phi;

  reco::Particle::LorentzVector rp4(ex,ey,ez,e); 
  l1extra::L1JetParticle tjet(rp4);
  
  if (taubit || et>m_L1Config.noTauVetoLevel) {
    m_TauJets.push_back(tjet);
  } else {
    if (std::abs(eta)<3.0)
      m_CenJets.push_back(tjet);
    else
      m_ForJets.push_back(tjet);
  }
  // sort by et 
  std::sort(m_TauJets.begin(),m_TauJets.end(), myspace::greaterEt);
  std::sort(m_CenJets.begin(),m_CenJets.end(), myspace::greaterEt);
  std::sort(m_ForJets.begin(),m_ForJets.end(), myspace::greaterEt);
 
}


// ------------ Fill Egammas ------------
void
FastL1GlobalAlgo::FillEgammas(edm::Event const& e) {
  m_Egammas.clear();
  m_isoEgammas.clear();

  //std::vector< edm::Handle<CaloTowerCollection> > input;
  //e.getManyByType(input);
  edm::Handle<CaloTowerCollection> input;
  e.getByLabel(m_L1Config.TowerInput,input);

  //std::vector< edm::Handle<CaloTowerCollection> >::iterator j;
  //for (j=input.begin(); j!=input.end(); j++) {
  //  const CaloTowerCollection& c=*(*j);
    
  //for (CaloTowerCollection::const_iterator cnd=c.begin(); cnd!=c.end(); cnd++) {
    for (CaloTowerCollection::const_iterator cnd=input->begin(); cnd!=input->end(); cnd++) {
      reco::Particle::LorentzVector rp4(0.,0.,0.,0.);
      l1extra::L1EmParticle* ph = new l1extra::L1EmParticle(rp4);
      CaloTowerDetId cid   = cnd->id();

      int emTag = isEMCand(cid,ph,e);
      
      // 1 = non-iso EM, 2 = iso EM
      if (emTag==1) {
	m_Egammas.push_back(*ph);
      } else if (emTag==2) {
	m_isoEgammas.push_back(*ph);
      }

    }
  //}

  std::sort(m_Egammas.begin(),m_Egammas.end(), myspace::greaterEt);
  std::sort(m_isoEgammas.begin(),m_isoEgammas.end(), myspace::greaterEt);
}

// ------------ Fill MET 1: loop over towers ------------
void
FastL1GlobalAlgo::FillMET(edm::Event const& e) {
  //std::vector< edm::Handle<CaloTowerCollection> > input;
  //e.getManyByType(input);
  edm::Handle<CaloTowerCollection> input;
  e.getByLabel(m_L1Config.TowerInput,input);

  double sum_hade = 0.0;
  double sum_hadet = 0.0;
  double sum_hadex = 0.0;
  double sum_hadey = 0.0;
  double sum_hadez = 0.0;
  double sum_e = 0.0;
  double sum_et = 0.0;
  double sum_ex = 0.0;
  double sum_ey = 0.0;
  double sum_ez = 0.0;

  //std::vector< edm::Handle<CaloTowerCollection> >::iterator i;

  //for (i=input.begin(); i!=input.end(); i++) {
  //  const CaloTowerCollection& c=*(*i);

  //  for (CaloTowerCollection::const_iterator candidate=c.begin(); candidate!=c.end(); candidate++) {
  for (CaloTowerCollection::const_iterator candidate=input->begin(); candidate!=input->end(); candidate++) {
      //double eme    = candidate->emEnergy();
      //double hade    = candidate->hadEnergy();
      double eme    = candidate->emEt();
      double hade    = candidate->hadEt();
      
      double EThres = 0.;
      double HThres = 0.;
      double EBthres = m_L1Config.TowerEBThreshold;
      double HBthres = m_L1Config.TowerHBThreshold;
      double EEthres = m_L1Config.TowerEBThreshold;
      double HEthres = m_L1Config.TowerEEThreshold;

      //if(std::abs(candidate->eta())<1.479) {
      if(std::abs(candidate->eta())<2.322) {
	EThres = EBthres;
      } else {
	EThres = EEthres;
      }
      //if(std::abs(candidate->eta())<1.305) {
      if(std::abs(candidate->eta())<2.322) {
	HThres = HBthres;
      } else {
	HThres = HEthres;
      }

      // rescale energies
      double emScale = 1.0;
      double hadScale = 1.0;
      if (std::abs(candidate->eta()>1.3050) && std::abs(candidate->eta())<3.0) {
	hadScale = m_L1Config.TowerHEScale;
	emScale = m_L1Config.TowerEEScale;
      }
      if (std::abs(candidate->eta()<1.3050)) {
	hadScale = m_L1Config.TowerHBScale;
	emScale = m_L1Config.TowerEBScale;
      }
      eme    *= emScale;
      hade   *= hadScale;
      
      if (eme>=EThres || hade>=HThres) {
	double phi   = candidate->phi();
	double eta = candidate->eta();
	//double et    = candidate->et();
	//double e     = candidate->energy();
	double theta = 2.*atan(exp(-eta));
	double et    = 0.;
	double e     = 0.;
	double had_et    = 0.;
	double had_e     = 0.;

	if (eme>=EThres) {
	  et    += candidate->emEt();
	  e    += candidate->emEnergy();
	}
	if (hade>=HThres) {
 	  et    += candidate->hadEt();
	  e    += candidate->hadEnergy();
 	  had_et  += candidate->hadEt();
	  had_e   += candidate->hadEnergy();
	}

	sum_et += et;
	sum_ex += et*cos(phi);
	sum_ey += et*sin(phi); 
	//sum_ex += e*sin(theta)*cos(phi);
	//sum_ey += e*sin(theta)*sin(phi); 
	//sum_e += e;
	sum_e += et/sin(theta);
	sum_ez += et*cos(theta)/sin(theta);

	sum_hadet += had_et;
	sum_hadex += had_et*cos(phi);
	sum_hadey += had_et*sin(phi); 
	//sum_hadex += had_e*sin(theta)*cos(phi);
	//sum_hadey += had_e*sin(theta)*sin(phi); 
	//sum_hade += had_e;
	sum_hade += had_et/sin(theta);
	sum_hadez += had_et*cos(theta)/sin(theta);
      }
    }
  //}

  reco::Particle::LorentzVector rp4(-sum_ex,-sum_ey,-sum_ez,sum_e); 
  m_MET = l1extra::L1EtMissParticle(rp4,sum_et,sum_hadet);  
}

// ------------ Fill MET 2: loop over regions ------------
void
FastL1GlobalAlgo::FillMET() {
  double sum_hade = 0.0;
  double sum_hadet = 0.0;
  double sum_hadex = 0.0;
  double sum_hadey = 0.0;
  double sum_hadez = 0.0;
  double sum_e = 0.0;
  double sum_et = 0.0;
  double sum_ex = 0.0;
  double sum_ey = 0.0;
  double sum_ez = 0.0;
  for (int i=0; i<396; i++) { 
    // barrel/endcap part only right now
    if ((i%22)>3 && (i%22)<18) {
      CaloTowerCollection c = m_Regions[i].GetCaloTowers();
      
      for (CaloTowerCollection::const_iterator candidate=c.begin(); candidate!=c.end(); candidate++) {
	//double eme    = candidate->emEnergy();
	//double hade    = candidate->hadEnergy();
	double eme    = candidate->emEt();
	double hade    = candidate->hadEt();
	
	double EThres = 0.;
	double HThres = 0.;
	double EBthres = m_L1Config.TowerEBThreshold;
	double HBthres = m_L1Config.TowerHBThreshold;
	double EEthres = m_L1Config.TowerEBThreshold;
	double HEthres = m_L1Config.TowerEEThreshold;
	
	//if(std::abs(candidate->eta())<1.479) {
	if(std::abs(candidate->eta())<2.322) {
	  EThres = EBthres;
	} else {
	  EThres = EEthres;
	}
	//if(std::abs(candidate->eta())<1.305) {
	if(std::abs(candidate->eta())<2.322) {
	  HThres = HBthres;
	} else {
	  HThres = HEthres;
	}
	
	// rescale energies
	double emScale = 1.0;
	double hadScale = 1.0;
	if (std::abs(candidate->eta()>1.3050) && std::abs(candidate->eta())<3.0) {
	  hadScale = m_L1Config.TowerHEScale;
	  emScale = m_L1Config.TowerEEScale;
	}
	if (std::abs(candidate->eta()<1.3050)) {
	  hadScale = m_L1Config.TowerHBScale;
	  emScale = m_L1Config.TowerEBScale;
	}
	eme    *= emScale;
	hade   *= hadScale;
	
	if (eme>=EThres || hade>=HThres) {
	  double phi   = candidate->phi();
	  double eta = candidate->eta();
	  //double et    = candidate->et();
	  //double e     = candidate->energy();
	  double theta = 2.*atan(exp(-eta));
	  double et    = 0.;
	  double e     = 0.;
	  double had_et    = 0.;
	  double had_e     = 0.;
	  
	  if (eme>=EThres) {
	    et    += candidate->emEt();
	    e    += candidate->emEnergy();
	  }
	  if (hade>=HThres) {
	    et    += candidate->hadEt();
	    e    += candidate->hadEnergy();
	    had_et    += candidate->hadEt();
	    had_e    += candidate->hadEnergy();
	  }
	  
	  sum_et += et;
	  sum_ex += et*cos(phi);
	  sum_ey += et*sin(phi); 
	  //sum_ex += e*sin(theta)*cos(phi);
	  //sum_ey += e*sin(theta)*sin(phi); 
	  //sum_e += e;
	  sum_e += et/sin(theta);
	  sum_ez += et*cos(theta)/sin(theta);
	  
	  sum_hadet += had_et;
	  sum_hadex += had_et*cos(phi);
	  sum_hadey += had_et*sin(phi); 
	  //sum_hadex += had_e*sin(theta)*cos(phi);
	  //sum_hadey += had_e*sin(theta)*sin(phi); 
	  //sum_hade += had_e;
	  sum_hade += had_et/sin(theta);
	  sum_hadez += had_et*cos(theta)/sin(theta);
	}
      }
    }
  }
  
  reco::Particle::LorentzVector rp4(-sum_ex,-sum_ey,-sum_ez,sum_e); 
  m_MET = l1extra::L1EtMissParticle(rp4,sum_et,sum_hadet);  
  
}

// ------------ Fill L1 Regions ------------
void 
FastL1GlobalAlgo::FillL1Regions(edm::Event const& e, const edm::EventSetup& iConfig) 
{
  m_Regions.clear();
  m_Regions = std::vector<FastL1Region>(396); // ieta: 0-21, iphi: 0-18

  // init regions
  for (int i=0; i<396; i++) {
    m_Regions[i].SetParameters(m_L1Config);

    std::pair<int, int> p = m_RMap->getRegionEtaPhiIndex(i);
    m_Regions[i].SetEtaPhiIndex(p.first,p.second,i);
    CaloTowerCollection c(16);
    for (int twrid=0; twrid<16; twrid++) {
      m_Regions[i].FillTower(c[twrid],twrid);
    }
  }

  // ascii visualisation of mapping
  //m_RMap->display();
  
 
  // works for barrel/endcap region only right now!
  //std::vector< edm::Handle<CaloTowerCollection> > input;
  //e.getManyByType(input);
  edm::Handle<CaloTowerCollection> input;
  e.getByLabel(m_L1Config.TowerInput,input);

  //std::vector< edm::Handle<CaloTowerCollection> >::iterator j;
  //for (j=input.begin(); j!=input.end(); j++) {
  //  const CaloTowerCollection& c=*(*j);
    
  //  for (CaloTowerCollection::const_iterator cnd=c.begin(); cnd!=c.end(); cnd++) {
  for (CaloTowerCollection::const_iterator cnd=input->begin(); cnd!=input->end(); cnd++) {

      CaloTowerDetId cid   = cnd->id();
      std::pair<int, int> pep = m_RMap->getRegionEtaPhiIndex(cid);
 
      int rgnid = 999;
      int twrid = 999;

      //std::cout << "FastL1GlobalAlgo::FillL1Regions calotowers dump: " << *cnd << std::endl;

      if (std::abs(pep.first)<=28) {
	rgnid = pep.second*22 + pep.first;
	twrid = m_RMap->getRegionTowerIndex(cid);
	//std::cout << rgnid << " " << twrid << std::endl;
      }

      if (rgnid<396 && twrid<16) {
	m_Regions[rgnid].FillTower_Scaled(*cnd,twrid);
	m_Regions[rgnid].SetRegionBits(e);
      } else {
	//std::cerr << "FastL1GlobalAlgo::FillL1Regions(): ERROR - invalid region or tower ID: " << rgnid << " | " << twrid  << std::endl;
      }

    }

  //}

  // Fill EM Crystals and Bitwords
  m_BitInfos.clear();
  for (int i=0; i<396; i++) {
    m_Regions[i].FillEMCrystals(e,iConfig,m_RMap);

    m_BitInfos.push_back(m_Regions[i].getBitInfo());
  }

  //checkMapping();
  
}


// ------------ Check if jet is taujet ------------
bool 
FastL1GlobalAlgo::isTauJet(int cRgn) {

  if (m_Regions[cRgn].GetTauBit()) 
    return false;

  int nwid = m_Regions[cRgn].GetNWId();
  int nid = m_Regions[cRgn].GetNorthId();
  int neid = m_Regions[cRgn].GetNEId();
  int wid = m_Regions[cRgn].GetWestId();
  int eid = m_Regions[cRgn].GetEastId();
  int swid = m_Regions[cRgn].GetSWId();
  int sid = m_Regions[cRgn].GetSouthId();
  int seid = m_Regions[cRgn].GetSEId();

  if (nwid==999 || neid==999 || nid==999 || swid==999 || seid==999 || sid==999 || wid==999 || 
      eid==999 ) { 
    std::cerr << "FastL1GlobalAlgo::isTauJet(): RegionId out of bounds: " << std::endl
	      << nwid << " " << nid << " "  << neid << " " << std::endl
	      << wid << " " << cRgn << " "  << eid << " " << std::endl
	      << swid << " " << sid << " "  << seid << " " << std::endl;    
    return false;
  }

  if (
      m_Regions[nwid].GetTauBit() ||
      m_Regions[nid].GetTauBit()  ||
      m_Regions[neid].GetTauBit() ||
      m_Regions[wid].GetTauBit()  ||
      m_Regions[eid].GetTauBit()  ||
      m_Regions[swid].GetTauBit() ||
      m_Regions[seid].GetTauBit() ||
      m_Regions[sid].GetTauBit()
      ) 
    m_Regions[cRgn].BitInfo.IsolationVeto = true;

  if (
      m_Regions[nwid].GetTauBit() ||
      m_Regions[nid].GetTauBit()  ||
      m_Regions[neid].GetTauBit() ||
      m_Regions[wid].GetTauBit()  ||
      m_Regions[eid].GetTauBit()  ||
      m_Regions[swid].GetTauBit() ||
      m_Regions[seid].GetTauBit() ||
      m_Regions[sid].GetTauBit()  ||
      m_Regions[cRgn].GetTauBit()
      ) 
    return false;
  else return true;


}

// ------------ Check if tower is emcand ------------
// returns 1 = non-iso EM, 2 = iso EM, 0 = no EM
int
FastL1GlobalAlgo::isEMCand(CaloTowerDetId cid, l1extra::L1EmParticle* ph,const edm::Event& iEvent) {

  // center tower
  int crgn = m_RMap->getRegionIndex(cid);
  int ctwr = m_RMap->getRegionTowerIndex(cid);

  // barrel/endcap part only right now
  if ((crgn%22)<4 || (crgn%22)>17) return 0;
  if (crgn>395 || crgn < 1 || ctwr > 15 || ctwr < 0) return 0;

  CaloTowerCollection c = m_Regions.at(crgn).GetCaloTowers();
  double cenEt = c[ctwr].emEt();
  //double cenE = c[ctwr].emEnergy();
  
  // Using region position rather than tower position
  std::pair<double, double> crpos = m_RMap->getRegionCenterEtaPhi(crgn);
  //double cenEta = c[ctwr].eta();
  //double cenPhi = c[ctwr].phi();
  double cenEta = crpos.first;
  double cenPhi = crpos.second;

  double cenFGbit = m_Regions.at(crgn).GetFGBit(ctwr);
  double cenHOEbit = m_Regions.at(crgn).GetHOEBit(ctwr);

  if (cenEt<m_L1Config.TowerEBThreshold) return 0;

  // check fine grain bit
  if (cenFGbit) return 0;

  // check H/E bit
  if (cenHOEbit) return 0;

  // check neighbours
  std::pair<int, int> no = m_RMap->GetTowerNorthEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> so = m_RMap->GetTowerSouthEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> we = m_RMap->GetTowerWestEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> ea = m_RMap->GetTowerEastEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> nw = m_RMap->GetTowerNWEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> ne = m_RMap->GetTowerNEEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> sw = m_RMap->GetTowerSWEtaPhi(cid.ieta(),cid.iphi()); 
  std::pair<int, int> se = m_RMap->GetTowerSEEtaPhi(cid.ieta(),cid.iphi()); 
  if (no.first>28 || no.first<-28 || no.second>72 || no.second<0) return 0;
  if (so.first>28 || so.first<-28 || so.second>72 || so.second<0) return 0;
  if (we.first>28 || we.first<-28 || we.second>72 || we.second<0) return 0;
  if (ea.first>28 || ea.first<-28 || ea.second>72 || ea.second<0) return 0;
  if (nw.first>28 || nw.first<-28 || nw.second>72 || nw.second<0) return 0;
  if (ne.first>28 || ne.first<-28 || ne.second>72 || ne.second<0) return 0;
  if (sw.first>28 || sw.first<-28 || sw.second>72 || sw.second<0) return 0;
  if (se.first>28 || se.first<-28 || se.second>72 || se.second<0) return 0;

  int notwr = m_RMap->getRegionTowerIndex(no);
  int norgn = m_RMap->getRegionIndex(no.first,no.second);
  int sotwr = m_RMap->getRegionTowerIndex(so);
  int sorgn = m_RMap->getRegionIndex(so.first,so.second);
  int wetwr = m_RMap->getRegionTowerIndex(we);
  int wergn = m_RMap->getRegionIndex(we.first,we.second);
  int eatwr = m_RMap->getRegionTowerIndex(ea);
  int eargn = m_RMap->getRegionIndex(ea.first,ea.second);
  int setwr = m_RMap->getRegionTowerIndex(se);
  int sergn = m_RMap->getRegionIndex(se.first,sw.second);
  int swtwr = m_RMap->getRegionTowerIndex(sw);
  int swrgn = m_RMap->getRegionIndex(sw.first,sw.second);
  int netwr = m_RMap->getRegionTowerIndex(ne);
  int nergn = m_RMap->getRegionIndex(ne.first,ne.second);
  int nwtwr = m_RMap->getRegionTowerIndex(nw);
  int nwrgn = m_RMap->getRegionIndex(nw.first,nw.second);
  
  //
  if (norgn>395 || norgn < 0 || notwr > 15 || notwr < 0) return 0;
  c = m_Regions[norgn].GetCaloTowers();
  double noEt = c[notwr].emEt();
  //double noE = c[notwr].emEnergy();
  // check fine grain bit
  bool noFGbit = m_Regions[norgn].GetFGBit(notwr);
  // check H/E bit
  bool noHOEbit = m_Regions[norgn].GetHOEBit(notwr);

  //
  if (sorgn>395 || sorgn < 0 || sotwr > 15 || sotwr < 0) return 0;
  c = m_Regions[sorgn].GetCaloTowers();
  double soEt = c[sotwr].emEt();
  //double soE = c[sotwr].emEnergy();
  // check fine grain bit
  bool soFGbit = m_Regions[sorgn].GetFGBit(sotwr);
  // check H/E bit
  bool soHOEbit = m_Regions[sorgn].GetHOEBit(sotwr);

  //
  if (wergn>395 || wergn < 0 || wetwr > 15 || wetwr < 0) return 0;
  c = m_Regions[wergn].GetCaloTowers();
  double weEt = c[wetwr].emEt();
  //double weE = c[wetwr].emEnergy();
  // check fine grain bit
  bool weFGbit = m_Regions[wergn].GetFGBit(wetwr);
  // check H/E bit
  bool weHOEbit = m_Regions[wergn].GetHOEBit(wetwr);

  //
  if (eargn>395 || eargn < 0 || eatwr > 15 || eatwr < 0) return 0;
  c = m_Regions[eargn].GetCaloTowers();
  double eaEt = c[eatwr].emEt();
  //double eaE = c[eatwr].emEnergy();
  // check fine grain bit
  bool eaFGbit = m_Regions[eargn].GetFGBit(eatwr);
  // check H/E bit
  bool eaHOEbit = m_Regions[eargn].GetHOEBit(eatwr);

  //
  if (nwrgn>395 || nwrgn < 0 || nwtwr > 15 || nwtwr < 0) return 0;
  c = m_Regions[nwrgn].GetCaloTowers();
  double nwEt = c[nwtwr].emEt();
  //double nwE = c[nwtwr].emEnergy();
  // check fine grain bit
  bool nwFGbit = m_Regions[nwrgn].GetFGBit(nwtwr);
  // check H/E bit
  bool nwHOEbit = m_Regions[nwrgn].GetHOEBit(nwtwr);

  //
  if (nergn>395 || nergn < 0 || netwr > 15 || netwr < 0) return 0;
  c = m_Regions[nergn].GetCaloTowers();
  double neEt = c[netwr].emEt();
  //double neE = c[netwr].emEnergy();
  // check fine grain bit
  bool neFGbit = m_Regions[nergn].GetFGBit(netwr);
  // check H/E bit
  bool neHOEbit = m_Regions[nergn].GetHOEBit(netwr);

  //
  if (swrgn>395 || swrgn < 0 || swtwr > 15 || swtwr < 0) return 0;
  c = m_Regions[swrgn].GetCaloTowers();
  double swEt = c[swtwr].emEt();
  //double swE = c[swtwr].emEnergy();
  // check fine grain bit
  bool swFGbit = m_Regions[swrgn].GetFGBit(swtwr);
  // check H/E bit
  bool swHOEbit = m_Regions[swrgn].GetHOEBit(swtwr);

  //
  if (sergn>395 || sergn < 0 || setwr > 15 || setwr < 0) return 0;
  c = m_Regions[sergn].GetCaloTowers();
  double seEt = c[setwr].emEt();
  //double seE = c[setwr].emEnergy();
  // check fine grain bit
  bool seFGbit = m_Regions[sergn].GetFGBit(setwr);
  // check H/E bit
  bool seHOEbit = m_Regions[sergn].GetHOEBit(setwr);

  
  // check if highest et tower
  bool isHit = false;
  if ( cenEt > noEt && cenEt > soEt && cenEt > weEt &&
       cenEt > eaEt && cenEt > nwEt && cenEt > neEt &&
       cenEt > swEt && cenEt > seEt ) isHit = true;
  else 
    return 0;

  // find highest neighbour
  double hitEt = cenEt;
  //double hitE = cenE;
  double maxEt = std::max(noEt,std::max(soEt,std::max(weEt,eaEt)));
  //double maxE = std::max(noE,std::max(soE,std::max(weE,eaE)));

  // check 2 tower Et
  float emEtThres = m_L1Config.EMSeedEnThreshold;
  if ((hitEt+maxEt)<emEtThres) return 0;
  

  // at this point candidate is at least non-iso Egamma
  //double eme = (hitE+maxE);
  //double eme = (hitE+maxE);
  //double emet = (hitEt+maxEt);
  double emet = (hitEt+maxEt);
  double emtheta = 2.*atan(exp(-cenEta));
  //double emet = eme*sin(emtheta);
  double emex = emet*cos(cenPhi);
  double emey = emet*sin(cenPhi);
  //double emex = eme*sin(emtheta)*cos(cenPhi);
  //double emey = eme*sin(emtheta)*sin(cenPhi);
  double eme = emex/sin(emtheta)/cos(cenPhi);
  double emez = eme*cos(emtheta);

  reco::Particle::LorentzVector rp4(emex,emey,emez,eme);
  //reco::Particle::Point rp3(0.,0.,0.);
  //reco::Particle::Charge q(0);
  //*ph = reco::Photon(q,rp4,rp3);
  *ph = l1extra::L1EmParticle(rp4);
 
  // check isolation FG bits
  if (noFGbit || soFGbit || weFGbit || eaFGbit || 
      nwFGbit || neFGbit || swFGbit || seFGbit ||
      noHOEbit || soHOEbit || weHOEbit || eaHOEbit || 
      nwHOEbit || neHOEbit || swHOEbit || seHOEbit)
    return 1;
  
  // check isolation corners
  double corThres = 0.4;
  double emNoiseEt = m_L1Config.EMNoiseLevel;
  bool isoVeto1 = false,isoVeto2 = false,isoVeto3 = false,isoVeto4 = false;
  if (swEt>emNoiseEt || weEt>emNoiseEt || nwEt>emNoiseEt || noEt>emNoiseEt || neEt>emNoiseEt ) 
    if ((swEt + weEt + nwEt + noEt + neEt)/cenEt > corThres) 
      isoVeto1 = true;
  if (neEt>emNoiseEt || eaEt>emNoiseEt || seEt>emNoiseEt || soEt>emNoiseEt || swEt>emNoiseEt ) 
    if ((neEt + eaEt + seEt + soEt + swEt)/cenEt > corThres) 
      isoVeto2 = true;
  if (nwEt>emNoiseEt || noEt>emNoiseEt || neEt>emNoiseEt || eaEt>emNoiseEt || seEt>emNoiseEt ) 
    if ((nwEt + noEt + neEt + eaEt + seEt)/cenEt > corThres) 
      isoVeto3 = true;
  if (seEt>emNoiseEt || soEt>emNoiseEt || swEt>emNoiseEt || weEt>emNoiseEt || nwEt>emNoiseEt ) 
    if ((seEt + soEt + swEt + weEt + nwEt)/cenEt > corThres) 
      isoVeto4 = true;
  if (isoVeto1 && isoVeto2 && isoVeto3 && isoVeto4)
    return 1;

  return 2;    
}


// is central region the highest Et Region?
bool 
FastL1GlobalAlgo::isMaxEtRgn_Window33(int crgn) {

  int nwid = m_Regions.at(crgn).GetNWId();
  int nid = m_Regions.at(crgn).GetNorthId();
  int neid = m_Regions.at(crgn).GetNEId();
  int wid = m_Regions.at(crgn).GetWestId();
  int eid = m_Regions.at(crgn).GetEastId();
  int swid = m_Regions.at(crgn).GetSWId();
  int sid = m_Regions.at(crgn).GetSouthId();
  int seid = m_Regions.at(crgn).GetSEId();

  if (nwid==999 || neid==999 || nid==999 || swid==999 || seid==999 || sid==999 || wid==999 || 
      eid==999 ) { 
    std::cerr << "FastL1GlobalAlgo::isMaxEtRgn_Window33(): RegionId out of bounds: " << std::endl
	      << nwid << " " << nid << " "  << neid << " " << std::endl
	      << wid << " " << crgn << " "  << eid << " " << std::endl
	      << swid << " " << sid << " "  << seid << " " << std::endl;    
    return false;
  }

  double cenet = m_Regions.at(crgn).SumEt();
  double nwet =  m_Regions[nwid].SumEt();
  double noet = m_Regions[nid].SumEt();
  double neet = m_Regions[neid].SumEt();
  double weet = m_Regions[wid].SumEt();
  double eaet = m_Regions[eid].SumEt();
  double swet = m_Regions[swid].SumEt();
  double soet = m_Regions[sid].SumEt();
  double seet = m_Regions[seid].SumEt();

  if ( cenet > nwet &&  cenet > noet &&
       cenet > neet &&  cenet > eaet &&
       cenet > weet &&  cenet > soet &&
       cenet > swet &&  cenet > seet ) 
    {

      double cene = m_Regions.at(crgn).SumE();
      double nwe =  m_Regions[nwid].SumE();
      double noe = m_Regions[nid].SumE();
      double nee = m_Regions[neid].SumE();
      double wee = m_Regions[wid].SumE();
      double eae = m_Regions[eid].SumE();
      double swe = m_Regions[swid].SumE();
      double soe = m_Regions[sid].SumE();
      double see = m_Regions[seid].SumE();

      // if region is central: jet energy is sum of 3x3 region
      // surrounding the central region
      double jE = cene + nwe + noe + nee + wee + eae + swe + soe + see;
      double jEt = cenet + nwet + noet + neet + weet + eaet + swet + soet + seet;


      m_Regions.at(crgn).SetJetE(jE);
      m_Regions.at(crgn).SetJetEt(jEt);

      m_Regions.at(crgn).SetJetE3x3(cene);
      m_Regions.at(crgn).SetJetEt3x3(cenet);
      
      return true;
    } else { return false; }

}


void
FastL1GlobalAlgo::checkMapping() {

  // loop over towers ieta,iphi
  for (int j=1;j<=72;j++) {
    for (int i=-28; i<=28; i++) {
      if (i==0) continue;
      int iRgn =  m_RMap->getRegionIndex(i,j);
      std::pair<double, double> RgnEtaPhi = m_RMap->getRegionCenterEtaPhi(iRgn);
      //int iTwr = m_RMap->getRegionTowerIndex(i,j);
      std::pair<int, int> iRgnEtaPhi = m_RMap->getRegionEtaPhiIndex(iRgn);
      std::pair<int, int> iRgnEtaPhi2 = m_RMap->getRegionEtaPhiIndex(std::pair<int, int>(i,j));

      std::cout<<"---------------------------------------------------------------------------"<<std::endl;
      std::cout<<"Region:   "<<iRgn<<" | "<<RgnEtaPhi.first<<", "<<RgnEtaPhi.second*180./3.141<<std::endl;
      std::cout<<"   -      "<<iRgnEtaPhi.first<<", "<<iRgnEtaPhi.second<<std::endl;
      std::cout<<"   -      "<<iRgnEtaPhi2.first<<", "<<iRgnEtaPhi2.second<<std::endl;
      std::cout<<" Tower:   "<<i<<", "<<m_RMap->convertFromECal_to_HCal_iphi(j)<<std::endl;
      std::cout<<" TowerId: "<<m_RMap->getRegionTowerIndex(i,j)<<std::endl;

    }
  }

}

