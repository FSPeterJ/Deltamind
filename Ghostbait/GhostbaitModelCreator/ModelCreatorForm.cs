using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using GhostbaitModelCreator.Properties;

namespace GhostbaitModelCreator
{

    public partial class ModelCreatorForm : Form
    {
        // This seems weird
        const string appendedPath = "../../../GhostBait/";


        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_mesh_from_scene(string fbx_file_path, string output_file_path, string name = null);

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_material_from_scene(string fbx_file_path, string output_file_path, string name = null);

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_animdata_from_scene(string fbx_file_path, string output_file_path);

        [DllImport("..\\..\\FBXInterface.dll")]
        public static extern int get_bindpose_from_scene(string fbx_file_path, string output_file_path);

        internal class BaseComponent
        {
            //WHAT THE FUCK - DataGridView can't Bind without get / set.  I am ignorant
            public string ComponentIdentifier { get; set; }
            public string ComponentTag { get; set; }
            public string AbsolutePath { get; set; }

            public virtual MemoryStream GetSpecializedDataBlock()
            {
                throw new NotImplementedException();
            }
        }

        internal class BaseComponentGroup<T>
        {
            public BindingList<T> componentList;
            public DataGridView dataGridView;
            public int Count => componentList.Count;
            protected int blankTagCount = 0;


            protected ModelCreatorForm thisForm;


            public BaseComponentGroup(DataGridView _DataGridView, ModelCreatorForm form)
            {
                dataGridView = _DataGridView;
                thisForm = form;

                componentList = new BindingList<T>();

                dataGridView = _DataGridView;

                dataGridView.AutoGenerateColumns = false;

                dataGridView.AllowUserToResizeColumns = false;
                dataGridView.AllowUserToResizeRows = false;
                dataGridView.AllowUserToOrderColumns = false;

                // We programatically popluate the DataGridView with the columns. 
                // This allows Inhertited versions to extend the columns available or hide if necessary.  
                // It also lets us change the properties for all the grids ONCE following DRY better.
                // Unless there is a way to share properties across controls and make changes to them in desgin only once...
                // Would be nice to learn this if that's possible, but techincally irrelevant for me, just convienent

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
                EditButton.Visible = false;
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

                dataGridView.CellContentClick += CellContentClick;
                dataGridView.CellFormatting += CellFormat;

                dataGridView.DataSource = componentList;
            }

            //Standard interface functions
            //========================================

            public void Add(T comp)
            {
                componentList.Add(comp);
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

            public virtual void OpenEdit(int index)
            {
            }

            private void CellContentClick(object sender, DataGridViewCellEventArgs e)
            {
                var senderGrid = (DataGridView)sender;
                if (senderGrid.Columns[e.ColumnIndex] is DataGridViewButtonColumn &&
                    e.RowIndex >= 0)
                {
                    if (senderGrid.Columns[e.ColumnIndex].Name == "DeleteButton")
                    {
                        Remove(e.RowIndex);
                    }
                    else if (senderGrid.Columns[e.ColumnIndex].Name == "EditButton")
                    {
                        OpenEdit(e.RowIndex);
                    }
                }
            }

            public void TurnOffCellFormat()
            {
                dataGridView.CellFormatting -= CellFormat;
            }
            protected void CellFormat(object sender, DataGridViewCellFormattingEventArgs e)
            {
                var senderGrid = (DataGridView)sender;

                if (senderGrid.Columns[e.ColumnIndex].Name == "ComponentTag")
                {
                    if (e.RowIndex == 0)
                    {
                        blankTagCount = 0;
                    }
                    if (senderGrid.Rows[e.RowIndex].IsNewRow)
                    {
                        e.CellStyle.BackColor = Color.Gray;
                    }
                    else if (e.Value == null)
                    {
                        ++blankTagCount;
                        if (blankTagCount > 1)
                        {
                            e.CellStyle.BackColor = Color.Red;
                        }
                        else
                        {
                            e.Value = "(default)";
                            e.CellStyle.ForeColor = Color.LightGray;
                        }
                    }
                    else
                    {
                        e.CellStyle.ForeColor = Color.Black;
                    }
                }
            }

            public void Reset()
            {
                componentList.Clear();
            }
        }

