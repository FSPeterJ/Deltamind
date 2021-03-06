﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using GhostbaitModelCreator.Properties;


namespace GhostbaitModelCreator
{
    public partial class AnimationCreatorForm : Form
    {
        internal class AnimationData : ModelCreatorForm.BaseComponent
        {

        }

        private bool editing = false;
        private int editIndex = -1;
        private ModelCreatorForm mainForm;
        private AnimationData newAnim;
        public AnimationCreatorForm(ModelCreatorForm _main)
        {
            InitializeComponent();
            mainForm = _main;
        }
        //I forgot how windows forms stuff worked
        private void button1_Click(object sender, EventArgs e)//File button
        {
            OpenFileDialog open = new OpenFileDialog
            {
                Filter = "Animations (*.anim), (*.fbx)| *.anim; *.fbx;",
                InitialDirectory = Settings.Default.animation_path,
                Title = "An animation file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                Settings.Default.animation_path = Path.GetDirectoryName(open.FileName);
                Settings.Default.Save();
                if (open.FileName.Substring(open.FileName.Length - 4) == ".fbx")
                {

                    string animFile = Path.GetFileNameWithoutExtension(open.FileName) + ".anim";
                    if (ModelCreatorForm.get_animdata_from_scene(open.FileName, animFile) != -1)
                    {
                        filePathBox.Text = animFile;
                    }
                }
                else
                {

                    filePathBox.Text = Path.GetFileName(open.FileName);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)//OK Button
        {
            if (filePathBox.Text.Length == 0)
            {
                WarningLabel.Text = "No animation loaded!";
                return;
            }
            if (nameBox.Text.Length == 0)
            {
                WarningLabel.Text = "Animation needs a name!";
                return;
            }
            newAnim = new AnimationData();
            newAnim.ComponentIdentifier = Path.GetFileName(filePathBox.Text);
            newAnim.AbsolutePath = filePathBox.Text;
            newAnim.ComponentTag = nameBox.Text;
            if (editing)
            {
                editing = false;
                mainForm.CreateAnimationPressed(newAnim, editIndex);
                editIndex = -1;
            }
            else
                mainForm.CreateAnimationPressed(newAnim);
            this.Close();
        }

        internal void Edit(AnimationData data, int index)
        {
            editing = true;
            editIndex = index;
            filePathBox.Text = data.AbsolutePath;
            nameBox.Text = data.ComponentTag;
        }
    }
}
