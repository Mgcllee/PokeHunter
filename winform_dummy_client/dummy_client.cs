using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace winform_dummy_client
{
    public partial class dummy_client : Form
    {
        private network_module net = new network_module();
        private Thread recv_thread;
        private System.Timers.Timer timer;

        public dummy_client()
        {
            InitializeComponent();

            net.Connect();
            recv_thread = new Thread(recv_chat);
            recv_thread.IsBackground = true;
            recv_thread.Start();

            timer = new System.Timers.Timer();
        }

        private void user_textBox_Enter(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if(user_textBox.Text.Length == 0)
                {
                    // net.Colse();
                    user_textBox.Text = "연결이 종료되었습니다.";
                }
                if(net.name.Length == 0)
                {
                    net.name = user_textBox.Text.ToString();
                    net.send_name();
                }
                else
                {
                    net.send_msg(user_textBox.Text.ToString());
                    // recv_chat();
                }
                user_textBox.Clear();
            }
        }

        private void recv_chat()
        {
            while (true)
            {
                string recv_str = net.recv_msg();

                if (true == chat_box.InvokeRequired)
                {
                    chat_box.Invoke(new MethodInvoker(delegate
                    {
                        chat_box.Text += ("\n" + recv_str).ToString();
                    }));
                }
                else
                    chat_box.Text += ("\n" + recv_str).ToString();
            }
        }
    }
}
