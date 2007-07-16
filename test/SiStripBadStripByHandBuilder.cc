// system include files
#include <memory>

// user include files

#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"


#include "CondFormats/SiStripObjects/interface/SiStripBadStrip.h"

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



#include "CalibTracker/SiStripQuality/test/SiStripBadStripByHandBuilder.h"

using namespace std;
using namespace cms;

SiStripBadStripByHandBuilder::SiStripBadStripByHandBuilder( const edm::ParameterSet& iConfig ):
  printdebug_(iConfig.getUntrackedParameter<bool>("printDebug",false))
{
 ext_bad_detids = iConfig.getUntrackedParameter<std::vector<uint32_t> >("ext_bad_detids");
 }

void SiStripBadStripByHandBuilder::beginJob( const edm::EventSetup& iSetup ) {

  edm::ESHandle<TrackerGeometry> pDD;
  iSetup.get<TrackerDigiGeometryRecord>().get( pDD );     
  edm::LogInfo("SiStripBadStripByHandBuilder") <<" There are "<<pDD->detUnits().size() <<" detectors"<<std::endl;
  
  for(TrackerGeometry::DetUnitContainer::const_iterator it = pDD->detUnits().begin(); it != pDD->detUnits().end(); it++){
  
    if( dynamic_cast<StripGeomDetUnit*>((*it))!=0){
      uint32_t detid=((*it)->geographicalId()).rawId();            
      const StripTopology& p = dynamic_cast<StripGeomDetUnit*>((*it))->specificTopology();
      unsigned short Nstrips = p.nstrips();
      if(Nstrips<1 || Nstrips>768 ) {
	edm::LogError("SiStripBadStripByHandBuilder")<<" Problem with Number of strips in detector.. "<< p.nstrips() <<" Exiting program"<<endl;
	exit(1);
      }
      detid_strips.push_back( pair<uint32_t,unsigned short>(detid,Nstrips) );
      if (printdebug_)
	edm::LogInfo("SiStripBadStripByHandBuilder")<< "detid " << detid << " Number of Strips " << Nstrips;
    }
  }
}

void SiStripBadStripByHandBuilder::analyze(const edm::Event& evt, const edm::EventSetup& iSetup){

  unsigned int run=evt.id().run();

  edm::LogInfo("SiStripBadStripByHandBuilder") << "... creating dummy SiStripBadStrip Data for Run " << run << "\n " << std::endl;

  SiStripBadStrip* SiStripBadStrip_ = new SiStripBadStrip();

  
  // put manually inserted bad modules (detids) to DB object
  if(ext_bad_detids.size()!=0){
    std::vector<int> ext_temp(ext_bad_detids.size());
    ext_temp[0]=(((1 & 0xFFFF) << 16) | (768 & 0xFFFF));

    for(int i=0; i<ext_bad_detids.size();i++){
       
      for(std::vector<int>::const_iterator iter=ext_temp.begin(); iter!=ext_temp.end();iter++){
	SiStripBadStrip::Range ext_range(iter, iter);
	SiStripBadStrip_->put(ext_bad_detids[i], ext_range);
      }
    }
    edm::LogInfo("SiStripBadStripByHandBuilder") << "detid" << ext_bad_detids[0] << "t"
      //<< " strip " << jstrip << "\t "
						 << " firstBadStrip " << 1 << "\t "
						 << " NconsecutiveBadStrips " << 768 << "\t "
						 << " packed integer " << std::hex <<ext_temp[0]  << std::dec
						 << std::endl; 
  }
 

  
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
	  edm::LogInfo("SiStripBadStripByHandBuilder") << "detid " << it->first 
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
	    edm::LogInfo("SiStripBadStripByHandBuilder") << "detid " << it->first << " \t"
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
      edm::LogError("SiStripBadStripByHandBuilder")<<"[SiStripBadStripByHandBuilder::analyze] detid already exists"<<std::endl;
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
      edm::LogError("SiStripBadStripByHandBuilder")<<er.what()<<std::endl;
    }catch(const std::exception& er){
      edm::LogError("SiStripBadStripByHandBuilder")<<"caught std::exception "<<er.what()<<std::endl;
    }catch(...){
      edm::LogError("SiStripBadStripByHandBuilder")<<"Funny error"<<std::endl;
    }
  }else{
    edm::LogError("SiStripBadStripByHandBuilder")<<"Service is unavailable"<<std::endl;
  }
}
