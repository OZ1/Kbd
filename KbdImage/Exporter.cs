using System.Drawing.Imaging;
using System.Drawing.Printing;
using System.Text;

namespace KbdImage;

using Rendering;

using static File;
using static Path;
using static Math;
using static Int32;
using static ImageFormat;

/// <summary>Сохраненіе (png, jpg, bmp, gif, tif, svg, emf) и печать.</summary>
public static class Exporter
{
	public const int DefaultQuality = 85;

	/// <summary>Размѣръ по умолчанію — логическій размѣръ картинки.</summary>
	public static Size DefaultSize => new((int)Ceiling(KeyboardPainter.LogicalSize.Width),
	/**/                                  (int)Ceiling(KeyboardPainter.LogicalSize.Height));

	/// <summary>Растровые форматы GDI+ съ прозрачностью.</summary>
	static readonly Dictionary<string, ImageFormat> Raster = new(StringComparer.OrdinalIgnoreCase)
	{
		{ ".png",  Png },
		{ ".bmp",  Bmp },
		{ ".gif",  Gif },
		{ ".tif",  Tiff },
		{ ".tiff", Tiff },
	};

	/// <summary>Рамка изъ «ШИРИНАxВЫСОТА» или «ШИРИНА» (высота тогда любая — по пропорціямъ).</summary>
	public static Size ParseSize(string text)
	{
		string[] p = text.Split(['x', 'X', '×'], 2);
		int h = MaxValue;
		if (!TryParse(p[0], out int w) || w <= 0 || (p.Length > 1 && (!TryParse(p[1], out h) || h <= 0)))
			throw new InvalidDataException($"Размѣръ «{text}» непонятенъ, нужно ШИРИНАxВЫСОТА");
		return new Size(w, h);
	}

	/// <summary>Сохраняется ли файлъ съ такимъ расширеніемъ.</summary>
	public static bool CanSave(string path)
		=> GetExtension(path).ToLowerInvariant() is ".svg" or ".emf" or ".jpg" or ".jpeg" || Raster.ContainsKey(GetExtension(path).ToLowerInvariant());

	/// <summary>Наибольшій размѣръ съ пропорціями клавіатуры, умѣщающійся въ box.</summary>
	public static Size Proportional(Size box)
	{
		float s = Min(box.Width / KeyboardPainter.LogicalSize.Width, box.Height / KeyboardPainter.LogicalSize.Height);
		return new(Math.Max(1, (int)Round(KeyboardPainter.LogicalSize.Width * s)),
		/**/       Math.Max(1, (int)Round(KeyboardPainter.LogicalSize.Height * s)));
	}

	/// <summary>Форматъ — по расширенію. Картинка всегда съ пропорціями клавіатуры: size — только рамка, въ которую она умѣщается. Фонъ прозрачный (кромѣ jpg).</summary>
	public static void Save(KeyboardPainter painter, string path, Size size, int quality = DefaultQuality)
	{
		size = Proportional(size);
		string? dir = GetDirectoryName(GetFullPath(path));
		if (dir != null)
			Directory.CreateDirectory(dir);
		RectangleF rect = new(0, 0, size.Width, size.Height);
		switch (GetExtension(path).ToLowerInvariant())
		{
			case ".svg":
				using (Bitmap probe = new(1, 1))
				using (Graphics g = Graphics.FromImage(probe))
				using (GdiSurface m = new(g))
				WriteAllText(path, painter.ToSvg(size.Width, size.Height, m.Measure), new UTF8Encoding(false));
				break;
			case ".emf":
				SaveEmf(painter, path, rect);
				break;
			case ".jpg" or ".jpeg":
				using (Bitmap bmp = Render(painter, size, Color.White))
				using (EncoderParameters ep = new(1))
				{
					ep.Param[0] = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality, quality);
					bmp.Save(path, ImageCodecInfo.GetImageEncoders().First(c => c.FormatID == Jpeg.Guid), ep);
				}
				break;
			case string ext when Raster.TryGetValue(ext, out ImageFormat? format):
				using (Bitmap bmp = Render(painter, size, Color.Transparent))
					bmp.Save(path, format);
				break;
			default:
				throw new InvalidDataException($"Неизвѣстный форматъ «{GetExtension(path)}»");
		}
	}

	static Bitmap Render(KeyboardPainter painter, Size size, Color background)
	{
		Bitmap bmp = new(size.Width, size.Height, PixelFormat.Format32bppArgb);
		using Graphics g = Graphics.FromImage(bmp);
		g.Clear(background);
		painter.PaintAll(g, new(0, 0, size.Width, size.Height));
		return bmp;
	}

	static void SaveEmf(KeyboardPainter painter, string path, RectangleF rect)
	{
		using Bitmap probe = new(1, 1);
		using Graphics reference = Graphics.FromImage(probe);
		IntPtr hdc = reference.GetHdc();
		try
		{
			using Metafile mf = new(path, hdc, rect, MetafileFrameUnit.Pixel, EmfType.EmfPlusDual);
			using Graphics g = Graphics.FromImage(mf);
			painter.PaintAll(g, rect);
		}
		finally
		{
			reference.ReleaseHdc(hdc);
		}
	}

	public static void Print(KeyboardPainter painter, string title)
	{
		using PrintDocument doc = new() { DocumentName = title };
		doc.DefaultPageSettings.Landscape = true;
		doc.PrintPage += (_, e) => painter.PaintAll(e.Graphics!, e.MarginBounds);
		doc.Print();
	}
}
