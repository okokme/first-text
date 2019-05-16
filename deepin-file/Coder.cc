#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <string>
#include <memory>
#include "Channel.h"
#include "Buffer.h"
#include "Coder.h"
#include "Logger.h"

void fileMkdir(const char *pathname, const char *mac)
{
    std::string t("./backupfile/");
	t += mac;
	t += "/";
    t += pathname;

    auto t2 = t.begin() + 3;
    while (t2 <= t.end()) {
        for ( ; t2 != t.end(); t2++) {
            if(*t2 == '/')
                break;
        }
        if (t2 >= t.end())
            break;
        char c[100];
        bzero(c, sizeof(c));
        strncpy(c, t.c_str(), t2 - t.begin());
        mkdir(c, 0777);
        t2++;
    }
}

void parseLine(Request& req, Buffer& buf, int len)
{
    char *p = buf.peek();
    char *ep = p + len;

    req.type().clear();
    req.path().clear();
    req.mac().clear();
    while (p < ep && (*p == ' ' || *p == '\t'))
        p++;
    while (p < ep && !isspace(*p))
        req.type().push_back(*p++);
    while (p < ep && (*p == ' ' || *p == '\t'))
        p++;
    while (p < ep && !isspace(*p))
        req.path().push_back(*p++);
    while (p < ep && (*p == ' ' || *p == '\t'))
        p++;
    while (p < ep && !isspace(*p))
        req.mac().push_back(*p++);
    req.setState(Request::HEADER);
}

void parseHeader(Request& req, Buffer& buf, int len)
{
    char *p = buf.peek();
    char *ep = p + len;

    if (strncmp(p, "\r\n", 2) == 0) {
        // std::cout << "parse success" << std::endl;
        if (req.type() == "SAVE")
            req.setState(Request::OK | Request::RECVING);
        else if (req.type() == "GET")
            req.setState(Request::OK);
        return;
    }
    if (strncasecmp(p, "filesize:", 9) == 0) {
        p += 9;
        while (p < ep && (*p == ' ' || *p == '\t'))
            p++;
        req.setFilesize(atoi(p));
        while (p < ep && isalnum(*p))
            p++;
    } else {
        ; // 目前头部只有一个字段
    }
}

// 解析客户请求
void parseRequest(Buffer& buf, Request& req)
{
    // 可能有一行完整的消息
    while (buf.readable() >= 2) {
        int crlf = buf.findCrlf();
        // 至少有一行请求
        if (crlf >= 0) {
            if (req.state() == Request::LINE) {
                parseLine(req, buf, crlf);
                crlf += 2;
            } else if (req.state() == Request::HEADER) {
                parseHeader(req, buf, crlf);
                crlf += 2;
            } else
                break;
            buf.retrieve(crlf);
        } else
            break;
    }
}

std::string getPathname(Request& req)
{
    std::string pathname("./backupfile/");
    pathname += req.mac() + req.path();
    return pathname;
}

// SAVE-OK\r\n
// filesize: 0\r\n
// \r\n
void replySaveOk(Channel *chl)
{
    std::string s("SAVE-STATUS\r\n");
    s += "filesize: 0\r\n";
    s += "\r\n";
    chl->send(s.c_str(), s.size());
    logDebug("send %d bytes to fd(%d): %s", s.size(),
            chl->fd(), s.c_str());
}

// GET-OK path\r\n
// filesize: size\r\n
// \r\n
// Text
void replyGetOk(Channel *chl, Request& req)
{
    char buf[32];
    struct stat st;
    if (lstat(getPathname(req).c_str(), &st) < 0) {
        logDebug("lstat %s error: %s", getPathname(req).c_str(),
                strerror(errno));
        return;
    }
    snprintf(buf, sizeof(buf), "%lld\r\n", st.st_size);
    std::string s("GET-STATUS ");
    s += req.path();
    s += "\r\n";
    s += "filesize: ";
    s += buf;
    s += "\r\n";
    chl->send(s.c_str(), s.size());
    logDebug("send %d bytes to fd(%d): %s", s.size(),
            chl->fd(), s.c_str());
}

// 接收客户端发来的文件以更新本地文件
void recvFile(Channel *chl, Buffer& buf, Request& req)
{
    std::string pathname = getPathname(req);
    if (req.fd() < 0) {
        int fd = open(pathname.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666);
        if (fd < 0) {
            logWarn("file(%s) can't open: %s", pathname.c_str(), strerror(errno));
            return;
        }
        req.setFd(fd);
    }
    size_t n = buf.readable();
    if (n >= req.filesize()) {
        n = req.filesize();
        req.setState(Request::LINE);
    } else
        req.setFilesize(req.filesize() - n);
    write(req.fd(), buf.peek(), n);
    buf.retrieve(n);
    logDebug("write %zu bytes to file(%s)", n, pathname.c_str());
    if (req.state() == Request::LINE) {
        replySaveOk(chl);
        close(req.fd());
        req.setFd(-1);
        if (buf.readable() > 0)
            req.setState(req.state() | Request::CONTINUE);
    }
}

// 将备份文件发送给客户
void sendFile(Channel *chl, Request& req)
{
    Buffer buf;
    std::string pathname = getPathname(req);
    int fd = open(pathname.c_str(), O_RDONLY);
    while (buf.readFd(fd) > 0) {
        logDebug("read %zu bytes from file(%s): %s", buf.readable(),
                pathname.c_str(), buf.c_str());
        chl->send(buf.peek(), buf.readable());
        buf.retrieveAll();
    }
    close(fd);
}

// 向客户回复响应信息
void replyResponse(Channel *chl, Buffer& buf, Request& req)
{
    if (req.type() == "SAVE") {
        recvFile(chl, buf, req);
    } else if (req.type() == "GET") {
        replyGetOk(chl, req);
        sendFile(chl, req);
    }
}

void printInfo(Request& req)
{
    std::cout << "TYPE:     " << req.type() << std::endl
              << "PATH:     " << req.path() << std::endl
              << "MAC:      " << req.mac()  << std::endl
              << "FILESIZE: " << req.filesize() << std::endl;
}

void onMessage(std::shared_ptr<Channel> chl, Buffer& buf)
{
    // std::cout << buf.c_str();
    // buf.retrieveAll();
    Channel *chlptr = chl.get();
    Request& req = chl->req();
    if (req.state() & Request::RECVING) {
        // 继续接收剩余的文件
        recvFile(chlptr, buf, req);
        return;
    } else {
        // 解析新到来的请求
_next:
        parseRequest(buf, req);
    }
    if (req.state() & Request::OK) {
        printInfo(req);
        logDebug("recv packages from fd(%d), type: %s, path: %s, mac: %s, filesize: %zu",
                chl->fd(), req.type().c_str(), req.path().c_str(), req.mac().c_str(), req.filesize());
        fileMkdir(req.path().c_str(), req.mac().c_str());
        replyResponse(chlptr, buf, req);
        if (req.state() & Request::CONTINUE) {
            req.setState(req.state() & ~Request::CONTINUE);
            goto _next;
        }
    }
}
