#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>


void TreeToCSV(){

  //make output file
  std::ofstream outfile;
  outfile.open("JetInfo_cc.csv");

  TFile* f = new TFile("JetInfo-QCD.root");
  TTree* t = (TTree*) f->Get("JetInfo/JetVariables");
  //load data
  std::vector<float>* jetPt =0;
  TBranch* b_jetpt;
  t->SetBranchAddress("JetPt",&jetPt,&b_jetpt);
  std::vector<float>* jetEta =0;
  TBranch* b_jeteta;
  t->SetBranchAddress("JetEta",&jetEta,&b_jeteta);
  std::vector<float>* jetPhi =0;
  TBranch* b_jetphi;
  t->SetBranchAddress("JetPhi",&jetPhi,&b_jetphi);
  std::vector<int>* nConst =0;
  TBranch* b_nConst;
  t->SetBranchAddress("nJetConst",&nConst,&b_nConst);
  std::vector<float>* constPhi =0;
  TBranch* b_constPhi;
  t->SetBranchAddress("phiConsts",&constPhi,&b_constPhi);
  std::vector<float>* constEta =0;
  TBranch* b_constEta;
  t->SetBranchAddress("etaConsts",&constEta,&b_constEta);
  std::vector<float>* constPt =0;
  TBranch* b_constPt;
  t->SetBranchAddress("ptConsts",&constPt,&b_constPt);

  std::vector<float>* Tau1 =0;
  TBranch* b_Tau1;
  t->SetBranchAddress("Tau1",&Tau1,&b_Tau1);
  std::vector<float>* Tau2 =0;
  TBranch* b_Tau2;
  t->SetBranchAddress("Tau2",&Tau2,&b_Tau2);
  std::vector<float>* Tau3 =0;
  TBranch* b_Tau3;
  t->SetBranchAddress("Tau3",&Tau3,&b_Tau3);
  std::vector<float>* SDMass =0;
  TBranch* b_SDMass;
  t->SetBranchAddress("SDMass",&SDMass,&b_SDMass);
  std::vector<float>* PrunedMass =0;
  TBranch* b_PrunedMass;
  t->SetBranchAddress("PrunedMass",&PrunedMass,&b_PrunedMass);

  
  int nEntries = t->GetEntries();
  for(int i =0; i< nEntries; i++){
    t->GetEntry(i);
    std::stringstream line;
    //add background bit
    line<<"0,";
    TH2F* h = new TH2F("h","h",20,-1,1,20,-1,1);
    for(int j =0 ; j<(*nConst)[0];j++){
      float pt = (*constPt)[j];
      float eta = (*constEta)[j];
      float phi = (*constPhi)[j];      
      h->Fill(eta,phi,pt);
    } 
    for(int ieta =0; ieta<h->GetNbinsX(); ieta++){
      for(int iphi=0; iphi<h->GetNbinsX(); iphi++){
	line<<h->GetBinContent(ieta+1,iphi+1)<<",";
      }
    }
    line<<(*jetPt)[0]<<","<<(*Tau1)[0]<<","<<(*Tau2)[0]<<","<<(*Tau3)[0]<<","<<(*SDMass)[0]<<","<<(*PrunedMass)[i]<<std::endl;
    outfile<<line.str();
    delete h;
  }

  outfile.close();

}
