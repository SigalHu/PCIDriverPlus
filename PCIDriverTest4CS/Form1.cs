using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using PCIDriverHelper4CS;

namespace PCIDriverTest4CS
{
    public partial class PCIDriverTest4CS : Form
    {
        System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();  // hu 计时器

        public PCIDriverTest4CS()
        {
            InitializeComponent();
        }

        private void PCIDriverTest_Load(object sender, EventArgs e)
        {
            #region 驱动初始化
            for (int ii = 0; ii < PCIDriverFun.dspDriver.Length; ii++)
            {
                PCIDriverFun.dspDriver[ii] = new PciDriverFun.DSPDriver4CS();
            }
#if DEBUG_HU

            PCIDriverFun.pciStatus = 99;
#else
            PCIDriverFun.pciStatus = PciDriverFun.InitAllDSPDriver4CS(PCIDriverFun.dspDriver);
#endif
            if (0 == PCIDriverFun.pciStatus) PCIStatusShow.Text = "初始化驱动程序成功!\r\n";
            else PCIStatusShow.Text = PciDriverFun.GetLastDriverError4CS();

            if (0 == PCIDriverFun.pciStatus)
            {
                for (uint ii = 0; ii < PciDriverFun.devNum_Max; ii++)
                {
                    PCIStatusShow.Text = PCIStatusShow.Text + "PCI设备" + ii.ToString() + ":\r\n";

                    if (0 == PCIDriverFun.dspDriver[ii].status) PCIDriverFun.deviceNum = ii + 1;

                    PCIStatusShow.Text += PciDriverFun.DriverStatus2Str4CS(PCIDriverFun.dspDriver[ii].status);
                    PCIStatusShow.Text += "\r\n";

                    for (uint jj = 0; jj < PCIDriverFun.dspDriver[ii].memNum; jj++)
                    {
                        PCIStatusShow.Text += PciDriverFun.DriverMemStatus2Str4CS(PCIDriverFun.dspDriver[ii].memStatus[jj], PCIDriverFun.dspDriver[ii].memSize[jj]);
                        PCIStatusShow.Text += "\r\n";
                    }
                }

                // hu 初始化组合框
                if (0 == PCIDriverFun.deviceNum) PCISelect.Items.Add("无设备");
                else
                {
                    PCISelect.Text = "设备0";
                    for (uint ii = 0; ii < PCIDriverFun.deviceNum; ii++)
                    {
                        PCISelect.Items.Add("设备" + ii.ToString());
                    }
                }
                PCISelect.SelectedIndex = 0;
                PCIDriverFun.deviceSelect = 0;

                // hu 注册消息函数
                // hu IntPtr p = PCIDriverFun.FindWindowEx(IntPtr.Zero, IntPtr.Zero, null, "驱动测试");
                PciDriverFun.RegistHWNDToDev4CS(this.Handle);
                PciDriverFun.SetDSPAutoRun4CS(true);
            }
            #endregion
        }

        private void PCISelect_SelectedIndexChanged(object sender, EventArgs e)
        {
            PCIDriverFun.deviceSelect = (uint)PCISelect.SelectedIndex;
        }

