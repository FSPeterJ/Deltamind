﻿namespace GhostbaitSceneCreator
{
    partial class Form1
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newFromfbxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.txt_SceneName = new System.Windows.Forms.TextBox();
            this.lbl_SceneName = new System.Windows.Forms.Label();
            this.lbo_Levels = new System.Windows.Forms.ListBox();
            this.lbl_Levels = new System.Windows.Forms.Label();
            this.btn_AddLevel = new System.Windows.Forms.Button();
            this.btn_RemoveLevel = new System.Windows.Forms.Button();
            this.lbo_AdditionalPrefabs = new System.Windows.Forms.ListBox();
            this.lbl_AdditionalPrefabs = new System.Windows.Forms.Label();
            this.btn_AddPrefab = new System.Windows.Forms.Button();
            this.btn_RemovePrefab = new System.Windows.Forms.Button();
            this.txt_FbxFile = new System.Windows.Forms.TextBox();
            this.lbl_FbxFile = new System.Windows.Forms.Label();
            this.btn_OpenFbx = new System.Windows.Forms.Button();
            this.emptyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.fromfbxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(345, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newFromfbxToolStripMenuItem,
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newFromfbxToolStripMenuItem
            // 
            this.newFromfbxToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.emptyToolStripMenuItem,
            this.fromfbxToolStripMenuItem});
            this.newFromfbxToolStripMenuItem.Name = "newFromfbxToolStripMenuItem";
            this.newFromfbxToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.newFromfbxToolStripMenuItem.Text = "New";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // txt_SceneName
            // 
            this.txt_SceneName.Location = new System.Drawing.Point(96, 28);
            this.txt_SceneName.Name = "txt_SceneName";
            this.txt_SceneName.Size = new System.Drawing.Size(156, 20);
            this.txt_SceneName.TabIndex = 1;
            // 
            // lbl_SceneName
            // 
            this.lbl_SceneName.AutoSize = true;
            this.lbl_SceneName.Location = new System.Drawing.Point(18, 31);
            this.lbl_SceneName.Name = "lbl_SceneName";
            this.lbl_SceneName.Size = new System.Drawing.Size(72, 13);
            this.lbl_SceneName.TabIndex = 2;
            this.lbl_SceneName.Text = "Scene Name:";
            // 
            // lbo_Levels
            // 
            this.lbo_Levels.FormattingEnabled = true;
            this.lbo_Levels.Location = new System.Drawing.Point(15, 108);
            this.lbo_Levels.Name = "lbo_Levels";
            this.lbo_Levels.Size = new System.Drawing.Size(120, 134);
            this.lbo_Levels.TabIndex = 3;
            this.lbo_Levels.DoubleClick += new System.EventHandler(this.lbo_Levels_DoubleClick);
            // 
            // lbl_Levels
            // 
            this.lbl_Levels.AutoSize = true;
            this.lbl_Levels.Location = new System.Drawing.Point(12, 92);
            this.lbl_Levels.Name = "lbl_Levels";
            this.lbl_Levels.Size = new System.Drawing.Size(78, 13);
            this.lbl_Levels.TabIndex = 4;
            this.lbl_Levels.Text = "Level .xml files:";
            // 
            // btn_AddLevel
            // 
            this.btn_AddLevel.Location = new System.Drawing.Point(15, 248);
            this.btn_AddLevel.Name = "btn_AddLevel";
            this.btn_AddLevel.Size = new System.Drawing.Size(57, 23);
            this.btn_AddLevel.TabIndex = 5;
            this.btn_AddLevel.Text = "Add";
            this.btn_AddLevel.UseVisualStyleBackColor = true;
            this.btn_AddLevel.Click += new System.EventHandler(this.btn_AddLevel_Click);
            // 
            // btn_RemoveLevel
            // 
            this.btn_RemoveLevel.Location = new System.Drawing.Point(78, 248);
            this.btn_RemoveLevel.Name = "btn_RemoveLevel";
            this.btn_RemoveLevel.Size = new System.Drawing.Size(57, 23);
            this.btn_RemoveLevel.TabIndex = 6;
            this.btn_RemoveLevel.Text = "Remove";
            this.btn_RemoveLevel.UseVisualStyleBackColor = true;
            this.btn_RemoveLevel.Click += new System.EventHandler(this.btn_RemoveLevel_Click);
            // 
            // lbo_AdditionalPrefabs
            // 
            this.lbo_AdditionalPrefabs.FormattingEnabled = true;
            this.lbo_AdditionalPrefabs.Location = new System.Drawing.Point(205, 108);
            this.lbo_AdditionalPrefabs.Name = "lbo_AdditionalPrefabs";
            this.lbo_AdditionalPrefabs.Size = new System.Drawing.Size(120, 134);
            this.lbo_AdditionalPrefabs.TabIndex = 7;
            this.lbo_AdditionalPrefabs.DoubleClick += new System.EventHandler(this.lbo_AdditionalPrefabs_DoubleClick);
            // 
            // lbl_AdditionalPrefabs
            // 
            this.lbl_AdditionalPrefabs.AutoSize = true;
            this.lbl_AdditionalPrefabs.Location = new System.Drawing.Point(202, 92);
            this.lbl_AdditionalPrefabs.Name = "lbl_AdditionalPrefabs";
            this.lbl_AdditionalPrefabs.Size = new System.Drawing.Size(91, 13);
            this.lbl_AdditionalPrefabs.TabIndex = 8;
            this.lbl_AdditionalPrefabs.Text = "Additional prefabs";
            // 
            // btn_AddPrefab
            // 
            this.btn_AddPrefab.Location = new System.Drawing.Point(205, 248);
            this.btn_AddPrefab.Name = "btn_AddPrefab";
            this.btn_AddPrefab.Size = new System.Drawing.Size(57, 23);
            this.btn_AddPrefab.TabIndex = 9;
            this.btn_AddPrefab.Text = "Add";
            this.btn_AddPrefab.UseVisualStyleBackColor = true;
            this.btn_AddPrefab.Click += new System.EventHandler(this.btn_AddPrefab_Click);
            // 
            // btn_RemovePrefab
            // 
            this.btn_RemovePrefab.Location = new System.Drawing.Point(268, 248);
            this.btn_RemovePrefab.Name = "btn_RemovePrefab";
            this.btn_RemovePrefab.Size = new System.Drawing.Size(57, 23);
            this.btn_RemovePrefab.TabIndex = 10;
            this.btn_RemovePrefab.Text = "Remove";
            this.btn_RemovePrefab.UseVisualStyleBackColor = true;
            this.btn_RemovePrefab.Click += new System.EventHandler(this.btn_RemovePrefab_Click);
            // 
            // txt_FbxFile
            // 
            this.txt_FbxFile.Location = new System.Drawing.Point(96, 54);
            this.txt_FbxFile.Name = "txt_FbxFile";
            this.txt_FbxFile.Size = new System.Drawing.Size(156, 20);
            this.txt_FbxFile.TabIndex = 11;
            // 
            // lbl_FbxFile
            // 
            this.lbl_FbxFile.AutoSize = true;
            this.lbl_FbxFile.Location = new System.Drawing.Point(10, 57);
            this.lbl_FbxFile.Name = "lbl_FbxFile";
            this.lbl_FbxFile.Size = new System.Drawing.Size(80, 13);
            this.lbl_FbxFile.TabIndex = 12;
            this.lbl_FbxFile.Text = "Scene FBX file:";
            // 
            // btn_OpenFbx
            // 
            this.btn_OpenFbx.Location = new System.Drawing.Point(258, 54);
            this.btn_OpenFbx.Name = "btn_OpenFbx";
            this.btn_OpenFbx.Size = new System.Drawing.Size(52, 21);
            this.btn_OpenFbx.TabIndex = 13;
            this.btn_OpenFbx.Text = "Open";
            this.btn_OpenFbx.UseVisualStyleBackColor = true;
            this.btn_OpenFbx.Click += new System.EventHandler(this.btn_OpenFbx_Click);
            // 
            // emptyToolStripMenuItem
            // 
            this.emptyToolStripMenuItem.Name = "emptyToolStripMenuItem";
            this.emptyToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.emptyToolStripMenuItem.Text = "Empty";
            this.emptyToolStripMenuItem.Click += new System.EventHandler(this.emptyToolStripMenuItem_Click);
            // 
            // fromfbxToolStripMenuItem
            // 
            this.fromfbxToolStripMenuItem.Name = "fromfbxToolStripMenuItem";
            this.fromfbxToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.fromfbxToolStripMenuItem.Text = "From .fbx";
            this.fromfbxToolStripMenuItem.Click += new System.EventHandler(this.newFromfbxToolStripMenuItem_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(345, 283);
            this.Controls.Add(this.btn_OpenFbx);
            this.Controls.Add(this.lbl_FbxFile);
            this.Controls.Add(this.txt_FbxFile);
            this.Controls.Add(this.btn_RemovePrefab);
            this.Controls.Add(this.btn_AddPrefab);
            this.Controls.Add(this.lbl_AdditionalPrefabs);
            this.Controls.Add(this.lbo_AdditionalPrefabs);
            this.Controls.Add(this.btn_RemoveLevel);
            this.Controls.Add(this.btn_AddLevel);
            this.Controls.Add(this.lbl_Levels);
            this.Controls.Add(this.lbo_Levels);
            this.Controls.Add(this.lbl_SceneName);
            this.Controls.Add(this.txt_SceneName);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Ghostbait Scene Creator";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newFromfbxToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.TextBox txt_SceneName;
        private System.Windows.Forms.Label lbl_SceneName;
        private System.Windows.Forms.ListBox lbo_Levels;
        private System.Windows.Forms.Label lbl_Levels;
        private System.Windows.Forms.Button btn_AddLevel;
        private System.Windows.Forms.Button btn_RemoveLevel;
        private System.Windows.Forms.ListBox lbo_AdditionalPrefabs;
        private System.Windows.Forms.Label lbl_AdditionalPrefabs;
        private System.Windows.Forms.Button btn_AddPrefab;
        private System.Windows.Forms.Button btn_RemovePrefab;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem emptyToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem fromfbxToolStripMenuItem;
        private System.Windows.Forms.TextBox txt_FbxFile;
        private System.Windows.Forms.Label lbl_FbxFile;
        private System.Windows.Forms.Button btn_OpenFbx;
    }
}

