using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GhostbaitModelCreator
{
    public partial class Form2 : Form
    {

        public struct ColliderData
        {
            public string type;
            public float offsetX, offsetY, offsetZ;


            //Sphere
            public float radius;
            //Capsule
            public float height;
            //Box
            public float point1X, point1Y, point1Z;
            public float point2X, point2Y, point2Z;

        }

        public ColliderData newCol;
        public Form2()
        {
            InitializeComponent();
            colliderType.SelectedItem = "Sphere";
            spherePanel.Visible = true;
        }
        Form1 form1;
        public Form2(Form1 f)
        {
            InitializeComponent();
            colliderType.SelectedItem = "Sphere";
            spherePanel.Visible = true;
            form1 = f;
        }

        private void colliderType_SelectedIndexChanged(object sender, EventArgs e)
        {
            sphereRadius.Text = "";
            capsuleRadius.Text = "";
            capsuleHeight.Text = "";
            boxPoint1X.Text = "";
            boxPoint1Y.Text = "";
            boxPoint1Z.Text = "";
            boxPoint2X.Text = "";
            boxPoint2Y.Text = "";
            boxPoint2Z.Text = "";



            switch (colliderType.Items[colliderType.SelectedIndex].ToString())
            {
                case "Sphere":
                    spherePanel.Visible = true;
                    capsulePanel.Visible = false;
                    boxPanel.Visible = false;
                    break;
                case "Capsule":
                    spherePanel.Visible = false;
                    capsulePanel.Visible = true;
                    boxPanel.Visible = false;
                    break;
                case "Box":
                    spherePanel.Visible = false;
                    capsulePanel.Visible = false;
                    boxPanel.Visible = true;
                    break;
                default:
                    spherePanel.Visible = false;
                    capsulePanel.Visible = false;
                    boxPanel.Visible = false;
                    break;
            }
        }

        private void createButton_Click(object sender, EventArgs e)
        {
            newCol = new ColliderData();
            newCol.type = colliderType.Items[colliderType.SelectedIndex].ToString();
            if (!float.TryParse(colliderOffsetX.Text, out newCol.offsetX)) {
                DialogResult result = MessageBox.Show(this, "X Offset not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                if(result == DialogResult.Yes)
                {
                    this.Close();
                }
                return;
            }
            if (!float.TryParse(colliderOffsetY.Text, out newCol.offsetY)) {
                DialogResult result = MessageBox.Show(this, "Y Offset not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                if (result == DialogResult.Yes)
                {
                    this.Close();
                }
                return;
            }
            if (!float.TryParse(colliderOffsetZ.Text, out newCol.offsetZ)) {
                DialogResult result = MessageBox.Show(this, "Z Offset not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                if (result == DialogResult.Yes)
                {
                    this.Close();
                }
                return;
            }

            if (newCol.type == "Sphere") { 
                if (!float.TryParse(sphereRadius.Text, out newCol.radius))
                {
                    DialogResult result = MessageBox.Show(this, "Radius not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
            }
            else if (newCol.type == "Capsule")
            {
                if (!float.TryParse(capsuleRadius.Text, out newCol.radius))
                {
                    DialogResult result = MessageBox.Show(this, "Radius not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
                if (!float.TryParse(capsuleHeight.Text, out newCol.height))
                {
                    DialogResult result = MessageBox.Show(this, "Height not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
            }
            else
            {
                if (!float.TryParse(boxPoint1X.Text, out newCol.point1X)){
                    DialogResult result = MessageBox.Show(this, "Point1's X not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
                if (!float.TryParse(boxPoint1Y.Text, out newCol.point1Y))
                {
                    DialogResult result = MessageBox.Show(this, "Point1's Y not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
                if (!float.TryParse(boxPoint1Z.Text, out newCol.point1Z))
                {
                    DialogResult result = MessageBox.Show(this, "Point1's Z not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
                if (!float.TryParse(boxPoint2X.Text, out newCol.point2X))
                {
                    DialogResult result = MessageBox.Show(this, "Point2's X not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
                if (!float.TryParse(boxPoint2Y.Text, out newCol.point2Y))
                {
                    DialogResult result = MessageBox.Show(this, "Point2's Y not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
                if (!float.TryParse(boxPoint2Z.Text, out newCol.point2Z))
                {
                    DialogResult result = MessageBox.Show(this, "Point2's Z not a float! Cancel the colliders creation?", "Entry Not Valid", MessageBoxButtons.YesNo);
                    if (result == DialogResult.Yes)
                    {
                        this.Close();
                    }
                    return;
                }
            }
            this.Close();
            form1.CreateColliderPressed();
        }
    }
}
