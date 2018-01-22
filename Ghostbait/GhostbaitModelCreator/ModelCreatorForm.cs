using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using static GhostbaitModelCreator.ModelCreatorForm.ComponentType;

namespace GhostbaitModelCreator {

    public partial class ModelCreatorForm : Form {

        [DllImport("FBXInterface.dll")]
        public static extern int get_mesh_from_scene(string fbx_file_path, string output_file_path, string name = null);
        [DllImport("FBXInterface.dll")]
        public static extern int get_material_from_scene(string fbx_file_path, string output_file_path, string name = null);
        [DllImport("FBXInterface.dll")]
        public static extern int get_animdata_from_scene(string fbx_file_path, string output_file_path);
        [DllImport("FBXInterface.dll")]
        public static extern int get_bindpose_from_scene(string fbx_file_path, string output_file_path);


        private string GenerateRelativeComponentFilePath(string path, ComponentType type)
        {
            int dotIndex = path.LastIndexOf('.');
            int nameStartIndex = path.LastIndexOf('\\') + 1;
            string relativePath = path.Substring(nameStartIndex, dotIndex - nameStartIndex);
            //relativePath = relativePath.Insert(0, "");

            switch (type)
            {
                case MESH:
                    //relativePath = relativePath.Insert(0, "Output\\");
                    relativePath = relativePath.Insert(relativePath.Length, ".mesh");

                    break;
                case MATERIAL:
                    //relativePath = relativePath.Insert(0, "Output\\");
                    relativePath = relativePath.Insert(relativePath.Length, ".mat");
                    break;
                case BINDPOSE:
                    //relativePath = relativePath.Insert(0, "Output\\");
                    relativePath = relativePath.Insert(relativePath.Length, ".bind");
                    break;
                case ANIMATION:
                    //relativePath = relativePath.Insert(0, "Output\\");
                    relativePath = relativePath.Insert(relativePath.Length, ".anim");
                    break;
                default:
                    break;
            }
            return relativePath;
        }
        private string GetExtension(string file)
        {
            return file.Substring(file.LastIndexOf('.'));
        }

        [Flags]
        internal enum ComponentType {
            MESH = 1 << 0,
            MATERIAL = 1 << 1,
            BINDPOSE = 1 << 2,
            COLLIDERS = 1 << 3,
            AUDIO = 1 << 4,
            ANIMATION = 1 << 5,
        }

        private struct Collider {
            private List<ColliderCreatorForm.ColliderData> colliderList;
            private ListBox listBox;
            public int ColliderCount => colliderList.Count;

            public void AddCollider(ColliderCreatorForm.ColliderData col) {
                listBox.Items.Add(col.type);
                colliderList.Add(col);
            }

            public void Edit(ColliderCreatorForm.ColliderData col, int index) {
                colliderList[index] = col;
                listBox.Items[index] = col.type;
            }

            public ColliderCreatorForm.ColliderData GetCollider(int index) {
                return colliderList[index];
            }

            public void Init(ListBox _listBox) {
                listBox = _listBox;
                colliderList = new List<ColliderCreatorForm.ColliderData>();
            }

            public void RemoveCollider() {
                int index = listBox.SelectedIndex;
                if (index < 0 || index > listBox.Items.Count)
                    return;
                listBox.Items.RemoveAt(index);
                colliderList.RemoveAt(index);
            }

            public void Reset() {
                listBox.Items.Clear();
                colliderList.Clear();
            }
        }

        private struct MiltiFileData {
            public List<string> filePaths;
            private ListBox listBox;

            public void AddFile(string fileName) {
                listBox.Items.Add(fileName);
                filePaths.Add(fileName);
            }

            public void Init(ListBox _listBox) {
                listBox = _listBox;
                filePaths = new List<string>();
            }

            public void RemoveFile() {
                int index = listBox.SelectedIndex;
                if (index < 0 || index > listBox.Items.Count) return;
                listBox.Items.RemoveAt(index);
                filePaths.RemoveAt(index);
            }

            public void Reset() {
                listBox.Items.Clear();
                filePaths.Clear();
            }
        }

        private struct SingleFileData {
            private string filePath;
            private TextBox textBox;

            public string FilePath {
                get => filePath;
                set => filePath = textBox.Text = value;
            }

