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
    public struct prefab
    {
        public string name;
        public List<float[]> matrices;
    }
    public partial class Form1 : Form
    {
        string levelPath = "Level Files/";
        string prefabPath = "Assets/";

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int write_scene_to_file(string fbx_file_path, string output_file_path);

        string currentFile = "";
        public Form1()
        {
            InitializeComponent();
        }

        public void ClearItems()
        {
            lbo_Levels.Items.Clear();
            lbo_AdditionalPrefabs.Items.Clear();
            if (currentFile != "")
            {
                File.Delete(currentFile);
                currentFile = "";
            }
            txt_SceneName.Text = "";
            txt_FbxFile.Text = "";
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
               txt_FbxFile.Text = Path.GetFileName(open.FileName);
               lbo_Levels.Items.Clear();
               lbo_AdditionalPrefabs.Items.Clear();
            }
        }
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Scene files (*.ghostscene) | *.ghostscene;",
                InitialDirectory = Settings.Default.scene_path,
                Title = "Scene file"
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.scene_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                ClearItems();
                BinaryReader reader = new BinaryReader(open.OpenFile());

                string name = new string(reader.ReadChars(reader.ReadInt32()));
                txt_FbxFile.Text = "Loaded from ghostscene";
                txt_SceneName.Text = name;
                int lvlCount = reader.ReadInt32();
                for(int i = 0; i < lvlCount; ++i)
                {
                    string lvlName = new string(reader.ReadChars(reader.ReadInt32()));
                    lvlName = lvlName.Trim('\0');
                    lbo_Levels.Items.Add(Path.GetFileName(lvlName));
                }
                currentFile = txt_SceneName.Text + ".temp";
                BinaryWriter writer = new BinaryWriter(File.Open(currentFile, FileMode.Create));
                int size = reader.ReadInt32();
                int sizeOffset = 0;
                List<prefab> toWrite = new List<prefab>();
                for(int i = 0; i < size; ++i)
                {
                    prefab toAdd = new prefab();
                    toAdd.matrices = new List<float[]>();
                    string prefabName = new string(reader.ReadChars(reader.ReadInt32()));
                    prefabName = prefabName.Substring(prefabPath.Length, prefabName.Length - prefabPath.Length);
                    toAdd.name = prefabName;
                    prefabName = prefabName.Trim('\0');
                    int instanceCount = reader.ReadInt32();
                    if(instanceCount == 0)
                    {
                        sizeOffset++;
                        lbo_AdditionalPrefabs.Items.Add(prefabName);
                        continue;
                    }
                    else
                    {
                        for (int index = 0; index < instanceCount; ++index)
                        {
                            float[] matrix = new float[16];
                            for (int anotherIndex = 0; anotherIndex < 16; ++anotherIndex)
                            {
                                matrix[anotherIndex] = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            }
                            toAdd.matrices.Add(matrix);
                        }
                    }
                }
                reader.Close();
                writer.Close();
            }
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
            if (currentFile == "")
            {
                MessageBox.Show("No scene .fbx file loaded!", "Missing data", MessageBoxButtons.OK);
                return;
            }
            if (txt_SceneName.Text == string.Empty)
            {
                MessageBox.Show("Must have a scene name!", "Missing data", MessageBoxButtons.OK);
                return;
            }

            SaveFileDialog save = new SaveFileDialog
            {
                Filter = "Scene file (*.ghostscene)| *.ghostscene; |Binary file (*.bin)| *.bin;",
                InitialDirectory = Settings.Default.scene_path,
                Title = "Ghostbait scene file"
            };
            if (save.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.scene_path = Path.GetDirectoryName(save.FileName);
                Settings.Default.Save();
                using (BinaryWriter writer = new BinaryWriter(save.OpenFile()))
                {
                    //Scene name
                    string sceneName = txt_SceneName.Text + '\0';
                    writer.Write(sceneName.Length);
                    writer.Write(sceneName.ToCharArray());

                    //Level .xml files
                    writer.Write(lbo_Levels.Items.Count);
                    for(int i = 0; i < lbo_Levels.Items.Count; ++i)
                    {
                        string path = levelPath + (string)lbo_Levels.Items[i] + '\0';
                        writer.Write(path.Length);
                        writer.Write(path.ToCharArray());
                    }

                    BinaryReader reader = new BinaryReader(File.Open(currentFile, FileMode.Open));
                    int size = reader.ReadInt32();
                    writer.Write(size + lbo_AdditionalPrefabs.Items.Count);
                    for(int i = 0; i < size; ++i)
                    {
                        int nameLen = reader.ReadInt32();
                        string name = new string(reader.ReadChars(nameLen));
                        name = prefabPath + name;
                        int instanceCount = reader.ReadInt32();

                        writer.Write(name.Length);
                        writer.Write(name.ToCharArray());
                        writer.Write(instanceCount);
                        for(int index = 0; index < instanceCount; ++index)
                        {
                            float[] matrix = new float[16];
                            for(int anotherIndex = 0; anotherIndex < 16; ++anotherIndex)
                            {
                                matrix[anotherIndex] = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                writer.Write(matrix[anotherIndex]);//just using this for debugging
                            }
                        }
                    }
                    for(int i = 0; i < lbo_AdditionalPrefabs.Items.Count; ++i)
                    {
                        string path = prefabPath + lbo_AdditionalPrefabs.Items[i] + '\0';
                        writer.Write(path.Length);
                        writer.Write(path.ToCharArray());
                        writer.Write(0);
                    }
                    writer.Close();
                    reader.Close();
                }
            }
        }

        private void btn_AddLevel_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Level files (*.xml) | *.xml;",
                InitialDirectory = Settings.Default.level_path,
                Title = "Level file",
                RestoreDirectory = true
            };
            if(open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.level_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                lbo_Levels.Items.Add(Path.GetFileName(open.FileName));
            }
        }

        private void btn_RemoveLevel_Click(object sender, EventArgs e)
        {
            if(lbo_Levels.SelectedIndex != -1)
            {
                lbo_Levels.Items.RemoveAt(lbo_Levels.SelectedIndex);
            }
        }

        private void btn_AddPrefab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Prefab files (*.ghost) | *.ghost;",
                InitialDirectory = Settings.Default.prefab_path,
                Title = "Prefab file",
                RestoreDirectory = true
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.prefab_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                lbo_AdditionalPrefabs.Items.Add(Path.GetFileName(open.FileName));
            }
        }

        private void btn_RemovePrefab_Click(object sender, EventArgs e)
        {
            if (lbo_AdditionalPrefabs.SelectedIndex != -1)
            {
                lbo_AdditionalPrefabs.Items.RemoveAt(lbo_AdditionalPrefabs.SelectedIndex);
            }
        }

        private void lbo_Levels_DoubleClick(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Level files (*.xml) | *.xml;",
                InitialDirectory = Settings.Default.level_path,
                Title = "Level file",
                RestoreDirectory = true
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.level_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                lbo_Levels.Items[lbo_Levels.SelectedIndex] = Path.GetFileName(open.FileName);
            }
        }

        private void lbo_AdditionalPrefabs_DoubleClick(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Prefab files (*.ghost) | *.ghost;",
                InitialDirectory = Settings.Default.prefab_path,
                Title = "Prefab file",
                RestoreDirectory = true
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.prefab_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                lbo_AdditionalPrefabs.Items[lbo_AdditionalPrefabs.SelectedIndex] = Path.GetFileName(open.FileName);
            }
        }

        private void btn_OpenFbx_Click(object sender, EventArgs e)
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
                }
                Settings.Default.fbx_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                currentFile = Path.GetFileNameWithoutExtension(open.FileName) + ".temp";
                write_scene_to_file(open.FileName, currentFile);
                txt_FbxFile.Text = Path.GetFileName(open.FileName);
            }
        }

        private void emptyToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClearItems();
        }
    }
}
