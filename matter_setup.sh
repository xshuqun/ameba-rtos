#!/bin/bash

MATTER_DIR=$PWD/component/application/matter

if [ ! -d third_party ];then
    mkdir third_party
else
    rm third_party/connectedhomeip
fi

cd third_party
rm -rf connectedhomeip
ln -s ../../connectedhomeip connectedhomeip

cd ../

if [ ! -d ${MATTER_DIR} ] || [ -z "$(find ${MATTER_DIR} -mindepth 1)" ]; then
  mkdir -p ${MATTER_DIR}
  git clone https://github.com/Ameba-AIoT/ameba-rtos-matter.git ${MATTER_DIR} -b ameba-rtos-v1.1/test/v1.4
fi

echo "Matter setup complete"
