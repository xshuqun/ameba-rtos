#!/bin/bash

echo "Matter setup start"

cd third_party
rm -rf connectedhomeip
ln -s ../../connectedhomeip connectedhomeip
#remove build wheel error by commenting out esp32 requirements and constraints as we dont need them
sed -i '/esp32/s/^/#/' connectedhomeip/scripts/setup/requirements.all.txt

echo "Matter setup complete"

