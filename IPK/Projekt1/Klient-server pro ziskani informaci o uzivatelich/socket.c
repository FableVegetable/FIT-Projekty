/*
 * 	Author:		Tomas Zubrik, xzubri00
 * 	Subject:	IPK, FIT VUT Brno 2018
 *	Project:	Client-server application (1st Project 2nd Variant)	
 */

#include "socket.h"

void print_info(char * info, char *type)
{
	//printf("INFO ... %s %s\n", info, type);
}

void send_ack_message(int socket_fd, Message *msg, int msgtype)
{
	(*msg).type = msgtype;
	memset(&((*msg).payload), 0, PAYLOAD_SIZE);	
	(*msg).payload[0]= '\0';
	if((send(socket_fd, (char *)&(*msg), sizeof((*msg)), 0))==ERROR)
	{
		perror("Error in send_ack_message: ");
		exit_with_error("Error occured while sending the message");
	}
	print_info("Message sent      ",  get_msg_type((*msg).type));
}

void send_data_message(int socket_fd, Message *msg, char *data)
{
	int count=0;
	int data_sent = 0;
	int data_len = strlen(data);

	while(data_sent < data_len)
	{
		(*msg).type = DATA_SENDING;
		memset(&((*msg).payload),0,PAYLOAD_SIZE);	
		(*msg).payload[0]= '\0';

		const int bytelen = (data_len - data_sent < PAYLOAD_SIZE-1) ?  data_len - data_sent : PAYLOAD_SIZE-1;
		strncat((*msg).payload, data + (count*(PAYLOAD_SIZE-1)), bytelen);

		if((send(socket_fd, (char *)&(*msg), sizeof((*msg)), 0))==ERROR)
		{
			perror("Error in send_data)message: ");
			exit_with_error("Error occured while sending the message");
		}
		print_info("Message sent      ", get_msg_type((*msg).type));
		
		count++;
		data_sent += strlen((*msg).payload);	
	}
}

/* Receives the message. Returns length of message.*/
int recv_ack_message(int socket_fd, char *buffer, Message *msg, int buffer_size)
{
	if((recv(socket_fd, buffer, buffer_size, 0))==ERROR)
	{
		perror("Error in recv_ack_message: ");
		exit_with_error("Error occured while receiving the message");
	}
	print_info("Message received  ", get_msg_type((*msg).type));

	return (*msg).type;
}

int recv_data_message(int socket_fd, Message *msg, String *data)
{
	do{
		if((recv(socket_fd, (char *)&(*msg), sizeof((*msg)), 0))==ERROR)
		{
			perror("Error in recv_ack_message: ");
			exit_with_error("Error occured while receiving the message");
		}
		print_info("Message received  ",get_msg_type((*msg).type));

		append_str_to_str(&(*data), (*msg).payload);
	}
	while((*msg).type == DATA_SENDING);

	return (*msg).type;
}

