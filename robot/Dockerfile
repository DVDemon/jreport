FROM ubuntu:22.04
ENV TZ=Europe/Moscow
RUN apt-get clean && apt-get update -y 
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata git sudo build-essential postgresql-client libpq-dev postgresql-server-dev-all autoconf libtool libssl-dev zlib1g-dev librdkafka-dev pkg-config cmake gcc git g++ mysql-client libmysqlclient-dev libboost-all-dev libssl-dev && apt-get clean
RUN git clone -b master https://github.com/pocoproject/poco.git && cd poco && mkdir cmake-build && cd cmake-build && cmake .. && make && sudo make install && cd .. && cd ..
RUN git clone https://github.com/troldal/OpenXLSX.git
RUN cd OpenXLSX && mkdir build && cd build && cmake  .. && make && sudo make install && cd .. && cd ..
COPY . jreport
RUN sudo ldconfig
RUN cd jreport && mkdir build && cd build && cmake  .. && make && make && cd .. && cd ..
WORKDIR /jreport
ENTRYPOINT [ "build/xlsx_report"]