namespace send_protocol_file
{
    partial class file_converter
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(file_converter));
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.open_file_btn = new System.Windows.Forms.Button();
            this.target_file_dir = new System.Windows.Forms.Label();
            this.folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.SuspendLayout();
            // 
            // openFileDialog
            // 
            this.openFileDialog.FileName = "openFileDialog";
            // 
            // open_file_btn
            // 
            this.open_file_btn.Font = new System.Drawing.Font("굴림", 10F);
            this.open_file_btn.Location = new System.Drawing.Point(10, 21);
            this.open_file_btn.Name = "open_file_btn";
            this.open_file_btn.Size = new System.Drawing.Size(87, 26);
            this.open_file_btn.TabIndex = 4;
            this.open_file_btn.Text = "파일열기";
            this.open_file_btn.UseVisualStyleBackColor = true;
            this.open_file_btn.Click += new System.EventHandler(this.open_file_btn_Click);
            // 
            // target_file_dir
            // 
            this.target_file_dir.BackColor = System.Drawing.Color.White;
            this.target_file_dir.Font = new System.Drawing.Font("굴림", 9F);
            this.target_file_dir.Location = new System.Drawing.Point(105, 21);
            this.target_file_dir.Name = "target_file_dir";
            this.target_file_dir.Size = new System.Drawing.Size(482, 26);
            this.target_file_dir.TabIndex = 5;
            this.target_file_dir.Text = "C:";
            this.target_file_dir.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // file_converter
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(607, 67);
            this.Controls.Add(this.target_file_dir);
            this.Controls.Add(this.open_file_btn);
            this.ForeColor = System.Drawing.Color.Black;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximumSize = new System.Drawing.Size(625, 114);
            this.MinimumSize = new System.Drawing.Size(625, 114);
            this.Name = "file_converter";
            this.RightToLeftLayout = true;
            this.Text = "Converter .CS file";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.Button open_file_btn;
        private System.Windows.Forms.Label target_file_dir;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog;
    }
}

