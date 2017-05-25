#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <math.h>  
#include "TMath.h"
#include "TBranch.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
void TreeToCSV(){
  TString sample="qcd";
  //make output file
  std::ofstream outfile;
  std::string outname = sample == "ttbar" ? "JetInfo_TopJets.csv" : "JetInfo_qcd.csv";
  TString pdfname = sample == "ttbar" ? "TTbar_Energy.pdf" : "QCD_Energy.pdf";
  TString rotpdfname = sample == "ttbar" ? "TTbar_Energy_Rotated.pdf" : "QCD_Energy_Rotated.pdf";
  TString flippdfname = sample == "ttbar" ? "TTbar_Energy_Flipped.pdf" : "QCD_Energy_Flipped.pdf";
  TString thetapdfname = sample == "ttbar" ? "Theta_TTbar.pdf" : "Theta_QCD.pdf";
  TString subthetapdfname = sample == "ttbar" ? "subTheta_TTbar.pdf" : "subTheta_QCD.pdf";
  TString diffthetapdfname = sample == "ttbar" ? "diffTheta_TTbar.pdf" : "diffTheta_QCD.pdf";
  outfile.open(outname);

  TString infile = sample=="ttbar" ? "JetInfo-TopJets_ttbar.root" : "JetInfo_qcd.root";

  TFile* f = new TFile(infile);
  TTree* t = (TTree*) f->Get("demo/JetVariables");
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
  TH2F* h_tot = new TH2F("h_tot","Energy;#eta;#phi",25,-1,1,25,-1,1);
  TH2F* h_tot_rot = new TH2F("h_tot_rot","Energy;#eta;#phi",25,-1,1,25,-1,1);
  TH2F* h_tot_flip = new TH2F("h_tot_flip","Energy;#eta;#phi",25,-1,1,25,-1,1);
  TH1F* h_theta = new TH1F("h_theta","Rotation of max energy const",25,0,6.25);
  TH1F* h_subtheta = new TH1F("h_subtheta","Rotation of sub-leading energy const",25,0,6.25);
  TH1F* h_difftheta = new TH1F("h_difftheta","#Delta#theta leading, sub-leading",25,0,6.25);
  float totconst=0;
  for(int i =0; i< nEntries; i++){
    if(i%1000==0) std::cout<<"Completed "<<i<<" out of "<<nEntries<<" entries"<<std::endl;
    t->GetEntry(i);
    //loop over each jet in event
    for(int k=0; k<jetPt->size();k++){
      //skip for jets outside pT range
      if( (*jetPt)[k] < 470 || (*jetPt)[k] > 600) continue;
      std::stringstream line;
      //add background bit
      if(sample=="ttbar")line<<"1,";
      else line<<"0,";
      TH2F* h = new TH2F("h","h",20,-1,1,20,-1,1);
      totconst = totconst + (*nConst)[k];
      int start =0;
      int end =0;
      if(k==0){
	start = 0;
	end = (*nConst)[0];
      }
      else{
	for(int ijet=0; ijet<k;ijet++){
	  start = start + (*nConst)[ijet];
	  end = end + (*nConst)[ijet];
	}
	end = end + (*nConst)[k];
      }

      //find highest momentum constituent
      float maxpt=0;
      float subpt=0;
      float maxeta=-99; float maxphi=-99;
      float subeta=-99; float subphi=-99;
      for(int icon = start; icon < end;icon++){
	if( (*constPt)[icon] > maxpt){ maxpt = (*constPt)[icon]; maxeta = (*constEta)[icon]; maxphi = (*constPhi)[icon];}	
      }
      for(int icon = start; icon < end;icon++){
	if( (*constPt)[icon] > subpt && (*constPt)[icon] < maxpt){ subpt = (*constPt)[icon]; subeta = (*constEta)[icon]; subphi = (*constPhi)[icon];}	
      }
      
      //make rotation angle
      float theta = atan2(maxphi,maxeta);
      //max rotation angle for subleading const
      float subtheta = atan2(subphi,subeta);
      float difftheta = theta-subtheta;
      if(theta<0) theta = 6.28+theta;
      if(subtheta<0) subtheta = 6.28+subtheta;
      h_theta->Fill(theta);
      h_subtheta->Fill(subtheta);
      h_difftheta->Fill(difftheta);

      //now make theta-> -theta so we rotate back
      theta = -1.0*theta;
      float costheta = TMath::Cos(theta);
      float sintheta = TMath::Sin(theta);
      float topEn=0;
      float botEn=0;

      for(int j =start ; j<end;j++){
	float pt = (*constPt)[j];
	float etatemp = (*constEta)[j];
	float phitemp = (*constPhi)[j];
	//now rotate
	float eta = etatemp*costheta - phitemp*sintheta;
	float phi = etatemp*sintheta + phitemp*costheta;
	
	h->Fill(eta,phi,pt);
	h_tot->Fill(etatemp,phitemp,pt);
	h_tot_rot->Fill(eta,phi,pt);
	//check energy
	if( phi > 0) topEn += pt;
	else botEn +=pt;
      }

      //now make flipped version
      for(int m =start ; m<end;m++){
	float pt = (*constPt)[m];
	float etatemp = (*constEta)[m];
	float phitemp = (*constPhi)[m];
	//now rotate
	float eta = etatemp*costheta - phitemp*sintheta;
	float phi = etatemp*sintheta + phitemp*costheta;
	if(botEn > topEn) phi = -1.0*phi; //flip picture if more energy is in bottom half of jet;
	h_tot_flip->Fill(eta,phi,pt);
	//check energy
	if( eta > 0) topEn += pt;
	else botEn +=pt;
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
  }

  outfile.close();
  //float avgconst = totconst/20000.;
  //std::cout<<"average number of constitutents: "<<avgconst<<std::endl;
  TCanvas* c = new TCanvas();
  h_tot->Draw("colz");
  c->Print(pdfname);
  h_tot_rot->Draw("colz");
  c->Print(rotpdfname);
  h_tot_flip->Draw("colz");
  c->Print(flippdfname);

  h_theta->Draw();
  c->Print(thetapdfname);
  h_subtheta->Draw();
  c->Print(subthetapdfname);
  h_difftheta->Draw();
  c->Print(diffthetapdfname);


}
