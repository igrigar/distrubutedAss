import java.io.*;
import java.net.*;
import gnu.getopt.Getopt;
import webservice.Md5Service;
import webservice.Md5ServiceService;


class client {

    /********************* TYPES **********************/

    /**
      * @brief Return codes for the protocol methods
      */
    private static enum RC {OK, ERROR, USER_ERROR};

    /******************* ATTRIBUTES *******************/

    private static String _server   = null;
    private static String _ws = null;
    private static int _port = -1;
    private static String _user = "";
    private static Thread listener;

    /********************* METHODS ********************/

    /**
      * @param user - User name to register in the system
      *
      * @return OK if successful
      * @return USER_ERROR if the user is already registered
      * @return ERROR if another error occurred
      */
    static RC register(String user) {
        int response = -1;

        try {
            // Creation of the socket
            Socket sc = new Socket(_server, _port);

            OutputStream out = sc.getOutputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(
                sc.getInputStream()));

            out.write("REGISTER\0".getBytes());
            out.write((user + "\0").getBytes());

            response = in.read();
        } catch (Exception e) {
            e.printStackTrace();
        }

        switch (response) {
            case 0: System.out.println("REGISTER OK"); return RC.OK;
            case 1: System.out.println("USERNAME IN USE"); return RC.USER_ERROR;
            case 2: System.out.println("REGISTER FAIL"); return RC.ERROR;
            default: return RC.ERROR;
        }
    }

    /**
      * @param user - User name to unregister from the system
      *
      * @return OK if successful
      * @return USER_ERROR if the user does not exist
      * @return ERROR if another error occurred
      */
    static RC unregister(String user) {
        int response = -1;

        try {
            // Creation of the socket
            Socket sc = new Socket(_server, _port);

            OutputStream out = sc.getOutputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(
                sc.getInputStream()));

            out.write("UNREGISTER\0".getBytes());
            out.write((user + "\0").getBytes());

            response = in.read();
        } catch (Exception e) {
            e.printStackTrace();
        }

        switch (response) {
            case 0: System.out.println("UNREGISTER OK"); return RC.OK;
            case 1: System.out.println("USER DOES NOT EXIST"); return RC.USER_ERROR;
            case 2: System.out.println("UNREGISTER FAIL"); return RC.ERROR;
            default: return RC.ERROR;
        }
    }

    /**
      * @param user - User name to connect to the system
      *
      * @return OK if successful
      * @return USER_ERROR if the user does not exist or if it is already
      * connected
      * @return ERROR if another error occurred
      */
    static RC connect(String user) {
        int response = -1;

        if (!_user.equals("")) {
            System.out.println("ERROR, CONNECTED AS " + _user);
            return RC.USER_ERROR;
        }

        final ServerSocket msg_sock;
        final String usr = user;

        try { // Creating the socket for message listener.
            msg_sock = new ServerSocket(0);
        } catch (IOException e) {
            return RC.ERROR;
        }

        // Creating the listener.
        listener = new Thread() {
            @Override
            public void run() {
                ServerSocket socket = msg_sock;
                while (!Thread.currentThread().isInterrupted()) {
                    try {
                        Socket cli = socket.accept();
                        //BufferedReader in = new BufferedReader(
                        //    new InputStreamReader(cli.getInputStream()));

                        DataInputStream in = new DataInputStream(cli.getInputStream());

                        //String input = in.readLine();
                        String input = "";
                        int b = in.read();
                        while (b != 0) {
                            input += (char) b;
                            b = in.read();
                        }

                        if (input.equals("SEND_MESSAGE")) {
                            String from = "";
                            String id = "";
                            String msg = "";
                            String md5 = "";

                            b = in.read();
                            while (b != 0) {from += (char) b; b = in.read();}

                            b = in.read();
                            while (b != 0) {id += (char) b; b = in.read();}

                            b = in.read();
                            while (b != 0) {md5 += (char) b;b = in.read();}

                            b = in.read();
                            while (b != 0) {msg += (char) b;b = in.read();}

                            System.out.println("Message: " + id + " FROM: " + from);
                            System.out.print("  " + msg + "\n  MD5:\n  "+ md5 + "\n  END\nc> ");
                        } else if (input.equals("SEND_MESS_ACK")) {
                            String id = "";

                            b = in.read();
                            while (b != 0) {id += (char) b;b = in.read();}

                            System.out.print("SEND MESSAGE " + id + " OK\nc> ");
                        } else System.out.println(input);
                    } catch (IOException e) {
                        continue;
                    } catch (Exception e) {
                        continue;
                    }
                }
            }
        };

        int msg_port = msg_sock.getLocalPort();
        try {
            // Creation of the socket
            Socket sc = new Socket(_server, _port);

            OutputStream out = sc.getOutputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(
                sc.getInputStream()));

            out.write("CONNECT\0".getBytes());
            out.write((user + "\0").getBytes());
            out.write((String.valueOf(msg_port) + "\0").getBytes());

            response = in.read();
        } catch (Exception e) {
            e.printStackTrace();
        }

        switch (response) {
            case 0:
                System.out.println("CONNECT OK");
                _user = user;
                listener.start();
                return RC.OK;
            case 1: System.out.println("CONNECT FAIL, USER DOES NOT EXIST"); return RC.USER_ERROR;
            case 2: System.out.println("USER ALREADY CONNECTED"); return RC.ERROR;
            case 3: System.out.println("CONNECT FAIL"); return RC.ERROR;
            default: return RC.ERROR;
        }
    }

    /**
      * @param user - User name to disconnect from the system
      *
      * @return OK if successful
      * @return USER_ERROR if the user does not exist
      * @return ERROR if another error occurred
      */
    static RC disconnect(String user) {
        int response = -1;

        if (!user.equals(_user)) {
            System.out.println("ERROR, CONNECTED AS " + _user); 
            return RC.ERROR;
        } else if (_user.equals("")) {
            System.out.println("ERROR, NO USER CONNECTED."); 
            return RC.ERROR;
        }

        try {
            // Creation of the socket
            Socket sc = new Socket(_server, _port);

            OutputStream out = sc.getOutputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(
                sc.getInputStream()));

            out.write("DISCONNECT\0".getBytes());
            out.write((user + "\0").getBytes());

            response = in.read();
        } catch (Exception e) {
            e.printStackTrace();
        }

        listener.interrupt();

        switch (response) {
            case 0: System.out.println("DISCONNECT OK"); _user = ""; return RC.OK;
            case 1: System.out.println("DICSONNECT FAIL / USER DOES NOT EXIST"); return RC.USER_ERROR;
            case 2: System.out.println("DISCONNECT FAIL / USER NOT CONNECTED"); return RC.ERROR;
            case 3: System.out.println("DISCONNECT FAIL");
            default: return RC.ERROR;
        }
    }

    /**
      * @param user    - Receiver user name
      * @param message - Message to be sent
      *
      * @return OK if the server had successfully delivered the message
      * @return USER_ERROR if the user is not connected (the message is queued 
      * for delivery)
      * @return ERROR the user does not exist or another error occurred
      */
    static RC send(String user, String message) {
        int response = -1;
        String seq = "";
        String md5 = "";

        if (_user.equals("")) {
            System.out.println("ERROR, NOT CONNECTED");
            return RC.ERROR;
        }

        try {
            // Web Service.
            //Md5ServiceService service = new Md5ServiceService();
            Md5ServiceService service = new Md5ServiceService(new URL("http://" + _ws + ":8888/rs?wsdl"));
            Md5Service port = service.getMd5ServicePort();

            md5 = port.md5(message);
            System.out.println(md5);
        } catch (Exception e) {
            System.out.println("ERROR , SEND FAIL/ERROR IN MD5\nc> ");
            return RC.ERROR;
        }

        try {
            // Creation of the socket
            Socket sc = new Socket(_server, _port);

            OutputStream out = sc.getOutputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(
                sc.getInputStream()));

            out.write("SEND\0".getBytes());
            out.write((_user + "\0").getBytes());
            out.write((user + "\0").getBytes());
            out.write((message + "\0").getBytes());
           out.write((md5 + "\0").getBytes());

            response = in.read();
            if (response == 0) seq = in.readLine();
        } catch (Exception e) {
            e.printStackTrace();
        }

        switch (response) {
            case 0:
                System.out.println("SEND OK - MESSAGE " + seq);
                return RC.OK;
            case 1:
                System.out.println("SEND FAIL / USER DOES NOT EXIST");
                return RC.ERROR;
            case 2:
                System.out.println("SEND FAIL");
                return RC.ERROR;
            default: return RC.ERROR;
        }
    }

    /**
      * @brief Command interpreter for the client. It calls the protocol
      * functions.
      */
    static void shell() {
        boolean exit = false;
        String input;
        String [] line;
        BufferedReader in =
            new BufferedReader(new InputStreamReader(System.in));

        while (!exit) {
            try {
                System.out.print("c> ");
                input = in.readLine();
                line = input.split("\\s");

                if (line.length > 0) {
                /*********** REGISTER *************/
                if (line[0].equals("REGISTER")) {
                    if  (line.length == 2) {
                        register(line[1]); // userName = line[1]
                    } else {
                        System.out.println("Syntax error. Usage: REGISTER <userName>");
                    }
                }
                /********** UNREGISTER ************/
                else if (line[0].equals("UNREGISTER")) {
                    if  (line.length == 2) {
                        unregister(line[1]); // userName = line[1]
                    } else {
                        System.out.println("Syntax error. Usage: UNREGISTER <userName>");
                    }
                }
                /************ CONNECT *************/
                else if (line[0].equals("CONNECT")) {
                    if  (line.length == 2) {
                        connect(line[1]); // userName = line[1]
                    } else {
                        System.out.println("Syntax error. Usage: CONNECT <userName>");
                    }
                }
                /********** DISCONNECT ************/
                else if (line[0].equals("DISCONNECT")) {
                    if  (line.length == 2) {
                        disconnect(line[1]); // userName = line[1]
                    } else {
                        System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                    }
                } 
                /************** SEND **************/
                else if (line[0].equals("SEND")) {
                    if  (line.length >= 3) {
                        // Remove first two words
                        String message = input.substring(input.indexOf(' ')+1).substring(input.indexOf(' ')+1);
                        send(line[1], message); // userName = line[1]
                    } else {
                        System.out.println("Syntax error. Usage: SEND <userName> <message>");
                    }
                }
                /************** QUIT **************/
                else if (line[0].equals("QUIT")){
                    if (line.length == 1) {
                        exit = true;
                    } else {
                        System.out.println("Syntax error. Use: QUIT");
                    }
                }
                /************* UNKNOWN ************/
                else
                    System.out.println("Error: command '" + line[0] + "' not valid.");
                }
            } catch (java.io.IOException e) {
                System.out.println("Exception: " + e);
                e.printStackTrace();
            }
        }
    }

    /**
      * @brief Prints program usage
      */
    static void usage() {
        System.out.println("Usage: java -cp . client -s <server> -p <port> -w <WS IP>");
    }

    /**
      * @brief Parses program execution arguments 
      */
    static boolean parseArguments(String [] argv) {
        Getopt g = new Getopt("client", argv, "ds:p:w:");

        int c;
        String arg;

        while ((c = g.getopt()) != -1) {
            switch(c) {
                //case 'd':
                // _debug = true;
                // break;
                case 's':
                    _server = g.getOptarg();
                    break;
                case 'w':
                    _ws = g.getOptarg();
                    break;
                case 'p':
                    arg = g.getOptarg();
                    _port = Integer.parseInt(arg);
                    break;
                case '?':
                    System.out.print("getopt() returned " + c + "\0");
                    break; // getopt() already printed an error
                default:
                    System.out.print("getopt() returned " + c + "\0");
                }
        }

        if (_server == null)
            return false;

        if (_ws == null) return false;

        if ((_port < 1024) || (_port > 65535)) {
            System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
            return false;
        }

        return true;
    }


    /********************* MAIN **********************/
    public static void main(String[] argv) {
        if(!parseArguments(argv)) {
            usage();
            return;
        }
        // Write code here
        shell();
        System.out.println("+++ FINISHED +++");
    }
}
