package aclient;

//==========================================================================

interface CommObserver
{
  public void processData(String data);
}

//==========================================================================

interface Comm
{
  public void send(String data);
  public void registerObserver(CommObserver observer);
}