        internal class ColliderGroup : BaseComponentGroup<ColliderCreatorForm.ColliderData>
        {
            public ColliderGroup(DataGridView _DataGridView, ModelCreatorForm form) : base(_DataGridView, form)
            {
                dataGridView.AllowUserToAddRows = false;
                //They all say "Physical" anyways
                dataGridView.Columns["ComponentIdentifier"].Visible = false;
                dataGridView.Columns["EditButton"].Visible = true;
                dataGridView.Columns["ComponentTag"].Visible = false;
                dataGridView.CellFormatting -= CellFormat;

                DataGridViewTextBoxColumn ComponentData = new DataGridViewTextBoxColumn();
                ComponentData.Name = "colliderType";
                ComponentData.HeaderText = "Data";
                ComponentData.DataPropertyName = "colliderType";
                ComponentData.AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
                dataGridView.Columns.Add(ComponentData);
                ComponentData.DisplayIndex = 0;  // Shoves the other columns over so "I am first"
            }

            public override void OpenEdit(int index)
            {

                var f = new ColliderCreatorForm(thisForm);
                f.Show(thisForm);
                f.Edit(Get(index), index);
            }
        }

        internal class AnimationGroup : BaseComponentGroup<AnimationCreatorForm.AnimationData>
        {
            public AnimationGroup(DataGridView _DataGridView, ModelCreatorForm form) : base(_DataGridView, form)
            {
                dataGridView.AllowUserToAddRows = false;
                dataGridView.Columns["ComponentIdentifier"].Visible = true;
                dataGridView.Columns["EditButton"].Visible = true;
                dataGridView.Columns["ComponentTag"].Visible = true;

            }

            public override void OpenEdit(int index)
            {

                var f = new AnimationCreatorForm(thisForm);
                f.Show(thisForm);
                f.Edit(Get(index), index);
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

        private SingleFileData bindPose;
        private BaseComponentGroup<BaseComponent> audio;
        private BaseComponentGroup<BaseComponent> meshes;
        private BaseComponentGroup<BaseComponent> materials;
        private BaseComponentGroup<BaseComponent> childObjects;
        private ColliderGroup colliders;
        private AnimationGroup animations;

        private List<BaseComponentGroup<BaseComponent>> BaseComponentContainers = new List<BaseComponentGroup<BaseComponent>>();

        public ModelCreatorForm()
        {
            InitializeComponent();
            meshes = new BaseComponentGroup<BaseComponent>(dataGridViewMesh, this);
            materials = new BaseComponentGroup<BaseComponent>(dataGridViewMaterial, this);
            colliders = new ColliderGroup(dataGridViewColliders, this);
            animations = new AnimationGroup(dataGridViewAnimation, this);
            childObjects = new BaseComponentGroup<BaseComponent>(dataGridViewChildObjects, this);
            audio = new BaseComponentGroup<BaseComponent>(dataGridViewAudio, this);

            childObjects.TurnOffCellFormat();

            //--------------
            // ONLY ADD ITEMS WITH NO EXTRA DATA
            //===========================================
            BaseComponentContainers.Add(meshes);
            BaseComponentContainers.Add(materials);
            BaseComponentContainers.Add(childObjects);
            BaseComponentContainers.Add(audio);

            // Older system
            //=====================
            bindPose.Init(bindPoseFileName);
        }

        internal void CreateColliderPressed(ColliderCreatorForm.ColliderData d) => colliders.Add(d);

        internal void CreateColliderPressed(ColliderCreatorForm.ColliderData d, int index) => colliders.SaveEdit(d, index);

        internal void CreateAnimationPressed(AnimationCreatorForm.AnimationData d) => animations.Add(d);

        internal void CreateAnimationPressed(AnimationCreatorForm.AnimationData d, int index) => animations.SaveEdit(d, index);

        //Collider
        private void colliderAdd_Click(object sender, EventArgs e) => new ColliderCreatorForm(this).Show(this);

        //Animation
        private void animationAdd_Click(object sender, EventArgs e)
        {

            new AnimationCreatorForm(this).Show(this);
        }

        //Audio
        private void audioAdd_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Audio (*.wav, *.mp3) | *.wav; *.mp3;",
                InitialDirectory = Settings.Default.audio_path,
                Title = "An audio file for this Ghostbait object."

            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.audio_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                BaseComponent component = new BaseComponent();
                component.ComponentIdentifier = Path.GetFileName(open.FileName);
                component.AbsolutePath = open.FileName;
                audio.Add(component);
            }
        }

