#ifndef http_connect_H
#define http_connect_H

#include "ulity.h"
using namespace std;
const int EPOLL_WAIT_TIME = 500;
enum PARSE_STATE{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS ,
    STATE_PARSE_RECVBODY,
    STATE_PARSE_ANALYSIS,
    STATE_PARSE_FINISH
};

enum URI_STATE{
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCESS
};

enum HEADER_STATE{
    PARSE_HEADER_AGAIN = 1,
    PARSE_HEADER_ERROR,
    PARSE_HEADER_SUCESS
};

enum STATE_OF_ANANYSIS{
    ANALYSIS_ERROR = 1,
    ANALYSIS_SUNCESS,
};

enum METHOD{
    METHOD_GET = 1,
    METHOD_POST,
};

enum HTTP_VERSION{
    HTTP_10 = 1,
    HTTP_11 
};

enum LINE_STATE{
    LINE_START = 1,
    LINE_KEY,
    LINE_VALUE,
    LINE_END_CR,
    LINE_END_LF
};
class Http_connect{

public:
    Http_connect(){} 
    void handle_request();
    void init(int fd);

private:
    int m_fd;
    char m_buf[MAXSIZE];
    PARSE_STATE m_parse_state ;
    METHOD m_method;
    HTTP_VERSION m_http_version;
    LINE_STATE m_line_state;
    bool m_keep_alive;
    string m_content;
    string m_file_name;
    size_t m_now_pos;
    map<string,string> m_map;
private:
    STATE_OF_ANANYSIS analysisRequest();
    STATE_OF_ANANYSIS handleGet();
    URI_STATE parse_URI();
    HEADER_STATE parse_header();


};

#endif

