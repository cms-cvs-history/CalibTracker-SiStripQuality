#include "CalibTracker/SiStripQuality/test/SiStripBadStripBuilder.h"

#include "CalibTracker/SiStripCommon/interface/SiStripDetInfoFileReader.h"

#include <iostream>
#include <fstream>


SiStripBadStripBuilder::SiStripBadStripBuilder(const edm::ParameterSet& iConfig) : ConditionDBWriter<SiStripBadStrip>::ConditionDBWriter<SiStripBadStrip>(iConfig){

  edm::LogInfo("SiStripBadStripBuilder") << " ctor ";
  fp_ = iConfig.getUntrackedParameter<edm::FileInPath>("file",edm::FileInPath("CalibTracker/SiStripCommon/data/SiStripDetInfo.dat"));
  printdebug_ = iConfig.getUntrackedParameter<bool>("printDebug",false);
}


SiStripBadStripBuilder::~SiStripBadStripBuilder(){
  edm::LogInfo("SiStripBadStripBuilder") << " dtor";
}

void SiStripBadStripBuilder::algoAnalyze(const edm::Event & event, const edm::EventSetup& iSetup){
  
  edm::LogInfo("SiStripBadStripBuilder") <<"SiStripBadStripBuilder::algoAnalyze called"<<std::endl;
  unsigned int run=event.id().run();

  edm::LogInfo("SiStripBadStripBuilder") << "... creating dummy SiStripBadStrip Data for Run " << run << "\n " << std::endl;
  
  obj = new SiStripBadStrip();

  SiStripDetInfoFileReader reader(fp_.fullPath());
  
  const std::vector<uint32_t> DetIds = reader.getAllDetIds();
  
  for(std::vector<uint32_t>::const_iterator it=DetIds.begin(); it!=DetIds.end(); ++it){
    
    std::vector<int> theSiStripVector;
   
    short flag;
    
    //Generate bad channels for det detid: just for testing: channels 1, 37 , 258-265, 511 are always bad
    short TempBadChannels[11]={1,37,38,258,259,260,261,262,263,264,511};
    
    unsigned int firstBadStrip=999;
    unsigned short NconsecutiveBadStrips=0;

    int NStrips=reader.getNumberOfApvsAndStripLength(*it).first*128;

    //loop on strips
    for(unsigned short jstrip=0; jstrip<NStrips; jstrip++){
      
      // set channel good =0 or bad=1
      flag=0;
      for(int i=0;i<12;i++){
	if(jstrip==TempBadChannels[i]){
	  flag=1;
	  if (printdebug_)
	    edm::LogInfo("SiStripBadStripBuilder") << "detid " << *it
						   << " strip " << jstrip << "\t "
						   << " \t flag 1" << std::endl;
	  break;
	}
      }
      if (flag==1){ // On a bad strip
	if (NconsecutiveBadStrips==0){ //First bad strip of a range
	  firstBadStrip=jstrip;
	}
	NconsecutiveBadStrips++;
      }
      
      if (flag==0 ||  jstrip==NStrips-1){ //out from a range of bad strips or no bad strips before
	if (NconsecutiveBadStrips!=0){
	  int theBadStripRange = ((firstBadStrip & 0xFFFF) << 16) | (NconsecutiveBadStrips & 0xFFFF) ;
	 
	  if (printdebug_)
	    edm::LogInfo("SiStripBadStripBuilder") << "detid " << *it << " \t"
	      //<< " strip " << jstrip << "\t "
						   << " firstBadStrip " << firstBadStrip << "\t "
						   << " NconsecutiveBadStrips " << NconsecutiveBadStrips << "\t "
						   << " packed integer " << std::hex << theBadStripRange  << std::dec
						   << std::endl; 	    
	  
	  theSiStripVector.push_back(theBadStripRange);
	  NconsecutiveBadStrips=0;
	  firstBadStrip=999;
	}
      }     
    }
          
    SiStripBadStrip::Range range(theSiStripVector.begin(),theSiStripVector.end());
    if ( ! obj->put(*it,range) )
    edm::LogError("SiStripBadStripBuilder")<<"[SiStripBadStripBuilder::analyze] detid already exists"<<std::endl;
  }
}


