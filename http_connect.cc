#include "http_connect.h"


/* inline Http_connect::Http_connect(){} */

void Http_connect::init(int fd){
    m_fd = fd;
    m_parse_state = STATE_PARSE_URI; 
    m_now_pos = 0;
    m_line_state = LINE_START;
    m_keep_alive = false;
}

STATE_OF_ANANYSIS Http_connect::handleGet(){
    char header[MAXSIZE];
    sprintf(header,"HTTP/1.1 %d %s\r\n",200,"OK");
    /* Exist connection and it is keep-alive. */
    if(m_map.find("Connection") != m_map.end() && m_map["Connection"] == "keep-alive"){
        m_keep_alive = true;
        sprintf(header,"%sConnection: keep-alive\r\n",header);
        sprintf(header,"%sKeep-Alive: timeout=%d\r\n",header,EPOLL_WAIT_TIME);
    }

    size_t dot_pos = m_file_name.find('.');

    const char* file_type = "text/html";

    /* if(dot_pos == string::npos){ */
    /*     file_type = MimeType::getMime("default"); */
    /* }else{ */
    /*     file_type = MimeType::getMime(m_file_name.substr(dot_pos + 1).c_str()); */
    /* } */

    /* Get the file'sinfomation. */
    struct stat sbuf;
    if(stat(m_file_name.c_str(),&sbuf) < 0){
        log_err("%s doesn't exist\n",m_file_name.c_str());
        /* handleError(); */
        return ANALYSIS_ERROR;
    }

    sprintf(header,"%sContent-type:%s\r\n",header,file_type);
    sprintf(header,"%sContent-length:%ld\r\n",header,sbuf.st_size);
    
    /* Write the end CR and LF. */
    sprintf(header,"%s\r\n",header);

    /* Send header to the client. */
    size_t send_len = (size_t)write(m_fd,header,strlen(header));
    if(send_len != strlen(header)){
        log_err("Socket:%d Send header failed.",m_fd);
        return ANALYSIS_ERROR;
    }

    /* Opend file and send file. */
    int file_fd = open(m_file_name.c_str(),O_RDONLY,0);
    /* Create map relation. */
    char *file_addr = static_cast<char*>(mmap(NULL,sbuf.st_size,PROT_READ,MAP_PRIVATE,file_fd,0));
    send_len = write(m_fd,file_addr,sbuf.st_size);
    if(send_len != (size_t)sbuf.st_size){
        log_err("Socket:%d Send file  failed.",m_fd);
    }
    /* Delete the map. */
    munmap(file_addr,sbuf.st_size);
    close(file_fd);
    return ANALYSIS_SUNCESS;
}
STATE_OF_ANANYSIS Http_connect::analysisRequest(){

    if(m_method == METHOD_GET){
        return handleGet();
    }else if(m_method == METHOD_POST){
        return ANALYSIS_SUNCESS;
    }else{
        return ANALYSIS_ERROR;
    }
}
URI_STATE Http_connect::parse_URI(){
    /* First parse the request line. */
    size_t pos = m_content.find('\r',m_now_pos);
    //Something is not good.
    if(pos == string::npos || m_content[pos+1] != '\n'){
        return PARSE_URI_ERROR;
    }

    /* Parse the request line. */
    string request_line = m_content.substr(0,pos);
    pos = request_line.find("GET");
    if(!pos){//GET
        m_method = METHOD_GET;
    }else{
        if(!request_line.find("POST")){
            m_method = METHOD_POST;
        }else{
            return PARSE_URI_ERROR;
        }
    }

    //URI
    pos = request_line.find("/",pos);
    if(pos == string::npos){
        return PARSE_URI_ERROR;
    }else{
        size_t _pos = request_line.find(" ",pos);
        if(_pos == string::npos)
            return PARSE_URI_ERROR;

        if(_pos - pos == 1) m_file_name = "index.html";
        else{
            m_file_name = request_line.substr(pos+1,_pos-1-pos);
        }
        pos = _pos;
    }
    debug("filename:%s\n",m_file_name.c_str());

    //HTTP version
    pos = request_line.find("/",pos);
    if(pos == string::npos){
        return PARSE_URI_ERROR;
    }else{
        string http_version = request_line.substr(pos+1.3);
        if(http_version == "1.0"){
            m_http_version = HTTP_10;
            debug("HTTP version:%s\n",http_version.c_str());
        }else if(http_version == "1.1"){
            m_http_version = HTTP_11;
            debug("HTTP version:%s\n",http_version.c_str());
        }else{
            return PARSE_URI_ERROR;
        }
    }

    /* Get new begin postion. */
    m_now_pos = m_content.find("\n") + 1;
    return PARSE_URI_SUCESS;
}

