using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace winform_dummy_client
{
    public partial class dummy_client : Form
    {
        private network_module net = new network_module();
        
        public dummy_client()
        {
            InitializeComponent();

            net.Connect();
        }

        private void user_textBox_Enter(object sender, EventArgs e)
        {
            net.send_msg(user_textBox.Text.ToString());
            user_textBox.Clear();
        }
    }
}
