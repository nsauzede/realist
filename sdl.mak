
ifndef SDLCONFIG

SDL1CONFIG=sdl-config
ifneq ($(shell a=`which $(SDL1CONFIG) 2>&1`;echo $$?),0)
SDL1CONFIG=
endif

SDL2CONFIG=sdl2-config
ifneq ($(shell a=`which $(SDL2CONFIG) 2>&1`;echo $$?),0)
SDL2CONFIG=
endif

ifdef SDL2CONFIG
SDLCONFIG=$(SDL2CONFIG)
else
ifdef SDL1CONFIG
SDLCONFIG=$(SDL1CONFIG)
endif
endif

ifndef SDLCONFIG
SDLCONFIG=NO_SDL_INSTALLED
SDL_CHECK:
	@echo "No SDL installed.\nTry : $$ sudo apt-get install libsdl2-dev";false
else
SDL_CHECK:
	@echo "Using detected SDLCONFIG=$(SDLCONFIG)"
endif

else

SDL_CHECK:
	@echo "Using forced SDLCONFIG=$(SDLCONFIG)"

endif

ifdef SDLCONFIG
SDL_FLAGS+=`$(SDLCONFIG) --cflags`
endif

OP_SYS=$(shell uname -o)
ifeq ($(OP_SYS),Msys)
WINDOWS=1
endif

ifdef WINDOWS
SDL_STATIC=1
endif

SDL_VER=$(shell $(SDLCONFIG) --version | cut -f 1 -d ".")
ifeq ($(SDL_VER),1)
USE_SDL1=1
else
USE_SDL1=
endif

ifdef SDL_STATIC
SDL_LIBS+=`$(SDLCONFIG) --static-libs` -static
else
SDL_LIBS+=`$(SDLCONFIG) --libs`
endif
ifdef WINDOWS
# this one to get text console output
SDL_LIBS+=-mno-windows
endif
