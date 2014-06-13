/*
 * proxy.c - CS:APP Web proxy
 *
 * TEAM MEMBERS:
 *     Andrew Carnegie, ac00@cs.cmu.edu 
 *     Harry Q. Bovik, bovik@cs.cmu.edu
 * 
 * A simple web proxy implement.
 * No cache. No thread pool. No post method.
 * 
 */ 

#include "csapp.h"
#include <stdarg.h>

#define _DEBUG

#define DEFPORT 8888

typedef struct client_info
{
  int connfd;
  struct sockaddr_in sockaddr;
} client_info_t;

/*
 * Gloval vars
 */
sem_t sem_log;
sem_t sem_dns;
FILE *log_file;

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, int *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);
void *work_init(void *arg);
void do_work(int connfd, struct sockaddr_in *sockaddr);
void my_clienterror(int fd, int errnum, char *errmsg, char *longmsg);
int open_clientfd_ts(char *hostname, int port);
int Open_clientfd_ts(char *hostname, int port) ;
void dbg_printf(char *format, ...);



/*
 * dbg_printf - only print msg in debug mode
 */
void dbg_printf(char *format, ...)
{
#ifdef _DEBUG
    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
#endif
}

/*
 * wrapper of three rio functions
 */
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n) 
{
    ssize_t rc;

    if ((rc = rio_readnb(rp, usrbuf, n)) < 0)
    {
      fprintf(stderr, "Rio_readnb error: %s\n", strerror(errno));
      return 0;
    }
    return rc;
}

ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen) 
{
    ssize_t rc;

    if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
    {
      fprintf(stderr, "Rio_readlineb error: %s\n", strerror(errno));
      return 0;
    }
    return rc;
} 

void Rio_writen_w(int fd, void *usrbuf, size_t n) 
{
    if (rio_writen(fd, usrbuf, n) != n)
      fprintf(stderr, "Rio_writen error: %s\n", strerror(errno));
}

/*
 * Open_clientfd_ts - wrapper of open_clientfd_s
 */
int Open_clientfd_ts(char *hostname, int port) 
{
    int rc = open_clientfd_ts(hostname, port);
    if (rc == -1)
      unix_error("Open_clientfd_s Unix error");
    else if(rc == -2)        
	    dns_error("Open_clientfd_s DNS error");
    return rc;
}

/*
 * open_clientfd_s - thread-safe version for open_clientfd
 */
int open_clientfd_ts(char *hostname, int port) 
{
    int clientfd;
    struct hostent *hp;
    struct sockaddr_in serveraddr;

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      return -1; /* check errno for cause of error */

    /* Fill in the server's IP address and port */
    P(&sem_dns);
    if ((hp = gethostbyname(hostname)) == NULL)
    {
      V(&sem_dns);
      return -2; /* check h_errno for cause of error */
    } 
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0], 
	  (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    V(&sem_dns);
    serveraddr.sin_port = htons(port);

    /* Establish a connection with the server */
    if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
      return -1;
    return clientfd;
}

/* 
 * my_clienterror - client error report on my own style
 */
void my_clienterror(int fd, int errnum, char *errmsg, char *longmsg)
{
  char buff[MAXLINE];

  sprintf(buff, "HTTP/1.1 %d %s\r\n", errnum, errmsg);
  Rio_writen_w(fd, buff, strlen(buff));
  sprintf(buff, "Content-type: text/html\r\n");
  Rio_writen_w(fd, buff, strlen(buff));
  sprintf(buff, "Content-length: %d\r\n\r\n", strlen(longmsg));
  Rio_writen_w(fd, buff, strlen(buff));
  Rio_writen_w(fd, longmsg, strlen(longmsg));
}


/* 
 * do_work - deal with the request by the client
 */
