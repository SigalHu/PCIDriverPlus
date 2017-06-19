namespace PCIDriverTest4CS
{
    partial class PCIDriverTest4CS
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle5 = new System.Windows.Forms.DataGridViewCellStyle();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle6 = new System.Windows.Forms.DataGridViewCellStyle();
            this.WriteSelectL2RAM = new System.Windows.Forms.RadioButton();
            this.WriteSelectDDR = new System.Windows.Forms.RadioButton();
            this.ReadDSP = new System.Windows.Forms.Button();
            this.WriteDSP = new System.Windows.Forms.Button();
            this.ReadDSPData = new System.Windows.Forms.TextBox();
            this.WriteDSPData = new System.Windows.Forms.TextBox();
            this.OffsetDSP = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.label4 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.WriteFile2FPGA = new System.Windows.Forms.Button();
            this.IniFile2FPGA = new System.Windows.Forms.Button();
            this.DSPPanel = new System.Windows.Forms.TabControl();
            this.FPGAPanel = new System.Windows.Forms.TabPage();
            this.WriteTimeShow = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.FPGAWriteFileSelect = new System.Windows.Forms.Button();
            this.FPGAWriteFilePath = new System.Windows.Forms.TextBox();
            this.载入表格ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.保存表格ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ListWriteMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.Column4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column1 = new System.Windows.Forms.DataGridViewCheckBoxColumn();
            this.ListWrite2FPGA = new System.Windows.Forms.DataGridView();
            this.PCISelect = new System.Windows.Forms.ComboBox();
            this.PCIStatusShow = new System.Windows.Forms.TextBox();
            this.tabPage2.SuspendLayout();
            this.DSPPanel.SuspendLayout();
            this.FPGAPanel.SuspendLayout();
            this.ListWriteMenu.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ListWrite2FPGA)).BeginInit();
            this.SuspendLayout();
            // 
            // WriteSelectL2RAM
            // 
            this.WriteSelectL2RAM.AutoSize = true;
            this.WriteSelectL2RAM.Location = new System.Drawing.Point(306, 29);
            this.WriteSelectL2RAM.Name = "WriteSelectL2RAM";
            this.WriteSelectL2RAM.Size = new System.Drawing.Size(74, 21);
            this.WriteSelectL2RAM.TabIndex = 9;
            this.WriteSelectL2RAM.Text = "L2RAM";
            this.WriteSelectL2RAM.UseVisualStyleBackColor = true;
            // 
            // WriteSelectDDR
            // 
            this.WriteSelectDDR.AutoSize = true;
            this.WriteSelectDDR.Checked = true;
            this.WriteSelectDDR.Location = new System.Drawing.Point(306, 8);
            this.WriteSelectDDR.Name = "WriteSelectDDR";
            this.WriteSelectDDR.Size = new System.Drawing.Size(56, 21);
            this.WriteSelectDDR.TabIndex = 8;
            this.WriteSelectDDR.TabStop = true;
            this.WriteSelectDDR.Text = "DDR";
            this.WriteSelectDDR.UseVisualStyleBackColor = true;
            // 
            // ReadDSP
            // 
            this.ReadDSP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.ReadDSP.Font = new System.Drawing.Font("SimSun", 10F);
            this.ReadDSP.Location = new System.Drawing.Point(306, 91);
            this.ReadDSP.Name = "ReadDSP";
            this.ReadDSP.Size = new System.Drawing.Size(75, 28);
            this.ReadDSP.TabIndex = 7;
            this.ReadDSP.Text = "读取";
            this.ReadDSP.UseVisualStyleBackColor = false;
            this.ReadDSP.Click += new System.EventHandler(this.ReadDSP_Click);
            // 
            // WriteDSP
            // 
            this.WriteDSP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.WriteDSP.Font = new System.Drawing.Font("SimSun", 10F);
            this.WriteDSP.Location = new System.Drawing.Point(306, 52);
            this.WriteDSP.Name = "WriteDSP";
            this.WriteDSP.Size = new System.Drawing.Size(75, 28);
            this.WriteDSP.TabIndex = 6;
            this.WriteDSP.Text = "写入";
            this.WriteDSP.UseVisualStyleBackColor = false;
            this.WriteDSP.Click += new System.EventHandler(this.WriteDSP_Click);
            // 
            // ReadDSPData
            // 
            this.ReadDSPData.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.ReadDSPData.Font = new System.Drawing.Font("SimSun", 10F);
            this.ReadDSPData.Location = new System.Drawing.Point(107, 91);
            this.ReadDSPData.Name = "ReadDSPData";
            this.ReadDSPData.ReadOnly = true;
            this.ReadDSPData.Size = new System.Drawing.Size(181, 27);
            this.ReadDSPData.TabIndex = 5;
            this.ReadDSPData.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // WriteDSPData
            // 
            this.WriteDSPData.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.WriteDSPData.Font = new System.Drawing.Font("SimSun", 10F);
            this.WriteDSPData.Location = new System.Drawing.Point(107, 53);
            this.WriteDSPData.Name = "WriteDSPData";
            this.WriteDSPData.Size = new System.Drawing.Size(181, 27);
            this.WriteDSPData.TabIndex = 4;
            this.WriteDSPData.Text = "20160506";
            this.WriteDSPData.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.WriteDSPData.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.WriteDSPData_KeyPress);
            // 
            // OffsetDSP
            // 
            this.OffsetDSP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.OffsetDSP.Font = new System.Drawing.Font("SimSun", 10F);
            this.OffsetDSP.Location = new System.Drawing.Point(107, 13);
            this.OffsetDSP.MaxLength = 10;
            this.OffsetDSP.Name = "OffsetDSP";
            this.OffsetDSP.Size = new System.Drawing.Size(181, 27);
            this.OffsetDSP.TabIndex = 3;
            this.OffsetDSP.Text = "0x0";
            this.OffsetDSP.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.OffsetDSP.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.OffsetDSP_KeyPress);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("SimSun", 10F);
            this.label5.Location = new System.Drawing.Point(16, 97);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(85, 17);
            this.label5.TabIndex = 2;
            this.label5.Text = "读取数据:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("SimSun", 10F);
            this.label3.Location = new System.Drawing.Point(16, 19);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(85, 17);
            this.label3.TabIndex = 0;
            this.label3.Text = "偏移地址:";
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(224)))), ((int)(((byte)(192)))));
            this.tabPage2.Controls.Add(this.WriteSelectL2RAM);
            this.tabPage2.Controls.Add(this.WriteSelectDDR);
            this.tabPage2.Controls.Add(this.ReadDSP);
            this.tabPage2.Controls.Add(this.WriteDSP);
            this.tabPage2.Controls.Add(this.ReadDSPData);
            this.tabPage2.Controls.Add(this.WriteDSPData);
            this.tabPage2.Controls.Add(this.OffsetDSP);
            this.tabPage2.Controls.Add(this.label5);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Location = new System.Drawing.Point(4, 27);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(397, 129);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "DSP";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("SimSun", 10F);
            this.label4.Location = new System.Drawing.Point(16, 59);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(85, 17);
            this.label4.TabIndex = 1;
            this.label4.Text = "写入数据:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("SimSun", 12F);
            this.label2.Location = new System.Drawing.Point(196, 77);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(19, 20);
            this.label2.TabIndex = 6;
            this.label2.Text = "S";
            // 
            // WriteFile2FPGA
            // 
            this.WriteFile2FPGA.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.WriteFile2FPGA.Font = new System.Drawing.Font("SimSun", 9F);
            this.WriteFile2FPGA.Location = new System.Drawing.Point(231, 71);
            this.WriteFile2FPGA.Name = "WriteFile2FPGA";
            this.WriteFile2FPGA.Size = new System.Drawing.Size(75, 35);
            this.WriteFile2FPGA.TabIndex = 5;
            this.WriteFile2FPGA.Text = "写入文件";
            this.WriteFile2FPGA.UseVisualStyleBackColor = false;
            this.WriteFile2FPGA.Click += new System.EventHandler(this.WriteFile2FPGA_Click);
            // 
            // IniFile2FPGA
            // 
            this.IniFile2FPGA.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.IniFile2FPGA.Font = new System.Drawing.Font("SimSun", 9F);
            this.IniFile2FPGA.Location = new System.Drawing.Point(312, 71);
            this.IniFile2FPGA.Name = "IniFile2FPGA";
            this.IniFile2FPGA.Size = new System.Drawing.Size(75, 35);
            this.IniFile2FPGA.TabIndex = 4;
            this.IniFile2FPGA.Text = "动态加载";
            this.IniFile2FPGA.UseVisualStyleBackColor = false;
            this.IniFile2FPGA.Click += new System.EventHandler(this.IniFile2FPGA_Click);
            // 
            // DSPPanel
            // 
            this.DSPPanel.Controls.Add(this.FPGAPanel);
            this.DSPPanel.Controls.Add(this.tabPage2);
            this.DSPPanel.Font = new System.Drawing.Font("SimSun", 10F);
            this.DSPPanel.Location = new System.Drawing.Point(366, 44);
            this.DSPPanel.Name = "DSPPanel";
            this.DSPPanel.SelectedIndex = 0;
            this.DSPPanel.Size = new System.Drawing.Size(405, 160);
            this.DSPPanel.TabIndex = 8;
            // 
            // FPGAPanel
            // 
            this.FPGAPanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.FPGAPanel.Controls.Add(this.label2);
            this.FPGAPanel.Controls.Add(this.WriteFile2FPGA);
            this.FPGAPanel.Controls.Add(this.IniFile2FPGA);
            this.FPGAPanel.Controls.Add(this.WriteTimeShow);
            this.FPGAPanel.Controls.Add(this.label1);
            this.FPGAPanel.Controls.Add(this.FPGAWriteFileSelect);
            this.FPGAPanel.Controls.Add(this.FPGAWriteFilePath);
            this.FPGAPanel.Location = new System.Drawing.Point(4, 27);
            this.FPGAPanel.Name = "FPGAPanel";
            this.FPGAPanel.Padding = new System.Windows.Forms.Padding(3);
            this.FPGAPanel.Size = new System.Drawing.Size(397, 129);
            this.FPGAPanel.TabIndex = 0;
            this.FPGAPanel.Text = "FPGA";
            // 
            // WriteTimeShow
            // 
            this.WriteTimeShow.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.WriteTimeShow.Font = new System.Drawing.Font("SimSun", 15F);
            this.WriteTimeShow.Location = new System.Drawing.Point(109, 70);
            this.WriteTimeShow.Name = "WriteTimeShow";
            this.WriteTimeShow.ReadOnly = true;
            this.WriteTimeShow.Size = new System.Drawing.Size(81, 36);
            this.WriteTimeShow.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("SimSun", 12F);
            this.label1.Location = new System.Drawing.Point(13, 77);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(99, 20);
            this.label1.TabIndex = 2;
            this.label1.Text = "所花时间:";
            // 
            // FPGAWriteFileSelect
            // 
            this.FPGAWriteFileSelect.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.FPGAWriteFileSelect.Font = new System.Drawing.Font("SimSun", 9F);
            this.FPGAWriteFileSelect.Location = new System.Drawing.Point(312, 16);
            this.FPGAWriteFileSelect.Name = "FPGAWriteFileSelect";
            this.FPGAWriteFileSelect.Size = new System.Drawing.Size(75, 35);
            this.FPGAWriteFileSelect.TabIndex = 1;
            this.FPGAWriteFileSelect.Text = "选择文件";
            this.FPGAWriteFileSelect.UseVisualStyleBackColor = false;
            this.FPGAWriteFileSelect.Click += new System.EventHandler(this.FPGAWriteFileSelect_Click);
            // 
            // FPGAWriteFilePath
            // 
            this.FPGAWriteFilePath.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.FPGAWriteFilePath.Font = new System.Drawing.Font("SimSun", 10F);
            this.FPGAWriteFilePath.Location = new System.Drawing.Point(17, 20);
            this.FPGAWriteFilePath.Name = "FPGAWriteFilePath";
            this.FPGAWriteFilePath.Size = new System.Drawing.Size(285, 27);
            this.FPGAWriteFilePath.TabIndex = 0;
            // 
            // 载入表格ToolStripMenuItem
            // 
            this.载入表格ToolStripMenuItem.Name = "载入表格ToolStripMenuItem";
            this.载入表格ToolStripMenuItem.Size = new System.Drawing.Size(138, 24);
            this.载入表格ToolStripMenuItem.Text = "载入表格";
            this.载入表格ToolStripMenuItem.Click += new System.EventHandler(this.载入表格ToolStripMenuItem_Click);
            // 
            // 保存表格ToolStripMenuItem
            // 
            this.保存表格ToolStripMenuItem.Name = "保存表格ToolStripMenuItem";
            this.保存表格ToolStripMenuItem.Size = new System.Drawing.Size(138, 24);
            this.保存表格ToolStripMenuItem.Text = "保存表格";
            this.保存表格ToolStripMenuItem.Click += new System.EventHandler(this.保存表格ToolStripMenuItem_Click);
            // 
            // ListWriteMenu
            // 
            this.ListWriteMenu.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.ListWriteMenu.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.ListWriteMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.保存表格ToolStripMenuItem,
            this.载入表格ToolStripMenuItem});
            this.ListWriteMenu.Name = "ListWriteMenu";
            this.ListWriteMenu.Size = new System.Drawing.Size(139, 52);
            // 
            // Column4
            // 
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            dataGridViewCellStyle1.Format = "N0";
            dataGridViewCellStyle1.NullValue = "0";
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.Color.Yellow;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.Color.Black;
            this.Column4.DefaultCellStyle = dataGridViewCellStyle1;
            this.Column4.HeaderText = "取消";
            this.Column4.Name = "Column4";
            this.Column4.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.Column4.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            this.Column4.Width = 125;
            // 
            // Column3
            // 
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            dataGridViewCellStyle2.Format = "N0";
            dataGridViewCellStyle2.NullValue = "1";
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.Color.Yellow;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.Color.Black;
            this.Column3.DefaultCellStyle = dataGridViewCellStyle2;
            this.Column3.HeaderText = "选中";
            this.Column3.Name = "Column3";
            this.Column3.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.Column3.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            this.Column3.Width = 125;
            // 
            // Column2
            // 
            dataGridViewCellStyle3.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            dataGridViewCellStyle3.NullValue = "0x";
            dataGridViewCellStyle3.SelectionBackColor = System.Drawing.Color.Yellow;
            dataGridViewCellStyle3.SelectionForeColor = System.Drawing.Color.Black;
            this.Column2.DefaultCellStyle = dataGridViewCellStyle3;
            this.Column2.HeaderText = "物理地址";
            this.Column2.MaxInputLength = 7;
            this.Column2.Name = "Column2";
            this.Column2.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.Column2.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
            // 
            // Column1
            // 
            dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            dataGridViewCellStyle4.NullValue = false;
            dataGridViewCellStyle4.SelectionBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.Column1.DefaultCellStyle = dataGridViewCellStyle4;
            this.Column1.HeaderText = "";
            this.Column1.Name = "Column1";
            this.Column1.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.Column1.Width = 30;
            // 
            // ListWrite2FPGA
            // 
            this.ListWrite2FPGA.AllowDrop = true;
            this.ListWrite2FPGA.AllowUserToResizeColumns = false;
            this.ListWrite2FPGA.AllowUserToResizeRows = false;
            this.ListWrite2FPGA.BackgroundColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            dataGridViewCellStyle5.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle5.BackColor = System.Drawing.SystemColors.Control;
            dataGridViewCellStyle5.Font = new System.Drawing.Font("SimSun", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            dataGridViewCellStyle5.ForeColor = System.Drawing.SystemColors.WindowText;
            dataGridViewCellStyle5.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle5.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle5.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
            this.ListWrite2FPGA.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle5;
            this.ListWrite2FPGA.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.ListWrite2FPGA.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column1,
            this.Column2,
            this.Column3,
            this.Column4});
            this.ListWrite2FPGA.ContextMenuStrip = this.ListWriteMenu;
            this.ListWrite2FPGA.GridColor = System.Drawing.Color.Yellow;
            this.ListWrite2FPGA.Location = new System.Drawing.Point(366, 210);
            this.ListWrite2FPGA.Name = "ListWrite2FPGA";
            this.ListWrite2FPGA.RowHeadersVisible = false;
            this.ListWrite2FPGA.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            dataGridViewCellStyle6.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
            dataGridViewCellStyle6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(128)))));
            this.ListWrite2FPGA.RowsDefaultCellStyle = dataGridViewCellStyle6;
            this.ListWrite2FPGA.RowTemplate.Height = 27;
            this.ListWrite2FPGA.Size = new System.Drawing.Size(405, 329);
            this.ListWrite2FPGA.TabIndex = 7;
            this.ListWrite2FPGA.TabStop = false;
            this.ListWrite2FPGA.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.ListWrite2FPGA_CellContentClick);
            this.ListWrite2FPGA.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.ListWrite2FPGA_CellValueChanged);
            this.ListWrite2FPGA.EditingControlShowing += new System.Windows.Forms.DataGridViewEditingControlShowingEventHandler(this.ListWrite2FPGA_EditingControlShowing);
            this.ListWrite2FPGA.DragDrop += new System.Windows.Forms.DragEventHandler(this.ListWrite2FPGA_DragDrop);
            this.ListWrite2FPGA.DragEnter += new System.Windows.Forms.DragEventHandler(this.ListWrite2FPGA_DragEnter);
            // 
            // PCISelect
            // 
            this.PCISelect.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.PCISelect.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.PCISelect.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.PCISelect.FormattingEnabled = true;
            this.PCISelect.Location = new System.Drawing.Point(650, 15);
            this.PCISelect.Name = "PCISelect";
            this.PCISelect.Size = new System.Drawing.Size(121, 23);
            this.PCISelect.TabIndex = 6;
            this.PCISelect.TabStop = false;
            this.PCISelect.SelectedIndexChanged += new System.EventHandler(this.PCISelect_SelectedIndexChanged);
            // 
            // PCIStatusShow
            // 
            this.PCIStatusShow.AcceptsReturn = true;
            this.PCIStatusShow.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.PCIStatusShow.Font = new System.Drawing.Font("SimSun", 11F);
            this.PCIStatusShow.Location = new System.Drawing.Point(12, 15);
            this.PCIStatusShow.Multiline = true;
            this.PCIStatusShow.Name = "PCIStatusShow";
            this.PCIStatusShow.ReadOnly = true;
            this.PCIStatusShow.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.PCIStatusShow.Size = new System.Drawing.Size(333, 524);
            this.PCIStatusShow.TabIndex = 5;
            this.PCIStatusShow.TabStop = false;
            // 
            // PCIDriverTest4CS
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(782, 555);
            this.Controls.Add(this.DSPPanel);
            this.Controls.Add(this.ListWrite2FPGA);
            this.Controls.Add(this.PCISelect);
            this.Controls.Add(this.PCIStatusShow);
            this.Name = "PCIDriverTest4CS";
            this.Text = "驱动测试";
            this.Load += new System.EventHandler(this.PCIDriverTest_Load);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.DSPPanel.ResumeLayout(false);
            this.FPGAPanel.ResumeLayout(false);
            this.FPGAPanel.PerformLayout();
            this.ListWriteMenu.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.ListWrite2FPGA)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RadioButton WriteSelectL2RAM;
        private System.Windows.Forms.RadioButton WriteSelectDDR;
        private System.Windows.Forms.Button ReadDSP;
        private System.Windows.Forms.Button WriteDSP;
        private System.Windows.Forms.TextBox ReadDSPData;
        private System.Windows.Forms.TextBox WriteDSPData;
        private System.Windows.Forms.TextBox OffsetDSP;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button WriteFile2FPGA;
        private System.Windows.Forms.Button IniFile2FPGA;
        private System.Windows.Forms.TabControl DSPPanel;
        private System.Windows.Forms.TabPage FPGAPanel;
        private System.Windows.Forms.TextBox WriteTimeShow;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button FPGAWriteFileSelect;
        private System.Windows.Forms.TextBox FPGAWriteFilePath;
        private System.Windows.Forms.ToolStripMenuItem 载入表格ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 保存表格ToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip ListWriteMenu;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column4;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column3;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column2;
        private System.Windows.Forms.DataGridViewCheckBoxColumn Column1;
        private System.Windows.Forms.DataGridView ListWrite2FPGA;
        private System.Windows.Forms.ComboBox PCISelect;
        private System.Windows.Forms.TextBox PCIStatusShow;

    }
}

