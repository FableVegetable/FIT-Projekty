/*
 * 	Author:		Tomas Zubrik, xzubri00
 * 	Subject:	IPK, FIT VUT Brno 2018
 *	Project:	Client-server application (1st Project 2nd Variant)	
 */

#include "socket.h"

int main(int argc, char **argv)
{	
	char *server_name = NULL;
	int server_port;
	char *login = NULL;
	char *option = NULL;
	Socket client;
	Message outgoing_message;
	Message incoming_message;
	String server_response_data;
	init_string(&server_response_data);

	check_args(argc, argv, &server_port, &server_name, &login, &option); /* Check input arguments */
	connect_to_server(&client, server_port, server_name); /* Connect to server with certain port and servername */

	send_ack_message((&client)->socket_fd, &outgoing_message, CONNECT_REQ); /* Send acknowledgment message CONNECT_REQ */	

	if(CONNECT_OK == recv_ack_message((&client)->socket_fd, (char *)&incoming_message, &incoming_message, sizeof(incoming_message)))
	{
		send_data_message((&client)->socket_fd, &outgoing_message, option);	/* Send message with option in payload*/
		send_ack_message((&client)->socket_fd, &outgoing_message, OPTION_SENT); /* Send acknowledgment message OPTION_SENT */

		if(OPTION_OK == recv_ack_message((&client)->socket_fd, (char *)&incoming_message, &incoming_message, sizeof(incoming_message)))			
		{
			send_data_message((&client)->socket_fd, &outgoing_message, login); /* Send message with login in payload */
			send_ack_message((&client)->socket_fd, &outgoing_message, LOGIN_SENT);/* Send acknowledgment message LOGIN_SENT */

			if(LOGIN_OK == recv_ack_message((&client)->socket_fd, (char *)&incoming_message, &incoming_message, sizeof(incoming_message)))
			{
				if(DATA_SENT == recv_data_message((&client)->socket_fd, &incoming_message, &server_response_data))
				{
					printf("%s", (&server_response_data)->str); 
					send_ack_message((&client)->socket_fd, &outgoing_message, SUCCESS); /* Send acknowledgment message SUCCESS */
				}
				else /*if DATA_FAIL*/
				{
					send_ack_message((&client)->socket_fd, &outgoing_message, FAILURE);
				}

				if(EXT_SUCCESS ==recv_ack_message((&client)->socket_fd, (char *)&incoming_message, &incoming_message, sizeof(incoming_message)))
				{
					return 0;
				}
				else /*if EXIT_FAILURE*/
				{
					exit_with_error((&server_response_data)->str);
				}
			}
			else
			{
				exit_with_error("LOGIN_FAIL\n");
			}
		}
		else
		{
			exit_with_error("OPTION_FAIL\n");
		}
	}
	else
	{
		exit_with_error("CONNECT_FAIL\n");
	}

	return 0;
}
