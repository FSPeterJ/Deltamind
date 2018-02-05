using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using static GhostbaitModelCreator.ModelCreatorForm.ComponentType;

namespace GhostbaitModelCreator
{

    public partial class ModelCreatorForm : Form
    {

        const string appendedPath = "../../../GhostBait/";
        const string toAppend = appendedPath + "Assets/";

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_mesh_from_scene(string fbx_file_path, string output_file_path, string name = null);

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_material_from_scene(string fbx_file_path, string output_file_path, string name = null);

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_animdata_from_scene(string fbx_file_path, string output_file_path);

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_bindpose_from_scene(string fbx_file_path, string output_file_path);

        public string GenerateRelativeComponentFilePath(string path, ComponentType type)
        {
            int dotIndex = path.LastIndexOf('.');
            int nameStartIndex = path.LastIndexOf('\\') + 1;
            string relativePath = path.Substring(nameStartIndex, dotIndex - nameStartIndex);
            relativePath = toAppend + relativePath;

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
        public enum ComponentType
        {
            MESH = 1 << 0,
            MATERIAL = 1 << 1,
            BINDPOSE = 1 << 2,
            COLLIDERS = 1 << 3,
            AUDIO = 1 << 4,
            ANIMATION = 1 << 5,
        }

        public class BaseComponent
        {
            //WHAT THE FUCK - DataGridView can't Bind without get / set.  I am ignorant
            public string ComponentIdentifier { get; set; }
            public string ComponentTag { get; set; }
        }

        public class BaseComponentGroup<T>
        {
            public BindingList<T> componentList;
            public DataGridView dataGridView;
            public int Count => componentList.Count;

            public void Init(DataGridView _DataGridView)
            {
                componentList = new BindingList<T>();

                dataGridView = _DataGridView;

                //dataGridView.AutoGenerateColumns = false;
                dataGridView.AllowUserToAddRows = false;
                dataGridView.AllowUserToResizeColumns = false;
                dataGridView.AllowUserToResizeRows = false;
                dataGridView.AllowUserToOrderColumns = false;

                // We programatically popluate the DataGridView with the columns.  T
                // his allows Inhertited versions to extend the columns available or hide if necessary.  
                // It also lets us change the properties for all the grids ONCE following DRY better.
                // Unless there is a way to share properties across controls and edit them in desgin only once...
                // Would be nice to learn this if that's possible, but techincally irrelevant for me, just convienent

                // Possible change: use constructor instead of per property set, 
                // however you will lose intellisense
                DataGridViewTextBoxColumn ComponentColumn = new DataGridViewTextBoxColumn();
                ComponentColumn.Name = "ComponentIdentifier";
                ComponentColumn.ReadOnly = false;
                ComponentColumn.HeaderText = "Component";
                ComponentColumn.DataPropertyName = "ComponentIdentifier";
                ComponentColumn.AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                dataGridView.Columns.Add(ComponentColumn);
                ComponentColumn.DisplayIndex = 0;

                DataGridViewTextBoxColumn ComponentTag = new DataGridViewTextBoxColumn();
                ComponentTag.Name = "ComponentTag";
                ComponentTag.HeaderText = "Tag";
                ComponentTag.ReadOnly = false;
                ComponentTag.DataPropertyName = "ComponentTag";
                ComponentTag.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
                ComponentTag.Width = 100;
                ComponentTag.DisplayIndex = 1;
                dataGridView.Columns.Add(ComponentTag);

                DataGridViewButtonColumn EditButton = new DataGridViewButtonColumn();
                EditButton.Name = "EditButton";
                EditButton.ToolTipText = "Edit Button";
                EditButton.HeaderText = "E";
                EditButton.ReadOnly = false;
                EditButton.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
                EditButton.Width = 20;
                EditButton.DisplayIndex = 98;
                dataGridView.Columns.Add(EditButton);

                DataGridViewButtonColumn DeleteButton = new DataGridViewButtonColumn();
                DeleteButton.Name = "DeleteButton";
                DeleteButton.ToolTipText = "Delete Button";
                DeleteButton.HeaderText = "D";
                DeleteButton.ReadOnly = false;
                DeleteButton.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
                DeleteButton.Width = 20;
                DeleteButton.DisplayIndex = 99;
                dataGridView.Columns.Add(DeleteButton);

                // This may seem weird, but we want to process base init first, then the derived init
                // Which is not the normal order of executing inherited functions
                // It allows for modifying the base grid columns to tailor the grid to the custom type in the derived class
                InternalInit();
                dataGridView.DataSource = componentList;
            }

            //Standard interface functions
            //========================================

            public void Add(T comp)
            {
                componentList.Add(comp);
                //dataGridView.Refresh();
            }

            public void SaveEdit(T comp, int index)
            {
                componentList[index] = comp;
            }

            public T Get(int index)
            {
                return componentList[index];
            }

            public void Remove(int index)
            {
                componentList.RemoveAt(index);
            }

            public void Reset()
            {
                componentList.Clear();
            }

            protected virtual void InternalInit() { }
        }



        public class ColliderGroup : BaseComponentGroup<ColliderCreatorForm.ColliderData>
        {
            protected override void InternalInit()
            {
                //They all say "Physical" anyways
                dataGridView.Columns["ComponentIdentifier"].Visible = false;

                //dataGridView.CellFormatting += this.CellFormat;  // Use this for custom types in columns
                // See comments Below

                DataGridViewTextBoxColumn ComponentData = new DataGridViewTextBoxColumn();
                ComponentData.Name = "colliderType";
                ComponentData.HeaderText = "Data";
                ComponentData.DataPropertyName = "colliderType";
                ComponentData.AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                dataGridView.Columns.Add(ComponentData);
                ComponentData.DisplayIndex = 0;  // Shoves the other columns over so "I am first"
            }



            // Save this if type conversion is needed later
            // This orgianlly coverted the enum to string, until I realized DataGridView intrinsically knows
            // how to convert enums on it's own. The bug was elseware.  
            // Convert this for custom typess or string display redefintions
            //private void CellFormat(object sender, DataGridViewCellFormattingEventArgs e)
            //{
            //    DataGridView ex = sender as DataGridView;
            //    if (ex.Columns[e.ColumnIndex].Name == "colliderType")
            //    {
            //        ;
            //        if (e.Value != null)
            //        {
            //            e.Value = ((ColliderCreatorForm.ColliderType)e.Value).ToString();
            //        }
            //        else
            //        {
            //            e.Value ="NULL";
            //        } 

            //    }
            //}
        }

        private struct animationFileData
        {
            private List<AnimationCreatorForm.AnimationData> animations;
            private ListBox listbox;

            public int animCount => animations.Count;

            public void addAnimation(AnimationCreatorForm.AnimationData anim)
            {
                listbox.Items.Add(anim.name);
                animations.Add(anim);
            }

            public void Edit(AnimationCreatorForm.AnimationData anim, int index)
            {
                animations[index] = anim;
                listbox.Items[index] = anim.name;
            }

            public AnimationCreatorForm.AnimationData GetAnimation(int index)
            {
                return animations[index];
            }

            public void Init(ListBox _listbox)
            {
                listbox = _listbox;
                animations = new List<AnimationCreatorForm.AnimationData>();
            }

            public void RemoveAnimation()
            {
                int index = listbox.SelectedIndex;
                if (index < 0 || index > listbox.Items.Count)
                    return;
                listbox.Items.RemoveAt(index);
                animations.RemoveAt(index);
            }
            public void Reset()
            {
                listbox.Items.Clear();
                animations.Clear();
            }
        }

        private struct MultiFileData
        {
            public List<string> filePaths;
            private ListBox listBox;

            public void AddFile(string fileName)
            {
                listBox.Items.Add(fileName);
                filePaths.Add(fileName);
            }

            public void Init(ListBox _listBox)
            {
                listBox = _listBox;
                filePaths = new List<string>();
            }

            public void RemoveFile()
            {
                int index = listBox.SelectedIndex;
                if (index < 0 || index > listBox.Items.Count) return;
                listBox.Items.RemoveAt(index);
                filePaths.RemoveAt(index);
            }

            public void Reset()
            {
                listBox.Items.Clear();
                filePaths.Clear();
            }
        }

        private struct SingleFileData
        {
            private string filePath;
            private TextBox textBox;

            public string FilePath
            {
                get => filePath;
                set => filePath = textBox.Text = value;
            }

            public void Init(TextBox _textBox)
            {
                textBox = _textBox;
                FilePath = string.Empty;
            }

            public void Reset() => FilePath = string.Empty;
        }

        private animationFileData anim;
        private MultiFileData audio;
        private SingleFileData bindPose;
        private BaseComponentGroup<BaseComponent> meshes;
        private BaseComponentGroup<BaseComponent> materials;
        private ColliderGroup colliders;

        private List<BaseComponentGroup<BaseComponent>> BaseComponentContainers = new List<BaseComponentGroup<BaseComponent>>();

        public ModelCreatorForm()
        {
            InitializeComponent();
            //Todo: Might as well just make this the constructor instead of a seperate init
            meshes = new BaseComponentGroup<BaseComponent>();
            materials = new BaseComponentGroup<BaseComponent>();
            colliders = new ColliderGroup();
            meshes.Init(dataGridViewMesh);
            materials.Init(dataGridViewMaterial);
            colliders.Init(dataGridViewColliders);
            BaseComponentContainers.Add(meshes);
            BaseComponentContainers.Add(materials);

            //Older system
            //=====================
            bindPose.Init(bindPoseFileName);
            audio.Init(audioListBox);
            anim.Init(animationListBox);
        }

        internal void CreateColliderPressed(ColliderCreatorForm.ColliderData d) => colliders.Add(d);

        internal void CreateColliderPressed(ColliderCreatorForm.ColliderData d, int index) => colliders.SaveEdit(d, index);

        internal void CreateAnimationPressed(AnimationCreatorForm.AnimationData d) => anim.addAnimation(d);

        internal void CreateAnimationPressed(AnimationCreatorForm.AnimationData d, int index) => anim.Edit(d, index);

        //Animation
        private void animationAdd_Click(object sender, EventArgs e)
        {
            //OpenFileDialog open = new OpenFileDialog {
            //    Filter = "Animations (*.anim), (*.fbx)| *.anim; *.fbx;",
            //    InitialDirectory = @"C:\",
            //    Title = "An animation file for this Ghostbait object."
            //};
            //if (open.ShowDialog() == DialogResult.OK) {
            //    if (open.FileName.Substring(open.FileName.Length - 4) == ".fbx") {
            //        string animFile = GenerateRelativeComponentFilePath(open.FileName, ANIMATION);
            //        if (get_animdata_from_scene(open.FileName, animFile) != -1) {
            //            animFile = animFile.Substring(/*Make const*/19);
            //            anim.AddFile(animFile);
            //        }
            //    } else {
            //        anim.AddFile(open.FileName);
            //    }
            //}
            new AnimationCreatorForm(this).Show(this);
        }

        private void animationRemove_Click(object sender, EventArgs e) => anim.RemoveAnimation();

        //Audio
        private void audioAdd_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Audio (*.wav, *.mp3) | *.wav; *.mp3;",
                InitialDirectory = @"C:\",
                Title = "An audio file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                audio.AddFile(open.FileName);
            }
        }

