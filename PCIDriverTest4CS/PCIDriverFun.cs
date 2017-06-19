using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using PCIDriverHelper4CS;

namespace PCIDriverTest4CS
{
    public class PCIDriverFun
    {
        #region 驱动相关定义
        public static PciDriverFun.DSPDriver4CS[] dspDriver = new PciDriverFun.DSPDriver4CS[PciDriverFun.devNum_Max];
        public static uint pciStatus;
        public static uint deviceNum = 0;
        public static uint deviceSelect = 0;
        #endregion

        #region 接收消息相关函数
        //[DllImport("User32.dll ")]
        //public static extern IntPtr FindWindowEx(IntPtr parent, IntPtr childe, string strclass, string strname);

        [DllImport("user32.dll")]
        public static extern void PostMessage(IntPtr hWnd, int msg, int wParam, int lParam);
        #endregion
    }
}
