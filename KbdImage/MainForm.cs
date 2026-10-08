using System.Drawing.Printing;
using System.Runtime.InteropServices;

namespace KbdImage;

using Parsing;

using Rendering;

using static Path;
using static Math;
using static Marshal;
using static StringComparison;

sealed partial class MainForm : Form
{
	readonly string? KLID;
	string? Path;

	const int WM_SIZING = 0x0214;

	int CanvasHeight(int canvasWidth)
		=> (int)Round((canvasWidth - 2 * canvas.Pad) * KeyboardPainter.LogicalSize.Height / KeyboardPainter.LogicalSize.Width) + 2 * canvas.Pad;

	/// <summary>Безъ параметровъ — для дизайнера формъ.</summary>
	public MainForm(string? path, string? klid)
	{
		Path = path;
		KLID = klid;

		InitializeComponent();

		ClientSize = new(ClientSize.Width, ClientSize.Height - canvas.Height + CanvasHeight(canvas.Width));
	}

	/// <summary>Открыть файлъ, какъ только окно покажется (изъ командной строки).</summary>
	protected override void OnShown(EventArgs e)
	{
		if (Path != null)
			Open(Path, KLID);
		base.OnShown(e);
	}

	[StructLayout(LayoutKind.Sequential)]
	record struct RECT(int Left, int Top, int Right, int Bottom);

	/// <summary>Размѣръ окна тянется только съ сохраненіемъ пропорцій клавіатуры: ширина мѣняется — высота подгоняется (за верхній/нижній край — наоборотъ).</summary>
	protected override void WndProc(ref Message m)
	{
		if (m.Msg == WM_SIZING && WindowState == FormWindowState.Normal)
		{
			RECT r = PtrToStructure<RECT>(m.LParam);
			int edge = (int)m.WParam; // 1 лѣво, 2 право, 3 верхъ, 4 верхъ-лѣво, 5 верхъ-право, 6 низъ, 7 низъ-лѣво, 8 низъ-право
			int chromeW = Width - canvas.Width;
			int chromeH = Height - canvas.Height;
			if (edge is 3 or 6)
			{
				int cw = (int)Round((r.Bottom - r.Top - chromeH - 2 * canvas.Pad) * KeyboardPainter.LogicalSize.Width / KeyboardPainter.LogicalSize.Height) + 2 * canvas.Pad;
				r.Right = r.Left + cw + chromeW;
			}
			else
			{
				int h = chromeH + CanvasHeight(r.Right - r.Left - chromeW);
				if (edge is 4 or 5)
					r.Top = r.Bottom - h;
				else
					r.Bottom = r.Top + h;
			}
			StructureToPtr(r, m.LParam, false);
		}
		base.WndProc(ref m);
	}

	void MainForm_DragEnter(object? sender, DragEventArgs e)
	{
		e.Effect = e.Data?.GetDataPresent(DataFormats.FileDrop) == true ? DragDropEffects.Copy : DragDropEffects.None;
	}

	void MainForm_DragDrop(object? sender, DragEventArgs e)
	{
		if (e.Data?.GetData(DataFormats.FileDrop) is string[] { Length: > 0 } files)
			Open(files[0]);
	}

	void Open_Click(object? sender, EventArgs e)
	{
		if (openFileDialog.ShowDialog(this) == DialogResult.OK)
			Open(openFileDialog.FileName);
	}

	void Save_Click(object? sender, EventArgs e)
	{
		if (Path == null) return;
		string name = GetFileNameWithoutExtension(Path);
		if (name.StartsWith("My", OrdinalIgnoreCase))
			name = name[2..];
		saveFileDialog.FileName = name + ".png";
		saveFileDialog.InitialDirectory = GetDirectoryName(GetFullPath(Path));
		if (saveFileDialog.ShowDialog(this) == DialogResult.OK)
			Exporter.Save(canvas.Painter, saveFileDialog.FileName, canvas.ClientSize);
	}

	void Print_Click(object? sender, EventArgs e)
	{
		printDialog.Document = new PrintDocument();
		if (printDialog.ShowDialog(this) == DialogResult.OK)
			Exporter.Print(canvas.Painter, Text);
	}

	void Open(string path, string? klid = null)
	{
		try
		{
			Path = path;
			Text = $"KbdImage — {GetFileName(path)}";
			canvas.Painter.SetLayout(LayoutLoader.Load(path, klid));
			canvas.Invalidate();
			printMenu.Visible = true;
			printMenu.Enabled = true;
			saveMenu.Visible = true;
			saveMenu.Enabled = true;
		}
		catch (Exception e) when (e is IOException or InvalidDataException or UnauthorizedAccessException)
		{
			MessageBox.Show(this, e.Message, Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
		}
	}
}
