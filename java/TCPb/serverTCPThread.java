import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.lang.ClassNotFoundException;
import java.lang.Runnable;
import java.lang.Thread;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.InetAddress;
import java.net.*;

//-Djava.net.preferIPv4Stack=true
 
public class serverTCPThread
{

	public static void main(String[] args )
	{
		System.out.print( "Server TCP\n" );

            	try {
    			ServerSocket    server = new ServerSocket( 10001, 10, InetAddress.getByName("ares.fav.zcu.cz") );
			int vlakna = 0;	
			while (true) 
			{
				System.out.print( "Server ceka ...\n" );
                		Socket socket = server.accept();
				vlakna++;
				System.out.print( "Pripojen klient \n");
				new serverTCPThreadWork( socket, vlakna ).start();
			}
            	} 
		catch (Exception e) 
		{
                	e.printStackTrace();
            	}
        }
}
class serverTCPThreadWork extends Thread
{
	Socket socket;
	int idVlakna;
	public serverTCPThreadWork( Socket socket, int idVlakna )
	{
		this. socket = socket;
		this.idVlakna = idVlakna;
	}

	public void run() 
	{
		System.out.print( "Spoustim vlakno s ID = "+idVlakna+"\n" );
		InetAddress adresa = socket.getInetAddress();
		System.out.print("Pripojil se klient z: "+adresa.getHostAddress()+"/"+adresa.getHostName()+"\n" );


		try
		{
           		
			ObjectInputStream ois = new ObjectInputStream(socket.getInputStream());
		        String message = (String) ois.readObject();
			System.out.println("Message Received: " + message);
       		 	ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
			Thread.currentThread().sleep(5000);
       	 		oos.writeObject("Hallo");
         		ois.close();
            		oos.close();
            		socket.close();
			System.out.print("Ukoncuji vlakno s ID="+idVlakna+"\n");
		}
		catch( Exception e2 )
		{
			e2.printStackTrace();
		}
	}
}

