from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'JetTagging-TTbar'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'JetInfo_TTbar_cfg.py'

config.Data.inputDataset = '/TT_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISpring16MiniAODv2-PUSpring16_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext3-v2/MINIAODSIM'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
#config.Data.lumiMask = 'json_STEAM_2016E_ParkedZeroBias_1e34_LS.txt'
#config.Data.runRange = '259721' # '193093-194075'
config.Data.outLFNDirBase = '/store/user/%s/JetTagging/' % (getUsernameFromSiteDB())
#config.Data.publication = True
#config.Data.outputDatasetTag = 'L1Menu_2016_v6'

config.Site.storageSite = 'T3_US_FNALLPC'