        private void audioRemove_Click(object sender, EventArgs e) => audio.RemoveFile();

        //BindPose
        private void bindPoseFileGrab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "BindPose (*.bind)| *.bind;",
                InitialDirectory = @"C:\",
                Title = "A bindPose file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                bindPose.FilePath = open.FileName;
            }
        }

        private void bindPoseFileRemove_Click(object sender, EventArgs e) => bindPose.Reset();

        //New
        private void blankToolStripMenuItem_Click(object sender, EventArgs e)
        {
            meshes.Reset();
            materials.Reset();
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
                meshes.Reset();
                materials.Reset();
                bindPose.Reset();
                colliders.Reset();
                audio.Reset();
                anim.Reset();

                string meshFile = GenerateRelativeComponentFilePath(open.FileName, MESH);
                string matFile = GenerateRelativeComponentFilePath(open.FileName, MATERIAL);
                string bindPoseFile = GenerateRelativeComponentFilePath(open.FileName, BINDPOSE);
                string animFile = GenerateRelativeComponentFilePath(open.FileName, ANIMATION);

                if (get_mesh_from_scene(open.FileName, meshFile) != -1)
                {
                    meshFile = meshFile.Substring(appendedPath.Length);
                    BaseComponent component = new BaseComponent();
                    component.ComponentIdentifier = meshFile;
                    meshes.Add(component);
                }
                if (get_material_from_scene(open.FileName, matFile) != -1)
                {
                    matFile = matFile.Substring(appendedPath.Length);
                    BaseComponent component = new BaseComponent();
                    component.ComponentIdentifier = matFile;
                    materials.Add(component);
                }
                if (get_bindpose_from_scene(open.FileName, bindPoseFile) != -1)
                {
                    bindPoseFile = bindPoseFile.Substring(appendedPath.Length);
                    bindPose.FilePath = bindPoseFile;
                }
            }
        }

        //Collider
        private void colliderAdd_Click(object sender, EventArgs e) => new ColliderCreatorForm(this).Show(this);

        //TODO : Test to see if we want to put this functionality back in
        //private void colliderListBox_MouseDoubleClick(object sender, MouseEventArgs e)
        //{
        //    int index = colliderListBox.IndexFromPoint(e.Location);
        //    if (index != ListBox.NoMatches)
        //    {
        //        var f = new ColliderCreatorForm(this);
        //        f.Show(this);
        //        f.Edit(colliders.Get(index), index);
        //    }
        //}

        //TODO : Replace with delete box column
        private void colliderRemove_Click(object sender, EventArgs e) => MessageBox.Show("Depreciated!", $@"This function does nothing and should be removed.", MessageBoxButtons.OK, MessageBoxIcon.Error);


        //Material
        private void materialFileGrab_Click(object sender, EventArgs e)
        {
            //Note: Later can make an extended version of OpenFileDialog which has fields for like "tag"
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Materials (*.mat)| *.mat;",
                InitialDirectory = @"C:\",
                Title = "A material file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                BaseComponent component = new BaseComponent();
                component.ComponentIdentifier = open.FileName;
                materials.Add(component);
            }
        }

        private void materialFileRemove_Click(object sender, EventArgs e) => materials.Reset();

        //Mesh
        private void meshFileGrab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Meshes (*.mesh)| *.mesh;",
                InitialDirectory = @"C:\",
                Title = "A mesh file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                BaseComponent component = new BaseComponent();
                component.ComponentIdentifier = open.FileName;
                meshes.Add(component);
            }
        }

        private void meshFileRemove_Click(object sender, EventArgs e) => meshes.Reset();

        //Save/Load
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Ghostbait files (*.ghost) | *.ghost;",
                InitialDirectory = @"C:\",
                Title = "Ghostbait model file"
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                meshes.Reset();
                materials.Reset();
                bindPose.Reset();
                colliders.Reset();
                audio.Reset();
                anim.Reset();

                //Read .ghost file to fill in data
                BinaryReader reader = new BinaryReader(open.OpenFile());

                //Class
                className.Text = new string(reader.ReadChars(reader.ReadInt32()));
                //className.Text = className.Text.Remove(className.Text.Length - 1);

                while (reader.BaseStream.Position != reader.BaseStream.Length)
                {
                    int size = reader.ReadInt32();
                    if (size > 0) //if normal string
                    {
                        string data = new string(reader.ReadChars(size));
                        data = data.Remove(data.Length - 1);
                        if (GetExtension(data).ToLower() == ".mesh")
                        {
                            BaseComponent component = new BaseComponent();
                            component.ComponentIdentifier = data;
                            meshes.Add(component);
                        }
                        else if (GetExtension(data).ToLower() == ".mat")
                        {
                            BaseComponent component = new BaseComponent();
                            component.ComponentIdentifier = data;
                            materials.Add(component);
                        }
                        else if (GetExtension(data).ToLower() == ".bind")
                        {
                            bindPose.FilePath = data;
                        }
                        else if (GetExtension(data).ToLower() == ".mp3" || GetExtension(data).ToLower() == ".wav")
                        {
                            audio.AddFile(data);
                        }
                        else if (GetExtension(data).ToLower() == ".anim")
                        {
                            int len = reader.ReadInt32();
                            String name = new string(reader.ReadChars(len));
                            AnimationCreatorForm.AnimationData toPush = new AnimationCreatorForm.AnimationData();
                            toPush.filePath = data;
                            toPush.name = name;
                            anim.addAnimation(toPush);
                        }
                        continue;
                    }
                    else
                    {
                        string componentName = new string(reader.ReadChars(reader.ReadInt32()));
                        componentName = componentName.Remove(componentName.Length - 1);
                        if (componentName == "Physical")
                        {
                            int colCount = reader.ReadInt32();
                            ColliderCreatorForm.ColliderData colData = new ColliderCreatorForm.ColliderData();
                            for (int i = 0; i < colCount; ++i)
                            {
                                var stringCol = new string(reader.ReadChars(reader.ReadInt32()));
                                stringCol = stringCol.Remove(stringCol.Length - 1);
                                ColliderCreatorForm.ColliderType temp;
                                if (!Enum.TryParse(stringCol, out temp))
                                {
                                    MessageBox.Show("Invalid Collider Type!", $@"The Collider string.Empty{stringCol}string.Empty is not a valid collider.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    return;
                                }
                                colData.ComponentIdentifier = componentName;
                                colData.colliderType = temp;
                                colData.offsetX = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                colData.offsetY = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);
                                colData.offsetZ = BitConverter.ToSingle(reader.ReadBytes(sizeof(float)), 0);

                                switch (colData.colliderType)
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

                                colliders.Add(colData);
                            }
                        }
                        //Is this an error? 
                        else if (componentName == "Animate")
                        {
                            int bindLen = reader.ReadInt32();
                            string bindName = new string(reader.ReadChars(bindLen));
                            bindName = bindName.Remove(bindName.Length - 1);
                            bindPose.FilePath = bindName;
                            int animCount = reader.ReadInt32();

                            for (int i = 0; i < animCount; ++i)
                            {
                                var stringAnim = new string(reader.ReadChars(reader.ReadInt32()));
                                stringAnim = stringAnim.Remove(stringAnim.Length - 1);
                                var stringName = new string(reader.ReadChars(reader.ReadInt32()));
                                stringName = stringName.Remove(stringName.Length - 1);
                                AnimationCreatorForm.AnimationData toPush = new AnimationCreatorForm.AnimationData();
                                toPush.filePath = stringAnim;
                                toPush.name = stringName;
                                anim.addAnimation(toPush);
                            }
                        }
                    }
                }
                reader.Close();
            }
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (className.Text == string.Empty)
            {
                MessageBox.Show(this, "Must have class name.", "Missing Data", MessageBoxButtons.OK);
                return;
            }
            if (anim.animCount > 0 && bindPoseFileName.Text == String.Empty)
            {
                MessageBox.Show(this, "Objects with animations must have a bindpose!", "Missing Data", MessageBoxButtons.OK);
                return;
            }
            SaveFileDialog save = new SaveFileDialog
            {
                Filter = "Ghostbait file (*.ghost)| *.ghost; |Binary file (*.bin)| *.bin;",
                InitialDirectory = @"C:\",
                Title = "Ghostbait model file"
            };
            if (save.ShowDialog() == DialogResult.OK)
            {
                using (BinaryWriter writer = new BinaryWriter(save.OpenFile()))
                {
                    //Class
                    string outstr = className.Text + '\0';
                    writer.Write(outstr.Length);
                    writer.Write(outstr.ToCharArray());

                    // Outputing simple components (Mesh, Material, etc)
                    foreach (var componentContainer in BaseComponentContainers)
                    {
                        if (componentContainer.Count > 0)
                        {
                            foreach (var component in componentContainer.componentList)
                            {
                                outstr = component.ComponentIdentifier + '\0';
                                writer.Write(outstr.Length);
                                writer.Write(outstr.ToCharArray());
                            }
                        }
                    }
                    //BindPose
                    //if (bindPose.FilePath != string.Empty) {
                    //    outstr = bindPose.FilePath + '\0';
                    //    writer.Write(outstr.Length);
                    //    writer.Write(outstr.ToCharArray());
                    //}
                    //Colliders
                    if (colliders.Count > 0)
                    {
                        //Find ColliderDataSize
                        int colliderDataSize = sizeof(Int32);
                        for (int i = 0; i < colliders.Count; ++i)
                        {
                            //Type
                            colliderDataSize += sizeof(Int32) + colliders.Get(i).colliderType.ToString().Length + 1;
                            //Offset
                            colliderDataSize += sizeof(float) * 3;
                            //Custom Data
                            switch (colliders.Get(i).colliderType)
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
                        writer.Write(colliders.Get(0).ComponentIdentifier.Length);
                        writer.Write(colliders.Get(0).ComponentIdentifier.ToCharArray());
                        //Write ColliderData
                        writer.Write(colliders.Count);
                        for (int i = 0; i < colliders.Count; ++i)
                        {
                            //Type
                            var enumString = colliders.Get(i).colliderType.ToString() + '\0';
                            writer.Write(enumString.Length);
                            writer.Write(enumString.ToCharArray());
                            //Offset
                            writer.Write(colliders.Get(i).offsetX);
                            writer.Write(colliders.Get(i).offsetY);
                            writer.Write(colliders.Get(i).offsetZ);
                            //Custom Data
                            switch (colliders.Get(i).colliderType)
                            {
                                case ColliderCreatorForm.ColliderType.SPHERE:
                                    writer.Write(colliders.Get(i).radius);
                                    break;

                                case ColliderCreatorForm.ColliderType.CAPSULE:
                                    writer.Write(colliders.Get(i).radius);
                                    writer.Write(colliders.Get(i).height);
                                    break;

                                case ColliderCreatorForm.ColliderType.BOX:
                                    writer.Write(colliders.Get(i).point1X);
                                    writer.Write(colliders.Get(i).point1Y);
                                    writer.Write(colliders.Get(i).point1Z);
                                    writer.Write(colliders.Get(i).point2X);
                                    writer.Write(colliders.Get(i).point2Y);
                                    writer.Write(colliders.Get(i).point2Z);
                                    break;

                                default: break;
                            }
                        }
                    }
                    //Audio
                    for (int i = 0; i < audio.filePaths.Count; ++i)
                    {
                        outstr = audio.filePaths[i] + '\0';
                        writer.Write(outstr.Length);
                        writer.Write(outstr.ToCharArray());
                    }
                    //Animations
                    if (anim.animCount > 0)
                    {
                        int animDataSize = sizeof(Int32);
                        string animName = "Animate\0";
                        animDataSize += sizeof(Int32) + bindPoseFileName.Text.Length + 1;
                        for (int i = 0; i < anim.animCount; ++i)
                        {
                            animDataSize += sizeof(Int32) + anim.GetAnimation(i).filePath.Length + 1;
                            animDataSize += sizeof(Int32) + anim.GetAnimation(i).name.Length + 1;
                        }
                        //Writer Animation Header
                        writer.Write(-animDataSize);
                        writer.Write(animName.Length);
                        writer.Write(animName.ToCharArray());
                        outstr = bindPose.FilePath + '\0';
                        writer.Write(outstr.Length);
                        writer.Write(outstr.ToCharArray());
                        writer.Write(anim.animCount);
                        for (int i = 0; i < anim.animCount; ++i)
                        {
                            outstr = anim.GetAnimation(i).filePath + '\0';
                            writer.Write(outstr.Length);
                            writer.Write(outstr.ToCharArray());
                            outstr = anim.GetAnimation(i).name + '\0';
                            writer.Write(outstr.Length);
                            writer.Write(outstr.ToCharArray());
                        }
                    }
                    writer.Close();
                }
            }
        }


        private void bindPoseFileName_TextChanged(object sender, EventArgs e)
        {
            bindPose.FilePath = bindPoseFileName.Text;
        }

        private void animationListBox_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            int index = animationListBox.IndexFromPoint(e.Location);
            if (index != ListBox.NoMatches)
            {
                var f = new AnimationCreatorForm(this);
                f.Show(this);
                f.Edit(anim.GetAnimation(index), index);
            }
        }

        private void ModelCreatorForm_Load(object sender, EventArgs e)
        {

        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void colliderListBox_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void dataGridViewColliders_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void groupBox3_Enter(object sender, EventArgs e)
        {

        }
    }
}
