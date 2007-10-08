#include "CalibTracker/SiStripQuality/test/SiStripBadChannelBuilder.h"

#include "CalibTracker/SiStripCommon/interface/SiStripDetInfoFileReader.h"

#include <iostream>
#include <fstream>


SiStripBadChannelBuilder::SiStripBadChannelBuilder(const edm::ParameterSet& iConfig) : ConditionDBWriter<SiStripBadStrip>::ConditionDBWriter<SiStripBadStrip>(iConfig){

  edm::LogInfo("SiStripBadChannelBuilder") << " ctor ";
  fp_ = iConfig.getUntrackedParameter<edm::FileInPath>("file",edm::FileInPath("CalibTracker/SiStripCommon/data/SiStripDetInfo.dat"));
  printdebug_ = iConfig.getUntrackedParameter<bool>("printDebug",false);
  BadModuleList_ = iConfig.getUntrackedParameter<std::vector<uint32_t> >("BadModuleList");
  BadChannelList_ = iConfig.getUntrackedParameter<std::vector<uint32_t> >("BadChannelList");
}


SiStripBadChannelBuilder::~SiStripBadChannelBuilder(){
  edm::LogInfo("SiStripBadChannelBuilder") << " dtor";
}

void SiStripBadChannelBuilder::algoAnalyze(const edm::Event & event, const edm::EventSetup& iSetup){
  
  edm::LogInfo("SiStripBadChannelBuilder") <<"SiStripBadChannelBuilder::algoAnalyze called"<<std::endl;
  unsigned int run=event.id().run();

  edm::LogInfo("SiStripBadChannelBuilder") << "... creating dummy SiStripBadStrip Data for Run " << run << "\n " << std::endl;
  
  obj = new SiStripBadStrip();

  SiStripDetInfoFileReader reader(fp_.fullPath());
  
  const std::vector<uint32_t> DetIds = reader.getAllDetIds();
  
  //for(std::vector<uint32_t>::const_iterator it=DetIds.begin(); it!=DetIds.end(); ++it){
  for(std::vector<uint32_t>::const_iterator it=BadModuleList_.begin(); it!=BadModuleList_.end(); ++it){
    
    std::vector<unsigned int> theSiStripVector;
    int NStrips=reader.getNumberOfApvsAndStripLength(*it).first*128;   
    
    unsigned short lastBad=999;
    unsigned short firstBadStrip=0, NconsecutiveBadStrips=0;
    unsigned int theBadStripRange;

    for(std::vector<uint32_t>::const_iterator is=BadChannelList_.begin(); is!=BadChannelList_.end(); ++is){
      if (*is>NStrips-1)
	break;
      if (*is!=lastBad+1){
	//new set 

	if ( lastBad!=999 ){
	  //save previous set
	  theBadStripRange = ((firstBadStrip & 0xFFFF) << 16) | (NconsecutiveBadStrips & 0xFFFF) ;

	  if (printdebug_)
	    edm::LogInfo("SiStripBadChannelBuilder") << "detid " << *it << " \t"
						   << " firstBadStrip " << firstBadStrip << "\t "
						   << " NconsecutiveBadStrips " << NconsecutiveBadStrips << "\t "
						   << " packed integer " << std::hex << theBadStripRange  << std::dec
						   << std::endl; 	    
	  
	  theSiStripVector.push_back(theBadStripRange);
	}
	
	firstBadStrip=*is;
	NconsecutiveBadStrips=0;
      } 	
      NconsecutiveBadStrips++;
      lastBad=*is;
    }

    theBadStripRange = ((firstBadStrip & 0xFFFF) << 16) | (NconsecutiveBadStrips & 0xFFFF) ;
    
    if (printdebug_)
      edm::LogInfo("SiStripBadChannelBuilder") << "detid " << *it << " \t"
					     << " firstBadStrip " << firstBadStrip << "\t "
					     << " NconsecutiveBadStrips " << NconsecutiveBadStrips << "\t "
					     << " packed integer " << std::hex << theBadStripRange  << std::dec
					     << std::endl; 	    
	  
    theSiStripVector.push_back(theBadStripRange);
        
    SiStripBadStrip::Range range(theSiStripVector.begin(),theSiStripVector.end());
    if ( ! obj->put(*it,range) )
    edm::LogError("SiStripBadChannelBuilder")<<"[SiStripBadChannelBuilder::analyze] detid already exists"<<std::endl;
  }
}


