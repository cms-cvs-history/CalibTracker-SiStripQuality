// system include files
#include <memory>

// user include files

#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"


#include "CalibTracker/SiStripQuality/interface/SiStripBadStrip.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h" 
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetType.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"


#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"



#include "CondTools/SiStrip/test/SiStripBadStripBuilder.h"

using namespace std;
using namespace cms;

SiStripBadStripBuilder::SiStripBadStripBuilder( const edm::ParameterSet& iConfig ):
  printdebug_(iConfig.getUntrackedParameter<bool>("printDebug",false)){}

void SiStripBadStripBuilder::beginJob( const edm::EventSetup& iSetup ) {

  edm::ESHandle<TrackerGeometry> pDD;
  iSetup.get<TrackerDigiGeometryRecord>().get( pDD );     
  edm::LogInfo("SiStripBadStripBuilder") <<" There are "<<pDD->detUnits().size() <<" detectors"<<std::endl;
  
  for(TrackerGeometry::DetUnitContainer::const_iterator it = pDD->detUnits().begin(); it != pDD->detUnits().end(); it++){
  
    if( dynamic_cast<StripGeomDetUnit*>((*it))!=0){
      uint32_t detid=((*it)->geographicalId()).rawId();            
      const StripTopology& p = dynamic_cast<StripGeomDetUnit*>((*it))->specificTopology();
      unsigned short Nstrips = p.nstrips();
      if(Nstrips<1 || Nstrips>768 ) {
	edm::LogError("SiStripBadStripBuilder")<<" Problem with Number of strips in detector.. "<< p.nstrips() <<" Exiting program"<<endl;
	exit(1);
      }
      detid_strips.push_back( pair<uint32_t,unsigned short>(detid,Nstrips) );
      if (printdebug_)
	edm::LogInfo("SiStripBadStripBuilder")<< "detid " << detid << " Number of Strips " << Nstrips;
    }
  }
}

void SiStripBadStripBuilder::analyze(const edm::Event& evt, const edm::EventSetup& iSetup){

  unsigned int run=evt.id().run();

  edm::LogInfo("SiStripBadStripBuilder") << "... creating dummy SiStripBadStrip Data for Run " << run << "\n " << std::endl;

  SiStripBadStrip* SiStripBadStrip_ = new SiStripBadStrip();

  
  for(std::vector< pair<uint32_t,unsigned short> >::const_iterator it = detid_strips.begin(); it != detid_strips.end(); it++){
    std::vector<int> theSiStripVector;
    short flag;
    
    //Generate bad channels for det detid: just for testing: channels 1, 37 , 258-265, 511 are always bad
    short TempBadChannels[11]={1,37,38,258,259,260,261,262,263,264,511};

    unsigned int firstBadStrip=999;
    unsigned short NconsecutiveBadStrips=0;

    //loop on strips
    for(unsigned short jstrip=0; jstrip<it->second; jstrip++){

      // set channel good =0 or bad=1
      flag=0;
      for(int i=0;i<12;i++){
	if(jstrip==TempBadChannels[i]){
	  flag=1;
	  edm::LogInfo("SiStripBadStripBuilder") << "detid " << it->first 
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

      if (flag==0 ||  jstrip==it->second-1){ //out from a range of bad strips or no bad strips before
	if (NconsecutiveBadStrips!=0){
	  unsigned int theBadStripRange = ((firstBadStrip & 0xFFFF) << 16) | (NconsecutiveBadStrips & 0xFFFF) ;
	 
	  if (printdebug_)
	    edm::LogInfo("SiStripBadStripBuilder") << "detid " << it->first << " \t"
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
    if ( ! SiStripBadStrip_->put(it->first,range) )
      edm::LogError("SiStripBadStripBuilder")<<"[SiStripBadStripBuilder::analyze] detid already exists"<<std::endl;
  }
  
  
  //End now write sistripnoises data in DB
  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  
  if( mydbservice.isAvailable() ){
    try{
      if( mydbservice->isNewTagRequest("SiStripBadStripRcd") ){
	mydbservice->createNewIOV<SiStripBadStrip>(SiStripBadStrip_,mydbservice->endOfTime(),"SiStripBadStripRcd");      
      } else {
	mydbservice->appendSinceTime<SiStripBadStrip>(SiStripBadStrip_,mydbservice->currentTime(),"SiStripBadStripRcd");      
      }
    }catch(const cond::Exception& er){
      edm::LogError("SiStripBadStripBuilder")<<er.what()<<std::endl;
    }catch(const std::exception& er){
      edm::LogError("SiStripBadStripBuilder")<<"caught std::exception "<<er.what()<<std::endl;
    }catch(...){
      edm::LogError("SiStripBadStripBuilder")<<"Funny error"<<std::endl;
    }
  }else{
    edm::LogError("SiStripBadStripBuilder")<<"Service is unavailable"<<std::endl;
  }
}