HEADER_STATE Http_connect::parse_header(){
    size_t r_pos = m_content.find('\r',m_now_pos);
    if(r_pos == string::npos || m_content[r_pos + 1] != '\n'){
        log_err();
        return PARSE_HEADER_ERROR;
    }

    /* Get one line including key and value. */
    string one_line = m_content.substr(m_now_pos,r_pos - m_now_pos + 1);
    string key,value;
    size_t pos = 0;

    /* END_CR symbol */
    if(one_line.size() == 1){
        if(m_content[r_pos + 1] == '\n')
            m_line_state = LINE_END_CR;
        else return PARSE_HEADER_ERROR;
    }

    /* debug("%s\n",one_line.c_str()); */
    while(true) {
        switch(m_line_state){
        case LINE_START:{//default
                            if((pos = one_line.find(':')) == string::npos){
                                log_err();
                                return PARSE_HEADER_ERROR;
                            }else{
                                key = one_line.substr(0,pos);
                                m_line_state = LINE_VALUE;
                            }
                        }
        case LINE_VALUE:{
                            /* There is a space after colon. */
                            value = one_line.substr(pos+2,r_pos - 1 - pos -1);
                            /* State transfer. */ 
                            m_line_state = LINE_START;
                            m_now_pos = r_pos + 2;
                            debug("key:%s value:%s",key.c_str(),value.c_str());
                            m_map[key] = value;
                            return PARSE_HEADER_AGAIN; 
                        }
        case LINE_END_CR:{
                             return PARSE_HEADER_SUCESS;
                         }
        }
    }
}

void Http_connect::handle_request(){

    while(true){
        /* Read from the socket. */
        int size = read(m_fd,m_buf,MAXSIZE);
        /* debug("%s\n\n",m_buf); */
        m_content = m_buf;

        if(m_parse_state == STATE_PARSE_URI){//default

            URI_STATE uri_state = parse_URI();
            if(uri_state == PARSE_URI_ERROR){
                log_err("Sockfd:%d parse_URI error",m_fd);
                break;
            }else{
                /* State transfer. */
                m_parse_state = STATE_PARSE_HEADERS;
            }

        }else if(m_parse_state == STATE_PARSE_HEADERS){

            HEADER_STATE header_state = parse_header();
            if(header_state == PARSE_HEADER_AGAIN){
                continue;
            }
            else if(header_state == PARSE_HEADER_ERROR){
                log_err("Sockfd:%d parse_header error",m_fd);
                break;
            }else if(header_state == PARSE_HEADER_SUCESS){
                if(m_method == METHOD_GET){
                    m_parse_state = STATE_PARSE_ANALYSIS;
                }else{
                    m_parse_state = STATE_PARSE_RECVBODY;
                }
            }

        }else if(m_parse_state == STATE_PARSE_ANALYSIS){

            STATE_OF_ANANYSIS state_of_analysis = analysisRequest();
            if(state_of_analysis == ANALYSIS_ERROR){
                log_err("Sockfd:%d analysis error",m_fd);
                break;
            }else if(state_of_analysis == ANALYSIS_SUNCESS){
                m_parse_state = STATE_PARSE_FINISH;
                break;
            }

        }else if(m_parse_state == STATE_PARSE_RECVBODY){

            ;

        }else if(m_parse_state == STATE_PARSE_FINISH){

            debug("Finish");
            break;

        }


    }
}
