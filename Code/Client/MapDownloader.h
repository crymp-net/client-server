#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <functional>
#include <future>
#include <tuple>
#include <iostream>
#include <filesystem>
#include <Urlmon.h>
#include <Shlwapi.h>
#include <ShlObj.h>

class MapDownloader {
private:
    std::atomic<bool> m_active = false;
    class StatusCallback : public IBindStatusCallback {
        MapDownloader* m_parent;
    public:
        StatusCallback(MapDownloader* parent) : m_parent(parent) {}
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
        virtual HRESULT WINAPI OnProgress(unsigned long progress, unsigned long progressMax, unsigned long statusCode, LPCWSTR status) {
            if (m_parent->m_active) {
                if (m_parent->m_onProgress)
                    m_parent->m_onProgress(progress, progressMax, std::string("Downloading, speed: NaN MB/s"));
                return S_OK;
            }
            else {
                return E_ABORT;
            }
        }
    };
    std::string m_url;
    std::function<void(unsigned int progress, unsigned int progressMax, const std::string & message)> m_onProgress = nullptr;
    std::function<void(int errorCode, const std::string & errorText)> m_onFinished = nullptr;
    StatusCallback m_status;
public:
    MapDownloader() : m_status(this) {}
    std::tuple<HRESULT, std::string> execute(const std::string& url, const std::function<void(unsigned int progress, unsigned int progressMax, const std::string & message)>& onProgress);
    std::tuple<HRESULT, std::string> download();
    std::tuple<HRESULT, std::string> extract(const std::string& path);
    void cancel();
};