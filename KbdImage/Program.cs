using System.Runtime.InteropServices;
using System.Diagnostics.CodeAnalysis;

namespace KbdImage;

using Parsing;
using Rendering;

using static Int32;
using static Console;
using static StringComparison;

static class Program
{
	[DllImport("Kernel32", SetLastError = true, ExactSpelling = true), SuppressMessage("Interoperability", "SYSLIB1054: Используйте LibraryImportAttribute вместо DllImportAttribute для генерирования кода маршализации P/Invoke во время компиляции")]
	static extern bool AttachConsole(uint dwProcessId = ~0u);

	const string Usage = """
		KbdImage [файлъ.cpp|файлъ.dll] [файлъ.png|.jpg|.svg|…] [save файлъ] [size ШxВ] [quality N] [print] [klid XXXXXXXX]

		  файлъ.cpp | файлъ.dll  таблицы разкладки (безъ параметровъ — окно, файлъ можно открыть Ctrl+O или перетащить)
		  файлъ.png | save файлъ  сохранить картинку и закрыться; форматъ — по расширенію:
		                         png, jpg, svg, emf, bmp, gif, tif (прозрачность — кромѣ jpg и bmp)
		  size ШxВ | size Ш      рамка въ пикселяхъ, въ которую умѣщается картинка; пропорціи клавіатуры сохраняются; безъ него — 1×
		  quality N              качество jpg, 1..100 (по умолчанію 85)
		  print                  напечатать на принтерѣ по умолчанію и закрыться
		  klid XXXXXXXX          KLID разкладки (a0000419), по нему выбирается сѵстемная разкраска для кирпичной обводки;
		                         безъ него KLID берётся изъ Setup\Kbd.wxs, а если нѣтъ — US
		  /?                     эта справка
		""";

	[STAThread]
	static int Main(string[] args)
	{
		try
		{
			ApplicationConfiguration.Initialize();
			Application.SetColorMode(SystemColorMode.System);

			string? input = null;
			string? save = null;
			string? klid = null;
			Size? size = null;
			bool print = false;
			int quality = Exporter.DefaultQuality;
			for (int i = 0; i < args.Length; i++)
			{
				string a = args[i];
				if (a is "/?" or "-?" or "--help" or "/h" or "-h")
				{
					if (AttachConsole()) WriteLine(Usage);
					else MessageBox.Show(Usage, "KbdImage", MessageBoxButtons.OK, MessageBoxIcon.Information);
					return 0;
				}
				if (a.Equals("save", OrdinalIgnoreCase) && i + 1 < args.Length)
					save = args[++i];
				else if (a.Equals("klid", OrdinalIgnoreCase) && i + 1 < args.Length)
					klid = args[++i];
				else if (a.Equals("size", OrdinalIgnoreCase) && i + 1 < args.Length)
					size = Exporter.ParseSize(args[++i]);
				else if (a.Equals("quality", OrdinalIgnoreCase) && i + 1 < args.Length)
					quality = Math.Clamp(TryParse(args[++i], out int q) ? q : Exporter.DefaultQuality, 1, 100);
				else if (a.Equals("print", OrdinalIgnoreCase))
					print = true;
				else if (Exporter.CanSave(a))
					save = a;
				else
					input = a;
			}

			if (save == null && !print)
			{
				Application.Run(new MainForm(input, klid));
				return 0;
			}

			AttachConsole();
			if (input == null)
			{
				Error.WriteLine("Не указанъ входной файлъ (.cpp или .dll)");
				return 2;
			}
			KeyboardPainter painter = new();
			painter.SetLayout(LayoutLoader.Load(input, klid));
			if (save != null)
				Exporter.Save(painter, save, size ?? Exporter.DefaultSize, quality);
			if (print)
				Exporter.Print(painter, Path.GetFileName(input));
			return 0;
		}
		catch (Exception e) when (e is IOException or InvalidDataException or UnauthorizedAccessException or InvalidOperationException or System.Drawing.Printing.InvalidPrinterException)
		{
			AttachConsole();
			Error.WriteLine(e.Message);
			return 1;
		}
	}
}
