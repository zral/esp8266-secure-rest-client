#include "WifiSecureRestClient.h"

#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string) (Serial.print(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)
#endif

WifiSecureRestClient::WifiSecureRestClient( const char* _host ) {

  host = _host;
  port = 443;
  num_headers = 0;
  contentType = "x-www-form-urlencoded";	// default
}

WifiSecureRestClient::WifiSecureRestClient(const char* _host, int _port) {

  host = _host;
  port = _port;
  num_headers = 0;
  contentType = "x-www-form-urlencoded";	// default
}

// GET path
int WifiSecureRestClient::get(const char* path){
  return request("GET", path, NULL, NULL);
}

//GET path with response
int WifiSecureRestClient::get(const char* path, String* response){
  return request("GET", path, NULL, response);
}

// POST path and body
int WifiSecureRestClient::post(const char* path, const char* body){
  return request("POST", path, body, NULL);
}

// POST path and body with response
int WifiSecureRestClient::post(const char* path, const char* body, String* response){
  return request("POST", path, body, response);
}

// PUT path and body
int WifiSecureRestClient::put(const char* path, const char* body){
  return request("PUT", path, body, NULL);
}

// PUT path and body with response
int WifiSecureRestClient::put(const char* path, const char* body, String* response){
  return request("PUT", path, body, response);
}

// DELETE path
int WifiSecureRestClient::del(const char* path){
  return request("DELETE", path, NULL, NULL);
}

// DELETE path and response
int WifiSecureRestClient::del(const char* path, String* response){
  return request("DELETE", path, NULL, response);
}

// DELETE path and body
int WifiSecureRestClient::del(const char* path, const char* body ){
  return request("DELETE", path, body, NULL);
}

// DELETE path and body with response
int WifiSecureRestClient::del(const char* path, const char* body, String* response){
  return request("DELETE", path, body, response);
}

void WifiSecureRestClient::write( const char* string ){
  HTTP_DEBUG_PRINT(string);
  client.print(string);
}

void WifiSecureRestClient::setHeader(const char* header){
  headers[num_headers] = header;
  num_headers++;
}

void WifiSecureRestClient::setContentType(const char* contentTypeValue){
  contentType = contentTypeValue;
}

// The mother- generic request method.
//
int WifiSecureRestClient::request(const char* method, const char* path,
                  const char* body, String* response){

  HTTP_DEBUG_PRINT("HTTPS: connect\n");

  if(client.connect(host, port)){
    HTTP_DEBUG_PRINT("HTTPS: connected\n");
    HTTP_DEBUG_PRINT("REQUEST: \n");
    // Make a HTTP request line:
    write(method);
    write(" ");
    write(path);
    write(" HTTP/1.1\r\n");
    for(int i=0; i<num_headers; i++){
      write(headers[i]);
      write("\r\n");
    }
    write("Host: ");
    write(host);
    write("\r\n");
    write("Connection: close\r\n");

    if(body != NULL){
      char contentLength[30];
      sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
      write(contentLength);

	  write("Content-Type: ");
	  write(contentType);
	  write("\r\n");
    }

    write("\r\n");

    if(body != NULL){
      write(body);
      write("\r\n");
      write("\r\n");
    }

    //make sure you write all those bytes.
    delay(100);

    HTTP_DEBUG_PRINT("HTTPS: call readResponse\n");
    int statusCode = readResponse(response);
    HTTP_DEBUG_PRINT("HTTPS: return readResponse\n");

    //cleanup
    HTTP_DEBUG_PRINT("HTTPS: stop client\n");
    num_headers = 0;
    client.stop();
    delay(50);
    HTTP_DEBUG_PRINT("HTTPS: client stopped\n");

    return statusCode;
  }else{
    HTTP_DEBUG_PRINT("HTTP Connection failed\n");
    return 0;
  }
}

int WifiSecureRestClient::readResponse(String* response) {

  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  boolean httpBody = false;
  boolean inStatus = false;

  char statusCode[4];
  int i = 0;
  int code = 0;

  if(response == NULL){
    HTTP_DEBUG_PRINT("HTTPS: NULL RESPONSE POINTER: \n");
  }else{
    HTTP_DEBUG_PRINT("HTTPS: NON-NULL RESPONSE POINTER: \n");
  }

  HTTP_DEBUG_PRINT("HTTPS: RESPONSE: \n");
  while (client.connected()) {
    HTTP_DEBUG_PRINT(".");

    if (client.available()) {
      HTTP_DEBUG_PRINT(",");

      char c = client.read();
      HTTP_DEBUG_PRINT(c);

      if(c == ' ' && !inStatus){
        inStatus = true;
      }

      if(inStatus && i < 3 && c != ' '){
        statusCode[i] = c;
        i++;
      }
      if(i == 3){
        statusCode[i] = '\0';
        code = atoi(statusCode);
      }

      if(httpBody){
        //only write response if its not null
        if(response != NULL) response->concat(c);
      }
      else
      {
          if (c == '\n' && currentLineIsBlank) {
            httpBody = true;
          }

          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          }
          else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
      }
    }
  }

  HTTP_DEBUG_PRINT("HTTPS: return readResponse3\n");
  return code;
}
