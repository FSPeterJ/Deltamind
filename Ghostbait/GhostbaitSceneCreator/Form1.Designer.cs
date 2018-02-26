namespace GhostbaitSceneCreator
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
            this.txt_SceneName = new System.Windows.Forms.TextBox();
            this.lbl_SceneName = new System.Windows.Forms.Label();
            this.lbo_Levels = new System.Windows.Forms.ListBox();
            this.lbl_Levels = new System.Windows.Forms.Label();
            this.btn_AddLevel = new System.Windows.Forms.Button();
            this.btn_RemoveLevel = new System.Windows.Forms.Button();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(396, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newFromfbxToolStripMenuItem,
            this.openToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newFromfbxToolStripMenuItem
            // 
            this.newFromfbxToolStripMenuItem.Name = "newFromfbxToolStripMenuItem";
            this.newFromfbxToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.newFromfbxToolStripMenuItem.Text = "New from .fbx";
            this.newFromfbxToolStripMenuItem.Click += new System.EventHandler(this.newFromfbxToolStripMenuItem_Click);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // txt_SceneName
            // 
            this.txt_SceneName.Location = new System.Drawing.Point(90, 27);
            this.txt_SceneName.Name = "txt_SceneName";
            this.txt_SceneName.Size = new System.Drawing.Size(156, 20);
            this.txt_SceneName.TabIndex = 1;
            // 
            // lbl_SceneName
            // 
            this.lbl_SceneName.AutoSize = true;
            this.lbl_SceneName.Location = new System.Drawing.Point(12, 30);
            this.lbl_SceneName.Name = "lbl_SceneName";
            this.lbl_SceneName.Size = new System.Drawing.Size(72, 13);
            this.lbl_SceneName.TabIndex = 2;
            this.lbl_SceneName.Text = "Scene Name:";
            // 
            // lbo_Levels
            // 
            this.lbo_Levels.FormattingEnabled = true;
            this.lbo_Levels.Location = new System.Drawing.Point(15, 75);
            this.lbo_Levels.Name = "lbo_Levels";
            this.lbo_Levels.Size = new System.Drawing.Size(120, 134);
            this.lbo_Levels.TabIndex = 3;
            // 
            // lbl_Levels
            // 
            this.lbl_Levels.AutoSize = true;
            this.lbl_Levels.Location = new System.Drawing.Point(12, 59);
            this.lbl_Levels.Name = "lbl_Levels";
            this.lbl_Levels.Size = new System.Drawing.Size(78, 13);
            this.lbl_Levels.TabIndex = 4;
            this.lbl_Levels.Text = "Level .xml files:";
            // 
            // btn_AddLevel
            // 
            this.btn_AddLevel.Location = new System.Drawing.Point(15, 215);
            this.btn_AddLevel.Name = "btn_AddLevel";
            this.btn_AddLevel.Size = new System.Drawing.Size(57, 23);
            this.btn_AddLevel.TabIndex = 5;
            this.btn_AddLevel.Text = "Add";
            this.btn_AddLevel.UseVisualStyleBackColor = true;
            // 
            // btn_RemoveLevel
            // 
            this.btn_RemoveLevel.Location = new System.Drawing.Point(78, 215);
            this.btn_RemoveLevel.Name = "btn_RemoveLevel";
            this.btn_RemoveLevel.Size = new System.Drawing.Size(57, 23);
            this.btn_RemoveLevel.TabIndex = 6;
            this.btn_RemoveLevel.Text = "Remove";
            this.btn_RemoveLevel.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(396, 279);
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
    }
}

