#include "winrt/Windows.Media.Control.h"
#include "winrt/Windows.Media.Playback.h"
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Storage.Streams.h>
#include "winrt/Windows.Foundation.h"

class SMTC {
public:
	static bool enable();
	static bool disable();
	static bool update(char* title, char* albumTitle, char* artist, char* thumbnail);
	static bool updatePlayState(int state);
	static bool updateTimeline(int position, int duration);
	static bool registerEventCallback(void (*callback_)(int));
};