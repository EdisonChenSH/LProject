using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.IO.Ports;
using System.Threading;

namespace MasterSimlator
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            CmdGroupBox.Enabled = false;
            CheckForIllegalCrossThreadCalls = false;
            SerialPortCombBox.Items.AddRange(SerialPort.GetPortNames());
            if (SerialPort.GetPortNames().Length == 0)
            {
                MessageBox.Show("找不到串口设备");
                this.Close();
            }
            SerialPortCombBox.Text = SerialPortCombBox.Items[0].ToString();
            CmdGroupBox.Enabled = false;
        }
        SerialPort ComSerialPort = new SerialPort();
        private void OpenPortButton_Click(object sender, EventArgs e)
        {
            ComSerialPort.PortName = SerialPortCombBox.Text;
            ComSerialPort.BaudRate = 19200;
            ComSerialPort.Parity = Parity.None;
            ComSerialPort.StopBits = StopBits.One;
            ComSerialPort.DataBits = 8;
            try 
            {
                ComSerialPort.Open();
                CmdGroupBox.Enabled = true;
                SystemRichTextBox.AppendText("COM Open!\r\n");
            }
            catch (Exception ex) 
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }

        }
        byte DeviceAddr=10;
        private void Read485AddrButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { 0x00, 0x00,};
            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 5);
                if (readlength == 5)
                {
                    SystemRichTextBox.AppendText(rxbuf[2].ToString()+":Device Addr\r\n");
                    DeviceAddr = rxbuf[2];
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }
        }

        private void Set485AddrButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { 0x00, 0x01, 0x0a };
            sendbuf[2] = byte.Parse(Rs485AddrTextBox.Text);
            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try 
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 5);
                if (readlength ==5)
                {
                    SystemRichTextBox.AppendText("Set485Addr OK\r\n");
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString()+ "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex) 
            {
                SystemRichTextBox.AppendText(ex.Message+"\r\n");
            }

        }

        private void ReadCANAddrButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { 0x00, 0x10, };
            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 5);
                if (readlength == 5)
                {
                    SystemRichTextBox.AppendText(rxbuf[2].ToString() + ":Device Addr\r\n");
                    DeviceAddr = rxbuf[2];
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }
        }

        private void SetCANAddrButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { 0x00, 0x11, 0x0a };
            sendbuf[2] = byte.Parse(CANAddrTextBox.Text);
            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 5);
                if (readlength == 5)
                {
                    SystemRichTextBox.AppendText("SetCANAddr OK\r\n");
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }

        }

        private void TurnonLCDButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { DeviceAddr, 0x02 };

            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 4);
                if (readlength == 4)
                {
                    SystemRichTextBox.AppendText("Turn On LCD OK\r\n");
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }
        }

        private void TurnoffLCDButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { DeviceAddr, 0x03 };

            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 4);
                if (readlength == 4)
                {
                    SystemRichTextBox.AppendText("Turn Off LCD OK\r\n");
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }
        }

        private void ReadSenorButton_Click(object sender, EventArgs e)
        {
            byte[] sendbuf = { DeviceAddr, 0x04, };
            short crc = ComputeCRC(sendbuf);
            byte[] crcBuf = BitConverter.GetBytes(crc);
            byte[] crcSendBuf = new byte[2];
            crcSendBuf[0] = crcBuf[1];
            crcSendBuf[1] = crcBuf[0];
            ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
            ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
            byte[] rxbuf = new byte[20];
            Thread.Sleep(1000);
            ComSerialPort.ReadTimeout = 2000;
            try
            {
                int readlength = ComSerialPort.Read(rxbuf, 0, 5);
                if (readlength == 5)
                {
                    SystemRichTextBox.AppendText(rxbuf[2].ToString() + ":Sensor Status\r\n");
                    
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }
            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }
        }
        struct picInfo
        {
            public UInt32 Size;
            public UInt16 x;
            public UInt16 y;
            public UInt16 weith;
            public UInt16 heigh;
        };

        void Pic2Bin() 
        {
            //File.ReadAllLines()
        }
        private void UpdatePicButton_Click(object sender, EventArgs e)
        {

            Thread thd = new Thread(new ThreadStart(updatePics));
            thd.Start();
            CmdGroupBox.Enabled = false;
        }
        void updatePics() 
        {
            try
            {
                byte[] sendbuf = { DeviceAddr, 0x05,
                                    0x00,0x02,0xEE,0x00,
                                    0x00,0x00,
                                    0x00,0x00,
                                    0x01,0x90,
                                    0x00,0xF0, 
                                 };

                short crc = ComputeCRC(sendbuf);
                byte[] crcBuf = BitConverter.GetBytes(crc);
                byte[] crcSendBuf = new byte[2];
                crcSendBuf[0] = crcBuf[1];
                crcSendBuf[1] = crcBuf[0];
                ComSerialPort.Write(sendbuf, 0, sendbuf.Length);
                ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
                byte[] rxbuf = new byte[20];
                Thread.Sleep(1000);
                ComSerialPort.ReadTimeout = 2000;

                int readlength = ComSerialPort.Read(rxbuf, 0, 5);
                if (readlength == 5)
                {
                    SystemRichTextBox.AppendText(rxbuf[2].ToString() + "Send Pic Info OK\r\n");
                }
                else
                {
                    SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                }

                int pkgCount = (400 * 240 * 2) / 1024;
                int lastPkgLength = (400 * 240 * 2) % 1024;
                if (lastPkgLength != 0) pkgCount++;
                else lastPkgLength = 1024;

                byte[] picFileBytes = File.ReadAllBytes("ico.bin");
                int picFileBytesPtr = 0;
                for (byte i = 0; i < pkgCount; i++)
                {
                    if (i != (pkgCount - 1))
                    {
                        byte[] sendbufpic = new byte[1027];
                        sendbufpic[0] = DeviceAddr;
                        sendbufpic[1] = 0x06;
                        sendbufpic[2] = i;
                        for (int j = 0; j < 1024; j++)
                        {
                            sendbufpic[j + 3] = picFileBytes[picFileBytesPtr];
                            picFileBytesPtr++;
                        }
                        crc = ComputeCRC(sendbufpic);
                        crcBuf = BitConverter.GetBytes(crc);
                        crcSendBuf = new byte[2];
                        crcSendBuf[0] = crcBuf[1];
                        crcSendBuf[1] = crcBuf[0];
                        ComSerialPort.Write(sendbufpic, 0, sendbufpic.Length);
                        ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
                        Thread.Sleep(1000);

                        ComSerialPort.ReadTimeout = 2000;
                        readlength = ComSerialPort.Read(rxbuf, 0, 5);
                        if (readlength == 5)
                        {
                            SystemRichTextBox.AppendText(i.ToString() + " " + rxbuf[2].ToString() + ":Send Pic data\r\n");
                        }
                        else
                        {
                            SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                        }
                    }
                    else
                    {
                        byte[] sendbufpic = new byte[1027];
                        sendbufpic[0] = DeviceAddr;
                        sendbufpic[1] = 0x06;
                        sendbufpic[2] = i;
                        for (int j = 0; j < lastPkgLength; j++)
                        {
                            sendbufpic[j + 3] = picFileBytes[picFileBytesPtr];
                            picFileBytesPtr++;
                        }
                        crc = ComputeCRC(sendbufpic);
                        crcBuf = BitConverter.GetBytes(crc);
                        crcSendBuf = new byte[2];
                        crcSendBuf[0] = crcBuf[1];
                        crcSendBuf[1] = crcBuf[0];
                        ComSerialPort.Write(sendbufpic, 0, sendbufpic.Length);
                        ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
                        Thread.Sleep(1000);

                        ComSerialPort.ReadTimeout = 2000;
                        readlength = ComSerialPort.Read(rxbuf, 0, 5);
                        if (readlength == 5)
                        {
                            SystemRichTextBox.AppendText(i.ToString() + " " + rxbuf[2].ToString() + ":Send Pic data\r\n");
                        }
                        else
                        {
                            SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                        }
                    }

                }
                byte[] endSendBytes={DeviceAddr,0x07};
                crc = ComputeCRC(endSendBytes);
                crcBuf = BitConverter.GetBytes(crc);
                crcSendBuf = new byte[2];
                crcSendBuf[0] = crcBuf[1];
                crcSendBuf[1] = crcBuf[0];
                ComSerialPort.Write(endSendBytes, 0, endSendBytes.Length);
                ComSerialPort.Write(crcSendBuf, 0, crcSendBuf.Length);
                rxbuf = new byte[20];
                Thread.Sleep(1000);
                ComSerialPort.ReadTimeout = 2000;

                    readlength = ComSerialPort.Read(rxbuf, 0, 8);
                    if (readlength == 8)
                    {
                        UInt32 length = (UInt32)((rxbuf[2] << 24) + (rxbuf[3] << 16) + (rxbuf[4] << 8) + (rxbuf[5]));
                        SystemRichTextBox.AppendText(length.ToString()+" bytes recived\r\n");
                    }
                    else
                    {
                        SystemRichTextBox.AppendText(readlength.ToString() + "Pkg Length Error\r\n");
                    }


            }
            catch (Exception ex)
            {
                SystemRichTextBox.AppendText(ex.Message + "\r\n");
            }
            CmdGroupBox.Enabled = true;

        }
        /// <summary>
        /// 计算CRC 网上抄的。
        /// </summary>
        /// <param name="bufDate"></param>
        /// <returns></returns>
        public short ComputeCRC(byte[] bufDate)
        {

            short cksum = 0;

            foreach (byte tmpBuf in bufDate)
            {

                cksum = (short)(cksum ^ (short)tmpBuf << 8);
                for (int i = 8; i != 0; i--)
                {
                    if ((cksum & 0x8000) != 0)
                        cksum = (short)(cksum << 1 ^ 0x1021);
                    else
                        cksum = (short)(cksum << 1);
                }
            }

            return cksum;
        }

        private void SystemRichTextBox_TextChanged(object sender, EventArgs e)
        {
            //让文本框获取焦点 
            this.SystemRichTextBox.Focus();
            //设置光标的位置到文本尾 
            this.SystemRichTextBox.Select(this.SystemRichTextBox.TextLength, 0);
            //滚动到控件光标处 
            this.SystemRichTextBox.ScrollToCaret();
        }
    }
}
