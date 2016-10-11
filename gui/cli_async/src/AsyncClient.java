package aclient;

import java.io.*;

//==========================================================================

class AsyncClient
{
  public static void main(String argv[])
  {
    System.out.println("starting");
    TcpComm comm = new TcpComm();
    comm.start();
    DummyWindow w = new DummyWindow(comm);
    w.run();
  }
}
