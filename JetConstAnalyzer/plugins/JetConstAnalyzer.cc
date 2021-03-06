// -*- C++ -*-
//
// Package:    JetInfo/JetConstAnalyzer
// Class:      JetConstAnalyzer
// 
/**\class JetConstAnalyzer JetConstAnalyzer.cc JetInfo/JetConstAnalyzer/plugins/JetConstAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  clint richardson
//         Created:  Sat, 22 Oct 2016 21:52:53 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <TTree.h>

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class JetConstAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit JetConstAnalyzer(const edm::ParameterSet&);
      ~JetConstAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;
      edm::EDGetTokenT<pat::JetCollection> jetToken_;
      edm::EDGetTokenT<edm::View<reco::GenParticle> > prunedGenToken_;
      // ----------member data ---------------------------
      TTree* tree;
  std::vector<float> JetPt;
  std::vector<float> JetEta;
  std::vector<float> JetPhi;
  std::vector<int> nJetConst;
  std::vector<float> ptConsts;
  std::vector<float> etaConsts;
  std::vector<float> phiConsts;
  std::vector<float> Tau1;
  std::vector<float> Tau2;
  std::vector<float> Tau3;
  std::vector<float> SDMass;
  std::vector<float> PrunedMass;
  std::string signalType;
  bool data;
  bool sigW;
  bool sigTop;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
JetConstAnalyzer::JetConstAnalyzer(const edm::ParameterSet& iConfig):
  jetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jetCollection"))),
  prunedGenToken_(consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("genParticles"))),
  signalType(iConfig.getParameter<std::string>("signalType")),
  data(iConfig.getParameter<bool>("isData"))
{
   //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  tree = fs->make<TTree>("JetVariables", "JetVariables");
  tree->Branch("JetPt",&JetPt);
  tree->Branch("JetEta",&JetEta);
  tree->Branch("JetPhi",&JetPhi);
  tree->Branch("nJetConst",&nJetConst);
  tree->Branch("ptConsts",&ptConsts);
  tree->Branch("etaConsts",&etaConsts);
  tree->Branch("phiConsts",&phiConsts);
  tree->Branch("Tau1",&Tau1);
  tree->Branch("Tau2",&Tau2);
  tree->Branch("Tau3",&Tau3);
  tree->Branch("SDMass",&SDMass);
  tree->Branch("PrunedMass",&PrunedMass);

   //check signal type
   sigW = signalType=="WJets" ? true : false;
   sigTop = signalType=="Topjets" ? true : false;
   if(sigTop) std::cout<<"runing for top jets"<<std::endl;


}


JetConstAnalyzer::~JetConstAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
JetConstAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;
   //clea vector content
   JetPt.clear();
   JetEta.clear();
   JetPhi.clear();
   nJetConst.clear();
   ptConsts.clear();
   etaConsts.clear();
   phiConsts.clear();
   //load jets
   edm::Handle<pat::JetCollection> jets;
   iEvent.getByToken(jetToken_, jets);

   //load gen particles
   // Pruned particles are the one containing "important" stuff
   Handle<edm::View<reco::GenParticle> > genParticles;
   iEvent.getByToken(prunedGenToken_,genParticles);

   //loop over jets
   for (const pat::Jet &ijet : *jets) {
     //make check on type of jet - only save data, actual boosted jets, or background
     bool saveJet=false;
     if(data) saveJet=true;
     else{
       if(sigW){
	 //check for Wjets
	 for(size_t i = 0; i < genParticles->size(); i++){
	   const reco::GenParticle & p = (*genParticles).at(i);
	   if(! ( fabs( p.pdgId()) ==24)) continue; //only check Ws
	   if(! (p.statusFlags().isPrompt() || p.isPromptDecayed())) continue; //only check for prompt Ws	   
	   float dR = deltaR(ijet.eta(),ijet.phi(),p.eta(),p.phi());
	   if( dR < 0.8 ) {
	     //check daughters
	     if(p.numberOfDaughters()<2) continue; //skip for w's which have radiation
	     else{
	       if( (abs(p.daughter(0)->pdgId()) > 10 && abs(p.daughter(0)->pdgId()) < 17) || abs(p.daughter(0)->pdgId())==24) {saveJet = false; continue;}//skip leptonic decays and radiation steps
	       else if( (abs(p.daughter(1)->pdgId()) > 10 && abs(p.daughter(1)->pdgId()) < 17) || abs(p.daughter(1)->pdgId())==24) {saveJet = false; continue;}
	       else{saveJet=true; std::cout<<"saving top jet with delta R: "<<dR<<std::endl;break;} //save jet and stop loop
	     }
	   }//end dR check
	 }//end loop over gen particles
       }//end check for matched W
       else if(sigTop){
	 //check for TOPjets
	 for(size_t i = 0; i < genParticles->size(); i++){
	   const reco::GenParticle & p = (*genParticles).at(i);
	   if(! ( fabs( p.pdgId()) ==6)) continue; //only check TOPs
	   if(! (p.statusFlags().isPrompt() || p.isPromptDecayed())) continue; //only check for prompt TOPs	   
	   float dR = deltaR(ijet.eta(),ijet.phi(),p.eta(),p.phi());
	   if( dR < 0.8 ) {
	     size_t nDs = p.numberOfDaughters();
	     for(size_t j = 0; j < nDs; j++){
	       int dauId = (p.daughter(j))->pdgId();
	       const reco::Candidate *d = p.daughter(j);	       
	       if(abs(dauId) == 24){  // check t->Wb->leptons 
		 if(d->numberOfDaughters()<2) {saveJet = false; continue;}
		 else{
		   if( (abs(d->daughter(0)->pdgId()) > 10 && abs(d->daughter(0)->pdgId()) < 17) || abs(d->daughter(0)->pdgId())==24) {saveJet = false; continue;}//skip leptonic decays and radiation steps
		   else if( (abs(d->daughter(1)->pdgId()) > 10 && abs(d->daughter(1)->pdgId()) < 17) || abs(d->daughter(1)->pdgId())==24) {saveJet = false; continue;}
	           else{saveJet=true; std::cout<<"saving top jet with delta R: "<<dR<<std::endl;break;} //save jet and stop loop
		 }
	       }
	     }//end loop over daughters	     
	   }//end if dR <0.8
	   if(saveJet) break; //break loop over gen particles if we are saving the jet
	 }//end loop over gen particles
       }//end check for matched TOP
       else { saveJet = true;} //qcd background

     }//end check for mc

     if(!saveJet) continue;

     //save jet info
     JetPt.push_back(ijet.pt());
     JetEta.push_back(ijet.eta());
     JetPhi.push_back(ijet.phi());
     Tau1.push_back(ijet.userFloat("NjettinessAK8:tau1"));
     Tau2.push_back(ijet.userFloat("NjettinessAK8:tau2"));
     Tau3.push_back(ijet.userFloat("NjettinessAK8:tau3"));
     SDMass.push_back(ijet.userFloat("ak8PFJetsCHSSoftDropMass"));
     PrunedMass.push_back(ijet.userFloat("ak8PFJetsCHSPrunedMass"));

     std::vector<reco::Candidate const *> constituents;
     for ( unsigned ida = 0; ida < ijet.numberOfDaughters(); ++ida ) {
       reco::Candidate const * cand = ijet.daughter(ida);
       if ( cand->numberOfDaughters() == 0 )
	 constituents.push_back( cand ) ;
       else {
	 for ( unsigned jda = 0; jda < cand->numberOfDaughters(); ++jda ) {
	   reco::Candidate const * cand2 = cand->daughter(jda);
	   constituents.push_back( cand2 );
	 }
       }
     }
     //std::sort( constituents.begin(), constituents.end(), [] (reco::Candidate const * ida, reco::Candidate const * jda){return ida->pt() > jda->pt();} );
     //save number of constituents
     nJetConst.push_back(constituents.size());
     for ( unsigned int ida = 0; ida < constituents.size(); ++ida ) {
       const pat::PackedCandidate &cand = dynamic_cast<const pat::PackedCandidate &>(*constituents[ida]);
       float ptrel = cand.pt()/ijet.pt();
       float etarel = ijet.eta() - cand.eta();
       float phirel = ijet.phi() - cand.phi();
       ptConsts.push_back(ptrel);
       etaConsts.push_back(etarel);
       phiConsts.push_back(phirel);
     }//end loop over jet const

   }//end loop over jets

   //fill tree if we've saved any jets
   if(JetPt.size()>0) tree->Fill();


}


// ------------ method called once each job just before starting event loop  ------------
void 
JetConstAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
JetConstAnalyzer::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
JetConstAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jetCollection",edm::InputTag("slimmedAK8Jets"));
  desc.add<edm::InputTag>("genParticles",edm::InputTag("prunedGenParticles"));
  desc.add<std::string>("signalType","Topjets");
  desc.add<bool>("isData",false);
  descriptions.addDefault(desc);//
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetConstAnalyzer);
