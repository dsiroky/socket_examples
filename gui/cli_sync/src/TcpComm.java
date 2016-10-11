package aclient;

import java.io.*;
import java.net.*;

//==========================================================================

public class TcpComm implements Comm
{
  private CommObserver m_observer;
  OutputStream m_output;
  InputStream m_input;

  //---------------------------------------------------------

  public void send(String data)
  {
    assert(data != null);
    assert(m_output != null);

    try
    {
      m_output.write(data.getBytes());
      byte[] buffer = new byte[1024];
      int count = m_input.read(buffer);
      if (count > 0)
      {
        String recvData = new String(buffer, 0, count);
        if (m_observer != null)
        {
          m_observer.processData(recvData);
        }
      }
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

  public void start()
  {
    try
    {
      InetAddress address = InetAddress.getByName("localhost");
      int port = 22434;
      Socket socket = new Socket(address, port);
      m_output = socket.getOutputStream();
      m_input = socket.getInputStream();
    } catch (IOException e) {
      System.err.println("Caught IOException: " + e.getMessage());
    }
  }
}
