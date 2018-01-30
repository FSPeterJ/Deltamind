using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static GhostbaitModelCreator.ModelCreatorForm.ComponentType;


namespace GhostbaitModelCreator
{
    public partial class AnimationCreatorForm : Form
    {
        internal struct AnimationData
        {
            public string filePath;
            public string name;
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
                InitialDirectory = @"C:\",
                Title = "An animation file for this Ghostbait object."
            };
            if (open.ShowDialog() == DialogResult.OK)
            {
                if (open.FileName.Substring(open.FileName.Length - 4) == ".fbx")
                {
                    string animFile = mainForm.GenerateRelativeComponentFilePath(open.FileName, ANIMATION);
                    if (ModelCreatorForm.get_animdata_from_scene(open.FileName, animFile) != -1)
                    {
                        animFile = animFile.Substring(/*Make const*/19);
                        filePathBox.Text = animFile;
                    }
                }
                else
                {
                    filePathBox.Text = open.FileName;
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)//OK Button
        {
            if(filePathBox.Text.Length == 0)
            {
                WarningLabel.Text = "No animation loaded!";
                return;
            }
            if(nameBox.Text.Length == 0)
            {
                WarningLabel.Text = "Animation needs a name!";
                return;
            }
            newAnim = new AnimationData();
            newAnim.filePath = filePathBox.Text;
            newAnim.name = nameBox.Text;
            mainForm.CreateAnimationPressed(newAnim);
            this.Close();
        }
    }
}
