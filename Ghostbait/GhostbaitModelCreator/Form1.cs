using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GhostbaitModelCreator
{
    public partial class Form1 : Form
    {
        public enum Flags
        {
            MESH = 1,
            MATERIAL = (1 << 1),
            BINDPOSE = (1 << 2),
            COLLIDERS = (1 << 3),
            AUDIO = (1 << 4),
            ANIMATION = (1 << 5)
        }

        struct Mesh {
            TextBox textBox;
            string filePath;

            public void Init(TextBox _textBox)
            {
                textBox = _textBox;
                filePath = "";
            }
            public void SetMeshFile(string fileName)
            {
                textBox.Text = fileName;
                filePath = fileName;
            }
            public string GetFileName()
            {
                return filePath;
            }
            public void Reset()
            {
                textBox.Text = "";
                filePath = "";
            }
        }
        struct Material
        {
            TextBox textBox;
            string filePath;

            public void Init(TextBox _textBox)
            {
                textBox = _textBox;
                filePath = "";
            }
            public void SetMatFile(string fileName)
            {
                textBox.Text = fileName;
                filePath = fileName;
            }
            public string GetFileName()
            {
                return filePath;
            }
            public void Reset()
            {
                textBox.Text = "";
                filePath = "";
            }
        }
        struct BindPose
        {
            TextBox textBox;
            string filePath;
            public void Init(TextBox _textBox)
            {
                textBox = _textBox;
                filePath = "";
            }
            public void SetBindPoseFile(string fileName)
            {
                textBox.Text = fileName;
                filePath = fileName;
            }
            public string GetFileName()
            {
                return filePath;
            }
            public void Reset()
            {
                textBox.Text = "";
                filePath = "";
            }
        }
        struct Collider
        {
            ListBox listBox;
            public List<Form2.ColliderData> colliderList;
            public void Init(ListBox _listBox)
            {
                listBox = _listBox;
                colliderList = new List<Form2.ColliderData>();
            }
            public void AddCollider(Form2.ColliderData col)
            {
                listBox.Items.Add(col.type);
                colliderList.Add(col);
            }
            public void RemoveCollider()
            {
                int index = listBox.SelectedIndex;
                if (index < 0 || index > listBox.Items.Count)
                    return;
                listBox.Items.RemoveAt(index);
                colliderList.RemoveAt(index);
            }
            public void Reset()
            {
                listBox.Items.Clear();
                colliderList.Clear();
            }
        }
        struct Audio
        {
            ListBox listBox;
            public List<string> filePaths;
            public void Init(ListBox _listBox)
            {
                listBox = _listBox;
                filePaths = new List<string>();
            }
            public void AddAudioFile(string fileName)
            {
                listBox.Items.Add(fileName);
                filePaths.Add(fileName);
            }
            public void RemoveAudioFile()
            {
                int index = listBox.SelectedIndex;
                if (index < 0 || index > listBox.Items.Count)
                    return;
                listBox.Items.RemoveAt(index);
                filePaths.RemoveAt(index);
            }
            public void Reset()
            {
                listBox.Items.Clear();
                filePaths.Clear();
            }
        }
        struct Animation
        {
            ListBox listBox;
            public List<string> filePaths;
            public void Init(ListBox _listBox)
            {
                listBox = _listBox;
                filePaths = new List<string>();
            }
            public void AddAnimationFile(string fileName)
            {
                listBox.Items.Add(fileName);
                filePaths.Add(fileName);
            }
            public void RemoveAnimationFile()
            {
                int index = listBox.SelectedIndex;
                if (index < 0 || index > listBox.Items.Count)
                    return;
                listBox.Items.RemoveAt(index);
                filePaths.RemoveAt(index);
            }
            public void Reset()
            {
                listBox.Items.Clear();
                filePaths.Clear();
            }
        }


        Form2 ColliderCreator;

        Mesh mesh;
        Material mat;
        BindPose bindPose;
        Collider colliders;
        Audio audio;
        Animation anim;

        public Form1()
        {
            InitializeComponent();

            mesh.Init(meshFileName);
            mat.Init(materialFileName);
            bindPose.Init(bindPoseFileName);
            colliders.Init(colliderListBox);
            audio.Init(audioListBox);
            anim.Init(animationListBox);
        }

        //New
        private void blankToolStripMenuItem_Click(object sender, EventArgs e)
        {
            mesh.Reset();
            mat.Reset();
            bindPose.Reset();
            colliders.Reset();
            audio.Reset();
            anim.Reset();
            className.Text = "";
        }

        //Mesh
        private void meshFileGrab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "Meshes (*.mesh)| *.mesh; |All files | *.*;";
            open.InitialDirectory = @"C:\";
            open.Title = "A mesh file for this Ghostbait object.";
            if (open.ShowDialog() == DialogResult.OK)
            {
                mesh.SetMeshFile(open.FileName);
            }
        }
        private void meshFileRemove_Click(object sender, EventArgs e)
        {
            mesh.Reset();
        }

        //Material
        private void materialFileGrab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "Materials (*.mat)| *.mat; |All files | *.*;";
            open.InitialDirectory = @"C:\";
            open.Title = "A material file for this Ghostbait object.";
            if (open.ShowDialog() == DialogResult.OK)
            {
                mat.SetMatFile(open.FileName);
            }
        }
        private void materialFileRemove_Click(object sender, EventArgs e)
        {
            mat.Reset();
        }

        //BindPose
        private void bindPoseFileGrab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "BindPose (*.bind)| *.bind; |All files | *.*;";
            open.InitialDirectory = @"C:\";
            open.Title = "A bindPose file for this Ghostbait object.";
            if (open.ShowDialog() == DialogResult.OK)
            {
                bindPose.SetBindPoseFile(open.FileName);
            }
        }
        private void bindPoseFileRemove_Click(object sender, EventArgs e)
        {
            bindPose.Reset();
        }

        //Audio
        private void audioAdd_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "Audio (*.wav, *.mp3) | *.wav; *.mp3; |All files | *.*;";
            open.InitialDirectory = @"C:\";
            open.Title = "An audio file for this Ghostbait object.";
            if (open.ShowDialog() == DialogResult.OK)
            {
                audio.AddAudioFile(open.FileName);
            }
        }
        private void audioRemove_Click(object sender, EventArgs e)
        {
            audio.RemoveAudioFile();
        }

        //Animation
        private void animationAdd_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "Animations (*.anim)| *.anim; |All files | *.*;";
            open.InitialDirectory = @"C:\";
            open.Title = "An animation file for this Ghostbait object.";
            if (open.ShowDialog() == DialogResult.OK)
            {
                anim.AddAnimationFile(open.FileName);
            }
        }
        private void animationRemove_Click(object sender, EventArgs e)
        {
            anim.RemoveAnimationFile();
        }

        //Save/Load
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "Ghostbait files (*.ghost) | *.ghost; |All files | *.*";
            open.InitialDirectory = @"C:\";
            open.Title = "Ghostbait model file";
            if (open.ShowDialog() == DialogResult.OK)
            {

                mesh.Reset();
                mat.Reset();
                bindPose.Reset();
                colliders.Reset();
                audio.Reset();
                anim.Reset();

                //Read .ghost file to fill in data
                BinaryReader reader = new BinaryReader(open.OpenFile());

                //Flags
                int flags = reader.ReadInt32();
                //Class
                int length = reader.ReadInt32();
                className.Text = new string(reader.ReadChars(length));
                //Colliders
                if((flags & (int)Flags.COLLIDERS) != 0)
                {
                    int colCount = reader.ReadInt32();
                    Form2.ColliderData colData = new Form2.ColliderData();
                    for(int i = 0; i < colCount; ++i)
                    {
                        length = reader.ReadInt32();
                        colData.type = new string(reader.ReadChars(length));
                        colData.offsetX = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                        colData.offsetY = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                        colData.offsetZ = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                        if(colData.type == "Sphere")
                        {
                            colData.radius = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                        }
                        else if(colData.type == "Capsule")
                        {
                            colData.radius = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            colData.height = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                        }
                        else if(colData.type == "Box")
                        {
                            colData.point1X = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            colData.point1Y = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            colData.point1Z = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            colData.point2X = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            colData.point2Y = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                            colData.point2Z = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                        }
                        colliders.AddCollider(colData);
                    }

                }
                //Mesh
                if ((flags & (int)Flags.MESH) != 0)
                {
                    length = reader.ReadInt32();
                    mesh.SetMeshFile(new string(reader.ReadChars(length)));
                }
                //Material
                if ((flags & (int)Flags.MATERIAL) != 0)
                {
                    length = reader.ReadInt32();
                    mat.SetMatFile(new string(reader.ReadChars(length)));
                }
                //BindPose
                if ((flags & (int)Flags.BINDPOSE) != 0)
                {
                    length = reader.ReadInt32();
                    bindPose.SetBindPoseFile(new string(reader.ReadChars(length)));
                }
                //Audio
                if ((flags & (int)Flags.AUDIO) != 0)
                {
                    int audioCount = reader.ReadInt32();
                    for(int i = 0; i < audioCount; ++i)
                    {
                        length = reader.ReadInt32();
                        audio.AddAudioFile(new string(reader.ReadChars(length)));
                    }
                }
                //Animations
                if ((flags & (int)Flags.ANIMATION) != 0)
                {
                    int animCount = reader.ReadInt32();
                    for (int i = 0; i < animCount; ++i)
                    {
                        length = reader.ReadInt32();
                        anim.AddAnimationFile(new string(reader.ReadChars(length)));
                    }
                }
                reader.Close();
            }
        }
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (className.Text == "")
            {
                MessageBox.Show(this, "Must have class name.", "Missing Data", MessageBoxButtons.OK);
                return;
            }
            SaveFileDialog save = new SaveFileDialog();
            save.Filter = "Ghostbait file (*.ghost)| *.ghost; |Binary file (*.bin)| *.bin;";
            save.InitialDirectory = @"C:\";
            save.Title = "Ghostbait model file";
            if (save.ShowDialog() == DialogResult.OK)
            {
                BinaryWriter writer = new BinaryWriter(save.OpenFile());
                //Set Flags
                Int32 flags = 0;
                if (mesh.GetFileName() != "") flags |= (int)Flags.MESH;
                if (mat.GetFileName() != "") flags |= (int)Flags.MATERIAL;
                if (bindPose.GetFileName() != "") flags |= (int)Flags.BINDPOSE;
                if (colliders.colliderList.Count > 0) flags |= (int)Flags.COLLIDERS;
                if (audio.filePaths.Count > 0) flags |= (int)Flags.AUDIO;
                if (anim.filePaths.Count > 0) flags |= (int)Flags.ANIMATION;

                //Flags
                writer.Write(flags);
                //Class
                writer.Write(className.Text.Length);
                writer.Write(className.Text.ToCharArray());
                //Colliders
                if ((flags & (int)Flags.COLLIDERS) != 0)
                {
                    writer.Write(colliders.colliderList.Count);
                    for (int i = 0; i < colliders.colliderList.Count; ++i)
                    {
                        //Type
                        writer.Write(colliders.colliderList[i].type.Length);
                        writer.Write(colliders.colliderList[i].type.ToCharArray());
                        //Offset
                        writer.Write(colliders.colliderList[i].offsetX);
                        writer.Write(colliders.colliderList[i].offsetY);
                        writer.Write(colliders.colliderList[i].offsetZ);
                        //Custom Data
                        if (colliders.colliderList[i].type == "Sphere")
                        {
                            writer.Write(colliders.colliderList[i].radius);
                        }
                        else if (colliders.colliderList[i].type == "Capsule")
                        {
                            writer.Write(colliders.colliderList[i].radius);
                            writer.Write(colliders.colliderList[i].height);
                        }
                        else if (colliders.colliderList[i].type == "Box")
                        {
                            writer.Write(colliders.colliderList[i].point1X);
                            writer.Write(colliders.colliderList[i].point1Y);
                            writer.Write(colliders.colliderList[i].point1Z);
                            writer.Write(colliders.colliderList[i].point2X);
                            writer.Write(colliders.colliderList[i].point2Y);
                            writer.Write(colliders.colliderList[i].point2Z);

                        }
                    }
                }
                //Mesh
                if ((flags & (int)Flags.MESH) != 0)
                {
                    writer.Write(mesh.GetFileName().Length);
                    writer.Write(mesh.GetFileName().ToCharArray());
                }
                //Material
                if ((flags & (int)Flags.MATERIAL) != 0)
                {
                    writer.Write(mat.GetFileName().Length);
                    writer.Write(mat.GetFileName().ToCharArray());
                }
                //BindPose
                if ((flags & (int)Flags.BINDPOSE) != 0)
                {
                    writer.Write(bindPose.GetFileName().Length);
                    writer.Write(bindPose.GetFileName().ToCharArray());
                }
                //Audio
                if ((flags & (int)Flags.AUDIO) != 0)
                {
                    writer.Write(audio.filePaths.Count);
                    for (int i = 0; i < audio.filePaths.Count; ++i)
                    {
                        writer.Write(audio.filePaths[i].Length);
                        writer.Write(audio.filePaths[i].ToCharArray());
                    }
                }
                //Animations
                if ((flags & (int)Flags.ANIMATION) != 0)
                {
                    writer.Write(anim.filePaths.Count);
                    for (int i = 0; i < anim.filePaths.Count; ++i)
                    {
                        writer.Write(anim.filePaths[i].Length);
                        writer.Write(anim.filePaths[i].ToCharArray());
                    }
                }

                writer.Close();
            }
        }

        private void colliderAdd_Click(object sender, EventArgs e)
        {
            ColliderCreator = new Form2(this);
            ColliderCreator.Show();
        }
        private void colliderRemove_Click(object sender, EventArgs e)
        {
            colliders.RemoveCollider();
        }
        public void CreateColliderPressed()
        {
            colliders.AddCollider(ColliderCreator.newCol);
        }
    }
}
