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
 
public class serverTCPSingle
{

	public static void main(String[] args )
	{
		System.out.print( "Server TCP\n" );

            	try {
    			ServerSocket    server = new ServerSocket( 10001, 10, InetAddress.getByName("localhost") );


		
			while (true) 
			{
                		Socket socket = server.accept();
				InetAddress adresa = socket.getInetAddress();
				System.out.print("Pripojil se klient z: "+adresa.getHostAddress()+"/"+adresa.getHostName()+"\n" );
				ObjectInputStream ois = new ObjectInputStream(socket.getInputStream());
			        String message = (String) ois.readObject();
				System.out.println("Message Received: " + message);
            			ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
				
				//Thread.currentThread().sleep(5000);

            			oos.writeObject("Hallo");
            			ois.close();
            			oos.close();
            			socket.close();
			}
            	} 
		catch (Exception e) 
		{
                	e.printStackTrace();
            	}
        }
}
