using Microsoft.SqlServer.Server;
using Protocol;
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
using System.Xml.Linq;
using static Protocol.PACKET_TYPE;

namespace winform_dummy_client
{
    public partial class dummy_client : Form
    {
        private network_module net = new network_module();
        private Thread recv_thread;

        public dummy_client()
        {
            InitializeComponent();

            recv_thread = new Thread(recv_packet);
            recv_thread.IsBackground = true;
        }

        private void user_textBox_Enter(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                if(user_textBox.Text.Length == 0)
                {
                    user_textBox.Text = "연결이 종료되었습니다.";
                }
                if(net._name.Length == 0)
                {
                    if(net.Connect(user_textBox.Text.ToString()))
                    {
                        recv_thread.Start();
                        chat_box.Text += ("\n" + String.Format("{0} [스킨: {1}, 펫: {2}]", net._name, net._player_skin, net._pet_num));
                    }
                    else
                    {
                        user_textBox.Text = "이름을 입력해 주세요.";
                    }
                }
                else
                {
                    net.send_msg(user_textBox.Text.ToString());
                }
                user_textBox.Clear();
            }
        }

        private void recv_packet()
        {
            while (true)
            {
                switch (net.recv_packet())
                {
                    case SC_LOGIN_INFO:
                        {
                            
                        }
                        break;
                    case SC_CHAT:
                        {
                            if (true == chat_box.InvokeRequired)
                            {
                                chat_box.Invoke(new MethodInvoker(delegate
                                {
                                    chat_box.Text += ("\n" + net.new_chat);
                                }));
                            }
                            else
                                chat_box.Text += ("\n" + net.new_chat);
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
