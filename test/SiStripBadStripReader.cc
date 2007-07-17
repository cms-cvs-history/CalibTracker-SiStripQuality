#include "CondFormats/SiStripObjects/interface/SiStripBadStrip.h"
#include "CondFormats/DataRecord/interface/SiStripBadStripRcd.h"

#include "CalibTracker/SiStripQuality/test/SiStripBadStripReader.h"

#include <iostream>
#include <stdio.h>
#include <sys/time.h>

SiStripBadStripReader::SiStripBadStripReader( const edm::ParameterSet& iConfig ):
  printdebug_(iConfig.getUntrackedParameter<bool>("printDebug",false)){}

SiStripBadStripReader::~SiStripBadStripReader(){}

void SiStripBadStripReader::analyze( const edm::Event& e, const edm::EventSetup& iSetup){

  edm::ESHandle<SiStripBadStrip> SiStripBadStrip_;
  iSetup.get<SiStripBadStripRcd>().get(SiStripBadStrip_);
  edm::LogInfo("SiStripBadStripReader") << "[SiStripBadStripReader::analyze] End Reading SiStripBadStrip" << std::endl;
  
  std::vector<uint32_t> detid;
  SiStripBadStrip_->getDetIds(detid);
  
  if (printdebug_)
    for (size_t id=0;id<detid.size();id++)
      {
	SiStripBadStrip::Range range=SiStripBadStrip_->getRange(detid[id]);
	
	for(int it=0;it<range.second-range.first;it++){
	  unsigned int value=(*(range.first+it));
	  edm::LogInfo("SiStripBadStripReader")  << "detid " << detid[id] << " \t"
						 << " firstBadStrip " <<  ((value >> 16)  & 0xFFFF) << "\t "
						 << " NconsecutiveBadStrips " << (value & 0xFFFF)  << "\t "
						 << " packed integer " <<  std::hex << value << std::dec << "\t "
	    //<< SiStripBadStrip_->getBadStrips(range)     << " \t"
						 << std::endl;
	} 
      }
}
