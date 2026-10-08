using System.Runtime.InteropServices;
using System.Runtime.Loader;

using Microsoft.Build.Framework;

namespace KbdImage;

using Parsing;
using Rendering;

using static Path;
using static RuntimeEnvironment;
using static AssemblyLoadContext;

/// <summary>Задача MSBuild: картинка разкладки изъ MyKbd*.cpp — то же, что <c>KbdImage файлъ.cpp save файлъ.png</c></summary>
public class SaveKbdImageTask : Microsoft.Build.Utilities.Task
{
	/// <summary>Таблицы разкладки, .cpp или .dll</summary>
	[Required] public string Input { get; set; } = "";

	/// <summary>Куда сохранить; форматъ — по расширенію</summary>
	[Required] public string Output { get; set; } = "";

	/// <summary>KLID разкладки; безъ него берётся изъ Setup\Kbd.wxs</summary>
	public string? KLID { get; set; }

	// .NET task host MSBuild работаетъ на одномъ Microsoft.NETCore.App, безъ WindowsDesktop:
	// WinForms и System.Drawing берутся изъ сосѣдней папки той же версіи
	static SaveKbdImageTask()
	{
		string core = TrimEndingDirectorySeparator(GetRuntimeDirectory());
		string desktop = GetFullPath(Combine(core, @"..\..\Microsoft.WindowsDesktop.App", GetFileName(core)));
		GetLoadContext(typeof(SaveKbdImageTask).Assembly)!.Resolving += (context, name) =>
		{
			string dll = Combine(desktop, name.Name + ".dll");
			return File.Exists(dll) ? context.LoadFromAssemblyPath(dll) : null;
		};
	}

	public override bool Execute()
	{
		try
		{
			KeyboardPainter painter = new();
			painter.SetLayout(LayoutLoader.Load(Input, string.IsNullOrEmpty(KLID) ? null : KLID));
			Exporter.Save(painter, Output, Exporter.DefaultSize, Exporter.DefaultQuality);
			return true;
		}
		catch (Exception e) when (e is IOException or InvalidDataException or UnauthorizedAccessException or InvalidOperationException)
		{
			Log.LogError(null, null, null, Input, 0, 0, 0, 0, e.Message);
			return false;
		}
	}
}
