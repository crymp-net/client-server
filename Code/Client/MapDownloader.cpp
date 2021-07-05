#include "MapDownloader.h"
#include "Library/External/miniz/miniz.h"
#include "Library/WinAPI.h"

#include "CryCommon/CrySystem/ISystem.h"

#pragma comment(lib, "Urlmon")

HRESULT WINAPI MapDownloader::StatusCallback::OnProgress(unsigned long progress, unsigned long progressMax, unsigned long statusCode, LPCWSTR status) {
    if (m_parent->m_active) {
        if (m_parent->m_onProgress) {
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            unsigned long chunk = progress - m_lastProgress;
            bool announce = false;
            m_speed.push((unsigned long long)chunk);
            m_lastProgress = progress;
            if (millis - m_lastAnnounce >= 1000) {
                m_lastAnnounce = millis;
                char infoMessage[100];
                double nDownloadSpeed = m_speed.getSpeed();
                if (nDownloadSpeed >= 1.0) {
                    unsigned int remaining = (unsigned int)((progressMax - progress) / nDownloadSpeed);
                    snprintf(infoMessage, sizeof(infoMessage), 
                        "Downloading map %.1f%% (%.2f KiB/s, %02d:%02d)", 
                        100.0 * progress / progressMax,
                        nDownloadSpeed / 1024.0,
                        remaining / 60, remaining % 60
                    );
                    if (progressMax == 0) progressMax = 1;
                    m_parent->m_onProgress(progress, progressMax, 0, infoMessage);
                }
            }
        }
        return S_OK;
    }
    else {
        return E_ABORT;
    }
}

MapDownloader::MapDownloader() {
    m_status = std::make_unique<StatusCallback>(this);
}

std::tuple<HRESULT, std::string> MapDownloader::execute(const std::string& url, const std::function<void(unsigned int progress, unsigned int progressMax, unsigned int stage, const std::string & message)>& onProgress) {
    m_url = url;
    m_onProgress = onProgress;
    m_active = true;
    m_status->m_speed.reset();
    m_status->m_lastProgress = 0;
    m_status->m_lastAnnounce = 0;
    auto [downloadResult, downloadText] = download();
    if (FAILED(downloadResult)) {
        m_active = false;
        return std::make_tuple(downloadResult, downloadText);
    }
    auto [extractResult, extractText] = extract(downloadText);
    if (FAILED(extractResult)) {
        m_active = false;
        return std::make_tuple(extractResult, extractText);
    }
    if (m_onFinished) {
        m_onFinished(extractResult, extractText);
    }
    m_active = false;
    return std::make_tuple(extractResult, extractText);
}

void MapDownloader::cancel() {
    m_active = false;
}

std::tuple<HRESULT, std::string> MapDownloader::download() {
    char cachePath[MAX_PATH];
    HRESULT downloadResult = URLDownloadToCacheFileA(0, m_url.c_str(), cachePath, sizeof(cachePath), 0, m_status.get());
    if (FAILED(downloadResult)) return std::make_tuple(downloadResult, "Download failed");
    return std::make_tuple(S_OK, cachePath);
}

std::tuple<HRESULT, std::string> MapDownloader::extract(const std::string& path) {
    mz_zip_archive zip_archive;
    memset(&zip_archive, 0, sizeof(zip_archive));
    mz_bool status = mz_zip_reader_init_file(&zip_archive, path.c_str(), 0);
    if (!status)
    {
        return std::make_tuple(E_ABORT, "Failed to open archive");
    }
    mz_zip_archive_file_stat entry;
    if (!mz_zip_reader_file_stat(&zip_archive, 0, &entry))
    {
        mz_zip_reader_end(&zip_archive);
        return std::make_tuple(E_ABORT, "Failed to get cursor for first file");
    }
    int n_files = (int)mz_zip_reader_get_num_files(&zip_archive);
    const std::string base = WinAPI::ConvertUTF16To8((std::filesystem::current_path() / "Game").c_str()) + "/";
    for (int i = 0; i < n_files; i++)
    {
        if (!mz_zip_reader_file_stat(&zip_archive, i, &entry)) continue;
        std::string path = base + entry.m_filename;
        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }
        auto last_slash = path.rfind('/');
        if (last_slash != std::string::npos) {
            std::string parent = path.substr(0, last_slash);
            DWORD dwAttrib = GetFileAttributesA(parent.c_str());
            bool exists = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
            if (!exists) {
                if (!std::filesystem::create_directories(parent)) {
                    mz_zip_reader_end(&zip_archive);
                    return std::make_tuple(E_ABORT, std::string("Failed to create directory: ") + parent);
                }
            }
        }
        if (m_onProgress) {
            char infoMessage[100];
            snprintf(infoMessage, sizeof(infoMessage), "Extracting map %.2f%%", 100.0 * i / n_files);
            m_onProgress(i, n_files, 1, infoMessage);
        }

        if (!mz_zip_reader_extract_to_file(&zip_archive, i, path.c_str(), 0))
        {
            mz_zip_reader_end(&zip_archive);
            return std::make_tuple(E_ABORT, std::string("Failed to extract: ") + entry.m_filename);
        }
    }
    return std::make_tuple(S_OK, "OK");
}
