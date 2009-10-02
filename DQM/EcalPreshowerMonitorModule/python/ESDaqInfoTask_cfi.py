import FWCore.ParameterSet.Config as cms

ecalEndcapDaqInfoTask = cms.EDAnalyzer("ESDaqInfoTask",
      esMapping = cms.PSet(LookupTable = cms.untracked.FileInPath("EventFilter/ESDigiToRaw/data/ES_lookup_table.dat")),
      prefixME = cms.untracked.string('EcalPreshower'),
      enableCleanup = cms.untracked.bool(False),
      mergeRuns = cms.untracked.bool(False),
      ESFedRangeMin = cms.untracked.int32(520),
      ESFedRangeMax = cms.untracked.int32(575)
)

