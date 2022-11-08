#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

/****************************
 * function proto-
 ****************************/
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void parse_uri_ctop(char *uri_ctop, char *uri_ptos, char *port, char *host);
void read_requesthdrs(rio_t *rp);
int do_it(int fd);
void toss(int connfd, int clientfd, char *buf_rl, char *buf_hdr);
void create_ptos_request(rio_t *rp_ctop, char *method, char *uri_ptos, char *version, char *host, char *buf_rl, char *buf_hdr);

///////////////////////////////

int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {     // 메인함수에 전달되는 정보의 개수가 2개여야 함 argv[0]은 실행 경로 [1] port (server well known)
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]); // port #
  while(1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);  // line:netp:tiny:accept /* socket address = SA clientaddr를 listenfd로 채워줘 */
    Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0); // host name이 채워짐 (port -> client의 단기 port)
    
    do_it(connfd); /* 1. 브러우져한테 요청을 받아여 */
    /* close connfd */
    Close(connfd);
  }

  return 0;
}

/*
 * do_it - do what proxy is supposed to do.
 * 1. read request from client (connfd)
 * 2. parse the request (line and hdrs)
 * (call toss)
 * 3. send a modified request to server (clientfd)
 * 4. get response from server (clientfd) and toss it to client(connfd)
 */
int do_it(connfd){
  int clientfd;
  char method[MAXLINE], version[MAXLINE], port[MAXLINE], host[MAXLINE];
  char uri_ptos[MAXLINE], uri_ctop[MAXLINE];
  char buf[MAXBUF], buf_rl[MAXLINE], buf_hdr[MAXLINE];
  
  rio_t rio_ctop; 

  /* Read request line and headers from connfd */
  Rio_readinitb(&rio_ctop, connfd);
  Rio_readlineb(&rio_ctop, buf, MAXLINE);
  
  /* request line valid 검사 필요 */

  /* preprocess to parse request line */
  sscanf(buf, "%s %s %s", method, uri_ctop, version);

  /* parse request line */
  parse_uri_ctop(uri_ctop, uri_ptos, port, host);
  version[strlen(version)-1] = '0'; // HTTP/1.1 -> HTTP/1.0
  
  /* connect with server */
  clientfd = Open_clientfd(host, port);

  /* 
   * make a new request line 
   * and request headers 
   */
  create_ptos_request(&rio_ctop, method, uri_ptos, version, host, buf_rl, buf_hdr);

  /* toss request from client to server : connfd -> clientfd
   * toss resp.   from server to client : clientfd -> connfd
   */
  toss(connfd, clientfd, buf_rl, buf_hdr);

  /* close clientfd */
  Close(clientfd);
  
}

/*
 * create_ptos_request - make a new request to be sent to server, which is made out of the request from client originally.
 * in this proxy, we make request line and headers as below (following CMU proxylab guide) 
 * (request line)
 * method URI HTTP/1.0 (: version must be HTTP1.0)
 * (request hdrs)
 * Host:
 * User-Agent: user_agent_hdr
 * Connection: close
 * Proxy-Connection: close 
 */
void create_ptos_request(rio_t *rp_ctop, char *method, char *uri_ptos, char *version, char *host, char *buf_rl, char *buf_hdr){
  char buf[MAXLINE];
  /* new request line */
  sprintf(buf_rl, "%s %s %s\r\n", method, uri_ptos, version);
  
  /* make request headers 
   */

  sprintf(buf_hdr, "Host: %s\r\n", host);
  sprintf(buf_hdr, "%s%s",buf_hdr, user_agent_hdr);
  sprintf(buf_hdr, "%sConnection: close\r\n",buf_hdr);
  sprintf(buf_hdr, "%sProxy-Connection: close\r\n",buf_hdr);
  sprintf(buf_hdr, "%s\r\n",buf_hdr);
}

/*
 * toss - do a middleman job in between client and server.
 * send an appropriate request to server (clienfd)
 * get a response from server : hdr + content (clientfd)
 * send a response to client (connfd)
 */
void toss(int connfd, int clientfd, char *buf_rl, char *buf_hdr){ /* <- clientfd */
  /* 서버한테 요청해요 */
  /* : request buffer를 clientfd에 write */
  /* 응답 받아서 전달 */
  /* : clientfd가 서버에게 받아온 걸 read해서 buf에.  */
  /* buf를 클라이언트와 연결된 connfd에게 write */
  
  rio_t rio_ptos;
  char buf[MAXLINE];
  char content_length[MAXLINE];
  int content_len;
  char *srcp;
  char *ptr;

  /* send request to server */
  Rio_writen(clientfd, buf_rl, strlen(buf_rl));
  Rio_writen(clientfd, buf_hdr, strlen(buf_hdr));

  /* 
   * toss response hdr to connfd (line by line)
   * and parse content-length 
   */
  Rio_readinitb(&rio_ptos, clientfd);
  Rio_readlineb(&rio_ptos, buf, MAXLINE);
  while(strcmp(buf, "\r\n")){
    /* content-length parsing */
    if(strstr(buf, "Content-length: ")) {
      // strcpy(ptr, buf);
      ptr = index(buf, ' ');
      // ptr += strlen("Content-length: ");
      // strcpy(content_length, ptr);
      sprintf(content_length, "%s", ptr);
      content_len = atoi(content_length);
    }

    Rio_writen(connfd, buf, strlen(buf));
    Rio_readlineb(&rio_ptos, buf, MAXLINE);
  }
  Rio_writen(connfd, buf, strlen(buf));

  /* 
   * allocate memory for content and put them in scrp 
   * write content to connfd 
   */
  srcp = malloc(content_len);
  Rio_readnb(&rio_ptos, srcp, content_len);
  Rio_writen(connfd, srcp, content_len);
  free(srcp);
}

/*
 * parse_uri_ctop - parse an uri from client.
 * so we get 1. port# 2. host 3. uri (w/o domain part, stripped)
 */
void parse_uri_ctop(char *uri_ctop, char *uri_ptos, char *port, char *host)
{ 
  char *ptr;
  if(strstr(uri_ctop, "http://")) uri_ctop += strlen("http://");  /* strip 'http://' */
  
  if((ptr = index(uri_ctop, '/')) != NULL){
    strcpy(uri_ptos, ptr);                        /* make uri p to s */
    *ptr = '\0';                                    
  } else *uri_ptos = '/';

  if((ptr = index(uri_ctop, ':')) != NULL){
    strcpy(port, ptr+1);                          /* get port # for server */
    *ptr = '\0';                                  /* strip port from uri c to p */ 
  } else port = "80";  

  strcpy(host, uri_ctop);                         /* make host */
}
