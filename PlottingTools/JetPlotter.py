#use with /usr/bin/python

import math
import numpy as np

from ROOT import *
gStyle.SetOptStat(False)
gStyle.SetPalette(kBird)
f = TFile("JetInfo-TopJets.root")
t = f.Get("demo/JetVariables");
c = TCanvas()
c.SetLogz()
it=0
for event in t:
#    if it!=0:
#        continue
    ptJets = event.JetPt
    etaJets = event.JetEta
    phiJets = event.JetPhi
    nConstJets = event.nJetConst
    nSubJet = nConstJets[0]
    hist = TH2F("hist","Jet;#eta;#phi",133,-1,1,133,-1,1)
    #initialize histogram to small background
    for i in np.arange(-0.8,0.8,0.015):
        for j in np.arange(-0.8,0.8,0.015):
            r =  math.sqrt(i*i+j*j)
            #print "R: %f" %r
            if(r<=0.8):
                hist.Fill(i,j,0.0004)
    for i in range(0,nSubJet):
        pt = event.ptConsts[i]
        eta = event.etaConsts[i]
        phi = event.phiConsts[i]
        hist.Fill(eta,phi,pt)
        #print "Subjet pt weight: %f, eta: %f, phi: %f" %(pt,eta,phi)
    hist.Draw("colz0")
    c.Print("Jet_%i.pdf" %it)
    it+=1
