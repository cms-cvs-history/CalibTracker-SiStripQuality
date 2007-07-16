#include "PluginManager/ModuleDef.h"

#include "FWCore/Framework/interface/MakerMacros.h"

#include "CalibTracker/SiStripQuality/test/SiStripBadStripByHandBuilder.h"
#include "CalibTracker/SiStripQuality/test/SiStripBadStripBuilder.h"
#include "CalibTracker/SiStripQuality/test/SiStripBadStripReader.h"

using cms::SiStripBadStripByHandBuilder;
using cms::SiStripBadStripReader;


DEFINE_SEAL_MODULE();
DEFINE_ANOTHER_FWK_MODULE(SiStripBadStripByHandBuilder);
DEFINE_ANOTHER_FWK_MODULE(SiStripBadStripBuilder);
DEFINE_ANOTHER_FWK_MODULE(SiStripBadStripReader);



