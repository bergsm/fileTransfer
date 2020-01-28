Simple File Transfer Program (Tested on flip servers and local Unix and Linux machines)

This program implements a simple file transfer system over TCP protocol between two processes.

There are two programs associated with the system. The server and the client. The server is implemented by ftserve.c and the client is implemented by ftclient.py.

To run the program first you must compile the server. To compile simply type 'make' at the command line

Once the server has compiled successfully (Ignore warning messages), you need to start the server.

To start the server simply run ./ftserver <PORT#> at the command line

Once the server is running, in a different terminal, start the client by running ./ftclient <SERVER-HOSTNAME> <PORT#> <COMMAND> [FILENAME] <DATA_PORT> at the command line.

You can also enter subsequent commands using the same syntax as above.

Command '-l' will list the directory contents of the server's current directory. The contents of the directory will be sent over a TCP connection over the specified data port. You do not need to includethe FILENAME option with this command.

Command '-g' will get the file used as an argument in the command. The file will be sent over a TCP connection over the specified data port. You do need to include the FILENAME option with this command.

Give the server a bit of time to work its magic :)



That's all there is to it! Enjoy!
