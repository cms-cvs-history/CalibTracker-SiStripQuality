#include "PluginManager/ModuleDef.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "CalibTracker/SiStripQuality/test/SiStripBadChannelBuilder.h"
#include "CalibTracker/SiStripQuality/test/SiStripBadFiberBuilder.h"
#include "CalibTracker/SiStripQuality/test/SiStripBadStripReader.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(SiStripBadChannelBuilder);
DEFINE_ANOTHER_FWK_MODULE(SiStripBadFiberBuilder);
DEFINE_ANOTHER_FWK_MODULE(SiStripBadStripReader);


