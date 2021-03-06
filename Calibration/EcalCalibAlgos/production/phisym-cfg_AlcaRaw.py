# cfg for phi symmetry calibration
# from ALCARAW applying laser corrections
#

import FWCore.ParameterSet.Config as cms

import RecoLocalCalo.EcalRecProducers.ecalRecalibRecHit_cfi 

process=cms.Process("PHISYM")

process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_cff')
process.load('Configuration/StandardSequences/GeometryPilot2_cff')
process.load('Calibration/EcalAlCaRecoProducers/alcastreamEcalPhiSym_cff' )
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cff")
process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')

process.load('FWCore/MessageService/MessageLogger_cfi')
process.MessageLogger.cerr = cms.untracked.PSet(placeholder =
cms.untracked.bool(True))
process.MessageLogger.cout = cms.untracked.PSet(INFO = cms.untracked.PSet(
    reportEvery = cms.untracked.int32(100000) # every 100th only
#    limit = cms.untracked.int32(10)       # or limit to 10 printouts...
    ))

isMC=False
recalib=True
runFromUncalibrator=True

if (not isMC):
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(100000)
        )
else:
    process.maxEvents = cms.untracked.PSet(
        input = cms.untracked.int32(400000)
        )
    
process.source = cms.Source("PoolSource",
#                            fileNames = cms.untracked.vstring('root://pccmsrm27///cms/local/meridian/phiSimmetry/Commisioning12_189770.root')
#                            fileNames = cms.untracked.vstring('/store/data/Commissioning12/AlCaPhiSym/RAW/v1/000/190/389/DA64A111-AC7E-E111-A2EF-003048F024DC.root')
#                            fileNames = cms.untracked.vstring('/store/data/Commissioning12/AlCaPhiSym/RAW/v1/000/190/389/6648778C-B47E-E111-9512-BCAEC518FF40.root')
                            fileNames = cms.untracked.vstring('/store/data/Run2012A/AlCaPhiSym/RAW/v1/000/190/482/D08E58E5-4B7F-E111-8BBD-003048D2C0F4.root')
#                            fileNames = cms.untracked.vstring('/store/relval/CMSSW_5_2_3/RelValMinBias/GEN-SIM-RECO/START52_V5-v1/0043/36B29A9E-2B7A-E111-98DB-003048678B7C.root')
                            )


process.phisymcalib = cms.EDAnalyzer("PhiSymmetryCalibration",
                                     ecalRecHitsProducer = cms.string("hltAlCaPhiSymStream"),
                                     barrelHitCollection = cms.string("phiSymEcalRecHitsEB"),
                                     endcapHitCollection = cms.string("phiSymEcalRecHitsEE"),
                                     eCut_barrel = cms.double(0.250),
                                     eCut_endcap = cms.double(0.000),
                                     ap = cms.double( -0.150),
                                     am = cms.double( -0.150),
                                     b  = cms.double(  0.600),
                                     cPhiKFactor = cms.bool(True),
                                     eventSet = cms.int32(1),
                                     statusThreshold = cms.untracked.int32(0)
                                     )

if (recalib):
    process.phisymcalib.ecalRecHitsProducer=cms.string("ecalPhiSymCorrected")
if (isMC):
    process.phisymcalib.ecalRecHitsProducer=cms.string("ecalRecHit")
    process.phisymcalib.barrelHitCollection = cms.string("EcalRecHitsEB")
    process.phisymcalib.endcapHitCollection = cms.string("EcalRecHitsEE")

# the xml file must be put in 
# $CMSSW_BASE/src/CalibCalorimetry/CaloMiscalibTools/data

#process.CaloMiscalibTools = cms.ESSource("CaloMiscalibTools",
#  fileNameEndcap = cms.untracked.string('EcalIntercalibConstants_2011B_Sept_a.xml'),
#  fileNameBarrel = cms.untracked.string('EcalIntercalibConstants_2011B_Sept_a.xml')
#  )
#process.prefer("CaloMiscalibTools")

#apply calibrations
process.ecalPhiSymCorrected =  RecoLocalCalo.EcalRecProducers.ecalRecalibRecHit_cfi.ecalRecHit.clone(
            doEnergyScale = cms.bool(True),
            doIntercalib = cms.bool(True),
            EERecHitCollection = cms.InputTag("hltAlCaPhiSymStream","phiSymEcalRecHitsEE"),
            EBRecHitCollection = cms.InputTag("hltAlCaPhiSymStream","phiSymEcalRecHitsEB"),
            doLaserCorrections = cms.bool(True),
            EBRecalibRecHitCollection = cms.string('phiSymEcalRecHitsEB'),
            EERecalibRecHitCollection = cms.string('phiSymEcalRecHitsEE')
)

if (runFromUncalibrator):
    process.ecalPhiSymCorrected.EBRecHitCollection = cms.InputTag("hltAlCaPhiSymUncalibrator","phiSymEcalRecHitsEB")
    process.ecalPhiSymCorrected.EERecHitCollection = cms.InputTag("hltAlCaPhiSymUncalibrator","phiSymEcalRecHitsEE")

process.GlobalTag.globaltag = 'GR_P_V32::All'

##############

#process.GlobalTag.toGet = cms.VPSet(
# 
#   cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
#            tag = cms.string("EcalIntercalibConstants_V20111021_PROMPT_relativeToHLTconstants"),
#            connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
#           ),
#	   
##   cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
##	     tag = cms.string("EcalLaserAPDPNRatios_2011fit_noVPT_nolim_online"),
##	     connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_ECAL") 
##	     )
#
#
#cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
##            tag = cms.string("EcalLaserAPDPNRatios_data_20111010_158851_177954"),
#            tag = cms.string("EcalLaserAPDPNRatios_fit_20110830_160400_175079"),
#            connect = cms.untracked.string("frontier://FrontierPrep/CMS_COND_ECAL") 
#            )
#
##cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
##            tag = cms.string("EcalLaserAPDPNRatios_fit_20110830_offline"),
##            connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS") 
##            )
# )
################

process.p = cms.Path(process.ecalphiSymHLT*process.offlineBeamSpot)
if (recalib):
    process.p *= process.ecalPhiSymCorrected
process.p *= process.phisymcalib
