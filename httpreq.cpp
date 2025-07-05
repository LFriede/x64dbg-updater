#include "httpreq.h"
#include "Windows.h"
#include "wininet.h"

HttpReq::HttpReq(QObject *parent, const char *url)
    : QThread{parent}
{
    this->url = url;
}

void HttpReq::run() {
    QString content;

    HINTERNET hInternet = NULL;
    HINTERNET hReq = NULL;

    DWORD status = 0;

    try {
        hInternet = InternetOpenW(
            L"x64dbg-updater",
            INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,
            NULL,
            0
        );
        if (!hInternet) {
            throw std::exception("InternetOpen() failed.");
        }

        hReq = InternetOpenUrlW(
            hInternet,
            url.toStdWString().c_str(),
            NULL,
            0,
            INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE,
            0
        );
        if (!hReq) {
            std::string err = "InternetOpenUrl() failed, LastError -> ";
            err += std::to_string(GetLastError());
            throw std::exception(err.c_str());
        }

        DWORD buflen = sizeof(status);
        if (!HttpQueryInfoW(
            hReq,
            HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
            &status,
            &buflen,
            NULL
        )) {
            throw std::exception("Unabled to get HTTP status code.");
        }
        if (status != 200) {
            throw std::exception(("Unexpected HTTP status code: " + std::to_string(status)).c_str());
        }

        char readBuf[1024];
        while (InternetReadFile(
            hReq,
            &readBuf,
            sizeof(readBuf),
            &buflen
        )) {
            // Read is finished if returnbuffer is empty
            if (buflen == 0) {
                break;
            }

            content.append(QString::fromUtf8(readBuf, buflen));
        }
    } catch (const std::exception &ex) {
        content = ex.what();
    }

    if (!hInternet) {
        InternetCloseHandle(hInternet);
    }
    if (!hReq) {
        InternetCloseHandle(hReq);
    }

    emit resultReady(status, content);
}
