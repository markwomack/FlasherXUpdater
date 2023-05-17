//
// Licensed under the MIT license.
// See accompanying LICENSE file for details.
//

#ifndef FLASHERXUPDATER_H
#define FLASHERXUPDATER_H

// Arduino includes
#include <Stream.h>

// Local includes
#include "CRCStream.h"

class FlasherXUpdater {
  public:
    static void performUpdate(Stream* updateStream);

  private:
    static CRCStream* getCRCStream(Stream* updateStream);
};

#endif // FLASHERXUPDATER_H
