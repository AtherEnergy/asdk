#!/bin/bash

# Apply the patch
git apply --reject --whitespace=fix sdk_patch

# Remove the specified directories
# rm -rf ./sdk/common/src/drivers/cxpi
# rm -rf ./sdk/common/src/drivers/smif
# rm -rf ./sdk/tviibe1m/hdr/rev_b
# rm -rf ./sdk/tviibe1m/hdr/rev_c
