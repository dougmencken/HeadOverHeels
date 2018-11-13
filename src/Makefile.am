AUTOMAKE_OPTIONS = -Wno-override

if DEBUG
    AM_CFLAGS = -g -O0 -DDEBUG=1
    AM_CXXFLAGS = -g -O0 -DDEBUG=1
else
    AM_CFLAGS = -O3
    AM_CXXFLAGS = -O3
endif

if USE_ALLEGRO5
    AM_CFLAGS += -DUSE_ALLEGRO5=1
    AM_CXXFLAGS += -DUSE_ALLEGRO5=1
else
if USE_ALLEGRO4
    AM_CFLAGS += -DUSE_ALLEGRO4=1
    AM_CXXFLAGS += -DUSE_ALLEGRO4=1
endif
endif

if DARWIN
    AM_CFLAGS += -DDARWIN=1
    AM_CXXFLAGS += -DDARWIN=1
endif

headoverheels_CXXFLAGS = -I. -Iactivities -Ibehaviors -Igui -Iguiactions -Wall $(AM_CXXFLAGS)

AM_LDFLAGS = -lpthread -lz -lpng -lvorbis -lvorbisenc -lvorbisfile -logg -ltinyxml2

if USE_ALLEGRO5
AM_LDFLAGS += -lallegro -lallegro_audio -lallegro_acodec -lallegro_image -lallegro_font -lallegro_primitives
if DARWIN
AM_LDFLAGS += -lallegro_main
endif
else
if USE_ALLEGRO4
AM_LDFLAGS += `allegro-config --libs release`
endif
endif

bin_PROGRAMS = headoverheels

