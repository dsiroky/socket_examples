import java.io.*;
import java.net.*;

class clientTCP
{
 public static void main(String argv[]) throws Exception
 {
  String sentence;
  String modifiedSentence;
  Socket socket = new Socket("127.0.0.1", 10001);
  InetAddress adresa = socket.getInetAddress();
  System.out.print("Pripojuju se na : "+adresa.getHostAddress()+" se jmenem : "+adresa.getHostName()+"\n" );


  ObjectOutputStream oos = new ObjectOutputStream(socket.getOutputStream());
                                oos.writeObject("HalloXXXX");


                                ObjectInputStream ois = new ObjectInputStream(socket.getInputStream());
                                String message = (String) ois.readObject();
                                System.out.println("Message Received: " + message);
                                ois.close();
                                oos.close();
                                socket.close();
 }
}
