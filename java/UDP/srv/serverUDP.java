import java.net.*;
import java.io.*;

//	-Djava.net.preferIPv4Stack=true
public abstract class serverUDP
{

 
	public static void main( String[] args )
	{
		int bufferSize = 1; 
  		DatagramSocket ds;
 		try 
		{

    			ds = new DatagramSocket( 10000 );
  
    			byte[] buffer = new byte[bufferSize];
    
			while (true) 
			{
				System.out.print( "Server ceka...\n");
      				DatagramPacket recv = new DatagramPacket(buffer, buffer.length);
	        		ds.receive(recv);

				InetAddress adresa = recv.getAddress();
				System.out.print("Pripojil se klient z: "+adresa.getHostAddress()+"/"+adresa.getHostName()+"\n" );


				String data = new String( buffer );
      				System.out.print( "Prijal sem data - "+data+"\n");
				char sdata = data.charAt(0);
				sdata++;
				data = String.valueOf( sdata );
				buffer = data.getBytes();
				InetAddress address = recv.getAddress();
				int port = recv.getPort();

				System.out.print("Odesilam = "+data+" o velikosti "+buffer.length+" \n");
      				DatagramPacket sendX = new DatagramPacket( buffer, buffer.length, address, port);
				ds.send( sendX );
				
      			}
      		}
		catch (Exception e) 
		{
        		System.err.println(e);
      		}      
	}
}

