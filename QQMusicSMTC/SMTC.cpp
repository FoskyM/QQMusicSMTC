#include "winrt/Windows.Media.Control.h"
#include "winrt/Windows.Media.Playback.h"
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Foundation.h>
#include <ppltasks.h>
#include <fstream>
#include "Utils.h"
#include "SMTC.h"
#include <iostream>

using namespace winrt::Windows::Media::Playback;
using namespace winrt::Windows::Media;
using namespace Control;

using namespace winrt;
using namespace Control;
using namespace Windows::ApplicationModel::Core;

using namespace Windows::Foundation;
using namespace Windows::Web::Http;
using namespace Windows::Storage::Streams;

using namespace std;

static std::optional<MediaPlayer> mediaPlayer;
static void (*callback)(int) = nullptr;

static void GetRandomAccessStreamFromHttp(const wstring uri, const function<void(IRandomAccessStream)>& callback, const function<void()>& failed)
{
	try
	{
		HttpClient client;

		InMemoryRandomAccessStream randomAccessStream;

		auto outputStream = randomAccessStream.GetOutputStreamAt(0);

		Uri imageUrl(uri);
		IBuffer buffer = client.GetBufferAsync(imageUrl).get();
		DataWriter dataWriter(outputStream);
		dataWriter.WriteBuffer(buffer);
		dataWriter.StoreAsync().get();
		dataWriter.Close();
		callback(randomAccessStream);
	}
	catch (const std::exception& ex)
	{
		std::wstring message(ex.what(), ex.what() + strlen(ex.what()));
		failed();
	}
}

bool SMTC::enable() {
	if (!mediaPlayer.has_value())
	{
		mediaPlayer = MediaPlayer();

		mediaPlayer->SystemMediaTransportControls().ButtonPressed([&](SystemMediaTransportControls sender,
			SystemMediaTransportControlsButtonPressedEventArgs
			args)
			{
				auto btn = static_cast<int32_t>(args.Button());
				if (callback)(*callback)(btn);
			});
	}

	const auto commandManager = mediaPlayer->CommandManager();
	commandManager.IsEnabled(false);

	SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();

	auto updater = smtc.DisplayUpdater();
	updater.ClearAll();
	updater.Type(MediaPlaybackType::Music);
	auto properties = updater.MusicProperties();

	properties.Title(to_hstring(std::string("Loading")));

	updater.Update();

	return true;
}

bool SMTC::disable() {
	if (!mediaPlayer.has_value()) mediaPlayer = MediaPlayer();

	SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();

	smtc.IsEnabled(false);
	return true;
}

bool SMTC::update(char* title, char* albumTitle, char* artist, char* thumbnail) {
	if (!mediaPlayer.has_value())
		mediaPlayer = MediaPlayer();

	try
	{
		auto imgurl = std::string(static_cast<char*>(thumbnail));
		// auto imgurl_online = std::string(static_cast<char*>(args[4]));
		if (imgurl.length() > 0)
		{
			// replace / to \

			// std::replace(imgurl.begin(), imgurl.end(), '/', '\\');

			GetRandomAccessStreamFromHttp(to_wstring(imgurl), [=](IRandomAccessStream raStream)
				{
					cout << "Image loaded" << endl;
					const auto commandManager = mediaPlayer->CommandManager();
					commandManager.IsEnabled(false);

					SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();

					smtc.IsEnabled(true);
					smtc.IsPlayEnabled(false);
					smtc.IsPauseEnabled(true);

					auto updater = smtc.DisplayUpdater();
					updater.ClearAll();
					updater.Thumbnail(RandomAccessStreamReference::CreateFromStream(raStream));
					updater.Type(MediaPlaybackType::Music);

					auto properties = updater.MusicProperties();
					properties.Title(to_hstring(std::string(static_cast<char*>(title))));
					properties.AlbumTitle(to_hstring(std::string(static_cast<char*>(albumTitle))));
					properties.Artist(to_hstring(std::string(static_cast<char*>(artist))));
					updater.Update();

				}, [=]()
					{
						const auto commandManager = mediaPlayer->CommandManager();
						commandManager.IsEnabled(false);

						SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();

						smtc.IsEnabled(true);
						smtc.IsPlayEnabled(false);
						smtc.IsPauseEnabled(true);

						auto updater = smtc.DisplayUpdater();
						updater.ClearAll();
						updater.Thumbnail(RandomAccessStreamReference::CreateFromUri(Uri(to_hstring(imgurl))));
						updater.Type(MediaPlaybackType::Music);

						auto properties = updater.MusicProperties();
						properties.Title(to_hstring(std::string(static_cast<char*>(title))));
						properties.AlbumTitle(to_hstring(std::string(static_cast<char*>(albumTitle))));
						properties.Artist(to_hstring(std::string(static_cast<char*>(artist))));
						updater.Update();
					});



		}
	}
	catch (std::exception& e)
	{
		return false;
	}
	return true;
}

bool SMTC::updatePlayState(int state) {
	try
	{
		SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();

		smtc.PlaybackStatus(static_cast<MediaPlaybackStatus>(state));
	}
	catch (std::exception& e)
	{
		return false;
	}
	return true;
}

bool SMTC::updateTimeline(int position, int duration) {
	try
	{
		SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();
		SystemMediaTransportControlsTimelineProperties timelineProperties;

		timelineProperties.StartTime(TimeSpan{ 0 });
		timelineProperties.MinSeekTime(TimeSpan{ 0 });
		timelineProperties.Position(TimeSpan{ position });
		timelineProperties.MaxSeekTime(TimeSpan{ duration });
		timelineProperties.EndTime(TimeSpan{ duration });

		smtc.UpdateTimelineProperties(timelineProperties);
	}
	catch (std::exception& e)
	{
		return false;
	}
	return true;
}

bool SMTC::registerEventCallback(void (*callback_)(int)) {
	callback = callback_;
	SystemMediaTransportControls smtc = mediaPlayer->SystemMediaTransportControls();
	smtc.IsEnabled(true);
	smtc.IsPlayEnabled(true);
	smtc.IsPauseEnabled(true);
	smtc.IsNextEnabled(true);
	smtc.IsPreviousEnabled(true);
	return true;
}