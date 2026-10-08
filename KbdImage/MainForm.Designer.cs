namespace KbdImage;

partial class MainForm
{
	System.ComponentModel.IContainer components = null!;

	protected override void Dispose(bool disposing)
	{
		if (disposing)
			components?.Dispose();
		base.Dispose(disposing);
	}

	#region Windows Form Designer generated code

	void InitializeComponent()
	{
		System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
		menuStrip = new MenuStrip();
		fileMenu = new ToolStripMenuItem();
		openMenu = new ToolStripMenuItem();
		saveMenu = new ToolStripMenuItem();
		printMenu = new ToolStripMenuItem();
		openFileDialog = new OpenFileDialog();
		saveFileDialog = new SaveFileDialog();
		printDialog = new PrintDialog();
		canvas = new Canvas();
		menuStrip.SuspendLayout();
		SuspendLayout();
		// 
		// menuStrip
		// 
		menuStrip.Items.AddRange(new ToolStripItem[] { fileMenu });
		menuStrip.Location = new Point(0, 0);
		menuStrip.Name = "menuStrip";
		menuStrip.Size = new Size(1100, 24);
		menuStrip.TabIndex = 1;
		// 
		// fileMenu
		// 
		fileMenu.DropDownItems.AddRange(new ToolStripItem[] { openMenu, saveMenu, printMenu });
		fileMenu.Name = "fileMenu";
		fileMenu.Size = new Size(55, 20);
		fileMenu.Text = "&Файлъ";
		// 
		// openMenu
		// 
		openMenu.Name = "openMenu";
		openMenu.ShortcutKeys = Keys.Control | Keys.O;
		openMenu.Size = new Size(208, 22);
		openMenu.Text = "&Открыть…";
		openMenu.Click += Open_Click;
		// 
		// saveMenu
		// 
		saveMenu.Name = "saveMenu";
		saveMenu.ShortcutKeys = Keys.Control | Keys.S;
		saveMenu.Size = new Size(208, 22);
		saveMenu.Text = "&Сохранить…";
		saveMenu.Visible = false;
		saveMenu.Enabled = false;
		saveMenu.Click += Save_Click;
		// 
		// printMenu
		// 
		printMenu.Name = "printMenu";
		printMenu.ShortcutKeys = Keys.Control | Keys.P;
		printMenu.Size = new Size(208, 22);
		printMenu.Text = "&Печать…";
		printMenu.Visible = false;
		printMenu.Enabled = false;
		printMenu.Click += Print_Click;
		// 
		// openFileDialog
		// 
		openFileDialog.Filter = "Таблицы разкладки (*.cpp;*.c;*.dll)|*.cpp;*.c;*.dll|Всѣ файлы|*";
		// 
		// saveFileDialog
		// 
		saveFileDialog.Filter = "PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg;*.jpeg|SVG (*.svg)|*.svg|EMF (*.emf)|*.emf|BMP (*.bmp)|*.bmp|GIF (*.gif)|*.gif|TIFF (*.tif)|*.tif;*.tiff";
		// 
		// printDialog
		// 
		printDialog.UseEXDialog = true;
		// 
		// canvas
		// 
		canvas.Dock = DockStyle.Fill;
		canvas.Location = new Point(0, 24);
		canvas.Name = "canvas";
		canvas.Size = new Size(1100, 306);
		canvas.TabIndex = 2;
		// 
		// MainForm
		// 
		AllowDrop = true;
		AutoScaleDimensions = new SizeF(7F, 15F);
		AutoScaleMode = AutoScaleMode.Font;
		ClientSize = new Size(1100, 330);
		Controls.Add(canvas);
		Controls.Add(menuStrip);
		Font = new Font("Segoe UI", 9F);
		Icon = (Icon)resources.GetObject("$this.Icon");
		MainMenuStrip = menuStrip;
		MinimumSize = new Size(480, 200);
		Name = "MainForm";
		Text = "KbdImage — Ctrl+O: открыть .cpp или .dll, либо перетащить файлъ сюда";
		DragDrop += MainForm_DragDrop;
		DragEnter += MainForm_DragEnter;
		menuStrip.ResumeLayout(false);
		menuStrip.PerformLayout();
		ResumeLayout(false);
		PerformLayout();
	}

	#endregion

	MenuStrip menuStrip = null!;
	ToolStripMenuItem fileMenu = null!;
	ToolStripMenuItem openMenu = null!;
	ToolStripMenuItem saveMenu = null!;
	ToolStripMenuItem printMenu = null!;
	OpenFileDialog openFileDialog;
	SaveFileDialog saveFileDialog;
	PrintDialog printDialog;
	Canvas canvas;
}