/* Starts new server with certain port */
int start_server(Socket *s, int port)
{
	//create socket
	if((s->socket_fd = socket(AF_INET, SOCK_STREAM, 0))==ERROR)
	{
		perror("Error code: ");
		exit_with_error("Error occured while creating the socket");
	}

	//setup socket
	s->sockaddr_len = sizeof(struct sockaddr_in);
	memset(&(s->remote), 0, sizeof(s->remote));
	s->local.sin_family = AF_INET;
	s->local.sin_addr.s_addr = INADDR_ANY;
	s->local.sin_port = htons(port);
	
	//allow using the same address
	int yes = 1;
	if ( setsockopt(s->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == ERROR)
	{
   		perror("Setsockopt");
   		exit_with_error("Setsockopt error occured !");
	}

	//bind socket to address
	if((bind(s->socket_fd, (struct sockaddr *)&(s->local), s->sockaddr_len))==ERROR)
	{
		perror("Error code: ");
		exit_with_error("Error occured while binding the socket");
	}

	print_info("Looking for clients...", "");

	//look for clients
	if((listen(s->socket_fd, LISTEN_QUEUE_NUMBER))==ERROR)
	{
		perror("Error code: ");
		exit_with_error("Error occured while looking for clients");
	}
	return 0;
}

/* Accepts connection. Returns new socket for communication */
int accept_connection(Socket *s)
{
	int accept_connection_fd;
	s->llen = sizeof(struct sockaddr_in);
	if((accept_connection_fd = accept(s->socket_fd, (struct sockaddr*)&(s->local), &(s->llen)))==ERROR)
	{
		perror("Error code:  ");
		exit_with_error("Error occured while accepting the connection");
	}
	else if(accept_connection_fd)
		return accept_connection_fd;
		
	return ERROR;
}

/* Creates client's socket. Setups up and connects to server. */
int connect_to_server(Socket *s, int port, char* hostname)
{
	
	//get address of server by DNS service
	if(!(s->host = gethostbyname(hostname)))
	{
		exit_with_error("No such host exists!\n");
	}

	//initialize address and port
	bzero((char *) &(s->remote), sizeof(s->remote));
	bcopy((char *)(s->host)->h_addr, (char *)&(s->remote).sin_addr.s_addr, (s->host)->h_length);
	s->remote.sin_family = AF_INET;
	s->remote.sin_port = htons(port);

	//create client socket
	if((s->socket_fd = socket(AF_INET, SOCK_STREAM, 0))==ERROR)
	{
		perror("Error code: ");
		exit_with_error("Error occured while creating the socket");
	}

	//connect to server
	if(connect(s->socket_fd, (struct sockaddr *)&(s->remote), sizeof(struct sockaddr_in)) == ERROR)
	{
		perror("Error code: ");
		close(s->socket_fd);
		exit_with_error("Connecting to server failed...");
	}		

	return 0;
}

/* Checks the client's input arguments */
void check_args(int argc, char **argv, int *portnum, char **hostname, char **login, char **option) 
{
	bool flag_h, flag_p, flag_n, flag_f, flag_l;
	int c;

	opterr = 0;
	while ((c = getopt (argc, argv, "h:p:n:f:l:")) != ERROR)
	{		
   		switch (c)
   		{
      		case 'h':	flag_h = true;	(*hostname) = optarg;	break;
      		case 'p': 	flag_p = true;	(*portnum) = atoi(optarg);	break;
		    case 'n':	flag_n = true;	(*login) = optarg;	(*option) = "n";	break;
		    case 'f':	flag_f = true;	(*login) = optarg;	(*option) = "f";	break;
		    case 'l':	flag_l = true;	(*login) = optarg;	(*option) = "l";	break;
      		case '?':
      			switch(optopt)
      			{
      				case 'h':	
      				case 'p':	
      				case 'n':	
      				case 'f':	
      					exit_with_error("Options [-h | -p | -n | -f] require an argument.");	
      					break;

      				case 'l':	flag_l = true;	(*login) = "";	(*option) = "l";	break;

      				default:	
      					exit_with_error("Unknown option character");	
      					break;
      			}
      			break;
        }
    }

    if(argc == 6 || argc == 7)
    {
    	if(!(flag_h && flag_p))
    	{
    		exit_with_error("Hostname and Portnum havent been defined !");
    	}
    	else
    	{
    		if(!flag_n && !flag_f && !flag_l)
    		{
    			exit_with_error("Wrong Argc Count - Usage:  ./ipk-client -h host -p port [-n|-f|-l] login");
    		}
   		}
    }
    else
    {
    	exit_with_error("Wrong Argc Count - Usage:  ./ipk-client -h host -p port [-n|-f|-l] login");
    }
    	
}

/* Writes error with info and exits program with -1 */
void exit_with_error(char *info)
{
	fprintf (stderr, "ERR ... %s\n", info);
	exit(ERROR);
}

/* Returns relevant information (string) about user */
char *get_users_info(char *login, char option, String *output)
{
	struct passwd *user_info = getpwnam(login);
	if(user_info)
	{
		if(option == 'n')
		{
			append_str_to_str(output, user_info->pw_gecos);
            append_str_to_str(output, "\n");
			return user_info->pw_gecos;
		}
		else if(option == 'f')
		{
			append_str_to_str(output, user_info->pw_dir);
            append_str_to_str(output, "\n");
			return user_info->pw_dir;
		}
		else
		{
			append_str_to_str(output, "Get users error");
            return NULL;
		}
	}
	else
	{
		append_str_to_str(output, "Username you set doesn't exist! Run program with [-l] option to see all usernames.");
		return NULL;
	}
}

/* Returns list (string) of users' logins matching the substring */
char *get_users_list(char *substr, String *output)
{
    FILE *fp;
 	 if((fp = fopen("/etc/passwd", "r")) == NULL)
 	 {
     	exit_with_error("Error occured while trying to open file '\\etc\\passwd'\n");
     }
        
    while(fgets(buffer, BUFFER_SIZE, fp))
    {
        if(starts_with(substr, buffer))
        {
            int colon_position = find_colon(buffer);
            char help[colon_position+1];
            memcpy(help, &buffer[0], colon_position);
            help[colon_position] = '\0';
                
            append_str_to_str(output, help);
            append_str_to_str(output, "\n");
        }
    }   
    return output->str;
}		

/* Checks if substring matches the string */
bool starts_with(char *substr, char *str)
{
	if(!substr)	return true;

    int lensubstr = strlen(substr);
    int lenstr = strlen(str);    
    return lenstr < lensubstr ? false : strncmp(substr, str, lensubstr) == 0;
}

/* Finds position of first colon in string */
int find_colon(char *str)
{
    int count = 0, i = 0;
    for(i=0; str[i] != ':' && str[i] != '\n'; i++) {count++;}   
    
    if(str[i] == '\n')
    	return 0;
    else
    	return count;
}

/* Initializes String variable*/
void init_string(String * s)
{
	if ((s->str = (char *) malloc(sizeof(char) * DEFAULT_STR_ALLOC)) == NULL)
		exit_with_error("Error occured when memory was allocated ! \n");
	s->alloc_size = DEFAULT_STR_ALLOC;
	s->len = 0;
	s->str[0] = '\0';
}

/* Appends string at the end of String variable */
void append_str_to_str(String *s, const char * append)
{
	if(!append)
		return;
	if ((s->len + strlen(append)) >= s->alloc_size)
	{
			if ((s->str = (char *) realloc(s->str, (s->alloc_size + (strlen(append)+1)) * sizeof(char))) == NULL)
			exit_with_error("Error occured when memory was allocated ! \n");
			s->alloc_size += strlen(append)+1;
	}
	s->len += strlen(append);
	strcat(s->str, append);
}

char *get_msg_type(int type)
{
	switch(type)	{
		case CONNECT_REQ	: return "CONNECT_REQ";
		case CONNECT_OK		: return "CONNECT_OK";
		case CONNECT_FAIL	: return "CONNECT_FAIL";

		case OPTION_REQ		: return "OPTION_REQ";
		case OPTION_SENT	: return "OPTION_SENT";
		case OPTION_OK		: return "OPTION_OK";
		case OPTION_FAIL	: return "OPTION_FAIL";

		case LOGIN_REQ		: return "LOGIN_REQ";
		case LOGIN_SENT		: return "LOGIN_SENT";
		case LOGIN_OK		: return "LOGIN_OK";
		case LOGIN_FAIL		: return "LOGIN_FAIL";

		case DATA_REQ		: return "DATA_REQ";
		case DATA_OK		: return "DATA_OK";
		case DATA_FAIL		: return "DATA_FAIL";
		case DATA_SENT		: return "DATA_SENT";
		case DATA_SENDING	: return "DATA_SENDING";

		case FAILURE 		: return "FAILURE";
		case SUCCESS 		: return "SUCCESS";

		case EXT_FAILURE	: return "EXT_FAILURE";
		case EXT_SUCCESS	: return "EXT_SUCCESS";
		default: return "DEFAULT - ERROR ";
	}
}
