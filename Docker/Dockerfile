# base image
FROM ubuntu

# MAINTAINER
MAINTAINER friendship-ing@qq.com

RUN apt-get update && apt-get install -y locales && rm -rf /var/lib/apt/lists/* && localedef -i en_US -c -f UTF-8 -A /usr/share/locale/locale.alias en_US.UTF-8

RUN apt-get update

RUN apt-get install -y libssl-dev openssl libevent-dev libmp3lame-dev libx264-dev libfdk-aac-dev gcc g++ make git

RUN mkdir soft

ADD ffmpeg-4.1.3.tar.bz2 /soft

WORKDIR /soft/ffmpeg-4.1.3

RUN ./configure --enable-shared --enable-static --disable-asm --enable-libx264 --enable-gpl --enable-libmp3lame --enable-libfdk-aac --enable-nonfree

RUN make -j4;make install

WORKDIR /soft

RUN git clone https://github.com/DaveGamble/cJSON.git 

WORKDIR /soft/cJSON

RUN make;make install

WORKDIR /soft

RUN git clone https://github.com/oujiangping/DDTranscoding.git

WORKDIR /soft/DDTranscoding

RUN g++ -g -o DDTranscoding ./Transcoding.cpp ./WorkersContainer.cpp ./main.cpp ./WorkContainerProxy.cpp ./MessageQueue.cpp ./BaseException.cpp ./Utils.cpp ./HttpResult.cpp ./HttpServer.cpp ./EvHttpHandler.cpp ./ErrorCode.cpp ./TranscodingWorker.cpp -Wall -g -fpermissive -std=c++11 -I/usr/local/include  -L/usr/local/lib -lavdevice -lavformat -lavfilter -lavcodec -lswresample -lswscale -lavutil  -lssl -lcrypto -lcjson -levent -lpthread

RUN cp DDTranscoding /usr/local/sbin

RUN ldconfig
