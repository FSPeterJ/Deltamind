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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ModelCreatorForm));
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.className = new System.Windows.Forms.TextBox();
            this.meshFileRemove = new System.Windows.Forms.Button();
            this.meshFileGrab = new System.Windows.Forms.Button();
            this.meshFileName = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.animationAdd = new System.Windows.Forms.Button();
            this.animationListBox = new System.Windows.Forms.ListBox();
            this.animationRemove = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
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
            this.materialFileRemove = new System.Windows.Forms.Button();
            this.materialFileName = new System.Windows.Forms.TextBox();
            this.materialFileGrab = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.blankToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromFBXToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.panel1.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.className);
            this.groupBox2.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox2.Location = new System.Drawing.Point(0, 0);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox2.Size = new System.Drawing.Size(267, 60);
            this.groupBox2.TabIndex = 3;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Class";
            // 
            // className
            // 
            this.className.Location = new System.Drawing.Point(8, 28);
            this.className.Margin = new System.Windows.Forms.Padding(4);
            this.className.Name = "className";
            this.className.Size = new System.Drawing.Size(249, 22);
            this.className.TabIndex = 3;
            // 
            // meshFileRemove
            // 
            this.meshFileRemove.Location = new System.Drawing.Point(227, 28);
            this.meshFileRemove.Margin = new System.Windows.Forms.Padding(4);
            this.meshFileRemove.Name = "meshFileRemove";
            this.meshFileRemove.Size = new System.Drawing.Size(32, 25);
            this.meshFileRemove.TabIndex = 2;
            this.meshFileRemove.Text = "X";
            this.meshFileRemove.UseVisualStyleBackColor = true;
            this.meshFileRemove.Click += new System.EventHandler(this.meshFileRemove_Click);
            // 
            // meshFileGrab
            // 
            this.meshFileGrab.Location = new System.Drawing.Point(187, 28);
            this.meshFileGrab.Margin = new System.Windows.Forms.Padding(4);
            this.meshFileGrab.Name = "meshFileGrab";
            this.meshFileGrab.Size = new System.Drawing.Size(32, 25);
            this.meshFileGrab.TabIndex = 1;
            this.meshFileGrab.Text = "...";
            this.meshFileGrab.UseVisualStyleBackColor = true;
            this.meshFileGrab.Click += new System.EventHandler(this.meshFileGrab_Click);
            // 
            // meshFileName
            // 
            this.meshFileName.Location = new System.Drawing.Point(8, 28);
            this.meshFileName.Margin = new System.Windows.Forms.Padding(4);
            this.meshFileName.Name = "meshFileName";
            this.meshFileName.Size = new System.Drawing.Size(169, 22);
            this.meshFileName.TabIndex = 0;
            this.meshFileName.TextChanged += new System.EventHandler(this.meshFileName_TextChanged);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.animationAdd);
            this.groupBox3.Controls.Add(this.animationListBox);
            this.groupBox3.Controls.Add(this.animationRemove);
            this.groupBox3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.groupBox3.Location = new System.Drawing.Point(0, 606);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox3.Size = new System.Drawing.Size(267, 180);
            this.groupBox3.TabIndex = 4;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Animation";
            // 
            // animationAdd
            // 
            this.animationAdd.Location = new System.Drawing.Point(4, 145);
            this.animationAdd.Margin = new System.Windows.Forms.Padding(4);
            this.animationAdd.Name = "animationAdd";
            this.animationAdd.Size = new System.Drawing.Size(112, 27);
            this.animationAdd.TabIndex = 4;
            this.animationAdd.Text = "Add";
            this.animationAdd.UseVisualStyleBackColor = true;
            this.animationAdd.Click += new System.EventHandler(this.animationAdd_Click);
            // 
            // animationListBox
            // 
            this.animationListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.animationListBox.FormattingEnabled = true;
            this.animationListBox.ItemHeight = 16;
            this.animationListBox.Location = new System.Drawing.Point(4, 19);
            this.animationListBox.Margin = new System.Windows.Forms.Padding(4);
            this.animationListBox.Name = "animationListBox";
            this.animationListBox.Size = new System.Drawing.Size(259, 116);
            this.animationListBox.TabIndex = 3;
            this.animationListBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.animationListBox_MouseDoubleClick);
            // 
            // animationRemove
            // 
            this.animationRemove.Location = new System.Drawing.Point(151, 145);
            this.animationRemove.Margin = new System.Windows.Forms.Padding(4);
            this.animationRemove.Name = "animationRemove";
            this.animationRemove.Size = new System.Drawing.Size(112, 27);
            this.animationRemove.TabIndex = 2;
            this.animationRemove.Text = "Remove";
            this.animationRemove.UseVisualStyleBackColor = true;
            this.animationRemove.Click += new System.EventHandler(this.animationRemove_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.groupBox7);
            this.panel1.Controls.Add(this.groupBox6);
            this.panel1.Controls.Add(this.groupBox5);
            this.panel1.Controls.Add(this.groupBox4);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox3);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(912, 28);
            this.panel1.Margin = new System.Windows.Forms.Padding(4);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(267, 786);
            this.panel1.TabIndex = 5;
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.colliderAdd);
            this.groupBox7.Controls.Add(this.colliderListBox);
            this.groupBox7.Controls.Add(this.colliderRemove);
            this.groupBox7.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.groupBox7.Location = new System.Drawing.Point(0, 246);
            this.groupBox7.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox7.Size = new System.Drawing.Size(267, 180);
            this.groupBox7.TabIndex = 8;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Colliders";
            // 
            // colliderAdd
            // 
            this.colliderAdd.Location = new System.Drawing.Point(4, 145);
            this.colliderAdd.Margin = new System.Windows.Forms.Padding(4);
            this.colliderAdd.Name = "colliderAdd";
            this.colliderAdd.Size = new System.Drawing.Size(112, 27);
            this.colliderAdd.TabIndex = 4;
            this.colliderAdd.Text = "Add";
            this.colliderAdd.UseVisualStyleBackColor = true;
            this.colliderAdd.Click += new System.EventHandler(this.colliderAdd_Click);
            // 
            // colliderListBox
            // 
            this.colliderListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.colliderListBox.FormattingEnabled = true;
            this.colliderListBox.ItemHeight = 16;
            this.colliderListBox.Location = new System.Drawing.Point(4, 19);
            this.colliderListBox.Margin = new System.Windows.Forms.Padding(4);
            this.colliderListBox.Name = "colliderListBox";
            this.colliderListBox.Size = new System.Drawing.Size(259, 116);
            this.colliderListBox.TabIndex = 3;
            this.colliderListBox.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.colliderListBox_MouseDoubleClick);
            // 
            // colliderRemove
            // 
            this.colliderRemove.Location = new System.Drawing.Point(151, 145);
            this.colliderRemove.Margin = new System.Windows.Forms.Padding(4);
            this.colliderRemove.Name = "colliderRemove";
            this.colliderRemove.Size = new System.Drawing.Size(112, 27);
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
            this.groupBox6.Location = new System.Drawing.Point(0, 426);
            this.groupBox6.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox6.Size = new System.Drawing.Size(267, 180);
            this.groupBox6.TabIndex = 7;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Audio";
            // 
            // audioAdd
            // 
            this.audioAdd.Location = new System.Drawing.Point(4, 145);
            this.audioAdd.Margin = new System.Windows.Forms.Padding(4);
            this.audioAdd.Name = "audioAdd";
            this.audioAdd.Size = new System.Drawing.Size(112, 27);
            this.audioAdd.TabIndex = 4;
            this.audioAdd.Text = "Add";
            this.audioAdd.UseVisualStyleBackColor = true;
            this.audioAdd.Click += new System.EventHandler(this.audioAdd_Click);
            // 
            // audioListBox
            // 
            this.audioListBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.audioListBox.FormattingEnabled = true;
            this.audioListBox.ItemHeight = 16;
            this.audioListBox.Location = new System.Drawing.Point(4, 19);
            this.audioListBox.Margin = new System.Windows.Forms.Padding(4);
            this.audioListBox.Name = "audioListBox";
            this.audioListBox.Size = new System.Drawing.Size(259, 116);
            this.audioListBox.TabIndex = 3;
            // 
            // audioRemove
            // 
            this.audioRemove.Location = new System.Drawing.Point(151, 145);
            this.audioRemove.Margin = new System.Windows.Forms.Padding(4);
            this.audioRemove.Name = "audioRemove";
            this.audioRemove.Size = new System.Drawing.Size(112, 27);
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
            this.groupBox5.Location = new System.Drawing.Point(0, 180);
            this.groupBox5.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox5.Size = new System.Drawing.Size(267, 60);
            this.groupBox5.TabIndex = 6;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Bind Pose";
            // 
            // bindPoseFileRemove
            // 
            this.bindPoseFileRemove.Location = new System.Drawing.Point(227, 28);
            this.bindPoseFileRemove.Margin = new System.Windows.Forms.Padding(4);
            this.bindPoseFileRemove.Name = "bindPoseFileRemove";
            this.bindPoseFileRemove.Size = new System.Drawing.Size(32, 25);
            this.bindPoseFileRemove.TabIndex = 2;
            this.bindPoseFileRemove.Text = "X";
            this.bindPoseFileRemove.UseVisualStyleBackColor = true;
            this.bindPoseFileRemove.Click += new System.EventHandler(this.bindPoseFileRemove_Click);
            // 
            // bindPoseFileName
            // 
            this.bindPoseFileName.Location = new System.Drawing.Point(8, 28);
            this.bindPoseFileName.Margin = new System.Windows.Forms.Padding(4);
            this.bindPoseFileName.Name = "bindPoseFileName";
            this.bindPoseFileName.Size = new System.Drawing.Size(169, 22);
            this.bindPoseFileName.TabIndex = 0;
            this.bindPoseFileName.TextChanged += new System.EventHandler(this.bindPoseFileName_TextChanged);
            // 
            // bindPoseFileGrab
            // 
            this.bindPoseFileGrab.Location = new System.Drawing.Point(187, 28);
            this.bindPoseFileGrab.Margin = new System.Windows.Forms.Padding(4);
            this.bindPoseFileGrab.Name = "bindPoseFileGrab";
            this.bindPoseFileGrab.Size = new System.Drawing.Size(32, 25);
            this.bindPoseFileGrab.TabIndex = 1;
            this.bindPoseFileGrab.Text = "...";
            this.bindPoseFileGrab.UseVisualStyleBackColor = true;
            this.bindPoseFileGrab.Click += new System.EventHandler(this.bindPoseFileGrab_Click);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.materialFileRemove);
            this.groupBox4.Controls.Add(this.materialFileName);
            this.groupBox4.Controls.Add(this.materialFileGrab);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox4.Location = new System.Drawing.Point(0, 120);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox4.Size = new System.Drawing.Size(267, 60);
            this.groupBox4.TabIndex = 5;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Material";
            // 
            // materialFileRemove
            // 
            this.materialFileRemove.Location = new System.Drawing.Point(227, 28);
            this.materialFileRemove.Margin = new System.Windows.Forms.Padding(4);
            this.materialFileRemove.Name = "materialFileRemove";
            this.materialFileRemove.Size = new System.Drawing.Size(32, 25);
            this.materialFileRemove.TabIndex = 2;
            this.materialFileRemove.Text = "X";
            this.materialFileRemove.UseVisualStyleBackColor = true;
            this.materialFileRemove.Click += new System.EventHandler(this.materialFileRemove_Click);
            // 
            // materialFileName
            // 
            this.materialFileName.Location = new System.Drawing.Point(8, 28);
            this.materialFileName.Margin = new System.Windows.Forms.Padding(4);
            this.materialFileName.Name = "materialFileName";
            this.materialFileName.Size = new System.Drawing.Size(169, 22);
            this.materialFileName.TabIndex = 0;
            this.materialFileName.TextChanged += new System.EventHandler(this.materialFileName_TextChanged);
            // 
            // materialFileGrab
            // 
            this.materialFileGrab.Location = new System.Drawing.Point(187, 28);
            this.materialFileGrab.Margin = new System.Windows.Forms.Padding(4);
            this.materialFileGrab.Name = "materialFileGrab";
            this.materialFileGrab.Size = new System.Drawing.Size(32, 25);
            this.materialFileGrab.TabIndex = 1;
            this.materialFileGrab.Text = "...";
            this.materialFileGrab.UseVisualStyleBackColor = true;
            this.materialFileGrab.Click += new System.EventHandler(this.materialFileGrab_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.meshFileRemove);
            this.groupBox1.Controls.Add(this.meshFileName);
            this.groupBox1.Controls.Add(this.meshFileGrab);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.Location = new System.Drawing.Point(0, 60);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(4);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(4);
            this.groupBox1.Size = new System.Drawing.Size(267, 60);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Mesh";
            // 
            // menuStrip1
            // 
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(8, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(1179, 28);
            this.menuStrip1.TabIndex = 6;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 24);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.blankToolStripMenuItem,
            this.fromFBXToolStripMenuItem});
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(120, 26);
            this.newToolStripMenuItem.Text = "New";
            // 
            // blankToolStripMenuItem
            // 
            this.blankToolStripMenuItem.Name = "blankToolStripMenuItem";
            this.blankToolStripMenuItem.Size = new System.Drawing.Size(150, 26);
            this.blankToolStripMenuItem.Text = "Blank";
            this.blankToolStripMenuItem.Click += new System.EventHandler(this.blankToolStripMenuItem_Click);
            // 
            // fromFBXToolStripMenuItem
            // 
            this.fromFBXToolStripMenuItem.Name = "fromFBXToolStripMenuItem";
            this.fromFBXToolStripMenuItem.Size = new System.Drawing.Size(150, 26);
            this.fromFBXToolStripMenuItem.Text = "From .FBX";
            this.fromFBXToolStripMenuItem.Click += new System.EventHandler(this.fromFBXToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(120, 26);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(120, 26);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::GhostbaitModelCreator.Properties.Resources.Ghostbait;
            this.pictureBox1.Location = new System.Drawing.Point(861, 33);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(32, 32);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pictureBox1.TabIndex = 7;
            this.pictureBox1.TabStop = false;
            // 
            // ModelCreatorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1179, 814);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "ModelCreatorForm";
            this.Text = "Ghostbait Model Editor";
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button meshFileRemove;
        private System.Windows.Forms.Button meshFileGrab;
        private System.Windows.Forms.TextBox meshFileName;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button animationAdd;
        private System.Windows.Forms.ListBox animationListBox;
        private System.Windows.Forms.Button animationRemove;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button materialFileRemove;
        private System.Windows.Forms.Button materialFileGrab;
        private System.Windows.Forms.TextBox materialFileName;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button bindPoseFileRemove;
        private System.Windows.Forms.TextBox bindPoseFileName;
        private System.Windows.Forms.Button bindPoseFileGrab;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem blankToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromFBXToolStripMenuItem;
        private System.Windows.Forms.TextBox className;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.Button colliderAdd;
        private System.Windows.Forms.ListBox colliderListBox;
        private System.Windows.Forms.Button colliderRemove;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Button audioAdd;
        private System.Windows.Forms.ListBox audioListBox;
        private System.Windows.Forms.Button audioRemove;
        private System.Windows.Forms.PictureBox pictureBox1;
    }
}

