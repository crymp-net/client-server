#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <filesystem>

struct MapDownloaderResult
{
	bool success = false;
	std::string map;
};

struct MapDownloaderRequest
{
	std::string map;                                     // "multiplayer/ps/mymap"
	std::string mapURL;                                  // "http://www.server.com/maps/MyMap.zip"
	std::string mapVersion;                              // "1"
	std::function<bool(const std::string&)> onProgress;  // progress message, return false to cancel download
	std::function<void(MapDownloaderResult&)> onComplete;
};

class MapDownloader
{
	std::filesystem::path m_downloadDir;

	void DownloadMap(MapDownloaderRequest && request);
	bool UnpackMap(const std::filesystem::path & zipPath, const std::string_view & mapName);
	void RemoveZip(const std::filesystem::path & zipPath);
	bool CheckMapExists(const std::string_view & mapName);
	bool CheckMapVersion(const std::string_view & mapName, const std::string_view & mapVersion);
	void StoreMapVersion(const std::string_view & mapName, const std::string_view & mapVersion);
	std::filesystem::path GetVersionFilePath(const std::string_view & mapName);
	void RescanMaps();
	void RegisterDownloadedMaps();
	void AddMapRedirect(const std::string& mapName);
	void CompleteRequest(const MapDownloaderRequest & request, bool success);

public:
	MapDownloader();
	~MapDownloader();

	void Request(MapDownloaderRequest && request);
};