            public void Init(TextBox _textBox) {
                textBox = _textBox;
                FilePath = string.Empty;
            }

            public void Reset() => FilePath = string.Empty;
        }

        private MiltiFileData anim;
        private MiltiFileData audio;
        private SingleFileData bindPose;
        private Collider colliders;
        private SingleFileData mat;
        private SingleFileData mesh;


        public ModelCreatorForm() {
            InitializeComponent();

            mesh.Init(meshFileName);
            mat.Init(materialFileName);
            bindPose.Init(bindPoseFileName);
            colliders.Init(colliderListBox);
            audio.Init(audioListBox);
            anim.Init(animationListBox);
        }

        internal void CreateColliderPressed(ColliderCreatorForm.ColliderData d) => colliders.AddCollider(d);

        internal void CreateColliderPressed(ColliderCreatorForm.ColliderData d, int index) => colliders.Edit(d, index);

        //Animation
        private void animationAdd_Click(object sender, EventArgs e) {
            OpenFileDialog open = new OpenFileDialog {
                Filter = "Animations (*.anim), (*.fbx)| *.anim; *.fbx;",
                InitialDirectory = @"C:\",
                Title = "An animation file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK) {
                if (open.FileName.Substring(open.FileName.Length - 4) == ".fbx")
                {
                    string animFile = GenerateRelativeComponentFilePath(open.FileName, ANIMATION);
                    if (get_animdata_from_scene(open.FileName, animFile) != -1) anim.AddFile(animFile);
                }
                else
                {
                    anim.AddFile(open.FileName);
                }
            }
        }

        private void animationRemove_Click(object sender, EventArgs e) => anim.RemoveFile();

        //Audio
        private void audioAdd_Click(object sender, EventArgs e) {
            OpenFileDialog open = new OpenFileDialog {
                Filter = "Audio (*.wav, *.mp3) | *.wav; *.mp3;",
                InitialDirectory = @"C:\",
                Title = "An audio file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK) {
                audio.AddFile(open.FileName);
            }
        }

        private void audioRemove_Click(object sender, EventArgs e) => audio.RemoveFile();

        //BindPose
        private void bindPoseFileGrab_Click(object sender, EventArgs e) {
            OpenFileDialog open = new OpenFileDialog {
                Filter = "BindPose (*.bind)| *.bind;",
                InitialDirectory = @"C:\",
                Title = "A bindPose file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK) {
                bindPose.FilePath = open.FileName;
            }
        }

        private void bindPoseFileRemove_Click(object sender, EventArgs e) => bindPose.Reset();

        //New
        private void blankToolStripMenuItem_Click(object sender, EventArgs e) {
            mesh.Reset();
            mat.Reset();
            bindPose.Reset();
            colliders.Reset();
            audio.Reset();
            anim.Reset();
            className.Text = string.Empty;
        }

        private void fromFBXToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "FBX files (*.fbx) | *.fbx;",
                InitialDirectory = @"C:\",
                Title = "FBX file"
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                mesh.Reset();
                mat.Reset();
                bindPose.Reset();
                colliders.Reset();
                audio.Reset();
                anim.Reset();

                string meshFile = GenerateRelativeComponentFilePath(open.FileName, MESH);
                string matFile = GenerateRelativeComponentFilePath(open.FileName, MATERIAL);
                string bindPoseFile = GenerateRelativeComponentFilePath(open.FileName, BINDPOSE);
                string animFile = GenerateRelativeComponentFilePath(open.FileName, ANIMATION);