        private void ListWrite2FPGA_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            if (0 == e.ColumnIndex)
            {
                uint fpgaAddrData, fpgaWriteData;
                bool isCheck = (bool)ListWrite2FPGA.Rows[e.RowIndex].Cells[0].EditedFormattedValue;
                string fpgaAddr = (string)ListWrite2FPGA.Rows[e.RowIndex].Cells[1].EditedFormattedValue;
                string fpgaIsCheck = (string)ListWrite2FPGA.Rows[e.RowIndex].Cells[2].EditedFormattedValue;
                string fpgaNoCheck = (string)ListWrite2FPGA.Rows[e.RowIndex].Cells[3].EditedFormattedValue;

                fpgaAddr = fpgaAddr.Remove(0, 2);
                if ("" == fpgaAddr)
                {
                    MessageBox.Show("地址栏不能为空!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                fpgaAddrData = Convert.ToUInt32(fpgaAddr, 16);

                if (true == isCheck)
                {
                    if ("" == fpgaIsCheck)
                    {
                        MessageBox.Show("数据栏不能为空!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    fpgaWriteData = Convert.ToUInt32(fpgaIsCheck);
                }
                else
                {
                    if ("" == fpgaNoCheck)
                    {
                        MessageBox.Show("数据栏不能为空!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    fpgaWriteData = Convert.ToUInt32(fpgaNoCheck);
                }

                PCIStatusShow.Text = "写入...\r\n0x" + fpgaAddr + ":" + fpgaWriteData.ToString();
#if DEBUG_HU
#else
                PciDriverFun.WriteToDSP4CS((int)PCIDriverFun.deviceSelect, 3, fpgaAddrData * 4, fpgaWriteData);
#endif
                PCIStatusShow.Text += "\r\n成功!";
            }
        }

        private void ListWrite2FPGA_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (1 == this.ListWrite2FPGA.CurrentCell.ColumnIndex ||
                2 == this.ListWrite2FPGA.CurrentCell.ColumnIndex ||
                3 == this.ListWrite2FPGA.CurrentCell.ColumnIndex)
            {
                e.Control.KeyPress += new KeyPressEventHandler(EditingControl_KeyPress);
            }
        }
        private void EditingControl_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (2 == this.ListWrite2FPGA.CurrentCell.ColumnIndex ||
                3 == this.ListWrite2FPGA.CurrentCell.ColumnIndex)
            {
                if (Char.IsDigit(e.KeyChar) || e.KeyChar == '\b')
                    e.Handled = false;
                else e.Handled = true; // hu 让操作取消
            }
            else
            {
                if (System.Text.RegularExpressions.Regex.IsMatch(e.KeyChar.ToString(), @"^[A-Fa-f0-9]+$") || e.KeyChar == '\b')
                    e.Handled = false;
                else e.Handled = true; // hu 让操作取消
            }
        }

        private void ListWrite2FPGA_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (1 == e.ColumnIndex && e.RowIndex >= 0)
            {
                string addrTmp = ListWrite2FPGA.Rows[e.RowIndex].Cells[1].Value.ToString();
                if (0 != addrTmp.IndexOf("0x"))
                {
                    int len;
                    addrTmp = addrTmp.Remove(0, addrTmp.IndexOf("x") + 1);
                    len = addrTmp.Length;
                    if (len > 5) len = 5;
                    ListWrite2FPGA.Rows[e.RowIndex].Cells[1].Value = "0x" + addrTmp.Substring(0, len);
                }
            }
        }

        private void 保存表格ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            #region 保存文件对话框
            SaveFileDialog chaSavFilDia = new SaveFileDialog();
            chaSavFilDia.DefaultExt = "txt";
            chaSavFilDia.Filter = "(*.txt)|*.txt";
            chaSavFilDia.AddExtension = true;
            chaSavFilDia.RestoreDirectory = true;
            string listSavePath = "";
            if (chaSavFilDia.ShowDialog() == DialogResult.OK)
            {
                listSavePath = chaSavFilDia.FileName;
            }
            if (listSavePath.IndexOf(":") < 0) return;
            #endregion

            FileStream listSaveFile = new FileStream(listSavePath, FileMode.Create);
            StreamWriter listSaveWrite = new StreamWriter(listSaveFile);
            for (int ii = 0; ii < ListWrite2FPGA.Rows.Count; ii++)
            {
                listSaveWrite.WriteLine(ListWrite2FPGA.Rows[ii].Cells[1].EditedFormattedValue.ToString() + "\t" +
                                        ListWrite2FPGA.Rows[ii].Cells[2].EditedFormattedValue.ToString() + "\t" +
                                        ListWrite2FPGA.Rows[ii].Cells[3].EditedFormattedValue.ToString());
            }
            listSaveWrite.Close();
            listSaveFile.Close();
        }

        private void 载入表格ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            #region 打开文件对话框
            OpenFileDialog chaOpeFilDia = new OpenFileDialog();
            chaOpeFilDia.DefaultExt = "txt";
            chaOpeFilDia.Filter = "(*.txt)|*.txt";
            chaOpeFilDia.AddExtension = true;
            chaOpeFilDia.RestoreDirectory = true;
            string listSavePath = "";
            if (chaOpeFilDia.ShowDialog() == DialogResult.OK)
            {
                listSavePath = chaOpeFilDia.FileName;
            }
            if (listSavePath.IndexOf(":") < 0) return;
            #endregion

            string listData;
            int ii;

            FileStream listSaveFile = new FileStream(listSavePath, FileMode.Open);
            StreamReader listSaveRead = new StreamReader(listSaveFile);

            listData = listSaveRead.ReadLine();
            ii = 0;
            while (null != listData)
            {
                if (ii + 1 == ListWrite2FPGA.Rows.Count) ListWrite2FPGA.Rows.Insert(ii, 1);
                ListWrite2FPGA.Rows[ii].Cells[1].Value = listData.Substring(0, listData.IndexOf("\t"));
                listData = listData.Remove(0, listData.IndexOf("\t") + 1);
                ListWrite2FPGA.Rows[ii].Cells[2].Value = listData.Substring(0, listData.IndexOf("\t"));
                listData = listData.Remove(0, listData.IndexOf("\t") + 1);
                ListWrite2FPGA.Rows[ii].Cells[3].Value = listData;

                ii++;
                listData = listSaveRead.ReadLine();
            }
            listSaveRead.Close();
            listSaveFile.Close();
            ListWrite2FPGA.Rows.RemoveAt(ListWrite2FPGA.Rows.Count - 2);
        }