void do_work(int connfd, struct sockaddr_in *sockaddr)
{
  //read the request line
  rio_t conn_rio;
  Rio_readinitb(&conn_rio, connfd);

  char req_line[MAXLINE];
  char method[MAXLINE],
       uri[MAXLINE],
       ver[MAXLINE];
  Rio_readlineb_w(&conn_rio, req_line, MAXLINE);
  if(sscanf(req_line, "%s %s %s", method, uri, ver) < 3)
  {
    my_clienterror(connfd, 400, "Bad Request", "request line parse error");
    return;
  }
  if(strcmp(method, "GET"))
  {
    my_clienterror(connfd, 501, "Not Implemented", "Not Implemented");
    return;
  }

  //parse uri
  char host[MAXLINE],
       path[MAXLINE];
  int  port;
  if(!parse_uri(uri, host, path, &port))
  {
    my_clienterror(connfd, 400, "Bad Request", "uri parse error");
    return;
  }

  //build the request line for real svr
  strcpy(req_line, method);
  strcat(req_line, " ");
  strcat(req_line, path);
  strcat(req_line, " ");
  strcat(req_line, ver);
  strcat(req_line, "\r\n");
  dbg_printf("[%u reqline] %s", Pthread_self(), req_line);

  //create a connection to real svr
  int svr_clientfd = Open_clientfd_ts(host, port);
  rio_t svr_rio;
  Rio_readinitb(&svr_rio, svr_clientfd);

  //Send the request line
  Rio_writen_w(svr_clientfd ,req_line, strlen(req_line));

  //Read the request hdr and send it
  char buff[MAXLINE];
  while(Rio_readlineb_w(&conn_rio, buff, MAXLINE) > 2)
  {
    //if(strstr(buff, "Proxy-Connection: keep-alive") != 0)
    //    continue;
    dbg_printf("[%u reqhdr] %s", Pthread_self(), buff);
    Rio_writen_w(svr_clientfd ,buff, strlen(buff));
  }

  //Send a empty line
  Rio_writen_w(svr_clientfd, "\r\n", 2);

  dbg_printf("[%u] client data sent up\n", Pthread_self());

  /*//get response line, hdr and send back
  int len = 0;
  int found = 0;
  while(Rio_readlineb_w(&svr_rio, buff, MAXLINE) > 2)
  {
    //get length
    char *result = strstr(buff, "Content-Length");
    if(result != 0)
    {
      found = 1;
      result += 16;
      len = atoi(result);
    }

    dbg_printf("[%u reshdr] %s", Pthread_self(), buff);
    Rio_writen_w(connfd, buff, strlen(buff));
  }

  //send a empty line
  Rio_writen_w(connfd, "\r\n", 2);

  //get length
  if(!found)
  {
    Rio_readlineb_w(&svr_rio, buff, MAXLINE);
    len = strtol(buff, 0, 16);
    Rio_writen_w(connfd, buff, strlen(buff));
  }
  dbg_printf("[%u] res data length: %d\n", Pthread_self(), len);*/

  //send back response data
  int res_size = 0;
  int size;
  while((size = Rio_readnb_w(&svr_rio, buff, MAXLINE)) != 0)
  {
    res_size += size;
    dbg_printf("[%u resdata]\n", Pthread_self());
    Rio_writen_w(connfd, buff, size);
  }

  dbg_printf("[%u] svr data sent up\n", Pthread_self());

  //write log
  format_log_entry(buff, sockaddr, uri, res_size);
  P(&sem_log);
  fprintf(log_file, "%s\n", buff);
  fflush(log_file);
  V(&sem_log);

  //close
  Close(svr_clientfd);
}

/* 
 * work_init - The function excecuted by sub threads 
 */
void *work_init(void *arg)
{
  client_info_t *p = (client_info_t *)arg;
  int connfd = p->connfd;
  struct sockaddr_in sockaddr = p->sockaddr;
  free(arg);
  Pthread_detach(pthread_self());
  do_work(connfd, &sockaddr);
  Close(connfd);
  return 0;
}

/* 
 * main - Main routine for the proxy program 
 */
int main(int argc, char **argv)
{
    int listenfd,
        connfd,
        port,
        clientlen;
    struct sockaddr_in clientaddr;
    pthread_t tid;

    /* Check arguments */
    /*if (argc != 2) {
      fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
      exit(0);
    }*/

    port = (argc != 2)? DEFPORT: atoi(argv[1]);
    signal(SIGPIPE, SIG_IGN);
    sem_init(&sem_log, 0, 1);
    sem_init(&sem_dns, 0, 1);
    log_file = fopen("proxy.log", "a");

    listenfd = Open_listenfd(port);
    while(1)
    {
      clientlen = sizeof(clientaddr);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      client_info_t *arg = (client_info_t *)malloc(sizeof(client_info_t));
      arg->connfd = connfd;
      arg->sockaddr = clientaddr;
      Pthread_create(&tid, 0, work_init, (void *)arg);
      dbg_printf("[%u] a client accepted\n", tid);
    }
    
    exit(0);
}


/*
 * parse_uri - URI parser
 * 
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return 0 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, int *port)
{
    int len = strlen(uri);
    int tmp_port = 80;

    if (strncasecmp(uri, "http://", 7) != 0) 
      return 0;

    char *hostbegin = uri + 7;
    char *pathbegin = strchr(hostbegin, '/');
    if(pathbegin == 0)
    {
      strcpy(pathname, "/");
      pathbegin = uri + len;
    }
    else
    {
      int path_len = len - (pathbegin - uri);
      strncpy(pathname, pathbegin, path_len);
      pathname[path_len] = 0;
    }

    char *portbegin = strchr(hostbegin, ':');
    if(portbegin != 0)
      tmp_port = atoi(portbegin + 1);
    else
      portbegin = pathbegin;

    int host_len = pathbegin - hostbegin;
    strncpy(hostname, hostbegin, host_len);
    hostname[host_len] = 0;
      
    *port = tmp_port;
    return 1;

    
    /* Extract the host name */
    /*hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';*/
    
    /* Extract the port number */
    //*port = 80; /* default */
    /*if (*hostend == ':')   
      *port = atoi(hostend + 1);*/
    
    /* Extract the path */
    /*pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
      pathname[0] = '\0';
    }
    else {
      pathbegin++;	
      strcpy(pathname, pathbegin);
    }

    return 0;*/
}

/*
 * format_log_entry - Create a formatted log entry in logstring. 
 * 
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, 
		      char *uri, int size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /* 
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 13, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;


    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, size);
}


