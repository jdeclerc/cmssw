import FWCore.ParameterSet.Config as cms

ecalPreshowerRecHitsValidation = cms.EDAnalyzer("EcalPreshowerRecHitsValidation",
    EErechitCollection = cms.InputTag("ecalRecHit","EcalRecHitsEE"),
    ESrechitCollection = cms.InputTag("ecalPreshowerRecHit","EcalRecHitsES"),
    EEuncalibrechitCollection = cms.InputTag("ecalMultiFitUncalibRecHit","EcalUncalibRecHitsEE"),
    verbose = cms.untracked.bool(False)
)



