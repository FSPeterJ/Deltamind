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
            this.groupBoxAnimation = new System.Windows.Forms.GroupBox();
            this.animationListBox = new System.Windows.Forms.ListBox();
            this.animationRemove = new System.Windows.Forms.Button();
            this.animationAdd = new System.Windows.Forms.Button();
            this.className = new System.Windows.Forms.TextBox();
            this.meshFileRemove = new System.Windows.Forms.Button();
            this.meshFileGrab = new System.Windows.Forms.Button();
            this.SidePanel = new System.Windows.Forms.Panel();
            this.groupBoxBindPose = new System.Windows.Forms.GroupBox();
            this.bindPoseFileRemove = new System.Windows.Forms.Button();
            this.bindPoseFileName = new System.Windows.Forms.TextBox();
            this.bindPoseFileGrab = new System.Windows.Forms.Button();
            this.groupBoxAudio = new System.Windows.Forms.GroupBox();
            this.audioAdd = new System.Windows.Forms.Button();
            this.audioListBox = new System.Windows.Forms.ListBox();
            this.audioRemove = new System.Windows.Forms.Button();
            this.groupBoxColliders = new System.Windows.Forms.GroupBox();
            this.dataGridViewColliders = new System.Windows.Forms.DataGridView();
            this.panel1 = new System.Windows.Forms.Panel();
            this.colliderAdd = new System.Windows.Forms.Button();
            this.colliderRemove = new System.Windows.Forms.Button();
            this.groupBoxMaterial = new System.Windows.Forms.GroupBox();
            this.dataGridViewMaterial = new System.Windows.Forms.DataGridView();
            this.materialFileRemove = new System.Windows.Forms.Button();
            this.materialFileGrab = new System.Windows.Forms.Button();
            this.groupBoxMesh = new System.Windows.Forms.GroupBox();
            this.dataGridViewMesh = new System.Windows.Forms.DataGridView();
            this.groupBoxClass = new System.Windows.Forms.GroupBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.blankToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromFBXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.colliderBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.colliderCreatorFormBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.programBindingSource = new System.Windows.Forms.BindingSource(this.components);
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.groupBoxAnimation.SuspendLayout();
            this.SidePanel.SuspendLayout();
            this.groupBoxBindPose.SuspendLayout();
            this.groupBoxAudio.SuspendLayout();
            this.groupBoxColliders.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewColliders)).BeginInit();
            this.panel1.SuspendLayout();
            this.groupBoxMaterial.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMaterial)).BeginInit();
            this.groupBoxMesh.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMesh)).BeginInit();
            this.groupBoxClass.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderCreatorFormBindingSource)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.programBindingSource)).BeginInit();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBoxAnimation
            // 
            this.groupBoxAnimation.Controls.Add(this.animationListBox);
            this.groupBoxAnimation.Controls.Add(this.animationRemove);
            this.groupBoxAnimation.Controls.Add(this.animationAdd);
            this.groupBoxAnimation.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxAnimation.Location = new System.Drawing.Point(0, 828);
            this.groupBoxAnimation.Name = "groupBoxAnimation";
            this.groupBoxAnimation.Size = new System.Drawing.Size(305, 183);
            this.groupBoxAnimation.TabIndex = 3;
            this.groupBoxAnimation.TabStop = false;
            this.groupBoxAnimation.Text = "Animation";
            // 
            // animationListBox
            // 
            this.animationListBox.FormattingEnabled = true;
            this.animationListBox.Location = new System.Drawing.Point(3, 65);
            this.animationListBox.Name = "animationListBox";
            this.animationListBox.Size = new System.Drawing.Size(299, 95);
            this.animationListBox.TabIndex = 3;
            this.animationListBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.animationListBox_MouseDoubleClick);
            // 
            // animationRemove
            // 
            this.animationRemove.Location = new System.Drawing.Point(96, 28);
            this.animationRemove.Name = "animationRemove";
            this.animationRemove.Size = new System.Drawing.Size(84, 22);
            this.animationRemove.TabIndex = 2;
            this.animationRemove.Text = "Remove";
            this.animationRemove.UseVisualStyleBackColor = true;
            this.animationRemove.Click += new System.EventHandler(this.animationRemove_Click);
            // 
            // animationAdd
            // 
            this.animationAdd.Location = new System.Drawing.Point(6, 28);
            this.animationAdd.Name = "animationAdd";
            this.animationAdd.Size = new System.Drawing.Size(84, 22);
            this.animationAdd.TabIndex = 4;
            this.animationAdd.Text = "Add";
            this.animationAdd.UseVisualStyleBackColor = true;
            this.animationAdd.Click += new System.EventHandler(this.animationAdd_Click);
            // 
            // className
            // 
            this.className.Location = new System.Drawing.Point(6, 19);
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
            // SidePanel
            // 
            this.SidePanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.SidePanel.AutoScroll = true;
            this.SidePanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.SidePanel.Controls.Add(this.groupBoxAnimation);
            this.SidePanel.Controls.Add(this.groupBoxBindPose);
            this.SidePanel.Controls.Add(this.groupBoxAudio);
            this.SidePanel.Controls.Add(this.groupBoxColliders);
            this.SidePanel.Controls.Add(this.groupBoxMaterial);
            this.SidePanel.Controls.Add(this.groupBoxMesh);
            this.SidePanel.Controls.Add(this.groupBoxClass);
            this.SidePanel.Location = new System.Drawing.Point(562, 24);
            this.SidePanel.Name = "SidePanel";
            this.SidePanel.Size = new System.Drawing.Size(322, 637);
            this.SidePanel.TabIndex = 5;
            // 
            // groupBoxBindPose
            // 
            this.groupBoxBindPose.Controls.Add(this.bindPoseFileRemove);
            this.groupBoxBindPose.Controls.Add(this.bindPoseFileName);
            this.groupBoxBindPose.Controls.Add(this.bindPoseFileGrab);
            this.groupBoxBindPose.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxBindPose.Location = new System.Drawing.Point(0, 772);
            this.groupBoxBindPose.Name = "groupBoxBindPose";
            this.groupBoxBindPose.Size = new System.Drawing.Size(305, 56);
            this.groupBoxBindPose.TabIndex = 6;
            this.groupBoxBindPose.TabStop = false;
            this.groupBoxBindPose.Text = "Bind Pose";
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
            // groupBoxAudio
            // 
            this.groupBoxAudio.Controls.Add(this.audioAdd);
            this.groupBoxAudio.Controls.Add(this.audioListBox);
            this.groupBoxAudio.Controls.Add(this.audioRemove);
            this.groupBoxAudio.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxAudio.Location = new System.Drawing.Point(0, 594);
            this.groupBoxAudio.Name = "groupBoxAudio";
            this.groupBoxAudio.Size = new System.Drawing.Size(305, 178);
            this.groupBoxAudio.TabIndex = 7;
            this.groupBoxAudio.TabStop = false;
            this.groupBoxAudio.Text = "Audio";
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
            // groupBoxColliders
            // 
            this.groupBoxColliders.AutoSize = true;
            this.groupBoxColliders.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBoxColliders.Controls.Add(this.dataGridViewColliders);
            this.groupBoxColliders.Controls.Add(this.panel1);
            this.groupBoxColliders.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxColliders.Location = new System.Drawing.Point(0, 418);
            this.groupBoxColliders.Name = "groupBoxColliders";
            this.groupBoxColliders.Size = new System.Drawing.Size(305, 176);
            this.groupBoxColliders.TabIndex = 8;
            this.groupBoxColliders.TabStop = false;
            this.groupBoxColliders.Text = "Colliders";
            // 
            // dataGridViewColliders
            // 
            this.dataGridViewColliders.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridViewColliders.Dock = System.Windows.Forms.DockStyle.Top;
            this.dataGridViewColliders.Location = new System.Drawing.Point(3, 48);
            this.dataGridViewColliders.Name = "dataGridViewColliders";
            this.dataGridViewColliders.RowHeadersVisible = false;
            this.dataGridViewColliders.Size = new System.Drawing.Size(299, 125);
            this.dataGridViewColliders.TabIndex = 10;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.colliderAdd);
            this.panel1.Controls.Add(this.colliderRemove);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(3, 16);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(299, 32);
            this.panel1.TabIndex = 8;
            // 
            // colliderAdd
            // 
            this.colliderAdd.Location = new System.Drawing.Point(3, 3);
            this.colliderAdd.Name = "colliderAdd";
            this.colliderAdd.Size = new System.Drawing.Size(84, 22);
            this.colliderAdd.TabIndex = 4;
            this.colliderAdd.Text = "Add";
            this.colliderAdd.UseVisualStyleBackColor = true;
            this.colliderAdd.Click += new System.EventHandler(this.colliderAdd_Click);
            // 
            // colliderRemove
            // 
            this.colliderRemove.Location = new System.Drawing.Point(93, 3);
            this.colliderRemove.Name = "colliderRemove";
            this.colliderRemove.Size = new System.Drawing.Size(84, 22);
            this.colliderRemove.TabIndex = 2;
            this.colliderRemove.Text = "Remove";
            this.colliderRemove.UseVisualStyleBackColor = true;
            this.colliderRemove.Click += new System.EventHandler(this.colliderRemove_Click);
            // 
            // groupBoxMaterial
            // 
            this.groupBoxMaterial.AutoSize = true;
            this.groupBoxMaterial.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBoxMaterial.Controls.Add(this.dataGridViewMaterial);
            this.groupBoxMaterial.Controls.Add(this.materialFileRemove);
            this.groupBoxMaterial.Controls.Add(this.panel2);
            this.groupBoxMaterial.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxMaterial.Location = new System.Drawing.Point(0, 241);
            this.groupBoxMaterial.Name = "groupBoxMaterial";
            this.groupBoxMaterial.Size = new System.Drawing.Size(305, 177);
            this.groupBoxMaterial.TabIndex = 5;
            this.groupBoxMaterial.TabStop = false;
            this.groupBoxMaterial.Text = "Material";
            // 
            // dataGridViewMaterial
            // 
            this.dataGridViewMaterial.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridViewMaterial.Dock = System.Windows.Forms.DockStyle.Top;
            this.dataGridViewMaterial.Location = new System.Drawing.Point(3, 48);
            this.dataGridViewMaterial.Name = "dataGridViewMaterial";
            this.dataGridViewMaterial.RowHeadersVisible = false;
            this.dataGridViewMaterial.Size = new System.Drawing.Size(299, 126);
            this.dataGridViewMaterial.TabIndex = 9;
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
            // materialFileGrab
            // 
            this.materialFileGrab.Location = new System.Drawing.Point(137, 7);
            this.materialFileGrab.Name = "materialFileGrab";
            this.materialFileGrab.Size = new System.Drawing.Size(24, 20);
            this.materialFileGrab.TabIndex = 1;
            this.materialFileGrab.Text = "...";
            this.materialFileGrab.UseVisualStyleBackColor = true;
            this.materialFileGrab.Click += new System.EventHandler(this.materialFileGrab_Click);
            // 
            // groupBoxMesh
            // 
            this.groupBoxMesh.AutoSize = true;
            this.groupBoxMesh.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.groupBoxMesh.Controls.Add(this.dataGridViewMesh);
            this.groupBoxMesh.Controls.Add(this.meshFileRemove);
            this.groupBoxMesh.Controls.Add(this.meshFileGrab);
            this.groupBoxMesh.Controls.Add(this.panel3);
            this.groupBoxMesh.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxMesh.Location = new System.Drawing.Point(0, 52);
            this.groupBoxMesh.Name = "groupBoxMesh";
            this.groupBoxMesh.Size = new System.Drawing.Size(305, 189);
            this.groupBoxMesh.TabIndex = 4;
            this.groupBoxMesh.TabStop = false;
            this.groupBoxMesh.Text = "Mesh";
            // 
            // dataGridViewMesh
            // 
            this.dataGridViewMesh.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridViewMesh.Dock = System.Windows.Forms.DockStyle.Top;
            this.dataGridViewMesh.Location = new System.Drawing.Point(3, 48);
            this.dataGridViewMesh.Name = "dataGridViewMesh";
            this.dataGridViewMesh.RowHeadersVisible = false;
            this.dataGridViewMesh.Size = new System.Drawing.Size(299, 138);
            this.dataGridViewMesh.TabIndex = 8;
            // 
            // groupBoxClass
            // 
            this.groupBoxClass.Controls.Add(this.className);
            this.groupBoxClass.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBoxClass.Location = new System.Drawing.Point(0, 0);
            this.groupBoxClass.Name = "groupBoxClass";
            this.groupBoxClass.Size = new System.Drawing.Size(305, 52);
            this.groupBoxClass.TabIndex = 4;
            this.groupBoxClass.TabStop = false;
            this.groupBoxClass.Text = "Class";
            this.groupBoxClass.Enter += new System.EventHandler(this.groupBox3_Enter);
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
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.Image = global::GhostbaitModelCreator.Properties.Resources.Ghostbait_Icon;
            this.pictureBox1.Location = new System.Drawing.Point(524, 27);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(32, 36);
            this.pictureBox1.TabIndex = 7;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
            // 
            // colliderCreatorFormBindingSource
            // 
            this.colliderCreatorFormBindingSource.DataSource = typeof(GhostbaitModelCreator.ColliderCreatorForm);
            // 
            // programBindingSource
            // 
            this.programBindingSource.DataSource = typeof(GhostbaitModelCreator.Program);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.materialFileGrab);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(3, 16);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(299, 32);
            this.panel2.TabIndex = 10;
            // 
            // panel3
            // 
            this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel3.Location = new System.Drawing.Point(3, 16);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(299, 32);
            this.panel3.TabIndex = 12;
            // 
            // ModelCreatorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(884, 661);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.SidePanel);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "ModelCreatorForm";
            this.Text = "Ghostbait Model Editor";
            this.Load += new System.EventHandler(this.ModelCreatorForm_Load);
            this.groupBoxAnimation.ResumeLayout(false);
            this.SidePanel.ResumeLayout(false);
            this.SidePanel.PerformLayout();
            this.groupBoxBindPose.ResumeLayout(false);
            this.groupBoxBindPose.PerformLayout();
            this.groupBoxAudio.ResumeLayout(false);
            this.groupBoxColliders.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewColliders)).EndInit();
            this.panel1.ResumeLayout(false);
            this.groupBoxMaterial.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMaterial)).EndInit();
            this.groupBoxMesh.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewMesh)).EndInit();
            this.groupBoxClass.ResumeLayout(false);
            this.groupBoxClass.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.colliderCreatorFormBindingSource)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.programBindingSource)).EndInit();
            this.panel2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBoxAnimation;
        private System.Windows.Forms.Button meshFileRemove;
        private System.Windows.Forms.Button meshFileGrab;
        private System.Windows.Forms.Panel SidePanel;
        private System.Windows.Forms.GroupBox groupBoxMesh;
        private System.Windows.Forms.Button materialFileRemove;
        private System.Windows.Forms.Button materialFileGrab;
        private System.Windows.Forms.GroupBox groupBoxMaterial;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem blankToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromFBXToolStripMenuItem;
        private System.Windows.Forms.TextBox className;
        private System.Windows.Forms.GroupBox groupBoxColliders;
        private System.Windows.Forms.Button colliderAdd;
        private System.Windows.Forms.Button colliderRemove;
        private System.Windows.Forms.GroupBox groupBoxAudio;
        private System.Windows.Forms.Button audioAdd;
        private System.Windows.Forms.ListBox audioListBox;
        private System.Windows.Forms.Button audioRemove;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.DataGridView dataGridViewMesh;
        private System.Windows.Forms.DataGridView dataGridViewColliders;
        private System.Windows.Forms.GroupBox groupBoxBindPose;
        private System.Windows.Forms.Button bindPoseFileRemove;
        private System.Windows.Forms.TextBox bindPoseFileName;
        private System.Windows.Forms.Button bindPoseFileGrab;
        private System.Windows.Forms.DataGridView dataGridViewMaterial;
        private System.Windows.Forms.GroupBox groupBoxClass;
        private System.Windows.Forms.Button animationAdd;
        private System.Windows.Forms.ListBox animationListBox;
        private System.Windows.Forms.Button animationRemove;
        private System.Windows.Forms.BindingSource programBindingSource;
        private System.Windows.Forms.BindingSource colliderCreatorFormBindingSource;
        private System.Windows.Forms.BindingSource colliderBindingSource;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
    }
}

