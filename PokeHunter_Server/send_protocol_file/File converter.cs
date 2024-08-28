using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace send_protocol_file
{
    public partial class file_converter : Form
    {
        private string target_file_name = "";

        public file_converter()
        {
            InitializeComponent();
        }

        private void open_file_btn_Click(object sender, EventArgs e)
        {
            string open_file_dir = "";
            openFileDialog.InitialDirectory = "C:\\Git\\PokeHunter_LoginServer\\IOCPServer";

            if(openFileDialog.ShowDialog() == DialogResult.OK)
            {
                open_file_dir = openFileDialog.FileName;
                target_file_name = open_file_dir;

                string result = Path.ChangeExtension(target_file_name, ".cs");
                File.Copy(target_file_name, result, true);

                string new_file = "C:\\Git\\PokeHunter_LoginServer\\winform_dummy_client\\" + Path.GetFileName(result);

                if (File.Exists(new_file))
                {
                    File.Delete(new_file);
                }
                File.Move(result, new_file);
                
                target_file_dir.Text = new_file + " 파일 저장 완료";
            }
            else
            {
                target_file_dir.Text = "존재하지 않는 파일 입니다.";
            }
        }
    }
}
