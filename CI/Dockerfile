FROM registry.orfeo-toolbox.org/orfeotoolbox/otb-build-env/otb-ubuntu-native:18.04

# Prepare ccache
RUN mkdir -p /opt/ccache \
 && echo "max_size = 2G" > /opt/ccache/ccache.conf \
 && echo "run_second_cpp = true" >> /opt/ccache/ccache.conf \
 && echo "compiler_check = content" >> /opt/ccache/ccache.conf \
 && echo "sloppiness = time_macros" >> /opt/ccache/ccache.conf

ENV CCACHE_BASEDIR=/opt CCACHE_DIR=/opt/ccache

RUN apt-get update -y \
 && apt-get install -y --no-install-recommends \
    python3-pip \
&& rm -rf /var/lib/apt/lists/*
RUN pip3 install config 

RUN cd /opt && git clone -b integrate-to-OTB https://gitlab.orfeo-toolbox.org/thibrom/otb-bv.git
# Get OTB sources and compile
RUN mkdir -p /opt/otb && git clone -b develop --depth 3 https://gitlab.orfeo-toolbox.org/orfeotoolbox/otb.git /opt/otb/src \
 && cd /opt/otb/src && git checkout develop
RUN git clone -b updateTo-current-OTB https://gitlab.orfeo-toolbox.org/thibrom/phenotb.git /opt/otb/src/Modules/Remote/OTBPhenology && rm /opt/otb/src/Modules/Remote/phenotb.remote.cmake
RUN mkdir /opt/otb/build \
 && cp /opt/otb-bv/CI/configure_otb_for_bv.cmake /opt/otb/build \
 && cd /opt/otb/build \
 && cmake -C configure_for_bv.cmake -DCMAKE_INSTALL_RPATH=/opt/otb/lib -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE ../src \
 && ninja install
