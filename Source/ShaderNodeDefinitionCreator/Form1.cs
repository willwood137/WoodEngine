using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using System.Xml;

namespace ShaderNodeDefinitionCreator
{
    
   
    public partial class Form1 : Form
    {
        List<NodeCategory> Definitions;
        NodeDefinition workingDefinition;

        public Form1()
        {
            InitializeComponent();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void AddNewNode(object sender, EventArgs e)
        {
            workingDefinition = new NodeDefinition();

            workingDefinition.canBeFrag = true;
            workingDefinition.canBeVert = true;
            workingDefinition.name = "Untitled";
            workingDefinition.category = "Untitled";

            UpdateWorkingNode();
        }

        private void UpdateWorkingNode()
        {
            textBox1.Text = workingDefinition.name;
            textBox2.Text = workingDefinition.category;
        }

        private void loadFile(string FileName)
        {

            XDocument xdoc = XDocument.Load(FileName);
            //XmlDocument doc = new XmlDocument();
            XElement rootElem = xdoc.Root;

            if (rootElem.Name == "ShaderEditor")
            {
                var definitions = rootElem.Elements("NodeDefinition");
                
            }
        }

        private void loadDirectory(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description =
            "Select the directory that you want to load";
            dialog.ShowNewFolderButton = false;
            dialog.SelectedPath = (AppDomain.CurrentDomain.BaseDirectory);
            DialogResult result = dialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                string chosenDirectory = dialog.SelectedPath;
                string[] array1 = Directory.GetFiles(chosenDirectory);
                for (int i = 0; i < array1.Length; ++i)
                {
                    string file = array1[i];
                    string ext= Path.GetExtension(file);

                    if (ext == ".xml")
                    {
                        loadFile(file);
                    }
                }

            }
        }
    }
}