        private void ResetAll()
        {
            meshes.Reset();
            materials.Reset();
            bindPose.Reset();
            colliders.Reset();
            audio.Reset();
            animations.Reset();
            childObjects.Reset();
        }

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
            ResetAll();
            className.Text = string.Empty;
        }

        private void fromFBXToolStripMenuItem_Click(object sender, EventArgs e)
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
                Settings.Default.fbx_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                ResetAll();

                string baseName = Path.GetFileNameWithoutExtension(open.FileName);
                string path = Path.GetDirectoryName(open.FileName);
                string meshFile = path + "/" + baseName + ".mesh";
                string matFile = path + "/" + baseName + ".mat";
                string bindPoseFile = path + "/" + baseName + ".bind";
                string animFile = path + "/" + baseName + ".anim";

                if (get_mesh_from_scene(open.FileName, meshFile) != -1)
                {
                    BaseComponent component = new BaseComponent();
                    component.AbsolutePath = meshFile;
                    component.ComponentIdentifier = Path.GetFileName(meshFile);
                    meshes.Add(component);
                }
                if (get_material_from_scene(open.FileName, matFile) != -1)
                {
                    BaseComponent component = new BaseComponent();
                    component.AbsolutePath = matFile;
                    component.ComponentIdentifier = Path.GetFileName(matFile);
                    materials.Add(component);
                }
                if (get_bindpose_from_scene(open.FileName, bindPoseFile) != -1)
                {
                    //Depreciated?
                    bindPose.FilePath = Path.GetFileName(bindPoseFile);
                }
                if (get_animdata_from_scene(open.FileName, animFile) != -1)
                {
                    AnimationCreatorForm.AnimationData component = new AnimationCreatorForm.AnimationData();
                    component.AbsolutePath = animFile;
                    component.ComponentIdentifier = Path.GetFileName(animFile);
                    animations.Add(component);
                }
            }
        }



        //Material
        private void materialFileGrab_Click(object sender, EventArgs e)
        {
            //Note: Later can make an extended version of OpenFileDialog which has fields for like "tag"
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Materials (*.mat)| *.mat;",
                InitialDirectory = Settings.Default.material_path,
                Title = "A material file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.material_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                BaseComponent component = new BaseComponent();
                component.ComponentIdentifier = Path.GetFileName(open.FileName);
                component.AbsolutePath = open.FileName;
                materials.Add(component);
            }
        }


        //Mesh
        private void meshFileGrab_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Meshes (*.mesh)| *.mesh;",
                InitialDirectory = Settings.Default.mesh_path,
                Title = "A mesh file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.mesh_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                BaseComponent component = new BaseComponent();
                component.ComponentIdentifier = Path.GetFileName(open.FileName);
                component.AbsolutePath = open.FileName;
                meshes.Add(component);
            }
        }

        //Save/Load
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Ghostbait files (*.ghost) | *.ghost;",
                InitialDirectory = Settings.Default.ghost_path,
                Title = "Ghostbait model file"
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.ghost_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                ResetAll();

                //Read .ghost file to fill in data
                BinaryReader reader = new BinaryReader(open.OpenFile());

                //Class
                className.Text = new string(reader.ReadChars(reader.ReadInt32()));

                while (reader.BaseStream.Position != reader.BaseStream.Length)
                {
                    int tagsize = reader.ReadInt32();
                    if (tagsize > 0) //if normal string
                    {
                        BaseComponent component = new BaseComponent();
                        string data = new string(reader.ReadChars(tagsize));
                        data = data.Trim('\0');
                        if (Path.GetExtension(data).ToLower() == ".mesh")
                        {
                            component.ComponentIdentifier = data;
                            meshes.Add(component);
                        }
                        else if (Path.GetExtension(data).ToLower() == ".mat")
                        {
                            component.ComponentIdentifier = data;
                            materials.Add(component);
                        }
                        else if (Path.GetExtension(data).ToLower() == ".ghost")
                        {
                            component.ComponentIdentifier = data;
                            childObjects.Add(component);
                        }
                        else if (Path.GetExtension(data).ToLower() == ".bind")
                        {
                            bindPose.FilePath = data;
                        }
                        else if (Path.GetExtension(data).ToLower() == ".mp3" || Path.GetExtension(data).ToLower() == ".wav")
                        {
                            component.ComponentIdentifier = data;
                            audio.Add(component);
                        }
                        else if (Path.GetExtension(data).ToLower() == ".anim")
                        {
                            component = new AnimationCreatorForm.AnimationData();
                            component.ComponentIdentifier = data;
                            component.AbsolutePath = Path.GetPathRoot(open.FileName) + data; // I am making an assumption about pathing here
                            animations.Add((AnimationCreatorForm.AnimationData)component);
                        }
                        //Getting the tag (if any)
                        tagsize = reader.ReadInt32();
                        if (tagsize > 0)
                        {
                            string tag = new string(reader.ReadChars(tagsize));
                            component.ComponentTag = tag;
                        }
                    }
                    else
                    {
                        string componentName = new string(reader.ReadChars(-tagsize));
                        componentName = componentName.Trim('\0');
                        if (componentName == "Physical")
                        {
                            int tagleng = reader.ReadInt32();
                            string tagstringg = null;
                            if (tagleng > 0)
                            {
                                tagstringg = new string(reader.ReadChars(tagleng));
                                tagstringg = tagstringg.Trim('\0');
                            }
                            int byteCount = reader.ReadInt32();
                            radioButtonStatic.Checked = reader.ReadBoolean();
                            radioButtonDynamic.Checked = !radioButtonStatic.Checked;
                            int colCount = reader.ReadInt32();
                            for (int i = 0; i < colCount; ++i)
                            {
                                ColliderCreatorForm.ColliderData colData = new ColliderCreatorForm.ColliderData();
                                // !!! Problem!  The first tag is written to all colliders.  This is due to a confusion between colliders / Physicals where
                                // the actual system should have colliders be a subcomponent of a Physical.  There is currently no fix for this planned but it can be done.
                                // The current system doesn't actually implement tags for colliders in the form ui, so technically a non-issue until it becomes one
                                colData.ComponentTag = tagstringg;
                                var stringCol = new string(reader.ReadChars(reader.ReadInt32()));
                                stringCol = stringCol.Trim('\0');
                                ColliderCreatorForm.ColliderType temp;
                                if (!Enum.TryParse(stringCol, out temp))
                                {
                                    MessageBox.Show("Invalid Collider Type!", $@"The Collider string.Empty{stringCol}string.Empty is not a valid collider.", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                    return;
                                }
                                colData.ComponentIdentifier = componentName;
                                colData.colliderType = temp;
                                colData.isTrigger = reader.ReadBoolean();
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
                        // We look at anims up top but there's an Animate tag down here.  I do not fully understand our system for this.
                        else if (componentName == "Animate")
                        {
                            tagsize = reader.ReadInt32();
                            if (tagsize > 0)
                            {
                                string tag = new string(reader.ReadChars(tagsize));
                            }
                            int Datablocksize = reader.ReadInt32();

                            int bindLen = reader.ReadInt32();
                            string bindName = new string(reader.ReadChars(bindLen));
                            bindName = bindName.Trim('\0');

                            bindPose.FilePath = bindName;
                            int animCount = reader.ReadInt32();

                            for (int i = 0; i < animCount; ++i)
                            {
                                var stringAnim = new string(reader.ReadChars(reader.ReadInt32()));
                                stringAnim = stringAnim.Trim('\0');
                                var stringName = new string(reader.ReadChars(reader.ReadInt32()));
                                stringName = stringName.Trim('\0');

                                AnimationCreatorForm.AnimationData toPush = new AnimationCreatorForm.AnimationData();
                                toPush.ComponentIdentifier = stringAnim;
                                //toPush.AbsolutePath = ; // I am making an assumption about pathing here
                                //toPush.ComponentTag = tag;
                                animations.Add(toPush);
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
                MessageBox.Show(this, "Must have a c++ object class name or alias.", "Missing Data", MessageBoxButtons.OK);
                return;
            }
            //Shouldn't Bind pose data be bound within the animation stage 0?
            if (animations.Count > 0 && bindPoseFileName.Text == String.Empty)
            {
                MessageBox.Show(this, "Objects with animations must have a bindpose!", "Missing Data", MessageBoxButtons.OK);
                return;
            }

            for (int i = 0; i < animations.Count; i++)
            {
                if (animations.Get(i).ComponentTag == null)
                {
                    MessageBox.Show(this, "Animations must have tags!", "Missing Data", MessageBoxButtons.OK);

                    return;
                }
            }

            SaveFileDialog save = new SaveFileDialog
            {
                Filter = "Ghostbait file (*.ghost)| *.ghost; |Binary file (*.bin)| *.bin;",
                InitialDirectory = Settings.Default.ghost_path,
                Title = "Ghostbait model file"
            };
            if (save.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.ghost_path = Path.GetDirectoryName(save.FileName);
                Settings.Default.Save();
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
                                if (string.IsNullOrWhiteSpace(component.ComponentTag))
                                {
                                    writer.Write(0);
                                }
                                else
                                {
                                    outstr = component.ComponentTag + '\0';
                                    writer.Write(outstr.Length);
                                    writer.Write(outstr.ToCharArray());
                                }
                            }
                        }
                    }
                    //TODO : Investigate usage
                    //BindPose
                    //if (bindPose.FilePath != string.Empty) {
                    //    outstr = bindPose.FilePath + '\0';
                    //    writer.Write(outstr.Length);
                    //    writer.Write(outstr.ToCharArray());
                    //}
                    //Colliders
                    if (colliders.Count > 0)
                    {

                        //Writer Collider Header
                        outstr = colliders.Get(0).ComponentIdentifier + '\0';
                        writer.Write(-outstr.Length);
                        writer.Write(outstr.ToCharArray());
                        if (string.IsNullOrWhiteSpace(colliders.Get(0).ComponentTag))
                        {
                            writer.Write(0);
                        }
                        else
                        {
                            outstr = colliders.Get(0).ComponentTag + '\0';
                            writer.Write(outstr.Length);
                            writer.Write(outstr.ToCharArray());
                        }

                        //Investigate a way to prevent double looping.
                        MemoryStream[] specializedData = new MemoryStream[colliders.Count];
                        int specialdatasize = 0;
                        for (int i = 0; i < colliders.Count; ++i)
                        {
                            specializedData[i] = colliders.Get(i).GetSpecializedDataBlock();
                            specialdatasize += (int)specializedData[i].Length;
                        }

                        specialdatasize += sizeof(int) + sizeof(bool);
                        writer.Write(specialdatasize);
                        writer.Write(radioButtonStatic.Checked);
                        writer.Write(colliders.Count); //This line of data prevents generlization.  Investigate possible alternatives
                        //Write ColliderData
                        for (int i = 0; i < colliders.Count; ++i)
                        {
                            writer.Write(specializedData[i].ToArray());
                        }
                    }
                    //Audio
                    //TODO : Fix Later
                    for (int i = 0; i < audio.Count; ++i)
                    {
                        outstr = audio.Get(i).ComponentIdentifier + '\0';
                        writer.Write(outstr.Length);
                        writer.Write(outstr.ToCharArray());

                    }
                    //Animations
                    if (animations.Count > 0)
                    {
                        int animDataSize = sizeof(Int32);
                        string animName = "Animate\0";



                        //This is an error?
                        animDataSize += sizeof(Int32) + bindPoseFileName.Text.Length + 1;
                        for (int i = 0; i < animations.Count; ++i)
                        {
                            animDataSize += sizeof(Int32) + animations.Get(i).ComponentIdentifier.Length + 1;
                            animDataSize += sizeof(Int32) + animations.Get(i).ComponentTag.Length + 1;
                        }
                        //Writer Animation Header
                        writer.Write(-animName.Length);
                        writer.Write(animName.ToCharArray());
                        writer.Write(0); // Until a new pass of this system is put in for component components, just leave it like this for now.

                        writer.Write(animDataSize);
                        outstr = bindPose.FilePath + '\0';
                        writer.Write(outstr.Length);
                        writer.Write(outstr.ToCharArray());
                        writer.Write(animations.Count);
                        for (int i = 0; i < animations.Count; ++i)
                        {
                            outstr = animations.Get(i).ComponentIdentifier + '\0';
                            writer.Write(outstr.Length);
                            writer.Write(outstr.ToCharArray());
                            outstr = animations.Get(i).ComponentTag + '\0';
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

        private void ChildObjectsAdd_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Ghostbait File (*.ghost) | *.ghost;",
                InitialDirectory = Settings.Default.ghost_path,
                Title = "An Object file for this Ghostbait Engine."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.ghost_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                BaseComponent component = new BaseComponent();
                component.ComponentIdentifier = Path.GetFileName(open.FileName);
                component.AbsolutePath = open.FileName;
                childObjects.Add(component);
            }
        }

    }
}
