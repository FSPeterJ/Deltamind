namespace GhostbaitModelCreator
{
    partial class Form2
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
            this.colliderType = new System.Windows.Forms.ComboBox();
            this.colliderOffsetZ = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.colliderOffsetY = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.colliderOffsetX = new System.Windows.Forms.TextBox();
            this.capsuleRadius = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.capsuleHeight = new System.Windows.Forms.TextBox();
            this.capsulePanel = new System.Windows.Forms.Panel();
            this.spherePanel = new System.Windows.Forms.Panel();
            this.sphereRadius = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.boxPanel = new System.Windows.Forms.Panel();
            this.label13 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.boxPoint2X = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.boxPoint2Y = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.boxPoint2Z = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.boxPoint1X = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.boxPoint1Y = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.boxPoint1Z = new System.Windows.Forms.TextBox();
            this.createButton = new System.Windows.Forms.Button();
            this.capsulePanel.SuspendLayout();
            this.spherePanel.SuspendLayout();
            this.boxPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // colliderType
            // 
            this.colliderType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.colliderType.FormattingEnabled = true;
            this.colliderType.Items.AddRange(new object[] {
            "Sphere",
            "Capsule",
            "Box"});
            this.colliderType.Location = new System.Drawing.Point(125, 24);
            this.colliderType.Name = "colliderType";
            this.colliderType.Size = new System.Drawing.Size(147, 21);
            this.colliderType.TabIndex = 0;
            this.colliderType.SelectedIndexChanged += new System.EventHandler(this.colliderType_SelectedIndexChanged);
            // 
            // colliderOffsetZ
            // 
            this.colliderOffsetZ.Location = new System.Drawing.Point(246, 51);
            this.colliderOffsetZ.Name = "colliderOffsetZ";
            this.colliderOffsetZ.Size = new System.Drawing.Size(26, 20);
            this.colliderOffsetZ.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(226, 54);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(14, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Z";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(174, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(14, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Y";
            // 
            // colliderOffsetY
            // 
            this.colliderOffsetY.Location = new System.Drawing.Point(194, 51);
            this.colliderOffsetY.Name = "colliderOffsetY";
            this.colliderOffsetY.Size = new System.Drawing.Size(26, 20);
            this.colliderOffsetY.TabIndex = 2;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(122, 54);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(14, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "X";
            // 
            // colliderOffsetX
            // 
            this.colliderOffsetX.Location = new System.Drawing.Point(142, 51);
            this.colliderOffsetX.Name = "colliderOffsetX";
            this.colliderOffsetX.Size = new System.Drawing.Size(26, 20);
            this.colliderOffsetX.TabIndex = 1;
            // 
            // capsuleRadius
            // 
            this.capsuleRadius.Location = new System.Drawing.Point(125, 3);
            this.capsuleRadius.Name = "capsuleRadius";
            this.capsuleRadius.Size = new System.Drawing.Size(147, 20);
            this.capsuleRadius.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(13, 24);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 8;
            this.label4.Text = "Type";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 54);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(35, 13);
            this.label5.TabIndex = 9;
            this.label5.Text = "Offset";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(13, 6);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(40, 13);
            this.label6.TabIndex = 10;
            this.label6.Text = "Radius";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(13, 32);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(38, 13);
            this.label7.TabIndex = 12;
            this.label7.Text = "Height";
            // 
            // capsuleHeight
            // 
            this.capsuleHeight.Location = new System.Drawing.Point(125, 29);
            this.capsuleHeight.Name = "capsuleHeight";
            this.capsuleHeight.Size = new System.Drawing.Size(147, 20);
            this.capsuleHeight.TabIndex = 11;
            // 
            // capsulePanel
            // 
            this.capsulePanel.Controls.Add(this.capsuleHeight);
            this.capsulePanel.Controls.Add(this.label7);
            this.capsulePanel.Controls.Add(this.capsuleRadius);
            this.capsulePanel.Controls.Add(this.label6);
            this.capsulePanel.Location = new System.Drawing.Point(0, 73);
            this.capsulePanel.Name = "capsulePanel";
            this.capsulePanel.Size = new System.Drawing.Size(279, 56);
            this.capsulePanel.TabIndex = 13;
            this.capsulePanel.Visible = false;
            // 
            // spherePanel
            // 
            this.spherePanel.Controls.Add(this.sphereRadius);
            this.spherePanel.Controls.Add(this.label9);
            this.spherePanel.Location = new System.Drawing.Point(0, 73);
            this.spherePanel.Name = "spherePanel";
            this.spherePanel.Size = new System.Drawing.Size(279, 56);
            this.spherePanel.TabIndex = 14;
            this.spherePanel.Visible = false;
            // 
            // sphereRadius
            // 
            this.sphereRadius.Location = new System.Drawing.Point(125, 3);
            this.sphereRadius.Name = "sphereRadius";
            this.sphereRadius.Size = new System.Drawing.Size(147, 20);
            this.sphereRadius.TabIndex = 7;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(13, 6);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(40, 13);
            this.label9.TabIndex = 10;
            this.label9.Text = "Radius";
            // 
            // boxPanel
            // 
            this.boxPanel.Controls.Add(this.label13);
            this.boxPanel.Controls.Add(this.label14);
            this.boxPanel.Controls.Add(this.boxPoint2X);
            this.boxPanel.Controls.Add(this.label15);
            this.boxPanel.Controls.Add(this.boxPoint2Y);
            this.boxPanel.Controls.Add(this.label16);
            this.boxPanel.Controls.Add(this.boxPoint2Z);
            this.boxPanel.Controls.Add(this.label8);
            this.boxPanel.Controls.Add(this.label10);
            this.boxPanel.Controls.Add(this.boxPoint1X);
            this.boxPanel.Controls.Add(this.label11);
            this.boxPanel.Controls.Add(this.boxPoint1Y);
            this.boxPanel.Controls.Add(this.label12);
            this.boxPanel.Controls.Add(this.boxPoint1Z);
            this.boxPanel.Location = new System.Drawing.Point(0, 73);
            this.boxPanel.Name = "boxPanel";
            this.boxPanel.Size = new System.Drawing.Size(279, 56);
            this.boxPanel.TabIndex = 15;
            this.boxPanel.Visible = false;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(13, 33);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(37, 13);
            this.label13.TabIndex = 23;
            this.label13.Text = "Point2";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(122, 32);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(14, 13);
            this.label14.TabIndex = 22;
            this.label14.Text = "X";
            // 
            // boxPoint2X
            // 
            this.boxPoint2X.Location = new System.Drawing.Point(142, 29);
            this.boxPoint2X.Name = "boxPoint2X";
            this.boxPoint2X.Size = new System.Drawing.Size(26, 20);
            this.boxPoint2X.TabIndex = 21;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(174, 32);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(14, 13);
            this.label15.TabIndex = 20;
            this.label15.Text = "Y";
            // 
            // boxPoint2Y
            // 
            this.boxPoint2Y.Location = new System.Drawing.Point(194, 29);
            this.boxPoint2Y.Name = "boxPoint2Y";
            this.boxPoint2Y.Size = new System.Drawing.Size(26, 20);
            this.boxPoint2Y.TabIndex = 19;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(226, 32);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(14, 13);
            this.label16.TabIndex = 18;
            this.label16.Text = "Z";
            // 
            // boxPoint2Z
            // 
            this.boxPoint2Z.Location = new System.Drawing.Point(246, 29);
            this.boxPoint2Z.Name = "boxPoint2Z";
            this.boxPoint2Z.Size = new System.Drawing.Size(26, 20);
            this.boxPoint2Z.TabIndex = 17;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(13, 7);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(37, 13);
            this.label8.TabIndex = 16;
            this.label8.Text = "Point1";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(122, 6);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(14, 13);
            this.label10.TabIndex = 15;
            this.label10.Text = "X";
            // 
            // boxPoint1X
            // 
            this.boxPoint1X.Location = new System.Drawing.Point(142, 3);
            this.boxPoint1X.Name = "boxPoint1X";
            this.boxPoint1X.Size = new System.Drawing.Size(26, 20);
            this.boxPoint1X.TabIndex = 14;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(174, 6);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(14, 13);
            this.label11.TabIndex = 13;
            this.label11.Text = "Y";
            // 
            // boxPoint1Y
            // 
            this.boxPoint1Y.Location = new System.Drawing.Point(194, 3);
            this.boxPoint1Y.Name = "boxPoint1Y";
            this.boxPoint1Y.Size = new System.Drawing.Size(26, 20);
            this.boxPoint1Y.TabIndex = 12;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(226, 6);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(14, 13);
            this.label12.TabIndex = 11;
            this.label12.Text = "Z";
            // 
            // boxPoint1Z
            // 
            this.boxPoint1Z.Location = new System.Drawing.Point(246, 3);
            this.boxPoint1Z.Name = "boxPoint1Z";
            this.boxPoint1Z.Size = new System.Drawing.Size(26, 20);
            this.boxPoint1Z.TabIndex = 10;
            // 
            // createButton
            // 
            this.createButton.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.createButton.Location = new System.Drawing.Point(0, 128);
            this.createButton.Name = "createButton";
            this.createButton.Size = new System.Drawing.Size(279, 38);
            this.createButton.TabIndex = 16;
            this.createButton.Text = "Create";
            this.createButton.UseVisualStyleBackColor = true;
            this.createButton.Click += new System.EventHandler(this.createButton_Click);
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(279, 166);
            this.Controls.Add(this.createButton);
            this.Controls.Add(this.spherePanel);
            this.Controls.Add(this.boxPanel);
            this.Controls.Add(this.capsulePanel);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.colliderOffsetX);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.colliderOffsetY);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.colliderOffsetZ);
            this.Controls.Add(this.colliderType);
            this.Name = "Form2";
            this.Text = "Collider Editor";
            this.capsulePanel.ResumeLayout(false);
            this.capsulePanel.PerformLayout();
            this.spherePanel.ResumeLayout(false);
            this.spherePanel.PerformLayout();
            this.boxPanel.ResumeLayout(false);
            this.boxPanel.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox colliderType;
        private System.Windows.Forms.TextBox colliderOffsetZ;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox colliderOffsetY;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox colliderOffsetX;
        private System.Windows.Forms.TextBox capsuleRadius;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox capsuleHeight;
        private System.Windows.Forms.Panel capsulePanel;
        private System.Windows.Forms.Panel spherePanel;
        private System.Windows.Forms.TextBox sphereRadius;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Panel boxPanel;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox boxPoint2X;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox boxPoint2Y;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox boxPoint2Z;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox boxPoint1X;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox boxPoint1Y;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox boxPoint1Z;
        private System.Windows.Forms.Button createButton;
    }
}