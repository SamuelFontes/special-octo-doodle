#include<string.h>
#include<sys/socket.h>
#include<fcntl.h>
#include <sys/sendfile.h>
#include<unistd.h>
#include<netinet/in.h>

int main(){
    // Create TPC SOCK_STREAM socket using the IPV4 domain
    printf("Setting up socket\n");
    int s = socket(AF_INET, SOCK_STREAM, 0);

    // Setup address object
    struct sockaddr_in addr = {
        AF_INET, // This defines the IPV4 thing
        0x901f, // This is the port number in HEX reversed, in this case 8080
       0  // This is set to 0 to use localhost, it's the IPV4 address, in this case localhost?
    };

    printf("Binding socket to localhost address on port 8080\n");
    // This will bind the websocket to the address object we created
    bind(s, &addr, sizeof(addr));

    // Set socket as passive, it will accept incoming connections using accept(), 256 is the queue limit of the backlog thingy
    listen(s,256);
    printf("Listening on http://localhost:8080\n");
    printf("Try this command on the terminal: wget localhost:8080/index.html & cat index.html\n");

    while(1){
        // Get the thing ready to accept incoming calls
        // We will pass the socket without anyo other parameter, the others would be related to addr, since we already binded it to the socket it is not necessary
        int client_fd = accept(s, 0 , 0); // This will look the program until it gets an http request on localhost:8080

        // This will get the request into the buffer, it should look something like this:
        /* 
            GET /index.html HTTP/1.1
            Host: localhost:8080
            Range: bytes=11-
            User-Agent: Wget/1.21.4
            Accept: * /*
            Accept-Encoding: identity
            Connection: Keep-Alive
        */
        char buffer[256] = {0};
        recv(client_fd, buffer, 256, 0);
        printf("Request Received: \n\n%s", buffer);

        // This will read the route we are getting, in this case is set to send a file with the name of the route, for example GET /index.html ....
        // This will only save the string index.html to the f variable
        char* f = buffer +5;
        *strchr(f, ' ') = 0;

        // We will open the file the request asked for
        int opened_fd = open(f, O_RDONLY);

        // We will return the opened file to the client through the socket
        sendfile(client_fd, opened_fd, 0, 256);

        // This will close the current request, the file will only be sent when the request is closed
        close(opened_fd);
        close(client_fd);
    }

    // Close the socket, we will never hit this because the API is not supposed to stop working
    close(s);

    return 0;
}