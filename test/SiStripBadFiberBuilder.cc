#include "CalibTracker/SiStripQuality/test/SiStripBadFiberBuilder.h"

#include "CalibTracker/SiStripCommon/interface/SiStripDetInfoFileReader.h"

#include <iostream>
#include <fstream>


SiStripBadFiberBuilder::SiStripBadFiberBuilder(const edm::ParameterSet& iConfig) : ConditionDBWriter<SiStripBadStrip>::ConditionDBWriter<SiStripBadStrip>(iConfig){

  edm::LogInfo("SiStripBadFiberBuilder") << " ctor ";
  fp_ = iConfig.getUntrackedParameter<edm::FileInPath>("file",edm::FileInPath("CalibTracker/SiStripCommon/data/SiStripDetInfo.dat"));
  printdebug_ = iConfig.getUntrackedParameter<bool>("printDebug",false);
  BadModuleList_ = iConfig.getUntrackedParameter<std::vector<uint32_t> >("BadModuleList");
}


SiStripBadFiberBuilder::~SiStripBadFiberBuilder(){
  edm::LogInfo("SiStripBadFiberBuilder") << " dtor";
}

void SiStripBadFiberBuilder::algoAnalyze(const edm::Event & event, const edm::EventSetup& iSetup){
  
  edm::LogInfo("SiStripBadFiberBuilder") <<"SiStripBadFiberBuilder::algoAnalyze called"<<std::endl;
  unsigned int run=event.id().run();

  edm::LogInfo("SiStripBadFiberBuilder") << "... creating dummy SiStripBadStrip Data for Run " << run << "\n " << std::endl;
  
  obj = new SiStripBadStrip();

  SiStripDetInfoFileReader reader(fp_.fullPath());
  
  const std::vector<uint32_t> DetIds = reader.getAllDetIds();
  
  //  for(std::vector<uint32_t>::const_iterator it=DetIds.begin(); it!=DetIds.end(); ++it){
  for(std::vector<uint32_t>::const_iterator it=BadModuleList_.begin(); it!=BadModuleList_.end(); ++it){
  
    std::vector<unsigned int> theSiStripVector;
   
    //Generate bad channels for det detid: just for testing: channels 1, 37 , 258-265, 511 are always bad
    
    unsigned int firstBadStrip=999;
    unsigned short NconsecutiveBadStrips=0;

    int NFibers=reader.getNumberOfApvsAndStripLength(*it).first/2;

    if ( (*it>>2) % 4 == 0 ){
      firstBadStrip=0;
      NconsecutiveBadStrips=256;
    } else if   ( (*it>>2) % 4 == 1 ) {
      firstBadStrip=256;
      NconsecutiveBadStrips=256;
      if (NFibers==3){
	NconsecutiveBadStrips=512;
      }
    } else  if   ( (*it>>2) % 4 == 2 ) {
      firstBadStrip=0;
      NconsecutiveBadStrips=512;
    } else {
      firstBadStrip=256;
      NconsecutiveBadStrips=256;
    }
      
    unsigned int theBadStripRange = obj->encode(firstBadStrip,NconsecutiveBadStrips);
    
    if (printdebug_)
      edm::LogInfo("SiStripBadFiberBuilder") << "detid " << *it << " \t"
					     << " firstBadStrip " << firstBadStrip << "\t "
					     << " NconsecutiveBadStrips " << NconsecutiveBadStrips << "\t "
					     << " packed integer " << std::hex << theBadStripRange  << std::dec
					     << std::endl; 	    
    
    theSiStripVector.push_back(theBadStripRange);
    NconsecutiveBadStrips=0;
    firstBadStrip=999;
            
    SiStripBadStrip::Range range(theSiStripVector.begin(),theSiStripVector.end());
    if ( ! obj->put(*it,range) )
      edm::LogError("SiStripBadFiberBuilder")<<"[SiStripBadFiberBuilder::analyze] detid already exists"<<std::endl;
  }
}


