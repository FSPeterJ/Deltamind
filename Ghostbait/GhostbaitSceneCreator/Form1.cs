using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using GhostbaitSceneCreator.Properties;

namespace GhostbaitSceneCreator
{
    public partial class Form1 : Form
    {
        string levelPath = "Level Files/";

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int write_scene_to_file(string fbx_file_path, string output_file_path);

        string currentFile = "";
        public Form1()
        {
            InitializeComponent();
        }

        private void newFromfbxToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "FBX files (*.fbx) | *.fbx;",
                InitialDirectory = Settings.Default.fbx_path,
                Title = "FBX file",
                RestoreDirectory = true
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
               if (currentFile != "")
               {
                   File.Delete(currentFile);
                   currentFile = "";
                   txt_SceneName.Text = currentFile;
               }
                Settings.Default.fbx_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                txt_SceneName.Text = Path.GetFileNameWithoutExtension(open.FileName);
                currentFile = txt_SceneName.Text + ".temp";
                write_scene_to_file(open.FileName, currentFile);
            }
        }
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if(currentFile != "")
            {
                File.Delete(currentFile);
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void btn_AddLevel_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Level files (*.xml) | *.xml;",
                InitialDirectory = Settings.Default.fbx_path,
                Title = "Level file",
                RestoreDirectory = true
            };
            if(open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.level_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
            }
        }

        private void btn_RemoveLevel_Click(object sender, EventArgs e)
        {

        }

        private void btn_AddPrefab_Click(object sender, EventArgs e)
        {

        }

        private void btn_RemovePrefab_Click(object sender, EventArgs e)
        {

        }
    }
}