headoverheels_SOURCES = main.cpp \
			tribool.hpp \
			util.hpp \
			pointers.cpp \
			pointers.hpp \
			Ism.cpp \
			Ism.hpp \
			WrappersAllegro.cpp \
			WrappersAllegro.hpp \
			Way.cpp \
			Way.hpp \
			Color.cpp \
			Color.hpp \
			FlickeringColor.cpp \
			FlickeringColor.hpp \
			Picture.cpp \
			Picture.hpp \
			Drawable.hpp \
			Mediated.hpp \
			Shady.hpp \
			Camera.cpp \
			Camera.hpp \
			Door.cpp \
			Door.hpp \
			FloorTile.cpp \
			FloorTile.hpp \
			Item.cpp \
			Item.hpp \
			GridItem.cpp \
			GridItem.hpp \
			FreeItem.cpp \
			FreeItem.hpp \
			PlayerItem.cpp \
			PlayerItem.hpp \
			Mediator.cpp \
			Mediator.hpp \
			ShadowCaster.cpp \
			ShadowCaster.hpp \
			Masker.cpp \
			Masker.hpp \
			GameSaverAndLoader.cpp \
			GameSaverAndLoader.hpp \
			GameManager.cpp \
			GameManager.hpp \
			InputManager.cpp \
			InputManager.hpp \
			Isomot.cpp \
			Isomot.hpp \
			ItemData.cpp \
			ItemData.hpp \
			ItemDataManager.cpp \
			ItemDataManager.hpp \
			MapManager.cpp \
			MapManager.hpp \
			RoomConnections.cpp \
			RoomConnections.hpp \
			RoomBuilder.cpp \
			RoomBuilder.hpp \
			Room.cpp \
			Room.hpp \
			BonusManager.cpp \
			BonusManager.hpp \
			SoundManager.cpp \
			SoundManager.hpp \
			Wall.cpp \
			Wall.hpp \
			Timer.hpp \
			TimerPOSIX.cpp \
			TimerPOSIX.hpp \
			TimerWindows.cpp \
			TimerWindows.hpp \
			activities/ActivityOfItem.hpp \
			activities/KindOfActivity.cpp \
			activities/KindOfActivity.hpp \
			activities/DisplaceKindOfActivity.cpp \
			activities/DisplaceKindOfActivity.hpp \
			activities/FallKindOfActivity.cpp \
			activities/FallKindOfActivity.hpp \
			activities/JumpKindOfActivity.cpp \
			activities/JumpKindOfActivity.hpp \
			activities/MoveKindOfActivity.cpp \
			activities/MoveKindOfActivity.hpp \
			behaviors/Behavior.cpp \
			behaviors/Behavior.hpp \
			behaviors/ConveyorBelt.cpp \
			behaviors/ConveyorBelt.hpp \
			behaviors/Detector.cpp \
			behaviors/Detector.hpp \
			behaviors/Doughnut.cpp \
			behaviors/Doughnut.hpp \
			behaviors/Driven.cpp \
			behaviors/Driven.hpp \
			behaviors/Elevator.cpp \
			behaviors/Elevator.hpp \
			behaviors/FinalBall.cpp \
			behaviors/FinalBall.hpp \
			behaviors/Hunter.cpp \
			behaviors/Hunter.hpp \
			behaviors/Impel.cpp \
			behaviors/Impel.hpp \
			behaviors/Mobile.cpp \
			behaviors/Mobile.hpp \
			behaviors/ThereAndBack.cpp \
			behaviors/ThereAndBack.hpp \
			behaviors/Patrol.cpp \
			behaviors/Patrol.hpp \
			behaviors/PlayerHeadAndHeels.cpp \
			behaviors/PlayerHeadAndHeels.hpp \
			behaviors/PlayerHead.cpp \
			behaviors/PlayerHead.hpp \
			behaviors/PlayerHeels.cpp \
			behaviors/PlayerHeels.hpp \
			behaviors/RemoteControl.cpp \
			behaviors/RemoteControl.hpp \
			behaviors/Sink.cpp \
			behaviors/Sink.hpp \
			behaviors/Special.cpp \
			behaviors/Special.hpp \
			behaviors/Switch.cpp \
			behaviors/Switch.hpp \
			behaviors/Teleport.cpp \
			behaviors/Teleport.hpp \
			behaviors/Trampoline.cpp \
			behaviors/Trampoline.hpp \
			behaviors/Turn.cpp \
			behaviors/Turn.hpp \
			behaviors/UserControlled.cpp \
			behaviors/UserControlled.hpp \
			behaviors/Volatile.cpp \
			behaviors/Volatile.hpp \
			gui/Font.cpp \
			gui/Font.hpp \
			gui/Gui.hpp \
			gui/GuiManager.cpp \
			gui/GuiManager.hpp \
			gui/PictureWidget.cpp \
			gui/PictureWidget.hpp \
			gui/AnimatedPictureWidget.cpp \
			gui/AnimatedPictureWidget.hpp \
			gui/Label.cpp \
			gui/Label.hpp \
			gui/ColorCyclingLabel.cpp \
			gui/ColorCyclingLabel.hpp \
			gui/LanguageText.cpp \
			gui/LanguageText.hpp \
			gui/LanguageManager.cpp \
			gui/LanguageManager.hpp \
			gui/Menu.cpp \
			gui/Menu.hpp \
			gui/MenuWithValues.cpp \
			gui/MenuWithValues.hpp \
			gui/MenuWithTwoColumns.cpp \
			gui/MenuWithTwoColumns.hpp \
			gui/Screen.cpp \
			gui/Screen.hpp \
			gui/TextField.cpp \
			gui/TextField.hpp \
			gui/Widget.cpp \
			gui/Widget.hpp \
			guiactions/Action.hpp \
			guiactions/PlaySound.hpp \
			guiactions/ContinueGame.cpp \
			guiactions/ContinueGame.hpp \
			guiactions/CreateAudioMenu.cpp \
			guiactions/CreateAudioMenu.hpp \
			guiactions/CreateVideoMenu.hpp \
			guiactions/CreateVideoMenu.cpp \
			guiactions/CreateMenuOfGraphicSets.hpp \
			guiactions/CreateMenuOfGraphicSets.cpp \
			guiactions/CreateCongratulationsScreen.cpp \
			guiactions/CreateCongratulationsScreen.hpp \
			guiactions/CreateEndScreen.cpp \
			guiactions/CreateEndScreen.hpp \
			guiactions/CreateListOfSavedGames.cpp \
			guiactions/CreateListOfSavedGames.hpp \
			guiactions/CreateKeyboardMenu.cpp \
			guiactions/CreateKeyboardMenu.hpp \
			guiactions/CreateLanguageMenu.cpp \
			guiactions/CreateLanguageMenu.hpp \
			guiactions/CreateMainMenu.cpp \
			guiactions/CreateMainMenu.hpp \
			guiactions/CreatePlanetsScreen.cpp \
			guiactions/CreatePlanetsScreen.hpp \
			guiactions/ExitApplication.cpp \
			guiactions/ExitApplication.hpp \
			guiactions/LoadGame.cpp \
			guiactions/LoadGame.hpp \
			guiactions/RedefineKey.cpp \
			guiactions/RedefineKey.hpp \
			guiactions/SaveGame.cpp \
			guiactions/SaveGame.hpp \
			guiactions/SelectLanguage.cpp \
			guiactions/SelectLanguage.hpp \
			guiactions/ShowAuthors.cpp \
			guiactions/ShowAuthors.hpp

if USE_ALLEGRO5
headoverheels_SOURCES += algif5/algif.c \
			algif5/gif.c \
			algif5/lzw.c \
			algif5/bitmap.c
else
if USE_ALLEGRO4
headoverheels_SOURCES += loadpng/loadpng.c \
			loadpng/savepng.c \
			loadpng/regpng.c \
			alogg/alogg.c \
			alogg/aloggcfg.h \
			alogg/alogg.h \
			alogg/aloggint.h \
			alogg/aloggpth.c \
			alogg/aloggpth.h \
			algif/algif.c \
			algif/gif.c \
			algif/lzw.c
endif
endif
