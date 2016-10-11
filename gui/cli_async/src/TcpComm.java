package aclient;

import java.io.*;
import java.net.*;

//==========================================================================

public class TcpComm implements Comm, Runnable
{
  private CommObserver m_observer;
  OutputStream m_output;

  //---------------------------------------------------------

  public void send(String data)
  {
    assert(data != null);
    assert(m_output != null);

    try
    {
      m_output.write(data.getBytes());
    } catch (IOException e) {
      System.err.println("Caught IOException: " + e.getMessage());
    }
  }

  //---------------------------------------------------------

  public void registerObserver(CommObserver observer)
  {
    m_observer = observer;
  }

  //---------------------------------------------------------

  public void run()
  {
    System.out.println("Hello from a thread!");

    try
    {
      InetAddress address = InetAddress.getByName("localhost");
      int port = 22434;
      Socket socket = new Socket(address, port);
      m_output = socket.getOutputStream();
      InputStream input = socket.getInputStream();

      // TODO quit
      for (;;)
      {
        byte[] buffer = new byte[1024];
        int count = input.read(buffer);
        if (count > 0)
        {
          String data = new String(buffer, 0, count);
          if (m_observer != null)
          {
            m_observer.processData(data);
          }
        }
      }
    } catch (IOException e) {
      System.err.println("Caught IOException: " + e.getMessage());
    }
  }

  //---------------------------------------------------------

  public void start()
  {
    (new Thread(this)).start();
  }
}
