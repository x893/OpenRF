using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FHSSChannelCalculator
{
    public partial class Form1 : Form
    {
        private ChannelTable[] tables;
        public Form1()
        {
            InitializeComponent();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void btnGenerate_Click(object sender, EventArgs e)
        {
            int channelCount = (int)numericUpDown1.Value;
            Random r = new Random(DateTime.Now.Second);
            tables = new ChannelTable[Convert.ToInt32(nudTableCount.Value)];
            for (int i = 0; i < nudTableCount.Value; i++)
            {
                tables[i] = new ChannelTable();
                tables[i].Channels = new int[channelCount];
                for (int a = 0; a < channelCount; a++) tables[i].Channels[a] = int.MaxValue;
                for (int chanCnt = 0; chanCnt < channelCount; chanCnt++)
                {
                    var nextChan = r.Next(channelCount);
                    while (TableHasChannel(i, nextChan))
                    {
                        nextChan = r.Next(channelCount);
                    }
                    tables[i].Channels[chanCnt] = nextChan;
                }
            }

            tbCodeBox.Text = "";
            tbCodeBox.Text = "U8 hopTable[" + nudTableCount.Value.ToString("##") + "]["+channelCount+"]=" + Environment.NewLine + "{" +
                             Environment.NewLine;
            foreach (ChannelTable table in tables)
            {
                tbCodeBox.Text += "        {";
                for (int i = 0; i < channelCount; i++)
                {
                    tbCodeBox.Text += "0x"+table.Channels[i].ToString("x2");
                    if (i < channelCount-1) tbCodeBox.Text += ",";
                }
                tbCodeBox.Text += "}," + Environment.NewLine;
            }

            tbCodeBox.Text += "};";
        }

        private bool TableHasChannel(int i, int nextChan)
        {
            int channelCount = (int)numericUpDown1.Value;
            for(int j=0;j<channelCount;j++)
                if (tables[i].Channels[j] == nextChan)
                    return true;
            return false;
        }
    }

    public class ChannelTable
    {
        public int[] Channels { get; set; }
    
    }
}