                if (get_mesh_from_scene(open.FileName, meshFile) != -1) mesh.FilePath = meshFile;
                if (get_material_from_scene(open.FileName, matFile) != -1) mat.FilePath = matFile;
                if (get_bindpose_from_scene(open.FileName, bindPoseFile) != -1) bindPose.FilePath = bindPoseFile;
                if (get_animdata_from_scene(open.FileName, animFile) != -1) anim.AddFile(animFile);
            }
        }

        //Collider
        private void colliderAdd_Click(object sender, EventArgs e) => new ColliderCreatorForm(this).Show(this);

        private void colliderListBox_MouseDoubleClick(object sender, MouseEventArgs e) {
            int index = colliderListBox.IndexFromPoint(e.Location);
            if (index != ListBox.NoMatches) {
                var f = new ColliderCreatorForm(this);
                f.Show(this);
                f.Edit(colliders.GetCollider(index), index);
            }
        }

        private void colliderRemove_Click(object sender, EventArgs e) => colliders.RemoveCollider();

        //Material
        private void materialFileGrab_Click(object sender, EventArgs e) {
            OpenFileDialog open = new OpenFileDialog {
                Filter = "Materials (*.mat)| *.mat;",
                InitialDirectory = @"C:\",
                Title = "A material file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK) {
                mat.FilePath = open.FileName;
            }
        }

        private void materialFileRemove_Click(object sender, EventArgs e) => mat.Reset();

        //Mesh
        private void meshFileGrab_Click(object sender, EventArgs e) {
            OpenFileDialog open = new OpenFileDialog {
                Filter = "Meshes (*.mesh)| *.mesh;",
                InitialDirectory = @"C:\",
                Title = "A mesh file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK) {
                mesh.FilePath = open.FileName;
            }
        }

        private void meshFileRemove_Click(object sender, EventArgs e) => mesh.Reset();

        //Save/Load
        private void openToolStripMenuItem_Click(object sender, EventArgs e) {
            OpenFileDialog open = new OpenFileDialog {
                Filter = "Ghostbait files (*.ghost) | *.ghost;",
                InitialDirectory = @"C:\",
                Title = "Ghostbait model file"
            };
            if (open.ShowDialog() == DialogResult.OK) {
                mesh.Reset();
                mat.Reset();
                bindPose.Reset();
                colliders.Reset();
                audio.Reset();
                anim.Reset();

                //Read .ghost file to fill in data
                BinaryReader reader = new BinaryReader(open.OpenFile());

                //Class
                className.Text = new string(reader.ReadChars(reader.ReadInt32()));

                while (reader.BaseStream.Position != reader.BaseStream.Length)
                {
                    int size = reader.ReadInt32();
                    if (size > 0) //if normal string
                    {
                        string data = new string(reader.ReadChars(size));
                        if (GetExtension(data).ToLower() == ".mesh") mesh.FilePath = data;
                        else if (GetExtension(data).ToLower() == ".mat") mat.FilePath = data;
                        else if (GetExtension(data).ToLower() == ".bind") bindPose.FilePath = data;
                        else if (GetExtension(data).ToLower() == ".mp3" || GetExtension(data).ToLower() == ".wav") audio.AddFile(data);
                        else if (GetExtension(data).ToLower() == ".anim") anim.AddFile(data);
                        continue;
                    }
                    else
                    {
                        string componentName = new string(reader.ReadChars(reader.ReadInt32()));

                        if (componentName == "Physical")
                        {
                            int colCount = reader.ReadInt32();
                            ColliderCreatorForm.ColliderData colData = new ColliderCreatorForm.ColliderData();
                            for (int i = 0; i < colCount; ++i)
                            {
                                var stringCol = new string(reader.ReadChars(reader.ReadInt32()));
                                if (!Enum.TryParse(stringCol, out colData.type))
                                {
                                    MessageBox.Show("Invalid Collider Type!", $@"The Collider string.Empty{stringCol}string.Empty is not a valid collider.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    //return;
                                }
                                colData.offsetX = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                colData.offsetY = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                colData.offsetZ = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);

                                switch (colData.type)
                                {
                                    case ColliderCreatorForm.ColliderType.SPHERE:
                                        colData.radius = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        break;

                                    case ColliderCreatorForm.ColliderType.CAPSULE:
                                        colData.radius = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        colData.height = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        break;

                                    case ColliderCreatorForm.ColliderType.BOX:
                                        colData.point1X = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        colData.point1Y = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        colData.point1Z = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        colData.point2X = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        colData.point2Y = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        colData.point2Z = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                        break;

                                    default: break;
                                }

                                colliders.AddCollider(colData);
                            }
                        }
                    }
                }
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e) {
            if (className.Text == string.Empty) {
                MessageBox.Show(this, "Must have class name.", "Missing Data", MessageBoxButtons.OK);
                return;
            }
            SaveFileDialog save = new SaveFileDialog {
                Filter = "Ghostbait file (*.ghost)| *.ghost; |Binary file (*.bin)| *.bin;",
                InitialDirectory = @"C:\",
                Title = "Ghostbait model file"
            };
            if (save.ShowDialog() == DialogResult.OK) {
                using (BinaryWriter writer = new BinaryWriter(save.OpenFile())) {
                    //Class
                    writer.Write(className.Text.Length);
                    writer.Write(className.Text.ToCharArray());
                    //Mesh
                    if (mesh.FilePath != string.Empty) {
                        writer.Write(mesh.FilePath.Length);
                        writer.Write(mesh.FilePath.ToCharArray());
                    }
                    //Material
                    if (mat.FilePath != string.Empty) {
                        writer.Write(mat.FilePath.Length);
                        writer.Write(mat.FilePath.ToCharArray());
                    }
                    //BindPose
                    if (bindPose.FilePath != string.Empty) {
                        writer.Write(bindPose.FilePath.Length);
                        writer.Write(bindPose.FilePath.ToCharArray());
                    }
                    //Colliders
                    if (colliders.ColliderCount > 0) {
                        //Find ColliderDataSize
                        int colliderDataSize = sizeof(Int32);
                        string physicsName = "Physical";
                        for(int i = 0; i < colliders.ColliderCount; ++i)
                        {
                            //Type
                            colliderDataSize += sizeof(Int32) + colliders.GetCollider(i).type.ToString().Length;
                            //Offset
                            colliderDataSize += sizeof(float) * 3;
                            //Custom Data
                            switch (colliders.GetCollider(i).type)
                            {
                                case ColliderCreatorForm.ColliderType.SPHERE:
                                    colliderDataSize += sizeof(float);
                                    break;
                                case ColliderCreatorForm.ColliderType.CAPSULE:
                                    colliderDataSize += sizeof(float) * 2;
                                    break;
                                case ColliderCreatorForm.ColliderType.BOX:
                                    colliderDataSize += sizeof(float) * 6;
                                    break;
                                default: break;
                            }
                        }
                        //Writer Collider Header
                        writer.Write(-colliderDataSize);
                        writer.Write(physicsName.Length);
                        writer.Write(physicsName.ToCharArray());
                        //Write ColliderData
                        writer.Write(colliders.ColliderCount);
                        for (int i = 0; i < colliders.ColliderCount; ++i) {
                            //Type
                            var enumString = colliders.GetCollider(i).type.ToString();
                            writer.Write(enumString.Length);
                            writer.Write(enumString.ToCharArray());
                            //Offset
                            writer.Write(colliders.GetCollider(i).offsetX);
                            writer.Write(colliders.GetCollider(i).offsetY);
                            writer.Write(colliders.GetCollider(i).offsetZ);
                            //Custom Data
                            switch (colliders.GetCollider(i).type) {
                                case ColliderCreatorForm.ColliderType.SPHERE:
                                    writer.Write(colliders.GetCollider(i).radius);
                                    break;

                                case ColliderCreatorForm.ColliderType.CAPSULE:
                                    writer.Write(colliders.GetCollider(i).radius);
                                    writer.Write(colliders.GetCollider(i).height);
                                    break;

                                case ColliderCreatorForm.ColliderType.BOX:
                                    writer.Write(colliders.GetCollider(i).point1X);
                                    writer.Write(colliders.GetCollider(i).point1Y);
                                    writer.Write(colliders.GetCollider(i).point1Z);
                                    writer.Write(colliders.GetCollider(i).point2X);
                                    writer.Write(colliders.GetCollider(i).point2Y);
                                    writer.Write(colliders.GetCollider(i).point2Z);
                                    break;

                                default: break;
                            }
                        }
                    }
                    //Audio
                    for (int i = 0; i < audio.filePaths.Count; ++i) {
                        writer.Write(audio.filePaths[i].Length);
                        writer.Write(audio.filePaths[i].ToCharArray());
                    }
                    //Animations
                    for (int i = 0; i < anim.filePaths.Count; ++i) {
                        writer.Write(anim.filePaths[i].Length);
                        writer.Write(anim.filePaths[i].ToCharArray());
                    }
                }
            }
        }
    }
}
