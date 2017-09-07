#CLIENT
CLIENT_BUILD = client/build
CLIENT_SRC = client/src

CLIENT_SRCS = net.cc componentManager.cc entityManager.cc world.cc entity.cc \
	      game.cc main.cc
CLIENT_SRCS := $(CLIENT_SRCS:%.cc=$(CLIENT_SRC)/%.cc)

CLIENT_OBJS := $(CLIENT_SRCS:$(CLIENT_SRC)%.cc=$(CLIENT_BUILD)%.o)

CLIENT_DEPDIR := .d/client
CLIENT_DEPFLAGS = -MT $@ -MMD -MP -MF $(CLIENT_DEPDIR)\$(basename $(notdir $*)).Td
CLIENT_POSTCOMPILE = @move $(CLIENT_DEPDIR)\$(basename $(notdir $*)).Td $(CLIENT_DEPDIR)\$(basename $(notdir $*)).d


#SERVER
SERVER_BUILD = server/build
SERVER_SRC = server/src

SERVER_SRCS = main.cc net.cc
SERVER_SRCS := $(SERVER_SRCS:%.cc=$(SERVER_SRC)/%.cc)

SERVER_OBJS := $(SERVER_SRCS:$(SERVER_SRC)%.cc=$(SERVER_BUILD)%.o)

SERVER_DEPDIR := .d/server
SERVER_DEPFLAGS = -MT $@ -MMD -MP -MF $(SERVER_DEPDIR)/$(basename $(notdir $*)).Td
SERVER_POSTCOMPILE = @mv $(SERVER_DEPDIR)/$(basename $(notdir $*)).Td $(SERVER_DEPDIR)/$(basename $(notdir $*)).d && touch $@

ifeq ($(OS),Windows_NT)
	RM = del /f

	CXX = clang++
	CPPFLAGS = -std=c++14 -I "include" -I "client\include" -I "shared\include"
	LDFLAGS = -std=c++14 -L "lib"
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lSDL2_image

	CLIENT_SRCS := $(foreach src,$(CLIENT_SRCS),$(subst /src/,\src\,$(src)))
	CLIENT_OBJS := $(subst /,\,$(CLIENT_OBJS))
	CLIENT_BUILD := $(subst /,\,$(CLIENT_BUILD))
	CLIENT_SRC := $(subst /,\,$(CLIENT_SRC))

	CLIENT_DEPDIR := $(subst /,\,$(CLIENT_DEPDIR))
else
	RM = rm -f

	CXX = clang++
	CPPFLAGS = -std=c++14 -I "include" -I "client/include" -I "shared/include"
	LDFLAGS = -std=c++14 -L "lib"
	LDLIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_net -lSDL2_image

	SERVER_CPPFLAGS = -std=c++14 -I "server/include" -I "shared/include" -isystem "/usr/lib/clang/4.0/include"
	SERVER_LDFLAGS = -std=c++14 -L "/home/charles/.emscripten_cache/asmjs"
	SERVER_LDLIBS = -lSDL2main -lSDL2 -lSDL2_net
endif

.PHONY: server windows browser clean

$(CLIENT_BUILD)\\%.o: $(CLIENT_SRC)\\%.cc
$(CLIENT_BUILD)\\%.o: $(CLIENT_SRC)\\%.cc $(CLIENT_DEPDIR)\\%.d
	$(CXX) $(CLIENT_DEPFLAGS) $(CPPFLAGS) -o $@ -c $<
	$(CLIENT_POSTCOMPILE)


$(SERVER_BUILD)/%.o: $(SERVER_SRC)/%.cc
$(SERVER_BUILD)/%.o: $(SERVER_SRC)/%.cc $(SERVER_DEPDIR)/%.d
	$(CXX) $(SERVER_DEPFLAGS) $(SERVER_CPPFLAGS) -o $@ -c $<
	$(SERVER_POSTCOMPILE)

windows: $(CLIENT_OBJS)
	$(CXX) $(LDFLAGS) $(CLIENT_OBJS) -g -o client\bin\dragon-ponies.exe $(LDLIBS)

windows-prod: $(OBJS)
	$(CXX) -O3 $(LDFLAGS) $(OBJS) -g -o client\bin\dragon-ponies.exe $(LDLIBS)

linux: $(LINUX_OBJS)
	$(CXX) $(LDFLAGS) $(LINUX_OBJS) -o client/bin/dragon-ponies.out $(LDLIBS)

server: $(SERVER_OBJS)
	$(CXX) $(SERVER_LDFLAGS) $(SERVER_OBJS) -g -o server/bin/game-server $(SERVER_LDLIBS)

browser:
	emcc $(CLIENT_SRCS) -std=c++14 \
	-s USE_SDL=2 -s USE_SDL_NET=2 -s USE_SDL_IMAGE=2 -o client/bin/main.js \
	-I "./client/include"

clean:
	$(RM) $(CLIENT_OBJS)
	$(RM) $(SERVER_OBJS)

$(CLIENT_DEPDIR)\\%.d: ;
.PRECIOUS: $(CLIENT_DEPDIR)\\%.d

$(SERVER_DEPDIR)/%.d: ;
.PRECIOUS: $(SERVER_DEPDIR)/%.d

include $(wildcard $(patsubst %,$(CLIENT_DEPDIR)/%.d,$(basename $(notdir $(CLIENT_SRCS)))))
include $(wildcard $(patsubst %,$(SERVER_DEPDIR)/%.d,$(basename $(notdir $(SERVER_SRCS)))))
