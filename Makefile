FFMPEG_LIBS=    libavdevice                        \
				libavformat                        \
				libavfilter                        \
				libavcodec                         \
				libswresample                      \
				libswscale                         \
				libavutil                          \

OPENSSL_LIBS= libssl \
			  libcrypto

FFMPEG_CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
FFMPEG_LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)
OPENSSL_CFLAGS := $(shell pkg-config --cflags $(OPENSSL_LIBS))
OPENSSL_LDLIBS := $(shell pkg-config --libs $(OPENSSL_LIBS))
OTHER_FLAGS := $(shell pkg-config --cflags --libs libevent libcjson)


CFLAGS += -Wall -g -fpermissive -std=c++11

CC := g++

SRCS = $(wildcard ./*.cpp)
HEADERS = $(wildcard ./*.hpp)

DDTranscoding: $(SRCS) $(HEADERS)
	$(CC) -o DDTranscoding $(SRCS) $(CFLAGS) $(FFMPEG_CFLAGS) $(FFMPEG_LDLIBS) $(OPENSSL_LDLIBS) $(OPENSSL_CFLAGS) $(EVENT_FLAGS) $(OTHER_FLAGS)


clean:
	rm -R *.o *.gch DDTranscoding DDTranscoding.dSYM
