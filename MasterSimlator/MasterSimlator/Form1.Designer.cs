namespace MasterSimlator
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.SerialPortCombBox = new System.Windows.Forms.ComboBox();
            this.OpenPortButton = new System.Windows.Forms.Button();
            this.Read485AddrButton = new System.Windows.Forms.Button();
            this.Set485AddrButton = new System.Windows.Forms.Button();
            this.SetCANAddrButton = new System.Windows.Forms.Button();
            this.TurnonLCDButton = new System.Windows.Forms.Button();
            this.TurnoffLCDButton = new System.Windows.Forms.Button();
            this.ReadSenorButton = new System.Windows.Forms.Button();
            this.UpdatePicButton = new System.Windows.Forms.Button();
            this.SystemRichTextBox = new System.Windows.Forms.RichTextBox();
            this.UpdateProgressBar = new System.Windows.Forms.ProgressBar();
            this.CmdGroupBox = new System.Windows.Forms.GroupBox();
            this.ReadCANAddrButton = new System.Windows.Forms.Button();
            this.CANAddrTextBox = new System.Windows.Forms.TextBox();
            this.Rs485AddrTextBox = new System.Windows.Forms.TextBox();
            this.CmdGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // SerialPortCombBox
            // 
            this.SerialPortCombBox.FormattingEnabled = true;
            this.SerialPortCombBox.Location = new System.Drawing.Point(12, 12);
            this.SerialPortCombBox.Name = "SerialPortCombBox";
            this.SerialPortCombBox.Size = new System.Drawing.Size(176, 20);
            this.SerialPortCombBox.TabIndex = 0;
            // 
            // OpenPortButton
            // 
            this.OpenPortButton.Location = new System.Drawing.Point(194, 9);
            this.OpenPortButton.Name = "OpenPortButton";
            this.OpenPortButton.Size = new System.Drawing.Size(300, 23);
            this.OpenPortButton.TabIndex = 1;
            this.OpenPortButton.Text = "Open Serial Port";
            this.OpenPortButton.UseVisualStyleBackColor = true;
            this.OpenPortButton.Click += new System.EventHandler(this.OpenPortButton_Click);
            // 
            // Read485AddrButton
            // 
            this.Read485AddrButton.Location = new System.Drawing.Point(9, 22);
            this.Read485AddrButton.Name = "Read485AddrButton";
            this.Read485AddrButton.Size = new System.Drawing.Size(107, 23);
            this.Read485AddrButton.TabIndex = 2;
            this.Read485AddrButton.Text = "Read 485 Addr";
            this.Read485AddrButton.UseVisualStyleBackColor = true;
            this.Read485AddrButton.Click += new System.EventHandler(this.Read485AddrButton_Click);
            // 
            // Set485AddrButton
            // 
            this.Set485AddrButton.Location = new System.Drawing.Point(9, 51);
            this.Set485AddrButton.Name = "Set485AddrButton";
            this.Set485AddrButton.Size = new System.Drawing.Size(107, 23);
            this.Set485AddrButton.TabIndex = 3;
            this.Set485AddrButton.Text = "Set 485 Addr";
            this.Set485AddrButton.UseVisualStyleBackColor = true;
            this.Set485AddrButton.Click += new System.EventHandler(this.Set485AddrButton_Click);
            // 
            // SetCANAddrButton
            // 
            this.SetCANAddrButton.Location = new System.Drawing.Point(9, 109);
            this.SetCANAddrButton.Name = "SetCANAddrButton";
            this.SetCANAddrButton.Size = new System.Drawing.Size(107, 23);
            this.SetCANAddrButton.TabIndex = 4;
            this.SetCANAddrButton.Text = "Set CAN Addr";
            this.SetCANAddrButton.UseVisualStyleBackColor = true;
            this.SetCANAddrButton.Click += new System.EventHandler(this.SetCANAddrButton_Click);
            // 
            // TurnonLCDButton
            // 
            this.TurnonLCDButton.Location = new System.Drawing.Point(9, 138);
            this.TurnonLCDButton.Name = "TurnonLCDButton";
            this.TurnonLCDButton.Size = new System.Drawing.Size(107, 23);
            this.TurnonLCDButton.TabIndex = 5;
            this.TurnonLCDButton.Text = "Turnon LCD";
            this.TurnonLCDButton.UseVisualStyleBackColor = true;
            this.TurnonLCDButton.Click += new System.EventHandler(this.TurnonLCDButton_Click);
            // 
            // TurnoffLCDButton
            // 
            this.TurnoffLCDButton.Location = new System.Drawing.Point(9, 167);
            this.TurnoffLCDButton.Name = "TurnoffLCDButton";
            this.TurnoffLCDButton.Size = new System.Drawing.Size(107, 23);
            this.TurnoffLCDButton.TabIndex = 6;
            this.TurnoffLCDButton.Text = "Turnoff LCD";
            this.TurnoffLCDButton.UseVisualStyleBackColor = true;
            this.TurnoffLCDButton.Click += new System.EventHandler(this.TurnoffLCDButton_Click);
            // 
            // ReadSenorButton
            // 
            this.ReadSenorButton.Location = new System.Drawing.Point(9, 196);
            this.ReadSenorButton.Name = "ReadSenorButton";
            this.ReadSenorButton.Size = new System.Drawing.Size(107, 23);
            this.ReadSenorButton.TabIndex = 7;
            this.ReadSenorButton.Text = "Read Senor";
            this.ReadSenorButton.UseVisualStyleBackColor = true;
            this.ReadSenorButton.Click += new System.EventHandler(this.ReadSenorButton_Click);
            // 
            // UpdatePicButton
            // 
            this.UpdatePicButton.Location = new System.Drawing.Point(9, 225);
            this.UpdatePicButton.Name = "UpdatePicButton";
            this.UpdatePicButton.Size = new System.Drawing.Size(107, 23);
            this.UpdatePicButton.TabIndex = 8;
            this.UpdatePicButton.Text = "Update Pic";
            this.UpdatePicButton.UseVisualStyleBackColor = true;
            this.UpdatePicButton.Click += new System.EventHandler(this.UpdatePicButton_Click);
            // 
            // SystemRichTextBox
            // 
            this.SystemRichTextBox.Location = new System.Drawing.Point(194, 41);
            this.SystemRichTextBox.Name = "SystemRichTextBox";
            this.SystemRichTextBox.Size = new System.Drawing.Size(300, 216);
            this.SystemRichTextBox.TabIndex = 9;
            this.SystemRichTextBox.Text = "";
            this.SystemRichTextBox.TextChanged += new System.EventHandler(this.SystemRichTextBox_TextChanged);
            // 
            // UpdateProgressBar
            // 
            this.UpdateProgressBar.Location = new System.Drawing.Point(194, 266);
            this.UpdateProgressBar.Name = "UpdateProgressBar";
            this.UpdateProgressBar.Size = new System.Drawing.Size(300, 23);
            this.UpdateProgressBar.TabIndex = 10;
            // 
            // CmdGroupBox
            // 
            this.CmdGroupBox.Controls.Add(this.ReadCANAddrButton);
            this.CmdGroupBox.Controls.Add(this.CANAddrTextBox);
            this.CmdGroupBox.Controls.Add(this.Rs485AddrTextBox);
            this.CmdGroupBox.Controls.Add(this.UpdatePicButton);
            this.CmdGroupBox.Controls.Add(this.Read485AddrButton);
            this.CmdGroupBox.Controls.Add(this.Set485AddrButton);
            this.CmdGroupBox.Controls.Add(this.ReadSenorButton);
            this.CmdGroupBox.Controls.Add(this.SetCANAddrButton);
            this.CmdGroupBox.Controls.Add(this.TurnoffLCDButton);
            this.CmdGroupBox.Controls.Add(this.TurnonLCDButton);
            this.CmdGroupBox.Location = new System.Drawing.Point(12, 38);
            this.CmdGroupBox.Name = "CmdGroupBox";
            this.CmdGroupBox.Size = new System.Drawing.Size(176, 260);
            this.CmdGroupBox.TabIndex = 11;
            this.CmdGroupBox.TabStop = false;
            this.CmdGroupBox.Text = "CMD";
            // 
            // ReadCANAddrButton
            // 
            this.ReadCANAddrButton.Location = new System.Drawing.Point(9, 80);
            this.ReadCANAddrButton.Name = "ReadCANAddrButton";
            this.ReadCANAddrButton.Size = new System.Drawing.Size(107, 23);
            this.ReadCANAddrButton.TabIndex = 11;
            this.ReadCANAddrButton.Text = "Read CAN Addr";
            this.ReadCANAddrButton.UseVisualStyleBackColor = true;
            this.ReadCANAddrButton.Click += new System.EventHandler(this.ReadCANAddrButton_Click);
            // 
            // CANAddrTextBox
            // 
            this.CANAddrTextBox.Location = new System.Drawing.Point(123, 110);
            this.CANAddrTextBox.Name = "CANAddrTextBox";
            this.CANAddrTextBox.Size = new System.Drawing.Size(47, 21);
            this.CANAddrTextBox.TabIndex = 10;
            this.CANAddrTextBox.Text = "10";
            // 
            // Rs485AddrTextBox
            // 
            this.Rs485AddrTextBox.Location = new System.Drawing.Point(123, 52);
            this.Rs485AddrTextBox.Name = "Rs485AddrTextBox";
            this.Rs485AddrTextBox.Size = new System.Drawing.Size(47, 21);
            this.Rs485AddrTextBox.TabIndex = 9;
            this.Rs485AddrTextBox.Text = "10";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(506, 310);
            this.Controls.Add(this.UpdateProgressBar);
            this.Controls.Add(this.SystemRichTextBox);
            this.Controls.Add(this.OpenPortButton);
            this.Controls.Add(this.SerialPortCombBox);
            this.Controls.Add(this.CmdGroupBox);
            this.Name = "Form1";
            this.Text = "Master Simulator";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.CmdGroupBox.ResumeLayout(false);
            this.CmdGroupBox.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox SerialPortCombBox;
        private System.Windows.Forms.Button OpenPortButton;
        private System.Windows.Forms.Button Read485AddrButton;
        private System.Windows.Forms.Button Set485AddrButton;
        private System.Windows.Forms.Button SetCANAddrButton;
        private System.Windows.Forms.Button TurnonLCDButton;
        private System.Windows.Forms.Button TurnoffLCDButton;
        private System.Windows.Forms.Button ReadSenorButton;
        private System.Windows.Forms.Button UpdatePicButton;
        private System.Windows.Forms.RichTextBox SystemRichTextBox;
        private System.Windows.Forms.ProgressBar UpdateProgressBar;
        private System.Windows.Forms.GroupBox CmdGroupBox;
        private System.Windows.Forms.TextBox CANAddrTextBox;
        private System.Windows.Forms.TextBox Rs485AddrTextBox;
        private System.Windows.Forms.Button ReadCANAddrButton;
    }
}