        private void FPGAWriteFileSelect_Click(object sender, EventArgs e)
        {
            #region 打开文件对话框
            OpenFileDialog chaOpeFilDia = new OpenFileDialog();
            chaOpeFilDia.Filter = "(*.bin,*.bit)|*.bin;*.bit";
            chaOpeFilDia.AddExtension = true;
            chaOpeFilDia.RestoreDirectory = true;
            string listSavePath = "";
            if (chaOpeFilDia.ShowDialog() == DialogResult.OK)
            {
                listSavePath = chaOpeFilDia.FileName;
            }
            if (listSavePath.IndexOf(":") < 0) return;
            #endregion

            FPGAWriteFilePath.Text = listSavePath;
        }

        private void IniFile2FPGA_Click(object sender, EventArgs e)
        {
            PciDriverFun.WriteToDev4CS
                pPhyAddr = new PciDriverFun.WriteToDev4CS((int)PCIDriverFun.deviceSelect, 5, 8, 0),  // hu 物理地址
                dataLen = new PciDriverFun.WriteToDev4CS((int)PCIDriverFun.deviceSelect, 5, 0xc, 0),   // hu 物理空间中数据的长度
                endFlag = new PciDriverFun.WriteToDev4CS((int)PCIDriverFun.deviceSelect, 5, 0x10, 1);   // hu 传输结束标志位

            if (!File.Exists(FPGAWriteFilePath.Text))
            {
                MessageBox.Show("文件不存在!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            FileInfo iniFPGAFileInfo = new FileInfo(FPGAWriteFilePath.Text);
            if (iniFPGAFileInfo.Length >= 11 * 1024 * 1024)
            {
                MessageBox.Show("加载文件不能大于11MB!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

#if DEBUG_HU
            PCIDriverFun.PostMessage(this.Handle, (int)PciDriverFun.wm_CallbackMessage_0, 2, (int)PCIDriverFun.deviceSelect);
#else
            PciDriverFun.DmaFileToDSP4CS((int)PCIDriverFun.deviceSelect, 0, FPGAWriteFilePath.Text, pPhyAddr, dataLen, endFlag);
#endif
            WriteTimeShow.Text = "";
            PCIStatusShow.Text = "设备" + PCIDriverFun.deviceSelect.ToString() + ":\r\n开始动态加载...";
            stopwatch.Start();   // hu 开始计时
        }

        private void WriteFile2FPGA_Click(object sender, EventArgs e)
        {
            PciDriverFun.WriteToDev4CS
                pPhyAddr = new PciDriverFun.WriteToDev4CS((int)PCIDriverFun.deviceSelect, 5, 0, 0),  // hu 物理地址
                dataLen = new PciDriverFun.WriteToDev4CS((int)PCIDriverFun.deviceSelect, 5, 4, 0),   // hu 物理空间中数据的长度
                endFlag = new PciDriverFun.WriteToDev4CS((int)PCIDriverFun.deviceSelect, 5, 0x10, 1);   // hu 传输结束标志位

            if (!File.Exists(FPGAWriteFilePath.Text))
            {
                MessageBox.Show("文件不存在!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

#if DEBUG_HU
            PCIDriverFun.PostMessage(this.Handle, (int)PciDriverFun.wm_CallbackMessage_0, 3, (int)PCIDriverFun.deviceSelect);
#else
            PciDriverFun.DmaFileToDSP4CS((int)PCIDriverFun.deviceSelect, 0, FPGAWriteFilePath.Text, pPhyAddr, dataLen, endFlag);
#endif
            WriteTimeShow.Text = "";
            PCIStatusShow.Text = "设备" + PCIDriverFun.deviceSelect.ToString() + ":\r\n开始EDMA传输...";
            stopwatch.Start();   // hu 开始计时
        }

        protected override void DefWndProc(ref System.Windows.Forms.Message m)
        {
            if (m.Msg == PciDriverFun.wm_CallbackMessage_0)
            {
                #region 中断消息响应
                switch ((int)m.WParam)
                {
                    case 0:
                        {
                            // hu ///////////添加0号中断处理程序///////////////	
                            break;
                            // hu ///////////////////////////////////////////// 
                        }
                    case 1:
                        {
                            // hu ///////////添加1号中断处理程序///////////////	
                            break;
                            // hu ///////////////////////////////////////////// 
                        }
                    case 2:
                        {
                            // hu ///////////添加2号中断处理程序///////////////	
                            uint GPIO2FPGA_flag = 0;
                            stopwatch.Stop();
                            WriteTimeShow.Text = Convert.ToString(stopwatch.ElapsedMilliseconds / 1000.0);
                            stopwatch.Reset();
#if DEBUG_HU
                                    GPIO2FPGA_flag = 99;
#else
                            PciDriverFun.ReadFromDSP4CS((int)m.LParam, 5, 0x14, ref GPIO2FPGA_flag);
#endif
                            PCIStatusShow.Text = PCIStatusShow.Text + "\r\n设备" + m.LParam + ":";
                            switch (GPIO2FPGA_flag)
                            {
                                case 0: PCIStatusShow.Text += "\r\n动态加载成功!"; break;
                                case 1: PCIStatusShow.Text += "\r\nFPGA初始化失败!"; break;
                                case 2: PCIStatusShow.Text += "\r\n未收到FPGA的INTB信号，动态加载失败!"; break;
                                case 3: PCIStatusShow.Text += "\r\n未收到FPGA的DONE信号，动态加载失败!"; break;
                                default: PCIStatusShow.Text += "\r\n未收到约定返回值，动态加载失败!"; break;
                            }
                            break;
                            // hu ///////////////////////////////////////////// 
                        }
                    case 3:
                        {
                            // hu ///////////添加3号中断处理程序///////////////	
                            stopwatch.Stop();
                            WriteTimeShow.Text = Convert.ToString(stopwatch.ElapsedMilliseconds / 1000.0);
                            stopwatch.Reset();
#if DEBUG_HU
#else
                            PciDriverFun.WriteToDSP4CS((int)m.LParam, 3, 0xa0a00 * 4, 0xf0f0);// hu 通知fpga传输完毕 0xa0a00为fpga读取数据地址
#endif
                            PCIStatusShow.Text = PCIStatusShow.Text + "\r\n设备" + m.LParam + ":\r\nEDMA传输成功!";
                            break;
                            // hu ///////////////////////////////////////////// 
                        }
                    default:
                        {
                            MessageBox.Show(string.Format("中断号{0}不存在!", m.WParam),
                                "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            break;
                        }
                }
                #endregion
            }
            else base.DefWndProc(ref m); // hu 一定要调用基类函数，以便系统处理其它消息。
        }

        private void OffsetDSP_KeyPress(object sender, KeyPressEventArgs e)
        {
            OffsetDSP.Select(OffsetDSP.Text.Length, 0);
            if (2 == OffsetDSP.Text.Length)
            {
                if (System.Text.RegularExpressions.Regex.IsMatch(e.KeyChar.ToString(), @"^[A-Fa-f0-9]+$"))
                    e.Handled = false;
                else e.Handled = true; // hu 让操作取消
            }
            else
            {
                if (System.Text.RegularExpressions.Regex.IsMatch(e.KeyChar.ToString(), @"^[A-Fa-f0-9]+$") || e.KeyChar == '\b')
                    e.Handled = false;
                else e.Handled = true; // hu 让操作取消
            }
        }

        private void WriteDSPData_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (Char.IsDigit(e.KeyChar) || e.KeyChar == '\b')
                e.Handled = false;
            else e.Handled = true; // hu 让操作取消
        }

        private void WriteDSP_Click(object sender, EventArgs e)
        {
            string addrOffsStr, dataStr;
            uint addrOffs, data, barx;

            addrOffsStr = OffsetDSP.Text;
            addrOffsStr = addrOffsStr.Remove(0, 2);
            if ("" == addrOffsStr)
            {
                MessageBox.Show("偏移地址栏不能为空!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            addrOffs = Convert.ToUInt32(addrOffsStr, 16);

            dataStr = WriteDSPData.Text;
            if ("" == dataStr)
            {
                MessageBox.Show("写入数据栏不能为空!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            data = Convert.ToUInt32(dataStr);

            if (true == WriteSelectDDR.Checked) barx = 5;
            else barx = 0;

            PCIStatusShow.Text = string.Format("设备{0}:\r\n开始写入...\r\n偏移地址:0x{1}:{2}", PCIDriverFun.deviceSelect, addrOffsStr, dataStr);
#if DEBUG_HU
#else
            PciDriverFun.WriteToDSP4CS((int)PCIDriverFun.deviceSelect, (int)barx, addrOffs, data);
#endif

            PCIStatusShow.Text += "\r\n写入成功!";
        }

        private void ReadDSP_Click(object sender, EventArgs e)
        {
            string addrOffsStr;
            uint addrOffs, data = 0, barx;

            addrOffsStr = OffsetDSP.Text;
            addrOffsStr = addrOffsStr.Remove(0, 2);
            if ("" == addrOffsStr)
            {
                MessageBox.Show("偏移地址栏不能为空!", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            addrOffs = Convert.ToUInt32(addrOffsStr, 16);

            if (true == WriteSelectDDR.Checked) barx = 5;
            else barx = 0;

            PCIStatusShow.Text = string.Format("设备{0}:\r\n开始读取...", PCIDriverFun.deviceSelect);
#if DEBUG_HU
#else
            PciDriverFun.ReadFromDSP4CS((int)PCIDriverFun.deviceSelect, (int)barx, addrOffs, ref data);
#endif
            ReadDSPData.Text = data.ToString();
            PCIStatusShow.Text += string.Format("\r\n偏移地址:0x{0}:{1}\r\n读取成功!", PCIDriverFun.deviceSelect, data);
        }

        private void ListWrite2FPGA_DragDrop(object sender, DragEventArgs e)
        {
            string[] listSavePath = (string[])e.Data.GetData(DataFormats.FileDrop, false);
            if (listSavePath[0].IndexOf(".txt") > 0)
            {
                string listData;
                int ii;

                FileStream listSaveFile = new FileStream(listSavePath[0], FileMode.Open);
                StreamReader listSaveRead = new StreamReader(listSaveFile);

                listData = listSaveRead.ReadLine();
                ii = 0;
                while (null != listData)
                {
                    if (ii + 1 == ListWrite2FPGA.Rows.Count) ListWrite2FPGA.Rows.Insert(ii, 1);
                    ListWrite2FPGA.Rows[ii].Cells[1].Value = listData.Substring(0, listData.IndexOf("\t"));
                    listData = listData.Remove(0, listData.IndexOf("\t") + 1);
                    ListWrite2FPGA.Rows[ii].Cells[2].Value = listData.Substring(0, listData.IndexOf("\t"));
                    listData = listData.Remove(0, listData.IndexOf("\t") + 1);
                    ListWrite2FPGA.Rows[ii].Cells[3].Value = listData;

                    ii++;
                    listData = listSaveRead.ReadLine();
                }
                listSaveRead.Close();
                ListWrite2FPGA.Rows.RemoveAt(ListWrite2FPGA.Rows.Count - 2);
            }
        }

        private void ListWrite2FPGA_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) e.Effect = DragDropEffects.All;
            else e.Effect = DragDropEffects.None;
        }
    }
}
