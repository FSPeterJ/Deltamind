using System;
using System.Windows.Forms;

namespace GhostbaitModelCreator {

    public partial class ColliderCreatorForm : Form {

        public enum ColliderType {
            SPHERE =0,
            CAPSULE =1,
            BOX = 2,
        }

        public class ColliderData : ModelCreatorForm.BaseComponent
        {

            //Capsule
            public float height;

            public float offsetX, offsetY, offsetZ;

            //Box
            public float point1X, point1Y, point1Z;

            public float point2X, point2Y, point2Z;

            //Sphere
            public float radius;

            public ColliderType colliderType { get; set; }
        }

        private bool edit = false;
        private int editingIndex = -1;
        private ModelCreatorForm mainForm;
        private ColliderData newCol;

        public ColliderCreatorForm(ModelCreatorForm _main) {
            InitializeComponent();
            colliderType.DataSource = Enum.GetValues(typeof(ColliderType));
            colliderType.SelectedItem = ColliderType.SPHERE;
            spherePanel.Visible = true;
            mainForm = _main;
            createButton.Text = "CREATE";
        }

        internal void Edit(ColliderData col, int index) {
            createButton.Text = "SAVE";

            edit = true;
            editingIndex = index;
            switch (col.colliderType) {
                case ColliderType.SPHERE:
                    spherePanel.Visible = true;
                    capsulePanel.Visible = false;
                    boxPanel.Visible = false;
                    break;

                case ColliderType.CAPSULE:
                    spherePanel.Visible = false;
                    capsulePanel.Visible = true;
                    boxPanel.Visible = false;
                    break;

                case ColliderType.BOX:
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

            colliderType.SelectedItem = col.colliderType;

            colliderOffsetX.Value = (decimal)col.offsetX;
            colliderOffsetY.Value = (decimal)col.offsetY;
            colliderOffsetZ.Value = (decimal)col.offsetZ;

            sphereRadius.Value = (decimal)col.radius;
            capsuleRadius.Value = (decimal)col.radius;
            capsuleHeight.Value = (decimal)col.height;
            boxPoint1X.Value = (decimal)col.point1X;
            boxPoint1Y.Value = (decimal)col.point1Y;
            boxPoint1Z.Value = (decimal)col.point1Z;
            boxPoint2X.Value = (decimal)col.point2X;
            boxPoint2Y.Value = (decimal)col.point2Y;
            boxPoint2Z.Value = (decimal)col.point2Z;
        }

        private void colliderType_SelectedIndexChanged(object sender, EventArgs e) {
            sphereRadius.Value =
            capsuleRadius.Value =
            capsuleHeight.Value =
            boxPoint1X.Value =
            boxPoint1Y.Value =
            boxPoint1Z.Value =
            boxPoint2X.Value =
            boxPoint2Y.Value =
            boxPoint2Z.Value = 0;

            switch ((ColliderType)colliderType.Items[colliderType.SelectedIndex]) {
                case ColliderType.SPHERE:
                    spherePanel.Visible = true;
                    capsulePanel.Visible = false;
                    boxPanel.Visible = false;
                    break;

                case ColliderType.CAPSULE:
                    spherePanel.Visible = false;
                    capsulePanel.Visible = true;
                    boxPanel.Visible = false;
                    break;

                case ColliderType.BOX:
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

        private void createButton_Click(object sender, EventArgs e) {
            newCol = new ColliderData {
                colliderType = (ColliderType)Enum.Parse(typeof(ColliderType), colliderType.Items[colliderType.SelectedIndex].ToString(), true),
                offsetX = (float)colliderOffsetX.Value,
                offsetY = (float)colliderOffsetY.Value,
                offsetZ = (float)colliderOffsetZ.Value,
            };

            switch (newCol.colliderType) {
                case ColliderType.SPHERE:
                    newCol.radius = (float)sphereRadius.Value;
                    break;

                case ColliderType.CAPSULE:
                    newCol.radius = (float)capsuleRadius.Value;
                    newCol.height = (float)capsuleHeight.Value;
                    break;

                case ColliderType.BOX:
                    newCol.point1X = (float)boxPoint1X.Value;
                    newCol.point1Y = (float)boxPoint1Y.Value;
                    newCol.point1Z = (float)boxPoint1Z.Value;
                    newCol.point2X = (float)boxPoint2X.Value;
                    newCol.point2Y = (float)boxPoint2Y.Value;
                    newCol.point2Z = (float)boxPoint2Z.Value;
                    break;

                default: break;
            }

            if (edit) {
                edit = false;
                mainForm.CreateColliderPressed(newCol, editingIndex);
                editingIndex = -1;
            } else
            {
                newCol.ComponentIdentifier= "Physical";
                mainForm.CreateColliderPressed(newCol);
            }
            this.Close();
        }
    }
}
