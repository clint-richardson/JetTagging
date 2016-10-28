import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.TFileService = cms.Service("TFileService", 
      fileName = cms.string("JetInfo-TopJets.root"),
#      closeFileFast = cms.untracked.bool(True)
  )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'root://cmsxrootd.fnal.gov//store/mc/RunIISpring16MiniAODv2/TT_TuneCUETP8M1_13TeV-powheg-pythia8/MINIAODSIM/PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext3-v2/70000/00287FF4-0E40-E611-8D06-00266CFE78EC.root'
    )
)

process.JetInfo = cms.EDAnalyzer('JetConstAnalyzer',
                              jetCollection = cms.InputTag("slimmedJetsAK8"),
                              genParticles = cms.InputTag("prunedGenParticles"),
                              signalType = cms.string("Topjets"),
                              isData = cms.bool(False),
)


process.p = cms.Path(process.JetInfo)
