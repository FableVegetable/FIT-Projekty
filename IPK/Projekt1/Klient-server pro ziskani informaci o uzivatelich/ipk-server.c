/*
 * 	Author:		Tomas Zubrik, xzubri00
 * 	Subject:	IPK, FIT VUT Brno 2018
 *	Project:	Client-server application (1st Project 2nd Variant)	
 */

#include "socket.h"

int main(int argc, char **argv)
{
	Socket s;
	Message incoming_message;
	Message outgoing_message;
	char option;
	String login_from_client;
	init_string(&login_from_client);
	String option_from_client;
	init_string(&option_from_client);
	String list;
	init_string(&list);
	char *info;

	if(PAYLOAD_SIZE < 2)
		exit_with_error("PAYLOAD_SIZE has to be at least 2.");

	if(argc == 3 && !strcmp(argv[1],"-p") && atoi(argv[2]))
		start_server(&s, atoi(argv[2]));
	else
		exit_with_error("Wrong Arguments - Usage:  ./ipk-server -p port");

	while(1)
	{
		int pc_connection = accept_connection(&s); /*Accept connection*/
		if(pc_connection<=0)	continue;

		int pid = fork();		
		if(pid == 0)
		{
			if(CONNECT_REQ == recv_ack_message(pc_connection, (char *)&incoming_message, &incoming_message, sizeof(incoming_message)))
			{
				send_ack_message(pc_connection, &outgoing_message, CONNECT_OK);

				if(OPTION_SENT == recv_data_message(pc_connection, &incoming_message, &option_from_client))
				{
					send_ack_message(pc_connection, &outgoing_message, OPTION_OK);	
					option = (&option_from_client)->str[0];
					
					if(LOGIN_SENT == recv_data_message(pc_connection, &incoming_message, &login_from_client))
					{
						send_ack_message(pc_connection, &outgoing_message, LOGIN_OK);	

						if(option == 'l')	
							info = get_users_list((&login_from_client)->str, &list);
						else if(option == 'n' || option == 'f')	
							info = get_users_info((&login_from_client)->str, option,  &list);
						else
							info = NULL;

						send_data_message(pc_connection, &outgoing_message, (&list)->str);

						if(info)
							send_ack_message(pc_connection, &outgoing_message, DATA_SENT);
						else
							send_ack_message(pc_connection, &outgoing_message, DATA_FAIL);

						
						if(SUCCESS == recv_ack_message(pc_connection, (char *)&incoming_message, &incoming_message, sizeof(incoming_message)))
						{
							send_ack_message(pc_connection, &outgoing_message, EXT_SUCCESS);
						}
						else /*if FAILURE*/
						{
							send_ack_message(pc_connection, &outgoing_message, EXT_FAILURE);
						}

					}
					else
					{
						send_ack_message(pc_connection, &outgoing_message, LOGIN_FAIL);
					}
				}
				else
				{
					send_ack_message(pc_connection, &outgoing_message, OPTION_FAIL);	
				}
			}
			else
			{
				send_ack_message(pc_connection, &outgoing_message, CONNECT_FAIL);
			}

			close(pc_connection);			
			exit(0);
		}
		else if (pid  < 0) 
		{
			perror("Function fork() failed: ");
			exit_with_error("Error occured when function fork() was called");
		}
		else
		{
			close(pc_connection);
		}
	}
	return 0;
}
