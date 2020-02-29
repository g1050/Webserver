#include "http_connect.h"


/* inline Http_connect::Http_connect(){} */

void Http_connect::init(int fd){
    m_fd = fd;
    m_parse_state = STATE_PARSE_URI; 
    m_now_pos = 0;
}

URI_STATE Http_connect::parse_URI(){
    /* First parse the request line. */
    size_t pos = m_content.find('\r',m_now_pos);
    //Something is not good.
    if(pos == string::npos){
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

    /* State transfer. */
    m_parse_state = STATE_PARSE_HEADERS; 
    return PARSE_URI_SUCESS;
}

HEADER_STATE Http_connect::parse_header(){
    
}

void Http_connect::handle_request(){

    while(true){
        /* Read from the socket. */
        int size = read(m_fd,m_buf,MAXSIZE);
        debug("%s\n\n",m_buf);
        m_content = m_buf;

        if(m_parse_state == STATE_PARSE_URI){
            URI_STATE uri_state = parse_URI();
            if(uri_state == PARSE_URI_ERROR){
                log_err("Sockfd:%d parse_URI error",m_fd);
            }
        }else if(m_parse_state == STATE_PARSE_HEADERS){
            HEADER_STATE header_state = parse_header();
        }


    }
}
