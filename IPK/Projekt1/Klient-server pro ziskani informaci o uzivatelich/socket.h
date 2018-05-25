/*
 * 	Author:		Tomas Zubrik, xzubri00
 * 	Subject:	IPK, FIT VUT Brno 2018
 *	Project:	Client-server application (1st Project 2nd Variant)	
 */

#ifndef H_SOCKET
#define H_SOCKET 36

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <ctype.h>
#include <stdbool.h>
#include <getopt.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define PAYLOAD_SIZE 256
#define DEFAULT_STR_ALLOC 256
#define LISTEN_QUEUE_NUMBER 5
#define ERROR -1
#define h_addr h_addr_list[0]

/* Buffer for reading the line from /etc/passwd */
char buffer[BUFFER_SIZE];

/* Socket structure */
typedef struct Socket
{
	int socket_fd;
	struct sockaddr_in remote;
	struct sockaddr_in local;
	int sockaddr_len;
	socklen_t llen;
	socklen_t rlen;
	struct hostent *host;
} Socket;

/* String structure */
typedef struct 
{
	char *str;
	int alloc_size;
	int len;
} String;

/* Enumeration of message types */
enum message_type
{
	CONNECT_REQ = 1,
	CONNECT_OK = 2,
	CONNECT_FAIL = 3,

	OPTION_REQ = 4,
	OPTION_SENT = 6,
	OPTION_OK = 7,
	OPTION_FAIL = 8,

	LOGIN_REQ = 9,
	LOGIN_SENT = 11,
	LOGIN_OK = 12,
	LOGIN_FAIL = 13,

	DATA_REQ = 14,
	DATA_OK = 16,
	DATA_FAIL = 17,
	DATA_SENT = 18,
	DATA_SENDING = 19,

	SUCCESS = 20, 
	FAILURE = 21,

	EXT_FAILURE = 22,
	EXT_SUCCESS = 23,
};

/* Message structure */
typedef struct 
{
	enum message_type type;
	char payload[PAYLOAD_SIZE];
} Message;

/**
 * @brief	Returns message type as string from enum type (integer) constant.
 * @param	int 			type 			Integer that represents the message type. (f.e. CONNECT_OK) 		
 * @note	It is used only for debugging and informative purpose.
 * @return	string representation of message type - Success, empty string - Failure
 */
char *get_msg_type(int type);

/**
 * @brief	Sends acknowledgement message.
 * @param	int 		socket_fd		Integer number that represents socket file descriptor.
 * @param	Message* 	msg 			Message to send. In this case contains no data, but only type of message.
 * @param	int 		msgtype 		Type of message that represents acknowledgment information.
 * @note	Important function for communction between Client and Server.
 */
void send_ack_message(int socket_fd, Message *msg, int msgtype);

/**
 * @brief	Sends message containing data in its payload.
 * @param	int 		socket_fd		Integer number that represents socket file descriptor.
 * @param	Message* 	msg 			Message to send. Contains data and type of message.
 * @param	char*		data 			Data, which is append to message's payload.
 * @note	Macro PAYLOAD_SIZE sets number of sent messages.
 */
void send_data_message(int socket_fd, Message *msg, char *data);

/**
 * @brief	Receives message containing data in its payload.
 * @param	int 		socket_fd		Integer number that represents socket file descriptor.
 * @param	Message* 	msg 			Message to receive. Contains data and type of message.
 * @param	String*		data 			Data represents string to which information from payload will be appended.
 * @note	Macro PAYLOAD_SIZE sets number of received messages.
 */
int recv_data_message(int socket_fd, Message *msg, String *data);

/**
 * @brief	Receives the acknowledgment message.
 * @param	int 		socket_fd		Integer number that represents socket file descriptor.
 * @param	char*		buffer 			Buffer to which will be acknowledgment message written.
 * @param	Message* 	msg 			Message to receive. In this case contains no data, but only type of message.
 * @param	int			buffer_size 	Buffer size.
 * @note	Important function for communction between Client and Server.
 * @return 	Expected message type
 */
int recv_ack_message(int socket_fd, char *buffer, Message *msg, int buffer_size);

