FROM registry.orfeo-toolbox.org/orfeotoolbox/otb-build-env/otb-ubuntu-native:18.04

RUN apt-get update -y \
 && apt-get install -y --no-install-recommends \
    python3-pip \
&& rm -rf /var/lib/apt/lists/* \
&& pip3 install config 

# Get OTB sources and compile
RUN cd /opt && git clone -b integrate-to-OTB https://gitlab.orfeo-toolbox.org/thibrom/otb-bv.git \
 && mkdir -p /opt/otb && git clone -b develop --depth 3 https://gitlab.orfeo-toolbox.org/orfeotoolbox/otb.git /opt/otb/src \
 && cd /opt/otb/src && git checkout develop \
 && git clone -b updateTo-current-OTB https://gitlab.orfeo-toolbox.org/thibrom/phenotb.git /opt/otb/src/Modules/Remote/OTBPhenology && rm /opt/otb/src/Modules/Remote/phenotb.remote.cmake \
 && cd /opt/otb-bv/ && git pull && mkdir /opt/otb/build \
 && cp /opt/otb-bv/CI/configure_otb_for_bv.cmake /opt/otb/build \
 && cd /opt/otb/build \
 && cmake -C configure_otb_for_bv.cmake -DCMAKE_INSTALL_RPATH=/opt/otb/lib -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE ../src \
 && ninja install \
 && rm -r /opt/otb/src && rm -r /opt/otb/build
 