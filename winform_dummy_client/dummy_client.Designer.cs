namespace winform_dummy_client
{
    partial class dummy_client
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
            this.chat_box = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // chat_box
            // 
            this.chat_box.BackColor = System.Drawing.Color.White;
            this.chat_box.Location = new System.Drawing.Point(12, 9);
            this.chat_box.Name = "chat_box";
            this.chat_box.Size = new System.Drawing.Size(610, 176);
            this.chat_box.TabIndex = 0;
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(634, 388);
            this.Controls.Add(this.chat_box);
            this.Name = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label chat_box;
    }
}

