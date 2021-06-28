#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <functional>
#include <tuple>
#include <iostream>
#include <filesystem>
#include <chrono>

#include "SpeedAggregator.h"

#include <Urlmon.h>
#include <Shlwapi.h>
#include <ShlObj.h>

class MapDownloader {
private:
    std::atomic<bool> m_active = false;
    class StatusCallback : public IBindStatusCallback {
        friend MapDownloader;
        MapDownloader* m_parent;
        unsigned long m_lastProgress = 0;
        long long m_lastAnnounce = 0;
        SpeedAggregator m_speed;
    public:
        StatusCallback(MapDownloader* parent) : 
            m_parent(parent),
            m_speed(2000) {}
        virtual HRESULT __stdcall QueryInterface(const IID&, void**) {
            return E_NOINTERFACE;
        }
        virtual ULONG WINAPI AddRef(void) {
            return 1;
        }
        virtual  ULONG WINAPI Release(void) {
            return 1;
        }
        virtual  HRESULT WINAPI OnStartBinding(DWORD, IBinding*) {
            return E_NOTIMPL;
        }
        virtual HRESULT WINAPI GetPriority(LONG*) {
            return E_NOTIMPL;
        }
        virtual HRESULT WINAPI OnLowResource(DWORD) {
            return S_OK;
        }
        virtual HRESULT WINAPI OnStopBinding(HRESULT, LPCWSTR) {
            return E_NOTIMPL;
        }
        virtual HRESULT WINAPI GetBindInfo(DWORD*, BINDINFO*) {
            return E_NOTIMPL;
        }
        virtual HRESULT WINAPI OnDataAvailable(DWORD, DWORD, FORMATETC*, STGMEDIUM*) {
            return E_NOTIMPL;
        }
        virtual HRESULT WINAPI OnObjectAvailable(REFIID, IUnknown*) {
            return E_NOTIMPL;
        }
        virtual HRESULT WINAPI OnProgress(unsigned long progress, unsigned long progressMax, unsigned long statusCode, LPCWSTR status);
    };
    std::string m_url;
    std::function<void(unsigned int progress, unsigned int progressMax, unsigned int stage, const std::string & message)> m_onProgress = nullptr;
    std::function<void(int errorCode, const std::string & errorText)> m_onFinished = nullptr;
    std::unique_ptr<StatusCallback> m_status;
public:
    MapDownloader();
    std::tuple<HRESULT, std::string> execute(const std::string& url, const std::function<void(unsigned int progress, unsigned int progressMax, unsigned stage, const std::string & message)>& onProgress);
    std::tuple<HRESULT, std::string> download();
    std::tuple<HRESULT, std::string> extract(const std::string& path);
    void cancel();
};