/**
 * @brief	Initializes server process, opens server's socket, binds socket to IP address and start listening (waiting for clients requests).
 * @param	Socket* 		s				Integer number that represents socket file descriptor.
 * @param	int 			port 			Port number which is used for communication with server.
 * @note	Key function that initializes server process and setups it up.
 * @return 	Expected message type.
 */
int start_server(Socket *s, int port);

/**
 * @brief	Accepts connection. Returns common socket for communication 
 * @param	Socket*			s 				Socket structure used for getting its key properties for accept() call.	
 * @return	communction socket - Success, -1 - Failure
 */
int accept_connection(Socket *s);

/**
 * @brief	Creates client's socket. Setups up and connects to server.
 * @param	Socket*			s 				Socket structure contains information about socket properties. 
 * @param	int 			port 			Port number to which client connects.
 * @param	char* 			name 			Server DNS name or IP to which client connects.
 * @see 	Socket
 * @return	0 - Success
 */
int connect_to_server(Socket *s, int port, char* name);

/**
 * @brief	Prints error message on stderr and exits program with error code -1.
 * @param	char*			info 			Error message to be printed.
 */
void exit_with_error(char *info);

/**
 * @brief	Checks the client's input arguments
 * @param	int 			argc			Arguments count
 * @param	char**			argv			Array of arguments as strings
 * @param	int*			portnum			Port number passed as argument. 
 * @param	char**			hostname		Hostname passed as argument.
 * @param	char**			login 			Login passed as argument.
 * @param	char**			option 			Option passed as argument. 
 * @note	Value of portnum, hostname, login and option are stored in local variables in client application and are used further in program.	
 */
void check_args(int argc, char **argv, int *portnum, char **hostname, char **login, char **option); 

/**
 * @brief	Returns relevant information (string) about user. Uses user's username (login). 		
 * @param	char*			login			Pointer to char used as username (or login), from which we get requested information.
 * @param	char			option			Character recevied from client, that represents the client request.
 * @param	String*			output			Pointer to structure String used as buffer for data recevied from '/etc/passwd'.
 * @note	If username(login) isn't present in '/etc/passwd' server sends client error message.
 * @return	string with relevant information (folder or user personal information) - Success, error message - Failure
 */
char *get_users_info(char *login, char option, String *output);

/**
 * @brief	Returns list (string) of users' logins matching the substring	
 * @param	char*			substr			Substring pattern to match.
 * @param	String*			output			Pointer to structure String used as buffer for data recevied from '/etc/passwd'.
 * @note	If substring is empty returns list of all users.
 * @return	string with matching usernames - Success, NULL - Failure
 */
char *get_users_list(char *substr, String *output);

/**
 * @brief	Checks if substring matches the string from the begining.	
 * @param	char*			substr 			Substring pattern to match.
 * @param	char*			str 			String to check if starts with pattern.
 * @note	If substr is NULL, every str compared is "matched", what means success.
 * @return	true - Success, false - Failure
 */
bool starts_with(char *substr, char *str);

/**
 * @brief	Finds position of first colon in string.
 * @param	char*			str 			String in which we want to find a colon (line from /etc/passwd).
 * @note	Returns index of colon in string. If colon wasnt found, it fails.
 * @return	int - Success, 0 - Failure
 */
int find_colon(char *str);

/**
 * @brief	Initializes string variable dynamically.
 * @param	String*			s				Pointer to structure String used for dynamic initialization.
 * @note	It uses macro DEFAULT_STR_ALLOC as initiative size for allocation.
 */
void init_string(String * s);

/**
 * @brief	Appends string to string. Eventually reallocs the memory.
 * @param	String*			s 				Pointer to structure String to which the string will be appended.
 * @param	const char*		append			String to append.
 * @note	If append is NULL does nothing.
 * @return	0 - Success, 1 - Failure
 */
void append_str_to_str(String *s, const char * append);

/**
 * @brief	Prints info about messages and data flow between client and server.
 * @param	char*			info 			String that represents useful information.	
 * @param	char*			type 			String that represents type of message.
 * @note	Debugging informational function.
 */
void print_info(char * info, char *type);


#endif