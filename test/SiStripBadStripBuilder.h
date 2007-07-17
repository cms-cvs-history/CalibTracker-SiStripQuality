#ifndef SiStripBadStrip_H
#define SiStripBadStrip_H

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"


class SiStripBadStripBuilder : public edm::EDAnalyzer {
  
 public:

  explicit SiStripBadStripBuilder( const edm::ParameterSet& iConfig);
  
  ~SiStripBadStripBuilder(){};
  
  virtual void beginJob( const edm::EventSetup& );
  
  virtual void analyze(const edm::Event& , const edm::EventSetup& );
  
 private:
  bool printdebug_;
  std::vector< std::pair<uint32_t, unsigned short> > detid_strips;
};

#endif

