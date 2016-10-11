package aclient;

import javax.swing.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;

//==========================================================================

class UiCommObserver implements CommObserver
{
  private JLabel m_label;

  public UiCommObserver(JLabel label)
  {
    m_label = label;
  }

  public void processData(String data)
  {
    m_label.setText(data);
  }
}

//==========================================================================

public class DummyWindow
{
  private JLabel m_label;
  private JButton m_button;
  private int m_counter = 0;
  private Comm m_comm;
  private UiCommObserver m_commObserver;

  //---------------------------------------------------------

  public DummyWindow(Comm comm)
  {
    m_comm = comm;
  }

  //---------------------------------------------------------

  private void createAndShowGUI()
  {
    JFrame frame = new JFrame("Sync client");
    frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    frame.setLayout(new BoxLayout(frame.getContentPane(), BoxLayout.Y_AXIS));

    m_label = new JLabel("Hello World");
    frame.getContentPane().add(m_label);
    m_commObserver = new UiCommObserver(m_label);
    m_comm.registerObserver(m_commObserver);

    m_button = new JButton("baf!");
    frame.getContentPane().add(m_button);

    m_button.addActionListener(new ActionListener()
        {
          public void actionPerformed(ActionEvent e)
          {
            m_comm.send(Integer.toString(m_counter));
            ++m_counter;
          }
        });

    frame.pack();
    frame.setVisible(true);
  }

  //---------------------------------------------------------

  public void run()
  {
    javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run()
            {
              createAndShowGUI();
            }
        });
  }
}
