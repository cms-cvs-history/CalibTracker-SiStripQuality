#include "PluginManager/ModuleDef.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "CalibTracker/SiStripQuality/test/SiStripBadStripBuilder.h"
#include "CalibTracker/SiStripQuality/test/SiStripBadFiberBuilder.h"
#include "CalibTracker/SiStripQuality/test/SiStripBadStripReader.h"

DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(SiStripBadStripBuilder);
DEFINE_ANOTHER_FWK_MODULE(SiStripBadFiberBuilder);
DEFINE_ANOTHER_FWK_MODULE(SiStripBadStripReader);


