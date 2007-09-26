#ifndef SiStripBadStripbyHand_H
#define SiStripBadStripbyHand_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"


class SiStripBadStripByHandBuilder : public edm::EDAnalyzer {

 public:

  explicit SiStripBadStripByHandBuilder( const edm::ParameterSet& iConfig);

  ~SiStripBadStripByHandBuilder(){};

  virtual void beginJob( const edm::EventSetup& );

  virtual void analyze(const edm::Event& , const edm::EventSetup& );

 private:
  bool printdebug_;
  std::vector<uint32_t> ext_bad_detids;
  std::vector< std::pair<uint32_t, unsigned short> > detid_strips;
};

#endif
