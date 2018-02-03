namespace GhostbaitModelCreator
{
    partial class ModelCreatorForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ModelCreatorForm));
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.className = new System.Windows.Forms.TextBox();
            this.meshFileRemove = new System.Windows.Forms.Button();
            this.meshFileGrab = new System.Windows.Forms.Button();
            this.meshFileName = new System.Windows.Forms.TextBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.dataGridViewColliders = new System.Windows.Forms.DataGridView();
            this.colliderAdd = new System.Windows.Forms.Button();
            this.colliderListBox = new System.Windows.Forms.ListBox();
            this.colliderRemove = new System.Windows.Forms.Button();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.audioAdd = new System.Windows.Forms.Button();
            this.audioListBox = new System.Windows.Forms.ListBox();
            this.audioRemove = new System.Windows.Forms.Button();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.bindPoseFileRemove = new System.Windows.Forms.Button();
            this.bindPoseFileName = new System.Windows.Forms.TextBox();
            this.bindPoseFileGrab = new System.Windows.Forms.Button();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.dataGridViewMaterial = new System.Windows.Forms.DataGridView();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewButtonColumn1 = new System.Windows.Forms.DataGridViewButtonColumn();
            this.materialFileRemove = new System.Windows.Forms.Button();
            this.materialFileName = new System.Windows.Forms.TextBox();
            this.materialFileGrab = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.dataGridViewMesh = new System.Windows.Forms.DataGridView();
            this.Filepath = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.TagName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.ColumnDeleteRowMesh = new System.Windows.Forms.DataGridViewButtonColumn();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.animationAdd = new System.Windows.Forms.Button();
            this.animationListBox = new System.Windows.Forms.ListBox();
            this.animationRemove = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.blankToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromFBXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.Component = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.DataName = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Edit = new System.Windows.Forms.DataGridViewImageColumn();
            this.Delete = new System.Windows.Forms.DataGridViewButtonColumn();
            this.colliderBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.colliderCreatorFormBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.programBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.groupBox2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.groupBox7.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewColliders)).BeginInit();
            this.groupBox6.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMaterial)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMesh)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderCreatorFormBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.programBindingSource)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.className);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(305, 56);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Class";
            // 
            // className
            // 
            this.className.Location = new System.Drawing.Point(6, 23);
            this.className.Name = "className";
            this.className.Size = new System.Drawing.Size(188, 20);
            this.className.TabIndex = 3;
            // 
            // meshFileRemove
            // 
            this.meshFileRemove.Location = new System.Drawing.Point(170, 23);
            this.meshFileRemove.Name = "meshFileRemove";
            this.meshFileRemove.Size = new System.Drawing.Size(24, 20);
            this.meshFileRemove.TabIndex = 2;
            this.meshFileRemove.Text = "X";
            this.meshFileRemove.UseVisualStyleBackColor = true;
            this.meshFileRemove.Click += new System.EventHandler(this.meshFileRemove_Click);
            // 
            // meshFileGrab
            // 
            this.meshFileGrab.Location = new System.Drawing.Point(140, 23);
            this.meshFileGrab.Name = "meshFileGrab";
            this.meshFileGrab.Size = new System.Drawing.Size(24, 20);
            this.meshFileGrab.TabIndex = 1;
            this.meshFileGrab.Text = "...";
            this.meshFileGrab.UseVisualStyleBackColor = true;
            this.meshFileGrab.Click += new System.EventHandler(this.meshFileGrab_Click);
            // 
            // meshFileName
            // 
            this.meshFileName.Location = new System.Drawing.Point(6, 23);
            this.meshFileName.Name = "meshFileName";
            this.meshFileName.Size = new System.Drawing.Size(128, 20);
            this.meshFileName.TabIndex = 0;
            this.meshFileName.TextChanged += new System.EventHandler(this.meshFileName_TextChanged);
            // 
            // panel1
            // 
            this.panel1.AutoScroll = true;
            this.panel1.Controls.Add(this.groupBox7);
            this.panel1.Controls.Add(this.groupBox6);
            this.panel1.Controls.Add(this.groupBox5);
            this.panel1.Controls.Add(this.groupBox4);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox3);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(562, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(322, 637);
            this.panel1.TabIndex = 5;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.dataGridViewColliders);
            this.groupBox7.Controls.Add(this.colliderAdd);
            this.groupBox7.Controls.Add(this.colliderListBox);
            this.groupBox7.Controls.Add(this.colliderRemove);
            this.groupBox7.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.groupBox7.Location = new System.Drawing.Point(0, 559);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(305, 320);
            this.groupBox7.TabIndex = 8;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Colliders";
            // 
            // dataGridViewColliders
            // 
            this.dataGridViewColliders.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridViewColliders.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Component,
            this.DataName,
            this.dataGridViewTextBoxColumn4,
            this.Edit,
            this.Delete});
            this.dataGridViewColliders.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dataGridViewColliders.Location = new System.Drawing.Point(3, 145);
            this.dataGridViewColliders.Name = "dataGridViewColliders";
            this.dataGridViewColliders.RowHeadersVisible = false;
            this.dataGridViewColliders.Size = new System.Drawing.Size(299, 172);
            this.dataGridViewColliders.TabIndex = 10;
            this.dataGridViewColliders.CellContentClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewColliders_CellContentClick);
            // 
            // colliderAdd
            // 
            this.colliderAdd.Location = new System.Drawing.Point(3, 118);
            this.colliderAdd.Name = "colliderAdd";
            this.colliderAdd.Size = new System.Drawing.Size(84, 22);
            this.colliderAdd.TabIndex = 4;
            this.colliderAdd.Text = "Add";
            this.colliderAdd.UseVisualStyleBackColor = true;
            this.colliderAdd.Click += new System.EventHandler(this.colliderAdd_Click);
            // 
            // colliderListBox
            // 
            this.colliderListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.colliderListBox.FormattingEnabled = true;
            this.colliderListBox.Location = new System.Drawing.Point(3, 16);
            this.colliderListBox.Name = "colliderListBox";
            this.colliderListBox.Size = new System.Drawing.Size(299, 95);
            this.colliderListBox.TabIndex = 3;
            this.colliderListBox.SelectedIndexChanged += new System.EventHandler(this.colliderListBox_SelectedIndexChanged);
            this.colliderListBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.colliderListBox_MouseDoubleClick);
            // 
            // colliderRemove
            // 
            this.colliderRemove.Location = new System.Drawing.Point(113, 118);
            this.colliderRemove.Name = "colliderRemove";
            this.colliderRemove.Size = new System.Drawing.Size(84, 22);
            this.colliderRemove.TabIndex = 2;
            this.colliderRemove.Text = "Remove";
            this.colliderRemove.UseVisualStyleBackColor = true;
            this.colliderRemove.Click += new System.EventHandler(this.colliderRemove_Click);
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.audioAdd);
            this.groupBox6.Controls.Add(this.audioListBox);
            this.groupBox6.Controls.Add(this.audioRemove);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.groupBox6.Location = new System.Drawing.Point(0, 879);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(305, 149);
            this.groupBox6.TabIndex = 7;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Audio";
            // 
            // audioAdd
            // 
            this.audioAdd.Location = new System.Drawing.Point(3, 118);
            this.audioAdd.Name = "audioAdd";
            this.audioAdd.Size = new System.Drawing.Size(84, 22);
            this.audioAdd.TabIndex = 4;
            this.audioAdd.Text = "Add";
            this.audioAdd.UseVisualStyleBackColor = true;
            this.audioAdd.Click += new System.EventHandler(this.audioAdd_Click);
            // 
            // audioListBox
            // 
            this.audioListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.audioListBox.FormattingEnabled = true;
            this.audioListBox.Location = new System.Drawing.Point(3, 16);
            this.audioListBox.Name = "audioListBox";
            this.audioListBox.Size = new System.Drawing.Size(299, 95);
            this.audioListBox.TabIndex = 3;
            // 
            // audioRemove
            // 
            this.audioRemove.Location = new System.Drawing.Point(113, 118);
            this.audioRemove.Name = "audioRemove";
            this.audioRemove.Size = new System.Drawing.Size(84, 22);
            this.audioRemove.TabIndex = 2;
            this.audioRemove.Text = "Remove";
            this.audioRemove.UseVisualStyleBackColor = true;
            this.audioRemove.Click += new System.EventHandler(this.audioRemove_Click);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.bindPoseFileRemove);
            this.groupBox5.Controls.Add(this.bindPoseFileName);
            this.groupBox5.Controls.Add(this.bindPoseFileGrab);
            this.groupBox5.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox5.Location = new System.Drawing.Point(0, 503);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(305, 56);
            this.groupBox5.TabIndex = 6;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Bind Pose";
            // 
            // bindPoseFileRemove
            // 
            this.bindPoseFileRemove.Location = new System.Drawing.Point(170, 23);
            this.bindPoseFileRemove.Name = "bindPoseFileRemove";
            this.bindPoseFileRemove.Size = new System.Drawing.Size(24, 20);
            this.bindPoseFileRemove.TabIndex = 2;
            this.bindPoseFileRemove.Text = "X";
            this.bindPoseFileRemove.UseVisualStyleBackColor = true;
            this.bindPoseFileRemove.Click += new System.EventHandler(this.bindPoseFileRemove_Click);
            // 
            // bindPoseFileName
            // 
            this.bindPoseFileName.Location = new System.Drawing.Point(6, 23);
            this.bindPoseFileName.Name = "bindPoseFileName";
            this.bindPoseFileName.Size = new System.Drawing.Size(128, 20);
            this.bindPoseFileName.TabIndex = 0;
            this.bindPoseFileName.TextChanged += new System.EventHandler(this.bindPoseFileName_TextChanged);
            // 
            // bindPoseFileGrab
            // 
            this.bindPoseFileGrab.Location = new System.Drawing.Point(140, 23);
            this.bindPoseFileGrab.Name = "bindPoseFileGrab";
            this.bindPoseFileGrab.Size = new System.Drawing.Size(24, 20);
            this.bindPoseFileGrab.TabIndex = 1;
            this.bindPoseFileGrab.Text = "...";
            this.bindPoseFileGrab.UseVisualStyleBackColor = true;
            this.bindPoseFileGrab.Click += new System.EventHandler(this.bindPoseFileGrab_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.dataGridViewMaterial);
            this.groupBox4.Controls.Add(this.materialFileRemove);
            this.groupBox4.Controls.Add(this.materialFileName);
            this.groupBox4.Controls.Add(this.materialFileGrab);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox4.Location = new System.Drawing.Point(0, 279);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(305, 224);
            this.groupBox4.TabIndex = 5;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Material";
            // 
            // dataGridViewMaterial
            // 
            this.dataGridViewMaterial.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridViewMaterial.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewButtonColumn1});
            this.dataGridViewMaterial.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dataGridViewMaterial.Location = new System.Drawing.Point(3, 49);
            this.dataGridViewMaterial.Name = "dataGridViewMaterial";
            this.dataGridViewMaterial.RowHeadersVisible = false;
            this.dataGridViewMaterial.Size = new System.Drawing.Size(299, 172);
            this.dataGridViewMaterial.TabIndex = 9;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.dataGridViewTextBoxColumn1.HeaderText = "Filepath";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "Tag Name";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            // 
            // dataGridViewButtonColumn1
            // 
            this.dataGridViewButtonColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.dataGridViewButtonColumn1.HeaderText = "";
            this.dataGridViewButtonColumn1.Name = "dataGridViewButtonColumn1";
            this.dataGridViewButtonColumn1.Width = 20;
            // 
            // materialFileRemove
            // 
            this.materialFileRemove.Location = new System.Drawing.Point(170, 23);
            this.materialFileRemove.Name = "materialFileRemove";
            this.materialFileRemove.Size = new System.Drawing.Size(24, 20);
            this.materialFileRemove.TabIndex = 2;
            this.materialFileRemove.Text = "X";
            this.materialFileRemove.UseVisualStyleBackColor = true;
            this.materialFileRemove.Click += new System.EventHandler(this.materialFileRemove_Click);
            // 
            // materialFileName
            // 
            this.materialFileName.Location = new System.Drawing.Point(6, 23);
            this.materialFileName.Name = "materialFileName";
            this.materialFileName.Size = new System.Drawing.Size(128, 20);
            this.materialFileName.TabIndex = 0;
            this.materialFileName.TextChanged += new System.EventHandler(this.materialFileName_TextChanged);
            // 
            // materialFileGrab
            // 
            this.materialFileGrab.Location = new System.Drawing.Point(140, 23);
            this.materialFileGrab.Name = "materialFileGrab";
            this.materialFileGrab.Size = new System.Drawing.Size(24, 20);
            this.materialFileGrab.TabIndex = 1;
            this.materialFileGrab.Text = "...";
            this.materialFileGrab.UseVisualStyleBackColor = true;
            this.materialFileGrab.Click += new System.EventHandler(this.materialFileGrab_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.dataGridViewMesh);
            this.groupBox1.Controls.Add(this.meshFileRemove);
            this.groupBox1.Controls.Add(this.meshFileName);
            this.groupBox1.Controls.Add(this.meshFileGrab);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 56);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(305, 223);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Mesh";
            // 
            // dataGridViewMesh
            // 
            this.dataGridViewMesh.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridViewMesh.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Filepath,
            this.TagName,
            this.ColumnDeleteRowMesh});
            this.dataGridViewMesh.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.dataGridViewMesh.Location = new System.Drawing.Point(3, 48);
            this.dataGridViewMesh.Name = "dataGridViewMesh";
            this.dataGridViewMesh.RowHeadersVisible = false;
            this.dataGridViewMesh.Size = new System.Drawing.Size(299, 172);
            this.dataGridViewMesh.TabIndex = 8;
            // 
            // Filepath
            // 
            this.Filepath.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Filepath.HeaderText = "Filepath";
            this.Filepath.Name = "Filepath";
            // 
            // TagName
            // 
            this.TagName.HeaderText = "Tag Name";
            this.TagName.Name = "TagName";
            // 
            // ColumnDeleteRowMesh
            // 
            this.ColumnDeleteRowMesh.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.ColumnDeleteRowMesh.HeaderText = "";
            this.ColumnDeleteRowMesh.Name = "ColumnDeleteRowMesh";
            this.ColumnDeleteRowMesh.Width = 20;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.animationAdd);
            this.groupBox3.Controls.Add(this.animationListBox);
            this.groupBox3.Controls.Add(this.animationRemove);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.groupBox3.Location = new System.Drawing.Point(0, 1028);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(305, 151);
            this.groupBox3.TabIndex = 4;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Animation";
            // 
            // animationAdd
            // 
            this.animationAdd.Location = new System.Drawing.Point(3, 118);
            this.animationAdd.Name = "animationAdd";
            this.animationAdd.Size = new System.Drawing.Size(84, 22);
            this.animationAdd.TabIndex = 4;
            this.animationAdd.Text = "Add";
            this.animationAdd.UseVisualStyleBackColor = true;
            this.animationAdd.Click += new System.EventHandler(this.animationAdd_Click);
            // 
            // animationListBox
            // 
            this.animationListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.animationListBox.FormattingEnabled = true;
            this.animationListBox.Location = new System.Drawing.Point(3, 16);
            this.animationListBox.Name = "animationListBox";
            this.animationListBox.Size = new System.Drawing.Size(299, 95);
            this.animationListBox.TabIndex = 3;
            this.animationListBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.animationListBox_MouseDoubleClick);
            // 
            // animationRemove
            // 
            this.animationRemove.Location = new System.Drawing.Point(113, 118);
            this.animationRemove.Name = "animationRemove";
            this.animationRemove.Size = new System.Drawing.Size(84, 22);
            this.animationRemove.TabIndex = 2;
            this.animationRemove.Text = "Remove";
            this.animationRemove.UseVisualStyleBackColor = true;
            this.animationRemove.Click += new System.EventHandler(this.animationRemove_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(884, 24);
            this.menuStrip1.TabIndex = 6;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.ItemClicked += new System.Windows.Forms.ToolStripItemClickedEventHandler(this.menuStrip1_ItemClicked);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.blankToolStripMenuItem,
            this.fromFBXToolStripMenuItem});
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.newToolStripMenuItem.Text = "New";
            // 
            // blankToolStripMenuItem
            // 
            this.blankToolStripMenuItem.Name = "blankToolStripMenuItem";
            this.blankToolStripMenuItem.Size = new System.Drawing.Size(128, 22);
            this.blankToolStripMenuItem.Text = "Blank";
            this.blankToolStripMenuItem.Click += new System.EventHandler(this.blankToolStripMenuItem_Click);
            // 
            // fromFBXToolStripMenuItem
            // 
            this.fromFBXToolStripMenuItem.Name = "fromFBXToolStripMenuItem";
            this.fromFBXToolStripMenuItem.Size = new System.Drawing.Size(128, 22);
            this.fromFBXToolStripMenuItem.Text = "From .FBX";
            this.fromFBXToolStripMenuItem.Click += new System.EventHandler(this.fromFBXToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(103, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::GhostbaitModelCreator.Properties.Resources.Ghostbait;
            this.pictureBox1.Location = new System.Drawing.Point(524, 27);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(32, 36);
            this.pictureBox1.TabIndex = 7;
            this.pictureBox1.TabStop = false;
            // 
            // Component
            // 
            this.Component.HeaderText = "ComponentType";
            this.Component.Name = "Component";
            this.Component.Visible = false;
            // 
            // DataName
            // 
            this.DataName.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.DataName.HeaderText = "ColliderType";
            this.DataName.Name = "DataName";
            // 
            // dataGridViewTextBoxColumn4
            // 
            this.dataGridViewTextBoxColumn4.HeaderText = "Tag Name";
            this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
            // 
            // Edit
            // 
            this.Edit.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.Edit.HeaderText = "";
            this.Edit.Name = "Edit";
            this.Edit.ReadOnly = true;
            this.Edit.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.Edit.Width = 20;
            // 
            // Delete
            // 
            this.Delete.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.None;
            this.Delete.HeaderText = "";
            this.Delete.Name = "Delete";
            this.Delete.Width = 20;
            // 
            // colliderBindingSource
            // 
            this.colliderBindingSource.DataSource = typeof(GhostbaitModelCreator.ModelCreatorForm.Collider);
            // 
            // colliderCreatorFormBindingSource
            // 
            this.colliderCreatorFormBindingSource.DataSource = typeof(GhostbaitModelCreator.ColliderCreatorForm);
            // 
            // programBindingSource
            // 
            this.programBindingSource.DataSource = typeof(GhostbaitModelCreator.Program);
            // 
            // ModelCreatorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(884, 661);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "ModelCreatorForm";
            this.Text = "Ghostbait Model Editor";
            this.Load += new System.EventHandler(this.ModelCreatorForm_Load);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewColliders)).EndInit();
            this.groupBox6.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMaterial)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMesh)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderCreatorFormBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.programBindingSource)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button meshFileRemove;
        private System.Windows.Forms.Button meshFileGrab;
        private System.Windows.Forms.TextBox meshFileName;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button materialFileRemove;
        private System.Windows.Forms.Button materialFileGrab;
        private System.Windows.Forms.TextBox materialFileName;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem blankToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromFBXToolStripMenuItem;
        private System.Windows.Forms.TextBox className;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.Button colliderAdd;
        private System.Windows.Forms.ListBox colliderListBox;
        private System.Windows.Forms.Button colliderRemove;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Button audioAdd;
        private System.Windows.Forms.ListBox audioListBox;
        private System.Windows.Forms.Button audioRemove;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.DataGridView dataGridViewMesh;
        private System.Windows.Forms.DataGridViewTextBoxColumn Filepath;
        private System.Windows.Forms.DataGridViewTextBoxColumn TagName;
        private System.Windows.Forms.DataGridViewButtonColumn ColumnDeleteRowMesh;
        private System.Windows.Forms.DataGridView dataGridViewColliders;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Button bindPoseFileRemove;
        private System.Windows.Forms.TextBox bindPoseFileName;
        private System.Windows.Forms.Button bindPoseFileGrab;
        private System.Windows.Forms.DataGridView dataGridViewMaterial;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewButtonColumn dataGridViewButtonColumn1;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button animationAdd;
        private System.Windows.Forms.ListBox animationListBox;
        private System.Windows.Forms.Button animationRemove;
        private System.Windows.Forms.DataGridViewTextBoxColumn Component;
        private System.Windows.Forms.DataGridViewTextBoxColumn DataName;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
        private System.Windows.Forms.DataGridViewImageColumn Edit;
        private System.Windows.Forms.DataGridViewButtonColumn Delete;
        private System.Windows.Forms.BindingSource programBindingSource;
        private System.Windows.Forms.BindingSource colliderCreatorFormBindingSource;
        private System.Windows.Forms.BindingSource colliderBindingSource;
    }
}

